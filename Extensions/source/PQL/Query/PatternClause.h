#pragma once
#include "QueryClause.h"
class PatternClause : public QueryClause{
 public:
    PatternClause(SYNONYM& syn, QUERY_ARG &leftArg, QUERY_ARG &rightArg);
};
