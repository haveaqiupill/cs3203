#include "QueryEvaluator.h"

#include <vector>
#include <iostream>
#include <utility>
#include <unordered_set>

#include "../Commons/Exceptions.h"
#include "Query/QueryClause.h"
#include "QueryParserUtility.h"

IntermediateAnswer QueryEvaluator::evaluateQueryObject(Query *queryObject) {
    // With condition list and clause list are stored separately
    WITH_CONDITION_LIST clauseList = queryObject->getWithConditionList();
    CLAUSE_LIST moreClauseList = queryObject->getClauseList();
    clauseList.insert(clauseList.end(), moreClauseList.begin(), moreClauseList.end());
    DECLARATION_TABLE declarationTable = queryObject->getDeclarations();

    std::vector<ClauseAnswer> clauseAnswerList = { };

    SelectionType selectionType = queryObject->getSelectionType();
    if (selectionType == SelectionType::BOOLEAN) {
        for (size_t i = 0; i < clauseList.size(); i++) {
            clauseAnswerList.push_back(evaluateClause(clauseList[i],
                declarationTable));
        }

        return IntermediateAnswer({}, selectionType, clauseAnswerList);
    } else {
        SELECTION_LIST selectionList = queryObject->getSelectionList();

        for (size_t i = 0; i < selectionList.size(); i++) {
            Declaration selection = selectionList.at(i);
            DesignEntity selectionSynType = declarationTable.at(selection.getSynonym());
            ClauseAnswer selResults = ClauseAnswer(true, getAllOfDesignEntity(selection,
                selectionSynType, pkb));
            clauseAnswerList.push_back(selResults);
        }

        // This is order sensitive, clause must be added after the selection synonym
        for (size_t i = 0; i < clauseList.size(); i++) {
            clauseAnswerList.push_back(evaluateClause(clauseList[i], declarationTable));
        }

        IntermediateAnswer result = IntermediateAnswer(queryObject->getSelectionList(),
            selectionType, clauseAnswerList);
        return result;
    }
}

IntermediateAnswer QueryEvaluator::evaluateOptimisedQuery(Query *query) {
    SelectionType selectionType = query->getSelectionType();
    DECLARATION_TABLE declarationTable = query->getDeclarations();
    GROUPED_CLAUSES gc = query->getGroupedClauses();

    std::unordered_set<DECLARATION, dec_hash> syns;
    std::vector<ClauseAnswer> clauseAnswerList = { };

    for (auto clauseList : gc) {
        // If there are no clauses to be evaluated
        if (clauseList.size() == 0) {
            continue;
        }

        ResultTable cumulativeRt = ResultTable();
        bool hasFalseClause = false;

        for (size_t i = 0; i < clauseList.size(); i++) {
            ClauseAnswer ca = evaluateClause(clauseList[i], declarationTable);
            // Early return if any clause answer is false
            if (!ca.getIsClauseTrue() ||
                (ca.getResultTable().getSynonyms().size() > 0 &&
                    ca.getResultTable().getResultTable().size() == 0)) {
                hasFalseClause = true;
                break;
            }
            cumulativeRt = cumulativeRt.merge(ca.getResultTable());
        }

        // Check if there was a false clause or cumulative result table is empty
        if (hasFalseClause || (cumulativeRt.getSynonyms().size() > 0 &&
            cumulativeRt.getResultTable().size() == 0)) {
            clauseAnswerList.push_back(ClauseAnswer(false, ResultTable()));
            break;
        }

        clauseAnswerList.push_back(ClauseAnswer(true, cumulativeRt));
    }

    if (selectionType == SelectionType::SINGLE || selectionType == SelectionType::TUPLE) {
        std::vector<DECLARATION> decs = query->getSelectionList();
        std::unordered_set<DECLARATION, dec_hash> synsToBeRetrieved;
        for (auto dec : decs) {
            if (syns.find(dec) == syns.end()) {
                synsToBeRetrieved.insert(dec);
            }
        }

        for (auto syn : synsToBeRetrieved) {
            DesignEntity selectionSynType = declarationTable.at(syn.getSynonym());
            ClauseAnswer selResults = ClauseAnswer(true, getAllOfDesignEntity(syn,
                selectionSynType, pkb));
            clauseAnswerList.push_back(selResults);
        }
    }

    if (selectionType == SelectionType::BOOLEAN) {
        return IntermediateAnswer({}, selectionType, clauseAnswerList);
    } else {
        return IntermediateAnswer(query->getSelectionList(), selectionType, clauseAnswerList);
    }
}

ClauseAnswer QueryEvaluator::evaluateClause(QueryClause& queryClause,
                                            DECLARATION_TABLE& declarationTable) {
    QueryArg leftArg = queryClause.getLeftArg();
    QueryArg rightArg = queryClause.getRightArg();

    ClauseType clType = queryClause.getClauseType();

    if (clType == ClauseType::SUCHTHAT_CL) {
        // Get Relationship of query clause
        Relationship relationship = queryClause.getRelationship();

        return suchThatEvaluator.evaluateSuchThatClause(relationship,
            leftArg, rightArg, declarationTable);
    } else if (clType == ClauseType::PATTERN_CL) {
        Declaration dec = Declaration(queryClause.getDesignEntitySynonym(),
            Attribute::STMT_NUM);
        return patternEvaluator.evaluatePatternClause(leftArg, rightArg,
            dec, declarationTable);
    } else if (clType == ClauseType::WITH_CL) {
        return withEvaluator.evaluateWithClause(leftArg, rightArg, declarationTable, pkb);
    }

    return ClauseAnswer();
}

ResultTable QueryEvaluator::getAllOfDesignEntity(Declaration selection,
                                                 DesignEntity designEntity,
                                                 PKB *pkb) {
    // For read, call, and print statements with two attrRef
    SYNONYM syn;
    Declaration decLeft, decRight;
    switch (designEntity) {
    case DesignEntity::READ:
        syn = selection.getSynonym();
        decLeft = Declaration(syn, ATTRIBUTE::STMT_NUM);
        decRight = Declaration(syn, ATTRIBUTE::VAR_NAME);
        return EvaluatorUtility::lineNumStrVecPairToResultTable(decLeft, decRight,
            pkb->getAllReadStatementsWithAttribute());
        break;
    case DesignEntity::PRINT:
        syn = selection.getSynonym();
        decLeft = Declaration(syn, ATTRIBUTE::STMT_NUM);
        decRight = Declaration(syn, ATTRIBUTE::VAR_NAME);
        return EvaluatorUtility::lineNumStrVecPairToResultTable(decLeft, decRight,
            pkb->getAllPrintStatementsWithAttribute());
        break;
    case DesignEntity::CALL:
        syn = selection.getSynonym();
        decLeft = Declaration(syn, ATTRIBUTE::STMT_NUM);
        decRight = Declaration(syn, ATTRIBUTE::PROC_NAME);
        return EvaluatorUtility::lineNumStrVecPairToResultTable(decLeft, decRight,
            pkb->getAllCallStatementsWithAttribute());
        break;
    case DesignEntity::VARIABLE:
        return EvaluatorUtility::strVecToResultTable(selection, pkb->getAllVariables());
        break;
    case DesignEntity::CONSTANT:
        return EvaluatorUtility::strVecToResultTable(selection, pkb->getAllConstants());
        break;
    case DesignEntity::PROCEDURE:
        return EvaluatorUtility::strVecToResultTable(selection, pkb->getAllProcedures());
        break;
    default:
        try {
            return EvaluatorUtility::lineNumVecToResultTable(selection,
                *pkb->selectDataStructure(designEntity));
        } catch (InvalidDesignEntity) {
            throw InvalidRelationshipException();
        }
    }
}
