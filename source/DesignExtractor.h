#pragma once

#include <algorithm>
#include <cassert>
#include <deque>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "PKB/PKBInterface.h"
#include "Commons/InvalidSimpleException.h"
#include "Commons/PairHash.h"
#include "Commons/Types.h"

class DesignExtractor {
 private:
    typedef std::string PREFIX_EXPR;
    PKBInterface* pkbPtr;
    bool extractExtension;

 public:
    explicit DesignExtractor(PKBInterface*, bool);

    std::vector<LINE_NUM> getCallStmtLineNums();
    std::unordered_map<PROC_NAME, std::vector<int>> getProcToCallStmt();
    std::vector<std::vector<int>> extractProcsIdxAdjList(std::vector<LINE_NUM> const&);
    std::vector<int> toposortProcsIdx(std::vector<std::vector<int>> const&);

    void extractFollowsStar(std::vector<LINE_NUM> const&);
    void extractFollowsStarNoParent();
    void extractParentStar(std::vector<LINE_NUM> const&);
    void extractModifies();
    void extractUses();
    void extractAllPrefixExpr();

    void extractCalls();
    void extractCallsStar(std::vector<int> const&);
    void updateProcedureUsesModifies(std::vector<int> const&,
                                     std::unordered_map<PROC_NAME, std::vector<LINE_NUM>>&);

    // Extensions
    std::unordered_map<LINE_NUM, PROC_NAME> getStmtToProcMap();
    std::vector<std::vector<int>> reverseAdjList(std::vector<std::vector<int>> const&);
    void extractNextBip(std::vector<LINE_NUM> const&, std::vector<int> const&);
    void traverseNextBip(std::vector<LINE_NUM> const&, std::vector<LINE_NUM> const&,
                         std::vector<int> const&);

    void extractDesign();
};
