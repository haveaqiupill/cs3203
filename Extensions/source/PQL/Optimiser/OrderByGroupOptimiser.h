#pragma once

#include <utility>
#include <vector>

#include "../../PKB/PKB.h"
#include "../Query/Query.h"

class OrderByGroupOptimiser {
 private:
    PKB* pkb;

 public:
    explicit OrderByGroupOptimiser(PKB* pkb) : pkb(pkb) {}
    Query orderGroups(std::vector<int>& noSyns, std::vector<std::vector<int>>& groupings,
        std::vector<std::pair<double, int>>& cumulativeScores, Query& query);
};
