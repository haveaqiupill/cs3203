#include <queue>
#include <unordered_set>

#include "NextStarEvaluator.h"

ClauseAnswer NextStarEvaluator::evaluateNextStar(QueryArg& leftArg,
    QueryArg& rightArg, DECLARATION_TABLE& dt) {
    // First argument can be WILDCARD, LINE_NUM (int) or DECLARATION
    // Second argument can be WILDCARD, LINE_NUM (int) or DECLARATION
    ResultTable resultTable;
    bool isClauseTrue = true;

    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if (leftArgType == QueryArgType::NUM) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Next*(1, _)
            // A line can only have a Next* line if it has a Next line
            return ClauseAnswer(pkb->hasNextLine(leftArg.getLineNum()), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Next*(1, ?)
            LINE_NUM leftLineNum = leftArg.getLineNum();
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> result = nextStarBreadthFirstSearch(leftLineNum, rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, result);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Next*(?, _)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);

            std::vector<LINE_NUM> result = pkb->getPrevLines(leftSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, result);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Next*(?, ?)
            Declaration leftDec = leftArg.getDeclaration();
            Declaration rightDec = rightArg.getDeclaration();

            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            bool sameSyn = leftSyn == rightSyn;

            auto leftDesignEntityVector = pkb->selectDataStructure(leftSynType);
            std::vector<std::pair<LINE_NUM, LINE_NUM>> lnPairs;
            std::vector<LINE_NUM> filtered;  // Use one-column only
            for (LINE_NUM lineBefore : *leftDesignEntityVector) {
                auto vec = nextStarBreadthFirstSearch(lineBefore, rightSynType);
                for (LINE_NUM lineAfter : vec) {
                    if (sameSyn) {
                        if (lineBefore == lineAfter) {
                            filtered.push_back(lineBefore);  // For similar synonym
                        }
                    } else {
                        lnPairs.push_back({ lineBefore, lineAfter });
                    }
                }
            }

            if (sameSyn) {
                resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, filtered);
            } else {
                resultTable = EvaluatorUtility::lineNumVecPairToResultTable(leftDec,
                    rightDec, lnPairs);
            }
        } else if (rightArgType == QueryArgType::NUM) {
            // Next*(?, 5)
            Declaration leftDec = leftArg.getDeclaration();
            SYNONYM leftSyn = leftDec.getSynonym();
            DesignEntity leftSynType = dt.at(leftSyn);
            LINE_NUM rightLineNum = rightArg.getLineNum();

            std::vector<LINE_NUM> result = nextStarBreadthFirstSearch(leftSynType, rightLineNum);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, result);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Next*(_, _)
            return ClauseAnswer(pkb->hasNextRelationship(), resultTable);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Next*(_, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> result = pkb->getNextLines(rightSynType);
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, result);
        } else if (rightArgType == QueryArgType::NUM) {
            // Next*(_, 5)
            return ClauseAnswer(pkb->hasPrevLine(rightArg.getLineNum()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}

bool NextStarEvaluator::doesLinesNextStar(LINE_NUM leftLine, LINE_NUM rightLine) {
    std::queue<LINE_NUM> frontier;
    std::unordered_set<LINE_NUM> explored;
    frontier.push(leftLine);

    while (!frontier.empty()) {
        LINE_NUM current = frontier.front();
        frontier.pop();
        if (!pkb->hasNextLine(current)) {
            continue;
        }
        std::vector<LINE_NUM> toSearch = (*pkb->getNextMapKeyLineBefore())[current];
        for (LINE_NUM lineNum : toSearch) {
            if (lineNum == rightLine) {
                return true;
            }
            if (explored.find(lineNum) == explored.end()) {
                frontier.push(lineNum);
                explored.insert(lineNum);
            }
        }
    }
    return false;
}

std::vector<LINE_NUM> NextStarEvaluator::nextStarBreadthFirstSearch
    (LINE_NUM start, DESIGN_ENTITY designEntity) {
    std::queue<LINE_NUM> frontier;
    std::unordered_set<LINE_NUM> explored;
    std::vector<LINE_NUM> result;

    frontier.push(start);
    auto designEntitySet = pkb->selectDataSet(designEntity);

    while (!frontier.empty()) {
        LINE_NUM current = frontier.front();
        frontier.pop();

        if (!pkb->hasNextLine(current)) {
            continue;
        }

        auto toSearch = (*pkb->getNextMapKeyLineBefore())[current];
        for (auto& lineNum : toSearch) {
            if (explored.find(lineNum) == explored.end()) {
                if (designEntitySet->find(lineNum) != designEntitySet->end()) {
                    result.push_back(lineNum);
                }
                frontier.push(lineNum);
                explored.insert(lineNum);
            }
        }
    }
    return result;
}

std::vector<LINE_NUM> NextStarEvaluator::nextStarBreadthFirstSearch
    (DESIGN_ENTITY designEntity, LINE_NUM end) {
    std::queue<LINE_NUM> frontier;
    std::unordered_set<LINE_NUM> explored;
    std::vector<LINE_NUM> result;

    frontier.push(end);
    auto designEntitySet = pkb->selectDataSet(designEntity);

    while (!frontier.empty()) {
        LINE_NUM current = frontier.front();
        frontier.pop();

        if (!pkb->hasPrevLine(current)) {
            continue;
        }

        auto toSearch = (*pkb->getNextMapKeyLineAfter())[current];
        for (auto& lineNum : toSearch) {
            if (explored.find(lineNum) == explored.end()) {
                if (designEntitySet->find(lineNum) != designEntitySet->end()) {
                    result.push_back(lineNum);
                }
                frontier.push(lineNum);
                explored.insert(lineNum);
            }
        }
    }
    return result;
}
