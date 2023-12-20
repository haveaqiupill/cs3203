#include "WithClause.h"

WithClause::WithClause(QUERY_ARG &leftArg, QUERY_ARG &rightArg)
    : QueryClause(ClauseType::WITH_CL, leftArg, rightArg) {
}
