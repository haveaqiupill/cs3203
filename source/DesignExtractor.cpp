#include "DesignExtractor.h"

DesignExtractor::DesignExtractor(PKBInterface* pkbPtr, bool extractExtension) {
    this->pkbPtr = pkbPtr;
    this->extractExtension = extractExtension;
}

void DesignExtractor::extractDesign() {
    // Initialise list of parent statements (just while and if blocks)
    std::vector<LINE_NUM> allWhileStmts = pkbPtr->getAllWhileStatements();
    std::vector<LINE_NUM> allIfStmts = pkbPtr->getAllIfStatements();
    std::vector<LINE_NUM> allParentStmts;
    allParentStmts.insert(allParentStmts.end(), allWhileStmts.begin(), allWhileStmts.end());
    allParentStmts.insert(allParentStmts.end(), allIfStmts.begin(), allIfStmts.end());
    std::sort(allParentStmts.begin(), allParentStmts.end());

    // Preparations for toposort
    std::vector<LINE_NUM> callStmtLineNums = this->getCallStmtLineNums();
    std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> procToCallStmtMap
        = this->getProcToCallStmt();
    std::vector<std::vector<int>> procsIdxAdjacencyList
        = this->extractProcsIdxAdjList(callStmtLineNums);
    std::vector<int> dependencyToposortIdx = this->toposortProcsIdx(procsIdxAdjacencyList);

    // Advanced extractions part 1
    this->updateProcedureUsesModifies(dependencyToposortIdx, procToCallStmtMap);

    // Basic extractions
    this->extractFollowsStar(allParentStmts);
    this->extractFollowsStarNoParent();
    this->extractParentStar(allParentStmts);
    this->extractModifies();
    this->extractUses();
    this->extractAllPrefixExpr();

    // Advanced extractions part 2
    this->extractCalls();
    this->extractCallsStar(dependencyToposortIdx);
    this->pkbPtr->sortAllDataStructure();

    // extract extensions
    if (extractExtension) {
        std::vector<int> callToposortIdx = this->toposortProcsIdx(
            this->reverseAdjList(procsIdxAdjacencyList));

        this->extractNextBip(callStmtLineNums, callToposortIdx);
        this->pkbPtr->sortAllDataStructure();
        this->traverseNextBip(callStmtLineNums, allParentStmts, callToposortIdx);
    }
}

/** Get all call statement line numbers from callMap
 *  This also checks for invalid call statements that call non-existent procedures.
 *  However, no guarantees are made that the first encountered invalid call statement
 *  throws an error.
 */
std::vector<LINE_NUM> DesignExtractor::getCallStmtLineNums() {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();
    std::unordered_map<LINE_NUM, PROC_NAME> callsMap = this->pkbPtr->getCallMap();

    std::vector<LINE_NUM> callStmtLineNums;
    callStmtLineNums.reserve(callsMap.size());
    for (auto& pair : callsMap) {
        LINE_NUM callStmtLine = pair.first;
        PROC_NAME calledProc = pair.second;
        auto result = std::find(allProcs.begin(), allProcs.end(), calledProc);
        if (result == allProcs.end()) {
            throw InvalidSimpleException(InvalidSimpleException::PROC_DOES_NOT_EXIST_MSG,
                                         callStmtLine);
        }
        callStmtLineNums.push_back(callStmtLine);
    }
    std::sort(callStmtLineNums.begin(), callStmtLineNums.end());
    return callStmtLineNums;
}

/** Retrieves information for map of procedure to call statements that call said procedure
 *  This is helpful when updating Modifies and Uses by procedure later.
 */
std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> DesignExtractor::getProcToCallStmt() {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();
    std::unordered_map<LINE_NUM, PROC_NAME> callsMap = this->pkbPtr->getCallMap();

    std::unordered_map<PROC_NAME, std::vector<int>> procToCallStmtMap;
    for (auto& pair : callsMap) {
        LINE_NUM callStmtLine = pair.first;
        PROC_NAME calledProc = pair.second;
        procToCallStmtMap[calledProc].push_back(callStmtLine);
    }
    return procToCallStmtMap;
}

/** Extracts adjacency list of procedures connected by their Call statements. Note that the
 *  statements are connected in reverse order, i.e. if A calls B, then A depends on B (and
 *  hence the edge is directed from B to A). 
 *  It is assumed that all procedures exist as checked in getCallStmtLineNums.
 */
std::vector<std::vector<int>> DesignExtractor::extractProcsIdxAdjList(
    const std::vector<LINE_NUM>& callStmtLineNums) {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();
    std::unordered_map<LINE_NUM, PROC_NAME> callsMap = this->pkbPtr->getCallMap();

    // Create Adjacency List for toposort, where indexes to allProcs are used instead.
    std::vector<std::vector<int>> procsAdjacencyList(allProcs.size(), std::vector<int>());
    for (size_t callerIdx = 0; callerIdx < allProcs.size(); ++callerIdx) {
        PROC_NAME currentProc = allProcs[callerIdx];
        std::pair<LINE_NUM, LINE_NUM> procRange = this->pkbPtr->getProc(currentProc);

        auto it = callStmtLineNums.begin();
        for (; it != callStmtLineNums.end(); ++it) {
            if (*it >= procRange.first && *it <= procRange.second) {
                PROC_NAME calledProc = callsMap[*it];
                auto result = std::find(allProcs.begin(), allProcs.end(), calledProc);
                assert(result != allProcs.end());  // Procedure is in list of procedure names
                int calledIdx = std::distance(allProcs.begin(), result);
                std::vector<int> currentVector = procsAdjacencyList[calledIdx];

                // Only insert to adjacency list if not already in it (prevent duplicates)
                if (std::find(currentVector.begin(), currentVector.end(), callerIdx)
                    == currentVector.end()) {
                    procsAdjacencyList[calledIdx].push_back(callerIdx);
                }
            }
        }
    }
    return procsAdjacencyList;
}

/** Execute toposort (using the DFS algo, but iterative so it's harder)
 *  Returns indices to allProcs instead of procName!
 */
std::vector<int> DesignExtractor::toposortProcsIdx(
    const std::vector<std::vector<int>>& procsAdjacencyList) {
    std::vector<int> toposortList;
    std::unordered_set<int> visited;

    for (size_t idx = 0; idx < procsAdjacencyList.size(); ++idx) {
        std::vector<std::pair<bool, int>> dfs;
        if (visited.count(idx) == 0) {
            dfs.push_back({ false, idx });
        }

        while (!dfs.empty()) {
            std::pair<bool, int> node = dfs.back();
            dfs.pop_back();
            if (node.first) {
                // Node is leaf node, or all neighbours are leaf nodes, hence no cycles
                // query for this node is complete, add to list
                visited.insert(node.second);
                toposortList.insert(toposortList.begin(), node.second);
                continue;
            }
            if (std::find(dfs.begin(), dfs.end(), std::make_pair(true, node.second)) != dfs.end()) {
                // Node already present in DFS, cyclic loop found!
                throw InvalidSimpleException(InvalidSimpleException::CYCLIC_CALL_MSG);
            }

            // else node is new and not visited before, no information about it
            // Hence should also be queried! Add its neighbours into dfs too
            dfs.push_back({ true, node.second });
            for (auto& neighbour : procsAdjacencyList[node.second]) {
                if (visited.count(neighbour) == 0) {
                    dfs.push_back({ false, neighbour });
                }
            }
        }
    }
    return toposortList;
}


/** 1. Establish all Follows* relationships.
 *  This can be done by getting all statements with the same Parent statement.
 */
void DesignExtractor::extractFollowsStar(std::vector<LINE_NUM> const& allParentStmts) {
    std::vector<LINE_NUM> ifStmts = this->pkbPtr->getAllIfStatements();
    for (auto& parentLine : allParentStmts) {
        std::vector<LINE_NUM> siblingStmts = pkbPtr->getChildLineFromParent(parentLine);
        std::sort(siblingStmts.begin(), siblingStmts.end());
        if (siblingStmts.size() < 2) {
            continue;
        }

        // Check if parent statement is an if-else statement, and handle appropriately
        auto result = std::find(ifStmts.begin(), ifStmts.end(), parentLine);
        if (result != ifStmts.end()) {
            // Find index where else block happens. This occurs when two consecutive lines
            // do not follow each other (as they are in separate statement lists)
            size_t elseBlockIdx = 0;
            for (size_t idx = 0; idx < siblingStmts.size() - 1; ++idx) {
                if (!this->pkbPtr->doesFollows(siblingStmts[idx], siblingStmts[idx + 1])) {
                    elseBlockIdx = idx + 1;
                    break;
                }
            }

            // Process FollowsStar for then block, stopping before else block
            for (size_t i = 0; i < elseBlockIdx; ++i) {
                for (size_t j = i + 1; j < elseBlockIdx; ++j) {
                    pkbPtr->storeRelationshipFollowsStar(siblingStmts[i], siblingStmts[j]);
                }
            }

            // Process FollowsStar for else block, starting from else block index
            for (size_t i = elseBlockIdx; i < siblingStmts.size(); ++i) {
                for (size_t j = i + 1; j < siblingStmts.size(); ++j) {
                    pkbPtr->storeRelationshipFollowsStar(siblingStmts[i], siblingStmts[j]);
                }
            }
        } else {
            for (size_t i = 0; i < siblingStmts.size(); ++i) {
                for (size_t j = i + 1; j < siblingStmts.size(); ++j) {
                    pkbPtr->storeRelationshipFollowsStar(siblingStmts[i], siblingStmts[j]);
                }
            }
        }
    }
}

/** 1.5. Link all statements that do not have parents under followsStar
 *  Now works for multiple procedures!
 */
void DesignExtractor::extractFollowsStarNoParent() {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();

    for (auto& procName : allProcs) {
        std::vector<LINE_NUM> linesWithNoParent;
        std::pair<LINE_NUM, LINE_NUM> lineNumRange = this->pkbPtr->getProc(procName);
        for (int line = lineNumRange.first; line < lineNumRange.second + 1; ++line) {
            if (!(this->pkbPtr->isChild(line))) {
                linesWithNoParent.push_back(line);
            }
        }

        for (size_t idxLineBefore = 0; idxLineBefore < linesWithNoParent.size(); ++idxLineBefore) {
            size_t idxLineAfter = idxLineBefore + 1;
            for (; idxLineAfter < linesWithNoParent.size(); ++idxLineAfter) {
                this->pkbPtr->storeRelationshipFollowsStar(
                    linesWithNoParent[idxLineBefore],
                    linesWithNoParent[idxLineAfter]);
            }
        }
    }
}

/** 2. Establish all Parent* relationships.
 *  This can be done by iterating through all parent statements, but in a reverse manner.
 *  Parent statements must occur in earlier line numbers compared to children statements.
 */
void DesignExtractor::extractParentStar(std::vector<LINE_NUM> const& allParentStmts) {
    auto currLinePtr = allParentStmts.rbegin();
    for (; currLinePtr != allParentStmts.rend(); ++currLinePtr) {
        std::vector<LINE_NUM> parentStmts = this->pkbPtr->getParentLineFromParent(*currLinePtr);
        std::vector<LINE_NUM> descendantStmts =
            this->pkbPtr->getChildLineFromParentStar(*currLinePtr);
        for (auto& parent : parentStmts) {
            for (auto& descendant : descendantStmts) {
                this->pkbPtr->storeRelationshipParentStar(parent, descendant);
            }
        }
    }
}


/** 3. & 4. Establish Uses and Modifies relationships for container statements.
 *  By iterating through all variable names, we can get the line numbers in which it is
 *  modified or used. We can get the Parent* relationships of said line numbers and update
 *  these ancestor line numbers accordingly.
 */
void DesignExtractor::extractModifies() {
    std::vector<VAR_NAME> modifiedVarNames = this->pkbPtr->getAllVariables();
    for (auto& varName : modifiedVarNames) {
        std::vector<LINE_NUM> modifiesLineNums = this->pkbPtr
            ->getLineNumFromModifiesByVarName(varName);
        for (auto& modifiesLineNum : modifiesLineNums) {
            std::vector<LINE_NUM> ancestorLineNum = this->pkbPtr
                ->getParentLineFromParentStar(modifiesLineNum);
            for (auto& lineNum : ancestorLineNum) {
                this->pkbPtr->storeRelationshipModifiesByLineNum(lineNum, varName);
            }
        }
    }
}

void DesignExtractor::extractUses() {
    std::vector<VAR_NAME> usesVarNames = this->pkbPtr->getAllVariables();
    for (auto& varName : usesVarNames) {
        std::vector<LINE_NUM> usesLineNums = this->pkbPtr
            ->getLineNumFromUsesByVarName(varName);
        for (auto& usesLineNum : usesLineNums) {
            std::vector<LINE_NUM> ancestorLineNum = this->pkbPtr
                ->getParentLineFromParentStar(usesLineNum);
            for (auto& lineNum : ancestorLineNum) {
                this->pkbPtr->storeRelationshipUsesByLineNum(lineNum, varName);
            }
        }
    }
}

/** Extracts all sub-expressions of the stored prefix expression.
 *  Subexpressions are stored in the PKB partial prefix expressions table
 */
void DesignExtractor::extractAllPrefixExpr() {
    // Define valid operators
    std::unordered_set<std::string> validOperators{ "+", "-", "*", "/", "%" };

    std::vector<LINE_NUM> allAssignStatements = this->pkbPtr->getAllAssignStatements();
    for (auto& lineNum : allAssignStatements) {
        std::pair<VAR_NAME, PREFIX_EXPR> assignPair = this->pkbPtr->getExpr(lineNum);
        VAR_NAME varName = assignPair.first;
        PREFIX_EXPR prefixExpr = assignPair.second;

        // Tokenise prefix expression by whitespace
        std::istringstream iss(prefixExpr);
        std::vector<std::string> tokenizedPrefix{ std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{} };

        std::vector<std::string> exprList;
        std::vector<bool> isValidExpr;
        std::set<std::string> allAnswers;
        for (auto& token : tokenizedPrefix) {
            exprList.push_back(token);
            if (validOperators.find(token) == validOperators.end()) {
                isValidExpr.push_back(true);
                allAnswers.insert(token);
            } else {
                isValidExpr.push_back(false);
            }

            while (isValidExpr.size() >= 3) {
                auto endPtr = isValidExpr.rbegin();
                if (*endPtr && *(endPtr + 1)) {  // Two operands, have valid expression
                    std::vector<std::string> newExprLst;
                    for (int i = 0; i < 3; ++i) {
                        newExprLst.push_back(exprList.back());
                        exprList.pop_back();
                        isValidExpr.pop_back();
                    }

                    std::string newExpr = newExprLst[2] + " " + newExprLst[1] + " "
                        + newExprLst[0];
                    allAnswers.insert(newExpr);

                    exprList.push_back(newExpr);
                    isValidExpr.push_back(true);
                } else {
                    break;
                }
            }
        }

        auto ptr = allAnswers.begin();
        for (; ptr != allAnswers.end(); ++ptr) {
            this->pkbPtr->storePartialPrefixExpr(lineNum, varName, *ptr);
        }
    }
}

/** Extract Calls relationship from information in PKB, as Parser only stores call statements
 *  but not call relationships. This algo finds the procedure in which the call statement is
 *  located in and inserts the relationship appropriately. 
 */
void DesignExtractor::extractCalls() {
    std::vector<std::pair<LINE_NUM, PROC_NAME>> procRanges;
    for (auto& procName : this->pkbPtr->getAllProcedures()) {
        std::pair<LINE_NUM, LINE_NUM> pair = this->pkbPtr->getProc(procName);
        procRanges.push_back({ pair.first, procName });
    }
    std::sort(procRanges.begin(), procRanges.end());

    std::unordered_map<LINE_NUM, PROC_NAME> callsMap = this->pkbPtr->getCallMap();
    for (auto& callPair : callsMap) {
        LINE_NUM callStmt = callPair.first;
        size_t idx = 0;
        for (; idx < procRanges.size() && callStmt >= procRanges[idx].first; ++idx) {
            continue;
        }
        PROC_NAME calledBy = procRanges[--idx].second;
        PROC_NAME calledTo = this->pkbPtr->getCallMap()[callStmt];
        this->pkbPtr->storeRelationshipCalls(calledBy, calledTo);
    }
}

/** Extracts Calls* relationships from existing Call relationships in PKB. The algorithm is
 *  similar to the algorithm used in Parent*, except that the procedures must be iterated using
 *  the toposorted index instead. 
 */
void DesignExtractor::extractCallsStar(const std::vector<int>& toposortedProcIdx) {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();
    for (auto& idx : toposortedProcIdx) {
        PROC_NAME currProc = allProcs[idx];
        std::vector<PROC_NAME> parentProcs = this->pkbPtr->getCallers(currProc);
        std::vector<PROC_NAME> descendantProcs =
            this->pkbPtr->getCalleesStar(currProc);
        for (auto& parent : parentProcs) {
            for (auto& descendant : descendantProcs) {
                this->pkbPtr->storeRelationshipCallsStar(parent, descendant);
            }
        }
    }
}

/** This algo updates the uses and modifies for all procedures and the statements that call
 *  said procedure. This is done by updating the Uses and Modifies relationship of the procedures
 *  for every line in the procedure (including call statements). Procedures are iterated through
 *  the toposorted indices to ensure that there are no dependency errors as call statements are
 *  also updated; in this case, only one pass through all procedures and call statements is needed.
 */
void DesignExtractor::updateProcedureUsesModifies(std::vector<int> const& toposortedProcIdx,
                                                  std::unordered_map<PROC_NAME,
                                                  std::vector<LINE_NUM>> &
                                                  procToCallStmtMap) {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();

    for (auto& procIdx : toposortedProcIdx) {
        PROC_NAME procName = allProcs[procIdx];
        std::pair<LINE_NUM, LINE_NUM> procRange = this->pkbPtr->getProc(procName);
        std::vector<LINE_NUM> callStmtsToProc = procToCallStmtMap[procName];

        for (LINE_NUM line = procRange.first; line < procRange.second + 1; ++line) {
            std::vector<VAR_NAME> usesVariables = this->pkbPtr->getVariableFromUsesByLineNum(line);
            for (auto& var : usesVariables) {
                this->pkbPtr->storeRelationshipUsesByProcedure(procName, var);
                for (auto& callStmt : callStmtsToProc) {
                    this->pkbPtr->storeRelationshipUsesByLineNum(callStmt, var);
                }
            }

            std::vector<VAR_NAME> modifiesVariables
                = this->pkbPtr->getVariableFromModifiesByLineNum(line);
            for (auto& var : modifiesVariables) {
                this->pkbPtr->storeRelationshipModifiesByProcedure(procName, var);
                for (auto& callStmt : callStmtsToProc) {
                    this->pkbPtr->storeRelationshipModifiesByLineNum(callStmt, var);
                }
            }
        }
    }
}

std::unordered_map<LINE_NUM, PROC_NAME> DesignExtractor::getStmtToProcMap() {
    std::unordered_map<LINE_NUM, PROC_NAME> stmtToProcMap;
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();
    for (auto& proc : allProcs) {
        std::pair<LINE_NUM, LINE_NUM> procRange = this->pkbPtr->getProc(proc);
        for (int i = procRange.first; i <= procRange.second; ++i) {
            stmtToProcMap[i] = proc;
        }
    }
    return stmtToProcMap;
}

/** transpose adjacency list, as dependency is inverse of call relationship
 */
std::vector<std::vector<int>> DesignExtractor::reverseAdjList(std::vector<std::vector<int>> const&
                                                              procsIdxAdjacencyList) {
    size_t length = procsIdxAdjacencyList.size();
    std::vector<std::vector<int>> transposedAL;
    transposedAL.resize(length, std::vector<int> {});
    for (size_t i = 0; i < length; ++i) {
        for (size_t j = 0; j < procsIdxAdjacencyList[i].size(); ++j) {
            transposedAL[procsIdxAdjacencyList[i][j]].push_back(i);
        }
    }
    return transposedAL;
}

/** Extract NextBip for cross-procedure relationships (i.e. by call statements or by exiting a procedure.
 *  This is done by getting the reverse toposort by transposing the adjacency list (since a dependency is the
 *  inverse of a call relationship). Then by going through each procedure, the code iterates through all call
 *  statements in the procedure and stores the NextBip relationship with the first line of the called procedure
 *  (guaranteed to only have one starting stmt). Restrictions are put in place with the appropriate call
 *  statements such that procedures exit to the correct statement. Finally, the NextBip relationships are
 *  generated for statements exiting the procedure, obeying restrictions placed on them beforehand.
 */
void DesignExtractor::extractNextBip(std::vector<LINE_NUM> const& callStmtLineNums,
                                     std::vector<int> const& callStmtToposortedIdx) {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();
    std::unordered_map<LINE_NUM, PROC_NAME> callMap = this->pkbPtr->getCallMap();

    std::unordered_map<PROC_NAME, std::set<LINE_NUM>> restrictedToProc;

    for (auto& procIdx : callStmtToposortedIdx) {
        PROC_NAME proc = allProcs[procIdx];

        // Get NextBip relationships from calls to other procedures
        std::pair<LINE_NUM, LINE_NUM> procRange = this->pkbPtr->getProc(proc);
        std::vector<LINE_NUM> lastLinesOfThisProc = this->pkbPtr->getLastStmtsOfProc(proc);

        for (LINE_NUM line = procRange.first; line <= procRange.second; ++line) {
            if (!std::binary_search(callStmtLineNums.begin(), callStmtLineNums.end(), line)) {
                std::vector<LINE_NUM> nextLines = this->pkbPtr->getNextLines(line);
                for (auto& nextLine : nextLines) {
                    this->pkbPtr->storeRelationshipNextBip(line, nextLine);
                }
                continue;
            }
            LINE_NUM callStmt = line;
            PROC_NAME procedureCalled = callMap[callStmt];
            LINE_NUM firstLineOfProcCalled = this->pkbPtr->getProc(procedureCalled).first;

            auto findItr = std::find(lastLinesOfThisProc.begin(), lastLinesOfThisProc.end(),
                                     callStmt);
            if (findItr != lastLinesOfThisProc.end()) {
                // call statement is last line of proc, nothing to return to
                for (auto& restricted : restrictedToProc[proc]) {
                    restrictedToProc[procedureCalled].insert(restricted);
                    this->pkbPtr->storeRelationshipNextBip(callStmt, firstLineOfProcCalled,
                                                           restricted);
                }
                if (restrictedToProc[proc].size() == 0) {
                    this->pkbPtr->storeRelationshipNextBip(callStmt, firstLineOfProcCalled, 0);
                }
            } else {
                this->pkbPtr->storeRelationshipNextBip(callStmt, firstLineOfProcCalled, callStmt);
                restrictedToProc[procedureCalled].insert(callStmt);
            }
        }

        // Get NextBip relationships from calls from other procedures
        for (auto& restriction : restrictedToProc[proc]) {
            std::vector<LINE_NUM> nextLineFromRestriction
                = this->pkbPtr->getNextLines(restriction);
            for (auto& lastLine : lastLinesOfThisProc) {
                if (std::binary_search(callStmtLineNums.begin(), callStmtLineNums.end(),
                                       lastLine)) {
                    continue;
                }
                for (auto& nextLine : nextLineFromRestriction) {
                    this->pkbPtr->storeRelationshipNextBip(lastLine, nextLine, restriction);
                }
            }
        }
    }
}

typedef struct node {
    LINE_NUM currLine = 0;
    std::vector<LINE_NUM> callStmtRestrict;
    bool firstAffects = true;
    std::unordered_set<VAR_NAME> varUseCheck;
} NODE;

/** Extract NextBipStar and AffectsBip relationship
 */
void DesignExtractor::traverseNextBip(std::vector<LINE_NUM> const& callStmtLineNums,
                                      std::vector<LINE_NUM> const& parentStmtLineNums,
                                      std::vector<int> const& callToposortIdx) {
    std::vector<PROC_NAME> allProcs = this->pkbPtr->getAllProcedures();
    std::unordered_set<LINE_NUM> callStmtSet(callStmtLineNums.begin(), callStmtLineNums.end());
    std::vector<LINE_NUM> allAssign = this->pkbPtr->getAllAssignStatements();
    std::unordered_set<LINE_NUM> assignStmtSet(allAssign.begin(), allAssign.end());
    std::unordered_set<LINE_NUM> parentSet(parentStmtLineNums.begin(), parentStmtLineNums.end());

    std::unordered_map<LINE_NUM, PROC_NAME> callMap = this->pkbPtr->getCallMap();
    std::unordered_map<LINE_NUM, PROC_NAME> stmtToProcMap = this->getStmtToProcMap();

    std::unordered_set<PROC_NAME> visitedProcs;

    for (auto& procIdx : callToposortIdx) {
        PROC_NAME procName = allProcs[procIdx];
        std::pair<LINE_NUM, LINE_NUM> procRange = this->pkbPtr->getProc(procName);

        for (LINE_NUM startLine = procRange.first; startLine <= procRange.second;
             ++startLine) {
            NODE firstNode = { startLine };
            VAR_NAME startLineModifiedVar = "";

            if (assignStmtSet.count(startLine) == 1) {
                firstNode.firstAffects = true;
                startLineModifiedVar
                    = this->pkbPtr->getVariableFromModifiesByLineNum(startLine)[0];
                firstNode.varUseCheck.insert(startLineModifiedVar);
            }

            std::unordered_set<LINE_NUM> visitedLines;
            std::unordered_map<LINE_NUM, std::vector<std::vector<LINE_NUM>>> lineToAllRestricts;
            std::unordered_map<LINE_NUM, int> parentCount;
            std::deque<NODE> queue;
            bool firstOccurence = true;
            queue.push_back(firstNode);

            while (queue.size() > 0) {
                NODE currNode = queue.front();
                LINE_NUM currLine = currNode.currLine;
                std::vector<LINE_NUM> callStmtRestrict = currNode.callStmtRestrict;
                std::unordered_set<VAR_NAME> varUseCheck = currNode.varUseCheck;
                bool firstAffects = currNode.firstAffects;
                queue.pop_front();

                // node already visited with same restrict, but allow visiting of start lines again
                if (parentSet.count(currLine) == 1) {
                    /*if (visitedLines.count(currLine) == 1 && currLine != startLine
                        && callStmtRestrict.size() > 0) {
                        std::vector<std::vector<LINE_NUM>> possibleRestricts
                            = lineToAllRestricts[currLine];
                        auto it = std::find(possibleRestricts.begin(), possibleRestricts.end(),
                                            callStmtRestrict);
                        if (it != possibleRestricts.end()) {
                            continue;
                        }
                    }
                    if (callStmtRestrict.size() == 0) {*/
                    if (parentCount[currLine] > 4096) {
                        continue;
                    }
                }
                visitedLines.insert(currLine);
                lineToAllRestricts[currLine].push_back(callStmtRestrict);
                parentCount[currLine]++;

                if (!firstOccurence) {
                    // store NextBipStar relationship
                    this->pkbPtr->storeRelationshipNextBipStar(startLine, currLine);

                    // store AffectsBip(*) relationship for assignment stmt if allowed
                    if (varUseCheck.size() > 0 && assignStmtSet.count(currLine) == 1) {
                        // Store AffectsBip and AffectsBipStar relationship
                        std::vector<VAR_NAME> varsUsed
                            = this->pkbPtr->getVariableFromUsesByLineNum(currLine);
                        bool continueAffects = false;
                        for (auto& var : varsUsed) {
                            if (varUseCheck.count(var) == 1) {
                                if (firstAffects && var == startLineModifiedVar) {
                                    this->pkbPtr->storeRelationshipAffectsBip(startLine, currLine);
                                }
                                this->pkbPtr->storeRelationshipAffectsBipStar(startLine, currLine);
                                continueAffects = true;
                                break;
                            }
                        }

                        // Check if affects is nullified
                        VAR_NAME varModified
                            = this->pkbPtr->getVariableFromModifiesByLineNum(currLine)[0];
                        if (varUseCheck.count(varModified) == 1) {
                            if (continueAffects) {
                                if (varModified == startLineModifiedVar) {
                                    firstAffects = false;
                                }
                            } else {
                                varUseCheck.erase(varModified);
                            }
                        } else {
                            if (continueAffects) {
                                varUseCheck.insert(varModified);
                            }
                        }
                    }
                }

                if (currLine == startLine) {
                    firstOccurence = false;
                }

                // Check if current line is call stmt, then update restriction
                std::vector<LINE_NUM> newRestriction(callStmtRestrict);
                if (callStmtSet.count(currLine) == 1) {
                    PROC_NAME calledProc = callMap[currLine];
                    newRestriction.push_back(currLine);
                }

                // remove restriction if currently at last line of proc
                // this also removes a restriction if last line is call statement, which is intended
                PROC_NAME currProc = stmtToProcMap[currLine];
                std::vector<LINE_NUM> lastLines = this->pkbPtr->getLastStmtsOfProc(currProc);
                bool isLastLine = false;
                if (std::find(lastLines.begin(), lastLines.end(), currLine) != lastLines.end()) {
                    isLastLine = true;
                }

                // Get all NextBip lines with restrictions
                std::vector<std::pair<LINE_NUM, LINE_NUM>> nextLinePairs
                    = this->pkbPtr->getNextBipLines(currLine);

                for (auto& pair : nextLinePairs) {
                    LINE_NUM nextRestrict = pair.first;
                    LINE_NUM nextLine = pair.second;

                    // valid route to take
                    if (nextRestrict == 0 || nextRestrict == currLine
                        || callStmtRestrict.size() == 0
                        || (callStmtRestrict.size() > 0 &&
                            nextRestrict == callStmtRestrict.back())) {
                        if (isLastLine && stmtToProcMap[currLine] != stmtToProcMap[nextLine]
                            && newRestriction.size() > 0) {
                            newRestriction.pop_back();
                        }

                        // correct restriction if started in middle of procedure
                        if (nextRestrict != 0) {
                            if (callStmtSet.count(currLine) == 1) {
                                if (newRestriction.size() == 0 ||
                                    newRestriction.back() != nextRestrict) {
                                    newRestriction.push_back(nextRestrict);
                                }
                            }
                        }

                        NODE nextNode;
                        nextNode.currLine = nextLine;
                        nextNode.callStmtRestrict = newRestriction;
                        nextNode.firstAffects = firstAffects;
                        nextNode.varUseCheck = varUseCheck;
                        queue.push_front(nextNode);
                    }
                }
            }
        }
    }
}
