#include "ModifiesEvaluator.h"

#include <utility>
#include <string>
#include <vector>

ClauseAnswer ModifiesEvaluator::evaluateModifies(QueryArg& leftArg,
                                                 QueryArg& rightArg,
                                                 DECLARATION_TABLE& dt) {
    // First argument can be LINE_NUM (int) or DECLARATION or NAME (procedure)
    // Second argument can be WILDCARD, or DECLARATION
    ResultTable resultTable;

    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if (leftArgType == QueryArgType::NUM) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Modifies(1, _)
            return ClauseAnswer(pkb->doesLineNumModifiesWildcard(leftArg.getLineNum()),
                                                                 resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Right synonym type is always a variable
            // Modifies(1, v)
            LINE_NUM leftLineNum = leftArg.getLineNum();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();

            std::vector<VAR_NAME> variableNames = pkb->
                    getVariableFromModifiesByLineNum(leftLineNum);
            resultTable = EvaluatorUtility::strVecToResultTable(rightDec, variableNames);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Modifies(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<std::string> deNames = pkb->getModifiesVector(leftSynType);
            resultTable = EvaluatorUtility::strVecToResultTable(leftDec, deNames);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Modifies(?, ?)
            Declaration leftDec = leftArg.getDeclaration();
            Declaration rightDec = rightArg.getDeclaration();

            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<std::pair<std::string, VAR_NAME>> namePairs = pkb->
                    getModifiesPairVector(leftSynType, rightSynType);
            resultTable = EvaluatorUtility::strVecPairToResultTable(leftDec, rightDec, namePairs);
        } else if (rightArgType == QueryArgType::NAME) {
            // Modifies(?, "varName")
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            ARG_NAME rightName = rightArg.getName();

            std::vector<std::string> results = pkb->getModifiesByVarName(leftSynType, rightName);
            resultTable = EvaluatorUtility::strVecToResultTable(leftDec, results);
        }
    } else if (leftArgType == QueryArgType::NAME) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Modifies("main", _)
            return ClauseAnswer(pkb->
            doesProcedureModifiesWildcard(leftArg.getName()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Modifies("main", v)
            PROC_NAME leftProcName = leftArg.getName();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();

            std::vector<VAR_NAME> variableNames = pkb->
                    getVariableFromModifiesByProcedure(leftProcName);
            resultTable = EvaluatorUtility::strVecToResultTable(rightDec, variableNames);
        }
    }

    return ClauseAnswer(true, resultTable);
}
