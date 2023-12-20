#include <stack>
#include <utility>
#include <unordered_set>

#include "AffectsStarEvaluator.h"

ClauseAnswer AffectsStarEvaluator::evaluateAffectsStar(QueryArg& leftArg, QueryArg& rightArg,
    DECLARATION_TABLE& dt) {
    if (!initialized) {
        initializeDataStructures();
    }

    ResultTable resultTable;
    bool isClauseTrue = true;

    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if (leftArgType == QueryArgType::NUM) {
        LINE_NUM leftLineNum = leftArg.getLineNum();
        if (rightArgType == QueryArgType::WILDCARD) {
            // Affects*(1, _)
            if (forwardCache.find(leftLineNum) != forwardCache.end()) {
                return ClauseAnswer(true, resultTable);
            }
            return ClauseAnswer(doesLineNumAffects(leftLineNum), resultTable);

        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Affects*(1, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            if (assignSet->find(leftLineNum) == assignSet->end()) {
                return ClauseAnswer(false, resultTable);
            }
            std::vector<LINE_NUM> result;
            if (forwardCache.find(leftLineNum) != forwardCache.end()) {
                result = forwardCache[leftLineNum];
            } else {
                result = affectsStarDfsLineBefore(leftLineNum);
                forwardCache.insert({ leftLineNum, result });
            }
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, result);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        Declaration leftDec = leftArg.getDeclaration();
        SYNONYM leftSyn = leftDec.getSynonym();
        DesignEntity leftSynType = dt.at(leftSyn);

        if (rightArgType == QueryArgType::WILDCARD) {
            // Affects*(?, _)
            std::vector<LINE_NUM> result;
            for (LINE_NUM lineBefore : *assignList) {
                if (doesLineNumAffects(lineBefore)) {
                    result.push_back(lineBefore);
                }
            }
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, result);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Affects*(?, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            bool sameSyn = leftSyn == rightSyn;
            std::vector<std::pair<LINE_NUM, LINE_NUM>> lnPairs;
            std::vector<LINE_NUM> filtered;  // Use one column only
            for (LINE_NUM lineBefore : *assignList) {
                std::vector<LINE_NUM> toSearch;
                if (forwardCache.find(lineBefore) != forwardCache.end()) {
                    toSearch = forwardCache[lineBefore];
                } else {
                    toSearch = affectsStarDfsLineBefore(lineBefore);
                    forwardCache.insert({ lineBefore, toSearch });
                }
                for (LINE_NUM lineAfter : toSearch) {
                    if (sameSyn) {
                        if (lineBefore == lineAfter) {
                            filtered.push_back(lineBefore);  // For similar synonym
                        }
                    } else {
                        lnPairs.push_back({ lineBefore, lineAfter });
                    }
                    pairCache.insert({ lineBefore, lineAfter });
                }
            }

            if (sameSyn) {
                resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, filtered);
            } else {
                resultTable = EvaluatorUtility::lineNumVecPairToResultTable(leftDec,
                    rightDec, lnPairs);
            }
        } else if (rightArgType == QueryArgType::NUM) {
            // Affects*(?, 5)
            LINE_NUM rightLineNum = rightArg.getLineNum();
            if (assignSet->find(rightLineNum) == assignSet->end()) {
                // Wrong design entity
                return ClauseAnswer(false, resultTable);
            }
            std::vector<LINE_NUM> result;
            if (reverseCache.find(rightLineNum) != reverseCache.end()) {
                result = reverseCache[rightLineNum];
            } else {
                result = affectsStarDfsLineAfter(rightLineNum);
                reverseCache.insert({ rightLineNum, result });
            }
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, result);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Affects*(_, _)
            bool found = false;
            for (LINE_NUM lineBefore : *assignList) {
                if (doesLineNumAffects(lineBefore)) {
                    found = true;
                    break;
                }
            }
            isClauseTrue = found;
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Affects*(_, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            std::vector<LINE_NUM> result;
            for (LINE_NUM lineAfter : *assignList) {
                if (isLineNumAffected(lineAfter)) {
                    result.push_back(lineAfter);
                }
            }

            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, result);
        } else if (rightArgType == QueryArgType::NUM) {
            // Affects*(_, 5)
            return ClauseAnswer(isLineNumAffected(rightArg.getLineNum()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}

bool AffectsStarEvaluator::doLinesAffectsStar(LINE_NUM leftLine, LINE_NUM rightLine) {
    if (!initialized) {
        initializeDataStructures();
    }

    if (pairCache.find({ leftLine, rightLine }) != pairCache.end()) {
        return true;
    }

    if (assignSet->find(leftLine) == assignSet->end() ||
        assignSet->find(rightLine) == assignSet->end()) {
        return false;
    }

    std::stack<std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>> dfsStack;
    std::unordered_set<std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>, pair_hash_set> explored;

    VAR_NAME modifiedVar = pkb->getVariableFromModifiesByLineNum(leftLine)[0];
    dfsStack.push({ leftLine, { modifiedVar } });

    while (!dfsStack.empty()) {
        auto pair = dfsStack.top();
        dfsStack.pop();

        LINE_NUM current = pair.first;

        if (!pkb->hasNextLine(current)) {
            continue;
        }

        std::vector<LINE_NUM> nextLines = (*nextMapKeyLineBefore)[current];

        for (LINE_NUM nextLine : nextLines) {
            std::unordered_set<VAR_NAME> varSet = pair.second;
            if (assignSet->find(nextLine) != assignSet->end()) {
                std::vector<VAR_NAME> usesVars = pkb->getVariableFromUsesByLineNum(nextLine);
                VAR_NAME modifiedVar = pkb->getVariableFromModifiesByLineNum(nextLine)[0];
                bool isRightLine = nextLine == rightLine;
                bool uses = false;
                for (VAR_NAME usesVar : usesVars) {
                    if (varSet.find(usesVar) != varSet.end()) {
                        if (isRightLine) {
                            return true;
                        }
                        uses = true;
                        varSet.insert(modifiedVar);
                        break;
                    }
                }
                if (!uses && varSet.find(modifiedVar) != varSet.end()) {
                    varSet.erase(modifiedVar);
                }
            } else if (readSet->find(nextLine) != readSet->end() ||
                callSet->find(nextLine) != callSet->end()) {
                std::vector<VAR_NAME> modifiedVars = pkb->
                    getVariableFromModifiesByLineNum(nextLine);
                for (VAR_NAME modifiedVar : modifiedVars) {
                    varSet.erase(modifiedVar);
                }
            }

            if (varSet.empty()) {
                // no more var to link them
                continue;
            }

            std::pair<LINE_NUM, std::unordered_set<VAR_NAME>> newPair{ nextLine, varSet };
            if (explored.insert(newPair).second) {
                dfsStack.push(newPair);
            }
        }
    }
    return false;
}

std::vector<LINE_NUM> AffectsStarEvaluator::affectsStarDfsLineBefore(LINE_NUM lineBefore) {
    std::unordered_set<LINE_NUM> result;

    if (assignSet->find(lineBefore) == assignSet->end()) {
        // not an assignment statement
        return std::vector<LINE_NUM>();
    }

    std::stack<std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>> dfsStack;
    std::unordered_set <std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>, pair_hash_set> explored;

    // an assignment statement will only have one variable that it modifies
    VAR_NAME startVar = pkb->getVariableFromModifiesByLineNum(lineBefore)[0];
    dfsStack.push({ lineBefore, std::unordered_set<VAR_NAME> {startVar} });

    while (!dfsStack.empty()) {
        auto pair = dfsStack.top();
        dfsStack.pop();
        LINE_NUM current = pair.first;

        if (!pkb->hasNextLine(current))
            continue;

        std::vector<LINE_NUM> nextLines = (*nextMapKeyLineBefore)[current];
        for (LINE_NUM lineAfter : nextLines) {
            std::unordered_set<VAR_NAME> varSet = pair.second;
            if (assignSet->find(lineAfter) != assignSet->end()) {
                // only other assign statements can be a transiting line
                bool uses = false;
                std::vector<VAR_NAME> usesVars = pkb->getVariableFromUsesByLineNum(lineAfter);
                VAR_NAME newVar = pkb->getVariableFromModifiesByLineNum(lineAfter)[0];
                for (VAR_NAME var : usesVars) {
                    if (varSet.find(var) != varSet.end()) {
                        // var is within one of the variable that is affected by the first line
                        varSet.insert(newVar);
                        result.insert(lineAfter);
                        uses = true;
                        break;
                    }
                }

                if (!uses && (varSet.find(newVar) != varSet.end())) {
                    // var does not use any of the variables and modifies a variable
                    varSet.erase(newVar);
                }
            } else if (callSet->find(lineAfter) != callSet->end() ||
                readSet->find(lineAfter) != readSet->end()) {
                std::vector<VAR_NAME> modifiedVar = pkb->
                    getVariableFromModifiesByLineNum(lineAfter);
                for (VAR_NAME var : modifiedVar)
                    varSet.erase(var);
            }

            if (varSet.empty())
                continue;

            std::pair<LINE_NUM, std::unordered_set<VAR_NAME>> newPair{ lineAfter, varSet };
            if (explored.insert(newPair).second) {
                dfsStack.push(newPair);
            }
        }
    }

    return std::vector<LINE_NUM> (result.begin(), result.end());
}

std::vector<LINE_NUM> AffectsStarEvaluator::affectsStarDfsLineAfter(LINE_NUM lineAfter) {
    std::unordered_set<LINE_NUM> result;
    if (assignSet->find(lineAfter) == assignSet->end()) {
        return std::vector<LINE_NUM>();
    }

    std::stack<std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>> dfsStack;
    std::unordered_set <std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>, pair_hash_set> explored;

    std::vector<VAR_NAME> usesVar = pkb->getVariableFromUsesByLineNum(lineAfter);
    std::unordered_set<VAR_NAME> usesSet(usesVar.begin(), usesVar.end());

    dfsStack.push({ lineAfter, usesSet });
    while (!dfsStack.empty()) {
        auto pair = dfsStack.top();
        dfsStack.pop();
        LINE_NUM current = pair.first;

        if (!pkb->hasPrevLine(current))
            continue;

        std::vector<LINE_NUM> prevLines = (*nextMapKeyLineAfter)[current];
        for (LINE_NUM lineBefore : prevLines) {
            std::unordered_set<VAR_NAME> varSet = pair.second;
            if (readSet->find(lineBefore) != readSet->end() ||
                callSet->find(lineBefore) != callSet->end()) {
                auto modifiesVar = pkb->getVariableFromModifiesByLineNum(lineBefore);
                for (VAR_NAME var : modifiesVar) {
                    varSet.erase(var);
                }
            } else if (assignSet->find(lineBefore) != assignSet->end()) {
                VAR_NAME modifiedVar = pkb->getVariableFromModifiesByLineNum(lineBefore)[0];
                if (varSet.find(modifiedVar) != varSet.end()) {
                    // line modifies variable that is used in the statement, directly or indirectly
                    result.insert(lineBefore);
                    // remove the var as it is modified already
                    varSet.erase(modifiedVar);
                    std::vector<VAR_NAME> usedVar = pkb->getVariableFromUsesByLineNum(lineBefore);
                    for (VAR_NAME var : usedVar) {
                        varSet.insert(var);
                    }
                }
            }
            if (varSet.empty())
                continue;

            std::pair<LINE_NUM, std::unordered_set<VAR_NAME>> newPair{ lineBefore, varSet };
            if (explored.insert(newPair).second) {
                // successfully inserted means that it was not explored before
                dfsStack.push(newPair);
            }
        }
    }
    return std::vector<LINE_NUM>(result.begin(), result.end());
}
