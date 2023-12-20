#pragma once

#include "../../PKB/PKB.h"
#include "../Query/Query.h"
#include "WithOptimiser.h"
#include "GroupOptimiser.h"

class QueryOptimiser {
 private:
    PKB* pkb;
    WithOptimiser withOpt;
    GroupOptimiser groupOpt;

 public:
    explicit QueryOptimiser(PKB* pkb) : pkb(pkb),
                                        withOpt(WithOptimiser()),
                                        groupOpt(GroupOptimiser(pkb)) {}
    Query optimiseQuery(Query &queryObject);
};
