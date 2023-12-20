#pragma once
#include <string>
#include "SharedDefinitions.h"
#include "Attribute.h"
#include "QueryArgType.h"
#include "../PQLExceptions.h"
typedef QueryArgType ARG_TYPE;
typedef std::string ARG_NAME;
typedef Attribute ATTRIBUTE;
class QueryArg {
 public:
    QueryArg();
    // QueryArg(const std::string& argString, ARG_TYPE argType);
    QueryArg(std::string& argString, ARG_TYPE argType);
    explicit QueryArg(DECLARATION& declr);
    ARG_TYPE getArgType();
    ARG_NAME getName();
    ARG_NAME getSynonym();
    LINE_NUM getLineNum();
    PATTERN_EXPR getPatternExpr();
    DECLARATION getDeclaration();  // new addition
    void replaceDeclrWithVal(LINE_NUM num);
    void replaceDeclrWithVal(ARG_NAME& str);
    void replaceDeclrSynonym(SYNONYM& syn);
 private:
    ARG_TYPE argType;
    ARG_NAME argName;
    LINE_NUM lineNum;
    PATTERN_EXPR patternExpr;
    DECLARATION declaration;
};
