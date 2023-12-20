#include <queue>
#include <stack>

#include "AffectsEvaluator.h"
#include "../../Commons/PairHash.h"

ClauseAnswer AffectsEvaluator::evaluateAffects(QueryArg& leftArg,
    QueryArg& rightArg, DECLARATION_TABLE& dt) {
    // First argument can be WILDCARD, LINE_NUM (int) or DECLARATION
    // Second argument can be WILDCARD, LINE_NUM (int) or DECLARATION
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
            // Affects(1, _)
            if (forwardCache.find(leftLineNum) != forwardCache.end()) {
                return ClauseAnswer(true, resultTable);
            }
            return ClauseAnswer(doesLineNumAffects(leftLineNum), resultTable);

        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Affects(1, ?)
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
                result = affectsBfsLineBefore(leftLineNum);
                forwardCache.insert({ leftLineNum, result });
            }
            resultTable = EvaluatorUtility::lineNumVecToResultTable(rightDec, result);
        }
    } else if (leftArgType == QueryArgType::DECLARATION) {
        Declaration leftDec = leftArg.getDeclaration();
        SYNONYM leftSyn = leftDec.getSynonym();
        DesignEntity leftSynType = dt.at(leftSyn);

        if (rightArgType == QueryArgType::WILDCARD) {
            // Affects(?, _)
            std::vector<LINE_NUM> result;
            for (LINE_NUM lineBefore : *assignList) {
                if (forwardCache.find(lineBefore) != forwardCache.end()) {
                    result.push_back(lineBefore);
                } else {
                    if (doesLineNumAffects(lineBefore)) {
                        result.push_back(lineBefore);
                    }
                }
            }
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, result);
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Affects(?, ?)
            Declaration rightDec = rightArg.getDeclaration();
            SYNONYM rightSyn = rightDec.getSynonym();
            DesignEntity rightSynType = dt.at(rightSyn);

            bool sameSyn = leftSyn == rightSyn;
            std::vector<std::pair<LINE_NUM, LINE_NUM>> lnPairs;
            std::vector<LINE_NUM> filtered;  // Use one column only
            for (LINE_NUM lineBefore : *assignList) {
                std::vector<LINE_NUM> linesAfter;
                if (forwardCache.find(lineBefore) != forwardCache.end()) {
                    linesAfter = forwardCache[lineBefore];
                } else {
                    linesAfter = affectsBfsLineBefore(lineBefore);
                    forwardCache.insert({ lineBefore, linesAfter });
                }

                for (LINE_NUM lineAfter : linesAfter) {
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
            // Affects(?, 5)
            LINE_NUM rightLineNum = rightArg.getLineNum();
            if (assignSet->find(rightLineNum) == assignSet->end()) {
                // Wrong design entity
                return ClauseAnswer(false, resultTable);
            }
            std::vector<LINE_NUM> result;
            if (reverseCache.find(rightLineNum) != reverseCache.end()) {
                result = reverseCache[rightLineNum];
            } else {
                result = affectsDfsLineAfter(rightLineNum);
                reverseCache.insert({ rightLineNum, result });
            }
            resultTable = EvaluatorUtility::lineNumVecToResultTable(leftDec, result);
        }
    } else if (leftArgType == QueryArgType::WILDCARD) {
        if (rightArgType == QueryArgType::WILDCARD) {
            // Affects(_, _)
            bool found = false;
            if (!forwardCache.empty()) {
                found = true;
            } else {
                for (LINE_NUM lineBefore : *assignList) {
                    if (doesLineNumAffects(lineBefore)) {
                        found = true;
                        break;
                    }
                }
            }
            isClauseTrue = found;
        } else if (rightArgType == QueryArgType::DECLARATION) {
            // Affects(_, ?)
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
            // Affects(_, 5)
            return ClauseAnswer(isLineNumAffected(rightArg.getLineNum()), resultTable);
        }
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}

bool AffectsEvaluator::doLinesAffects(LINE_NUM leftLine, LINE_NUM rightLine) {
    if (!initialized) {
        initializeDataStructures();
    }

    if (pairCache.find({ leftLine, rightLine }) != pairCache.end()) {
        return true;
    }

    if (assignSet->find(leftLine) == assignSet->end() ||
        assignSet->find(rightLine) == assignSet->end()) {
        // either of the lines are not assignment
        return false;
    }
    std::queue<LINE_NUM> frontier;
    std::unordered_set<LINE_NUM> explored;

    frontier.push(leftLine);
    VAR_NAME variable = pkb->getVariableFromModifiesByLineNum(leftLine)[0];

    while (!frontier.empty()) {
        LINE_NUM current = frontier.front();
        frontier.pop();

        if (!pkb->hasNextLine(current))
            continue;

        auto toSearch = pkb->getNextMapKeyLineBefore()->at(current);
        for (LINE_NUM lineAfter : toSearch) {
            if (lineAfter == rightLine) {
                return pkb->doesLineNumUses(lineAfter, variable);
            }

            if (explored.find(lineAfter) == explored.end()) {
                if (!pkb->doesLineNumModifies(lineAfter, variable) ||
                    (readSet->find(lineAfter) == readSet->end() &&
                        callSet->find(lineAfter) == callSet->end() &&
                        assignSet->find(lineAfter) == assignSet->end())) {
                    frontier.push(lineAfter);
                    explored.insert(lineAfter);
                }
            }
        }
    }
    return false;
}

void AffectsEvaluator::initializeDataStructures() {
    assignSet = pkb->selectDataSet(DesignEntity::ASSIGN);
    readSet = pkb->selectDataSet(DesignEntity::READ);
    callSet = pkb->selectDataSet(DesignEntity::CALL);
    assignList = pkb->selectDataStructure(DesignEntity::ASSIGN);
    nextMapKeyLineAfter = pkb->getNextMapKeyLineAfter();
    nextMapKeyLineBefore = pkb->getNextMapKeyLineBefore();
    initialized = true;
}

bool AffectsEvaluator::doesLineNumAffects(LINE_NUM lineBefore) {
    if (assignSet->find(lineBefore) == assignSet->end()) {
        return false;
    }
    // one statement can only modifies one variable
    VAR_NAME variable = pkb->getVariableFromModifiesByLineNum(lineBefore)[0];

    std::queue<LINE_NUM> frontier;
    std::unordered_set<LINE_NUM> explored;

    frontier.push(lineBefore);
    while (!frontier.empty()) {
        LINE_NUM current = frontier.front();
        frontier.pop();

        if (!pkb->hasNextLine(current)) {
            continue;
        }

        auto toSearch = (*nextMapKeyLineBefore)[current];
        for (LINE_NUM lineAfter : toSearch) {
            if (assignSet->find(lineAfter) != assignSet->end() &&
                pkb->doesLineNumUses(lineAfter, variable)) {
                // found statement that uses the variable
                return true;
            }

            if (explored.find(lineAfter) == explored.end()) {
                if (!pkb->doesLineNumModifies(lineAfter, variable) ||
                    (readSet->find(lineAfter) == readSet->end() &&
                     callSet->find(lineAfter) == callSet->end() &&
                     assignSet->find(lineAfter) == assignSet->end())) {
                    frontier.push(lineAfter);
                    explored.insert(lineAfter);
                }
            }
        }
    }
    return false;
}

bool AffectsEvaluator::isLineNumAffected(LINE_NUM lineAfter) {
    if (assignSet->find(lineAfter) == assignSet->end()) {
        return false;
    }
    std::vector<VAR_NAME> variables = pkb->getVariableFromUsesByLineNum(lineAfter);
    std::unordered_set<VAR_NAME> variablesSet(variables.begin(), variables.end());

    std::queue<LINE_NUM> frontier;
    std::unordered_set<LINE_NUM> explored;

    frontier.push(lineAfter);
    while (!frontier.empty()) {
        LINE_NUM current = frontier.front();
        frontier.pop();

        if (!pkb->hasPrevLine(current)) {
            continue;
        }

        auto toSearch = (*nextMapKeyLineAfter)[current];
        for (LINE_NUM lineBefore : toSearch) {
            if (explored.find(lineBefore) != explored.end()) {
                continue;
            }

            if (assignSet->find(lineBefore) != assignSet->end()) {
                VAR_NAME variable = pkb->getVariableFromModifiesByLineNum(lineBefore)[0];
                if (variablesSet.find(variable) != variablesSet.end()) {
                    return true;
                }
            } else if (readSet->find(lineBefore) != readSet->end() ||
                callSet->find(lineBefore) != callSet->end()) {
                std::vector<VAR_NAME> vars = pkb->getVariableFromModifiesByLineNum(lineBefore);
                bool affects = false;
                for (VAR_NAME var : vars) {
                   if (variablesSet.find(var) != variablesSet.end()) {
                        affects = true;
                        break;
                   }
                }
                if (affects) {
                    continue;
                }
            }

            if (explored.insert(lineBefore).second) {
                frontier.push(lineBefore);
            }
        }
    }
    return false;
}

std::vector<LINE_NUM> AffectsEvaluator::affectsBfsLineBefore(LINE_NUM start) {
    // one statement can only modifies one variable
    VAR_NAME variable = pkb->getVariableFromModifiesByLineNum(start)[0];

    std::queue<LINE_NUM> frontier;
    std::unordered_set<LINE_NUM> explored;
    std::vector<LINE_NUM> result;

    frontier.push(start);
    while (!frontier.empty()) {
        LINE_NUM current = frontier.front();
        frontier.pop();

        if (!pkb->hasNextLine(current)) {
            continue;
        }

        auto toSearch = (*nextMapKeyLineBefore)[current];
        for (LINE_NUM lineAfter : toSearch) {
            if (assignSet->find(lineAfter) != assignSet->end() &&
                pkb->doesLineNumUses(lineAfter, variable)) {
                result.push_back(lineAfter);
            }

            if (explored.find(lineAfter) == explored.end()) {
                if (!pkb->doesLineNumModifies(lineAfter, variable) ||
                    (readSet->find(lineAfter) == readSet->end() &&
                        callSet->find(lineAfter) == callSet->end() &&
                        assignSet->find(lineAfter) == assignSet->end())) {
                    frontier.push(lineAfter);
                    explored.insert(lineAfter);
                }
            }
        }
    }
    return result;
}

std::vector<LINE_NUM> AffectsEvaluator::affectsDfsLineAfter(LINE_NUM end) {
    std::vector<VAR_NAME> variables = pkb->getVariableFromUsesByLineNum(end);
    std::unordered_set<VAR_NAME> variablesSet(variables.begin(), variables.end());
    std::vector<LINE_NUM> result;

    std::unordered_set<std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>, pair_hash_set> explored;
    std::stack<std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>> dfsStack;
    dfsStack.push({ end, variablesSet });

    while (!dfsStack.empty()) {
        auto pair = dfsStack.top();
        dfsStack.pop();

        LINE_NUM current = pair.first;

        if (!pkb->hasPrevLine(current))
            continue;

        auto toSearch = (*nextMapKeyLineAfter)[current];
        for (LINE_NUM lineBefore : toSearch) {
            auto newSet = pair.second;
            if (pkb->doesLineNumModifiesWildcard(lineBefore) &&
                (readSet->find(lineBefore) != readSet->end() ||
                    callSet->find(lineBefore) != callSet->end() ||
                    assignSet->find(lineBefore) != assignSet->end())) {
                bool modifies = false;
                for (VAR_NAME var : pkb->getVariableFromModifiesByLineNum(lineBefore)) {
                    // remove all variables that are modified by the statment
                    modifies = newSet.erase(var) || modifies;
                }

                if (modifies && assignSet->find(lineBefore) != assignSet->end()) {
                    result.push_back(lineBefore);
                }

                if (newSet.empty()) {
                    // no more variables to be modified
                    continue;
                }
            }
            std::pair<LINE_NUM, std::unordered_set<VAR_NAME>> newPair{ lineBefore, newSet };
            if (explored.find(newPair) == explored.end()) {
                explored.insert(newPair);
                dfsStack.push(newPair);
            }
        }
    }
    return result;
}
