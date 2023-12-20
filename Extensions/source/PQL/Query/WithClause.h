#pragma once
#include "QueryClause.h"
class WithClause : public QueryClause {
 public:
    WithClause(QUERY_ARG &leftArg, QUERY_ARG &rightArg);
};
