#include "QueryClause.h"

QueryClause::QueryClause(CLAUSE_TYPE clType, QUERY_ARG &left, QUERY_ARG &right, Relationship r)
    : clauseType(clType), leftArg(left), rightArg(right), rs(r) {
}

QueryClause::QueryClause(CLAUSE_TYPE clType, QUERY_ARG &left, QUERY_ARG &right, SYNONYM& syn)
    : clauseType(clType), leftArg(left), rightArg(right), designEntSyn(syn) {
}

QueryClause::QueryClause(CLAUSE_TYPE clType, QUERY_ARG &left, QUERY_ARG &right)
    : clauseType(clType), leftArg(left), rightArg(right) {
}

CLAUSE_TYPE QueryClause::getClauseType() {
    return this->clauseType;
}
QUERY_ARG QueryClause::getLeftArg() {
    return this->leftArg;
}
QUERY_ARG QueryClause::getRightArg() {
    return this->rightArg; }

Relationship QueryClause::getRelationship() {
    if (this->clauseType != ClauseType::SUCHTHAT_CL) {
        // throw exception as this method should not be called for non such that
        throw UnsupportedMethodCallException();
    } else {
        return this->rs;
    }
}

SYNONYM QueryClause::getDesignEntitySynonym() {
    if (this->clauseType != ClauseType::PATTERN_CL) {
        throw UnsupportedMethodCallException();
    } else {
        return this->designEntSyn;
    }
}

/*
* Reset argument helpers
*/
void QueryClause::replaceLeftDeclrWithVal(LINE_NUM replacement) {
    this->leftArg.replaceDeclrWithVal(replacement);
}

void QueryClause::replaceLeftDeclrWithVal(ARG_NAME& replacement) {
    this->leftArg.replaceDeclrWithVal(replacement);
}

void QueryClause::replaceRightDeclrWithVal(LINE_NUM replacement) {
    this->rightArg.replaceDeclrWithVal(replacement);
}

void QueryClause::replaceRightDeclrWithVal(ARG_NAME& replacement) {
    this->rightArg.replaceDeclrWithVal(replacement);
}

void QueryClause::replaceLeftDeclrSynonym(SYNONYM& syn) {
    this->leftArg.replaceDeclrSynonym(syn);
}

void QueryClause::replaceRightDeclrSynonym(SYNONYM& syn) {
    this->rightArg.replaceDeclrSynonym(syn);
}
