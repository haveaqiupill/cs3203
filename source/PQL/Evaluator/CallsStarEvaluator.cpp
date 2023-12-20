#include "CallsStarEvaluator.h"

ClauseAnswer CallsStarEvaluator::evaluateCallsStar(QueryArg& leftArg,
                                                   QueryArg& rightArg,
                                                   DECLARATION_TABLE& dt) {
    // First argument can be WILDCARD, PROCEDURE (string) or DECLARATION
    // Second argument can be WILDCARD, PROCEDURE (string) or DECLARATION
    ResultTable resultTable;
    bool isClauseTrue = true;

    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if (leftArgType == QueryArgType::NAME) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Calls*("First", _)
            return ClauseAnswer(pkb->hasCallees(leftArg.getName()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Calls*("First", ?)
            PROC_NAME leftName = leftArg.getName();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<PROC_NAME> callees = pkb->getCalleesStar(leftName);
            resultTable = EvaluatorUtility::strVecToResultTable(rightDec, callees);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Calls*(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<PROC_NAME> callers = pkb->getAllCallers();
            resultTable = EvaluatorUtility::strVecToResultTable(leftDec, callers);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Calls*(?, ?)
            Declaration leftDec = leftArg.getDeclaration();
            Declaration rightDec = rightArg.getDeclaration();

            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            if (leftSyn == rightSyn) {
                resultTable = ResultTable();
                isClauseTrue = false;
            } else {
                std::vector<std::pair<PROC_NAME, PROC_NAME>> procPairs = pkb->getCallsStarPair(
                    leftSynType, rightSynType);
                resultTable = EvaluatorUtility::strVecPairToResultTable(leftDec,
                    rightDec, procPairs);
            }
        } else if (rightArgType == QueryArgType::NAME) {
            // Calls*(?, "First")
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            PROC_NAME rightName = rightArg.getName();

            std::vector<PROC_NAME> callers = pkb->getCallersStar(rightName);
            resultTable = EvaluatorUtility::strVecToResultTable(leftDec, callers);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Calls*(_, _)
            return ClauseAnswer(pkb->hasCallsRelationship(), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Calls*(_, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<PROC_NAME> callees = pkb->getAllCallees();
            resultTable = EvaluatorUtility::strVecToResultTable(rightDec, callees);
        } else if (rightArgType == QueryArgType::NAME) {
            // Calls*(_, "First")
            return ClauseAnswer(pkb->hasCallers(rightArg.getName()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}
