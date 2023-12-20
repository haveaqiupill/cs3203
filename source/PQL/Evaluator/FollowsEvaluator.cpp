#include "FollowsEvaluator.h"

#include "EvaluatorUtility.h"

ClauseAnswer FollowsEvaluator::evaluateFollows(QueryArg& leftArg,
                                               QueryArg& rightArg,
                                               DECLARATION_TABLE& dt) {
    // First argument can be WILDCARD, LINE_NUM (int) or DECLARATION
    // Second argument can be WILDCARD, LINE_NUM (int) or DECLARATION
    ResultTable resultTable;
    bool isClauseTrue = true;

    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if (leftArgType == QueryArgType::NUM) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Follows(1, _)
            return ClauseAnswer(pkb->hasFollower(leftArg.getLineNum()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Follows(1, ?)
            LINE_NUM leftLineNum = leftArg.getLineNum();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> childLineNums = pkb->getFollowsAfter(leftLineNum, rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, childLineNums);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Follows(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<LINE_NUM> parentLineNums = pkb->getFollowsBefore(leftSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, parentLineNums);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Follows(?, ?)
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
                std::vector<std::pair<LINE_NUM, LINE_NUM>> lnPairs = pkb->getFollowsPair(
                    leftSynType, rightSynType);
                resultTable = EvaluatorUtility::lineNumVecPairToResultTable(leftDec,
                    rightDec, lnPairs);
            }
        } else if (rightArgType == QueryArgType::NUM) {
            // Follows(?, 5)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            LINE_NUM rightLineNum = rightArg.getLineNum();

            std::vector<LINE_NUM> parentLineNums = pkb->getFollowsBefore(leftSynType, rightLineNum);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, parentLineNums);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Follows(_, _)
            return ClauseAnswer(pkb->hasFollowsRelationship(), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Follows(_, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> childLineNums = pkb->getFollowsAfter(rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, childLineNums);
        } else if (rightArgType == QueryArgType::NUM) {
            // Follows(_, 5)
            return ClauseAnswer(pkb->hasFollowee(rightArg.getLineNum()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}
