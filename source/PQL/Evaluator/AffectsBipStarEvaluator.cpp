#include "AffectsBipStarEvaluator.h"

ClauseAnswer AffectsBipStarEvaluator::evaluateAffectsBipStar(QueryArg& leftArg,
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
            // AffectsBip*(1, _)
            return ClauseAnswer(pkb->hasAffectsBipLine(leftArg.getLineNum()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // AffectsBip*(1, ?)
            LINE_NUM leftLineNum = leftArg.getLineNum();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> linesAft = pkb->getAffectsBipStarLines(leftLineNum, rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, linesAft);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // AffectsBip*(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<LINE_NUM> parentLineNums = pkb->getAffectingBipStarLines(leftSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, parentLineNums);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // AffectsBip*(?, ?)
            Declaration leftDec = leftArg.getDeclaration();
            Declaration rightDec = rightArg.getDeclaration();

            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<std::pair<LINE_NUM, LINE_NUM>> lnPairs = pkb->getAffectsBipStarLinesPair(
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
            // AffectsBip*(?, 2)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            LINE_NUM rightNum = rightArg.getLineNum();

            std::vector<LINE_NUM> linesBef = pkb->getAffectingBipStarLines(leftSynType, rightNum);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, linesBef);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // AffectsBip*(_, _)
            return ClauseAnswer(pkb->hasAffectsBipStarRelationship(), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // AffectsBip*(_, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> childLineNums = pkb->getAffectsBipStarLines(rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, childLineNums);
        } else if (rightArgType == QueryArgType::NUM) {
            // AffectsBip*(_, 5)
            return ClauseAnswer(pkb->hasAffectingBipLine(rightArg.getLineNum()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}
