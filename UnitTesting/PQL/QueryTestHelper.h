#pragma once

#include <string>
#include "CppUnitTest.h"
#include "PQL/Query/Query.h"
#include "PQL/QueryParserLookup.h"

class QueryTestHelper {
 public:
    static SuchThatClause createSuchThatCl(
        QueryArgType argTypeL, std::string& argLStr, Attribute attrL,
        QueryArgType argTypeR, std::string& argRStr, Attribute attrR, Relationship rs);
    static WithClause createWithCl(
        QueryArgType argTypeL, std::string& argLStr, Attribute attrL,
        QueryArgType argTypeR, std::string& argRStr, Attribute attrR);
    static PatternClause createPatternCl(
        QueryArgType argTypeL, std::string& argLStr, Attribute attrL, QueryArgType argTypeR,
        std::string& argRStr, std::string& patternExpr, std::string& designEntSynonym);
    static QueryArg createQueryArg(QueryArgType argType, std::string& argStr, Attribute attr);
 private:
};
