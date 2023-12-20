#include "PatternClause.h"
PatternClause::PatternClause(SYNONYM& syn, QUERY_ARG &leftArg, QUERY_ARG &rightArg)
    : QueryClause(ClauseType::PATTERN_CL, leftArg, rightArg, syn) {
}
