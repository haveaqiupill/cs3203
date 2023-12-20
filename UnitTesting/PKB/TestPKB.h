#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>

#include "PKB/PKB.h"

// Testing class of PKB that is able to access the private attribute

class TestPKB : public PKB {
 public:
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> getFollowsSet() {
        return followsSet;
    }

    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> getFollowsStarSet() {
        return followsStarSet;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getFollowsMapKeyLineBefore() {
        return followsMapKeyLineBefore;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getFollowsStarMapKeyLineBefore() {
        return followsStarMapKeyLineBefore;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getFollowsMapKeyLineAfter() {
        return followsMapKeyLineAfter;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getFollowsStarMapKeyLineAfter() {
        return followsStarMapKeyLineAfter;
    }

    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> getParentSet() {
        return parentSet;
    }

    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> getParentStarSet() {
        return parentStarSet;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getParentMapKeyParentLine() {
        return parentMapKeyParentLine;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getParentStarMapKeyParentLine() {
        return parentStarMapKeyParentLine;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getParentMapKeyChildLine() {
        return parentMapKeyChildLine;
    }

    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> getParentStarMapKeyChildLine() {
        return parentStarMapKeyChildLine;
    }

    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> getNextSet() {
        return nextSet;
    }

    std::unordered_set<LINE_NUM> getStatementCallSet() {
        return statementCallSet;
    }

    std::unordered_map<LINE_NUM, PROC_NAME> getCallMap() {
        return callMapByLineNum;
    }

    std::unordered_set<std::pair<PROC_NAME, PROC_NAME>, pair_hash> getCallsSet() {
        return callsSet;
    }
    std::unordered_set<std::pair<PROC_NAME, PROC_NAME>, pair_hash> getCallsStarSet() {
        return callsStarSet;
    }
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> getCallsMapKeyCaller() {
        return callsMapKeyCaller;
    }
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> getCallsStarMapKeyCaller() {
        return callsStarMapKeyCaller;
    }
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> getCallsMapKeyCallee() {
        return callsMapKeyCallee;
    }
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> getCallsStarMapKeyCallee() {
        return callsStarMapKeyCallee;
    }
};
