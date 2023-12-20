#include "UsesEvaluator.h"

#include <utility>
#include <string>
#include <vector>

ClauseAnswer UsesEvaluator::evaluateUses(QueryArg& leftArg,
                                         QueryArg& rightArg,
                                         DECLARATION_TABLE& dt) {
    // First argument can be LINE_NUM (int) or DECLARATION or NAME (procedure)
    // Second argument can be WILDCARD, or DECLARATION
    ResultTable resultTable;

    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if (leftArgType == QueryArgType::NUM) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Uses(1, _)
            return ClauseAnswer(pkb->doesLineNumUsesWildcard(leftArg.getLineNum()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Right synonym type is always a variable
            // Uses(1, v)
            LINE_NUM leftLineNum = leftArg.getLineNum();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();

            std::vector<VAR_NAME> variableNames = pkb->
                    getVariableFromUsesByLineNum(leftLineNum);
            resultTable = EvaluatorUtility::strVecToResultTable(rightDec, variableNames);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Uses(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<std::string> deNames = pkb->getUsesVector(leftSynType);
            resultTable = EvaluatorUtility::strVecToResultTable(leftDec, deNames);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Uses(?, ?)
            Declaration leftDec = leftArg.getDeclaration();
            Declaration rightDec = rightArg.getDeclaration();

            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<std::pair<std::string, VAR_NAME>> namePairs = pkb->
                    getUsesPairVector(leftSynType, rightSynType);
            resultTable = EvaluatorUtility::strVecPairToResultTable(leftDec, rightDec, namePairs);
        } else if (rightArgType == QueryArgType::NAME) {
            // Uses(?, "varName")
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            ARG_NAME rightName = rightArg.getName();

            std::vector<std::string> results = pkb->getUsesByVarName(leftSynType, rightName);
            resultTable = EvaluatorUtility::strVecToResultTable(leftDec, results);
        }
    } else if (leftArgType == QueryArgType::NAME) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Uses("main", _)
            return ClauseAnswer(pkb->
                    doesProcedureUsesWildcard(leftArg.getName()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Uses("main", v)
            PROC_NAME leftProcName = leftArg.getName();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();

            std::vector<VAR_NAME> variableNames = pkb->
                    getVariableFromUsesByProcedure(leftProcName);
            resultTable = EvaluatorUtility::strVecToResultTable(rightDec, variableNames);
        }
    }

    return ClauseAnswer(true, resultTable);
}
