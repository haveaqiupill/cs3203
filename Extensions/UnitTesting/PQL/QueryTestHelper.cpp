#include "stdafx.h"
#include "QueryTestHelper.h"
/*
* Helper methods for creating query components
*/
SuchThatClause QueryTestHelper::createSuchThatCl(
    QueryArgType argTypeL, std::string& argLStr, Attribute attrL,
    QueryArgType argTypeR, std::string& argRStr, Attribute attrR, Relationship rs) {
    QueryArg argL = createQueryArg(argTypeL, argLStr, attrL);
    QueryArg argR = createQueryArg(argTypeR, argRStr, attrR);
    return SuchThatClause(rs, argL, argR);
}

WithClause QueryTestHelper::createWithCl(
    QueryArgType argTypeL, std::string& argLStr, Attribute attrL,
    QueryArgType argTypeR, std::string& argRStr, Attribute attrR) {
    QueryArg argL = createQueryArg(argTypeL, argLStr, attrL);
    QueryArg argR = createQueryArg(argTypeR, argRStr, attrR);
    return WithClause(argL, argR);
}

PatternClause QueryTestHelper::createPatternCl(
    QueryArgType argTypeL, std::string& argLStr, Attribute attrL, QueryArgType argTypeR,
    std::string& argRStr, std::string& patternExpr, std::string& designEntSynonym) {
    QueryArg leftArg = createQueryArg(argTypeL, argLStr, attrL);
    QueryArg rightArg = createQueryArg(argTypeR, argRStr, Attribute::NONE);
    return PatternClause(designEntSynonym, leftArg, rightArg);
}

QueryArg QueryTestHelper::createQueryArg(
    QueryArgType argType, std::string& argStr, Attribute attr) {
    QueryArg arg;
    switch (argType) {
        case QueryArgType::DECLARATION:
            arg = QueryArg(Declaration(argStr, attr));
            break;
        case QueryArgType::NUM:
        case QueryArgType::NAME:
        case QueryArgType::PATTERN_EXPR:
        case QueryArgType::PATTERN_EXPR_WITH_WILDCARDS:
            arg = QueryArg(argStr, argType);
            break;
    }
    return arg;
}
