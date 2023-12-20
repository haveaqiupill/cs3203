#include "ParentEvaluator.h"

#include <utility>
#include <vector>

#include "EvaluatorUtility.h"
#include "../../Commons/Exceptions.h"

ClauseAnswer ParentEvaluator::evaluateParent(QueryArg& leftArg,
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
            // Parent(1, _)
            return ClauseAnswer(pkb->isParent(leftArg.getLineNum()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Parent(1, ?)
            LINE_NUM leftLineNum = leftArg.getLineNum();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> childLineNums = pkb->getParentAfter(leftLineNum, rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, childLineNums);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Parent(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<LINE_NUM> parentLineNums = pkb->getParentBefore(leftSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, parentLineNums);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Parent(?, ?)
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
                std::vector<std::pair<LINE_NUM, LINE_NUM>> lnPairs = pkb->getParentPair(leftSynType,
                    rightSynType);
                resultTable = EvaluatorUtility::lineNumVecPairToResultTable(leftDec,
                    rightDec, lnPairs);
            }
        } else if (rightArgType == QueryArgType::NUM) {
            // Parent(?, 5)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            LINE_NUM rightLineNum = rightArg.getLineNum();

            std::vector<LINE_NUM> parentLineNums = pkb->getParentBefore(leftSynType, rightLineNum);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, parentLineNums);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Parent(_, _)
            return ClauseAnswer(pkb->hasParentRelationship(), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Parent(_, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> childLineNums = pkb->getParentAfter(rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, childLineNums);
        } else if (rightArgType == QueryArgType::NUM) {
            // Parent(_, 5)
            return ClauseAnswer(pkb->isChild(rightArg.getLineNum()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}
