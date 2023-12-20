#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

#include "../../PKB/PKB.h"
#include "../Query/Query.h"
#include "OrderWithinGroupOptimiser.h"
#include "UFDS.h"

typedef std::unordered_map<int, std::unordered_set<SYNONYM>> INDEX_SYN_MAP;

class GroupOptimiser {
 private:
    PKB* pkb;
    OrderWithinGroupOptimiser owgOpt;
    INDEX_SYN_MAP clauseIndexToDeclrMap;

 public:
    explicit GroupOptimiser(PKB* pkb) : pkb(pkb),
        owgOpt(OrderWithinGroupOptimiser(pkb)) {}
    Query optimiseClauses(Query& query);
};
