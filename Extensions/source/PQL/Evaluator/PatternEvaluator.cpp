#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

#include "PatternEvaluator.h"
#include "../QueryEvaluator.h"

ClauseAnswer PatternEvaluator::evaluatePatternClause(QueryArg leftArg,
                                                     QueryArg rightArg,
                                                     Declaration patternSyn,
                                                     DECLARATION_TABLE declarationTable) {
    DesignEntity patternDe = declarationTable.at(patternSyn.getSynonym());

    if (patternDe == DesignEntity::IF) {
        return evaluatePatternIf(leftArg, patternSyn);
    } else if (patternDe == DesignEntity::WHILE) {
        return evaluatePatternWhile(leftArg, patternSyn);
    } else {
        return evaluatePatternAssign(leftArg, rightArg, patternSyn);
    }
}

ClauseAnswer PatternEvaluator::evaluatePatternIf(QueryArg leftArg,
                                                 Declaration patternSyn) {
    // pattern if(leftArg, _, _)
    ARG_TYPE leftArgType = leftArg.getArgType();
    ResultTable resultTable;

    if (leftArgType == QueryArgType::NAME) {
        // pattern if("x1", _, _)

        std::vector<LINE_NUM> result =
                            pkb->getAllIfLineNumByVariable(leftArg.getName());

        resultTable = EvaluatorUtility::lineNumVecToResultTable(patternSyn, result);
    } else if (leftArgType == QueryArgType::DECLARATION) {
        // pattern if(v, _, _)

        std::vector<std::pair<LINE_NUM, VAR_NAME>> result = pkb->getAllIfLineNumByVariable();

        resultTable = EvaluatorUtility::lineNumStrVecPairToResultTable(patternSyn,
                                                                       leftArg.getDeclaration(),
                                                                       result);
    } else if (leftArgType == QueryArgType::WILDCARD) {
        // pattern if(_, _, _)
        std::vector<LINE_NUM> result = pkb->getAllIfStatements();
        resultTable = EvaluatorUtility::lineNumVecToResultTable(patternSyn, result);
    }

    if (resultTable.getResultTable().empty()) {
        return ClauseAnswer(false, resultTable);
    } else {
        return ClauseAnswer(true, resultTable);
    }
}

ClauseAnswer PatternEvaluator::evaluatePatternWhile(QueryArg leftArg, Declaration patternSyn) {
    // pattern while(leftArg, _)
    ARG_TYPE leftArgType = leftArg.getArgType();
    ResultTable resultTable;

    if (leftArgType == QueryArgType::NAME) {
        // pattern while("x1", _)

        std::vector<LINE_NUM> result =
                             pkb->getAllWhileLineNumByVariable(leftArg.getName());

        resultTable = EvaluatorUtility::lineNumVecToResultTable(patternSyn, result);
    } else if (leftArgType == QueryArgType::DECLARATION) {
        // pattern while(v, _)

        std::vector<std::pair<LINE_NUM, VAR_NAME>> result = pkb->getAllWhileLineNumByVariable();

        resultTable = EvaluatorUtility::lineNumStrVecPairToResultTable(patternSyn,
                                                                       leftArg.getDeclaration(),
                                                                       result);
    } else if (leftArgType == QueryArgType::WILDCARD) {
        // pattern while(_, _)
        std::vector<LINE_NUM> result = pkb->getAllWhileStatements();
        resultTable = EvaluatorUtility::lineNumVecToResultTable(patternSyn, result);
    }

    if (resultTable.getResultTable().empty()) {
        return ClauseAnswer(false, resultTable);
    } else {
        return ClauseAnswer(true, resultTable);
    }
}

ClauseAnswer PatternEvaluator::evaluatePatternAssign(QueryArg leftArg,
                                                       QueryArg rightArg,
                                                       Declaration assignSyn) {
    // pattern a(leftArg, rightArg)
    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();
    ResultTable resultTable;
    PATTERN_EXPR pattern = rightArg.getPatternExpr();

    if (rightArgType == QueryArgType::PATTERN_EXPR) {
        // pattern a(_, "x")
        if (leftArgType == QueryArgType::WILDCARD) {
            // pattern a(_, "x")
            std::vector<std::pair<LINE_NUM, VAR_NAME>> result =
                pkb->getAssignLineNumVariableByPrefixExpr(pattern);

            std::vector<LINE_NUM> lineNums;
            for each (std::pair<LINE_NUM, VAR_NAME> p in result) {
                lineNums.push_back(p.first);
            }

            resultTable = EvaluatorUtility::lineNumVecToResultTable(assignSyn, lineNums);
        } else if (leftArgType == QueryArgType::DECLARATION) {
            // pattern a(v, "x")
            std::vector<std::pair<LINE_NUM, VAR_NAME>> result =
                pkb->getAssignLineNumVariableByPrefixExpr(pattern);

            resultTable = EvaluatorUtility::lineNumStrVecPairToResultTable(assignSyn,
                leftArg.getDeclaration(), result);
        } else if (leftArgType == QueryArgType::NAME) {
            // pattern a("y", "x")
            std::vector<LINE_NUM> lineNums =
                pkb->getAssignLineNumByVarPrefixExpr(leftArg.getName(), pattern);

            resultTable = EvaluatorUtility::lineNumVecToResultTable(assignSyn, lineNums);
        }
    } else if (rightArgType == QueryArgType::PATTERN_EXPR_WITH_WILDCARDS) {
        // pattern a(_, _"x"_)
        if (leftArgType == QueryArgType::WILDCARD) {
            // pattern a(_, _"x"_)
            std::vector<std::pair<LINE_NUM, VAR_NAME>> result =
                pkb->getAssignLineNumVariableByPartialPrefixExpr(pattern);

            std::vector<LINE_NUM> lineNums;
            for each (std::pair<LINE_NUM, VAR_NAME> p in result) {
                lineNums.push_back(p.first);
            }

            resultTable = EvaluatorUtility::lineNumVecToResultTable(assignSyn, lineNums);
        } else if (leftArgType == QueryArgType::DECLARATION) {
            // pattern a(v, _"x"_)
            std::vector<std::pair<LINE_NUM, VAR_NAME>> result =
                pkb->getAssignLineNumVariableByPartialPrefixExpr(pattern);

            resultTable = EvaluatorUtility::lineNumStrVecPairToResultTable(assignSyn,
                leftArg.getDeclaration(), result);
        } else if (leftArgType == QueryArgType::NAME) {
            // pattern a("y", _"x"_)
            std::vector<LINE_NUM> lineNums =
                pkb->getAssignLineNumByVarPartialPrefixExpr(leftArg.getName(), pattern);

            resultTable = EvaluatorUtility::lineNumVecToResultTable(assignSyn, lineNums);
        }
    } else if (rightArgType == QueryArgType::WILDCARD) {
        if (leftArgType == QueryArgType::WILDCARD) {
            // pattern a(_, _)
            std::vector<LINE_NUM> result = pkb->getAllAssignStatements();

            resultTable = EvaluatorUtility::lineNumVecToResultTable(assignSyn, result);

        } else if (leftArgType == QueryArgType::DECLARATION) {
            // pattern a(v, _)
            std::vector<std::pair<LINE_NUM, VAR_NAME>> result = pkb->
                getAllAssignLineNumVariable();

            resultTable = EvaluatorUtility::lineNumStrVecPairToResultTable(assignSyn,
                leftArg.getDeclaration(), result);
        } else if (leftArgType == QueryArgType::NAME) {
            // pattern a("y", _)
            std::vector<LINE_NUM> result =
                pkb->getAssignLineNumByVariable(leftArg.getName());

            resultTable = EvaluatorUtility::lineNumVecToResultTable(assignSyn, result);
        }
    }

    if (resultTable.getResultTable().empty()) {
        return ClauseAnswer(false, resultTable);
    } else {
        return ClauseAnswer(true, resultTable);
    }
}
