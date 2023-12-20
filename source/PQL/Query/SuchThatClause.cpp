#include "SuchThatClause.h"
// constructor that calls the parent class constructor
SuchThatClause::SuchThatClause(Relationship relationship, QUERY_ARG &leftArg, QUERY_ARG &rightArg)
    : QueryClause(ClauseType::SUCHTHAT_CL, leftArg, rightArg, relationship) {
}
