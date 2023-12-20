#include "QueryArg.h"

QueryArg::QueryArg() {
}
// constructor
QueryArg::QueryArg(std::string& argString, ARG_TYPE type) {
    switch (type) {
    case QueryArgType::NUM:
        this->lineNum = std::stoi(argString);
        break;
    case QueryArgType::NAME:  // wrapped in quotation marks
        this->argName = argString;
        break;
    case QueryArgType::PATTERN_EXPR:
        this->patternExpr = argString;
        break;
    case QueryArgType::PATTERN_EXPR_WITH_WILDCARDS:
        this->patternExpr = argString;
        break;
    case QueryArgType::WILDCARD:  // no need to store argument string if wildcard
        break;
    }

    this->argType = type;
}

QueryArg::QueryArg(DECLARATION& declr) {
    this->argType = QueryArgType::DECLARATION;
    this->declaration = declr;
}

// public get methods
ARG_TYPE QueryArg::getArgType() {
    return this->argType;
}

ARG_NAME QueryArg::getName() {
    return this->argName;
}

LINE_NUM QueryArg::getLineNum() {
    return this->lineNum;
}

PATTERN_EXPR QueryArg::getPatternExpr() {
    return this->patternExpr;
}

DECLARATION QueryArg::getDeclaration() {
    return this->declaration;
}

/*
* Methods used during query optimisation
*/
void QueryArg::replaceDeclrWithVal(LINE_NUM num) {
    this->argType = QueryArgType::NUM;
    this->lineNum = num;
}

void QueryArg::replaceDeclrWithVal(ARG_NAME& str) {
    this->argType = QueryArgType::NAME;
    this->argName = str;
}

void QueryArg::replaceDeclrSynonym(SYNONYM& syn) {
    this->declaration.replaceSynonym(syn);
}
