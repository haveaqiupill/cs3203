#pragma once
#include "QueryClause.h"
class SuchThatClause : public QueryClause {
 public:
    SuchThatClause(Relationship relationship, QUERY_ARG &leftArg, QUERY_ARG &rightArg);
};
