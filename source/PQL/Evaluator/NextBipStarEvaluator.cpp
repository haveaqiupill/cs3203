#include "NextBipStarEvaluator.h"

ClauseAnswer NextBipStarEvaluator::evaluateNextBipStar(QueryArg& leftArg,
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
            // NextBip*(1, _)
            return ClauseAnswer(pkb->hasNextBipLine(leftArg.getLineNum()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // NextBip*(1, ?)
            LINE_NUM leftLineNum = leftArg.getLineNum();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> linesAft = pkb->getNextBipStarLines(leftLineNum, rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, linesAft);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // NextBip*(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<LINE_NUM> parentLineNums = pkb->getPrevBipStarLines(leftSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, parentLineNums);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // NextBip*(?, ?)
            Declaration leftDec = leftArg.getDeclaration();
            Declaration rightDec = rightArg.getDeclaration();

            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<std::pair<LINE_NUM, LINE_NUM>> lnPairs = pkb->getNextBipStarLinesPair(
                leftSynType, rightSynType);

            if (leftSyn == rightSyn) {
                std::vector<LINE_NUM> filtered;  // One column only
                for (auto pair : lnPairs) {
                    if (pair.first == pair.second) {
                        filtered.push_back(pair.first);
                    }
                }
                // Left and right synonyms are the same
                resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, filtered);
            } else {
                resultTable = EvaluatorUtility::lineNumVecPairToResultTable(leftDec,
                    rightDec, lnPairs);
            }
        } else if (rightArgType == QueryArgType::NUM) {
            // NextBip*(?, 5)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            LINE_NUM rightLineNum = rightArg.getLineNum();

            std::vector<LINE_NUM> linesBef = pkb->getPrevBipStarLines(leftSynType, rightLineNum);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, linesBef);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // NextBip*(_, _)
            return ClauseAnswer(pkb->hasNextBipStarRelationship(), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // NextBip*(_, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> childLineNums = pkb->getNextBipStarLines(rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, childLineNums);
        } else if (rightArgType == QueryArgType::NUM) {
            // NextBip*(_, 5)
            return ClauseAnswer(pkb->hasPrevBipLine(rightArg.getLineNum()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}
