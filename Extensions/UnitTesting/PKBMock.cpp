#include "stdafx.h"
#include "PKBMock.h"
#define to_s std::to_string

std::list<std::string> PKBMock::getAllMethodCalls() {
    return methodCalls;
}
void PKBMock::storeRelationshipFollows(LINE_NUM a1, LINE_NUM a2) {
    methodCalls.push_back("storeRelationshipFollows " + to_s(a1) + " " + to_s(a2));
}
void PKBMock::storeRelationshipParent(LINE_NUM a1, LINE_NUM a2) {
    methodCalls.push_back("storeRelationshipParent " + to_s(a1) + " " + to_s(a2));
}
void PKBMock::storeRelationshipUsesByLineNum(LINE_NUM a1, VAR_NAME a2) {
    methodCalls.push_back("storeRelationshipUsesByLineNum " + to_s(a1) + " " + a2);
}
void PKBMock::storeRelationshipModifiesByLineNum(LINE_NUM a1, VAR_NAME a2) {
    methodCalls.push_back("storeRelationshipModifiesByLineNum " + to_s(a1) + " " + a2);
}
void PKBMock::storeRelationshipNext(LINE_NUM a1, LINE_NUM a2) {
    methodCalls.push_back("storeRelationshipNext " + to_s(a1) + " " + to_s(a2));
}

void PKBMock::storeRelationshipNextBip(LINE_NUM a1, LINE_NUM a2) {
    methodCalls.push_back("storeRelationshipNextBip " + to_s(a1) + " " + to_s(a2));
}

void PKBMock::storeRelationshipNextBip(LINE_NUM a1, LINE_NUM a2, LINE_NUM a3) {
    methodCalls.push_back("storeRelationshipNextBip " + to_s(a1) + " " + to_s(a2) + " " + to_s(a3));
}

void PKBMock::storeConstant(CONSTANT a1) {
    methodCalls.push_back("storeConstant " + a1);
}
void PKBMock::storeVariable(VAR_NAME a1) {
    methodCalls.push_back("storeVariable " + a1);
}
void PKBMock::storeProcedure(PROC_NAME a1, LINE_NUM a2, LINE_NUM a3, std::list<LINE_NUM> list) {
    std::string nextCall = "storeProcedure " + a1 + " " + to_s(a2) + " " + to_s(a3);
    for (LINE_NUM line : list) {
        nextCall += " " + line;
    }
    methodCalls.push_back(nextCall);
}
void PKBMock::storeAssignStatement(LINE_NUM a1, VAR_NAME a2, PREFIX_EXPR a3) {
    methodCalls.push_back("storeAssignStatement " + to_s(a1) + " " + a2 + " " + a3);
}
void PKBMock::storePrintStatement(LINE_NUM a1, VAR_NAME a2) {
    methodCalls.push_back("storePrintStatement " + to_s(a1) + " " + a2);
}
void PKBMock::storeReadStatement(LINE_NUM a1, VAR_NAME a2) {
    methodCalls.push_back("storeReadStatement " + to_s(a1) + " " + a2);
}
void PKBMock::storeIfElseStatement(LINE_NUM a1, std::list<VAR_NAME> a2) {
    std::string nextCall = "storeIfElseStatement " + to_s(a1);
    for (VAR_NAME name : a2) {
        nextCall += " " + name;
    }
    methodCalls.push_back(nextCall);
}
void PKBMock::storeWhileStatement(LINE_NUM a1, std::list<VAR_NAME> a2) {
    std::string nextCall = "storeWhileStatement " + to_s(a1);
    for (VAR_NAME name : a2) {
        nextCall += " " + name;
    }
    methodCalls.push_back(nextCall);
}
void PKBMock::storeCallStatement(LINE_NUM a1, PROC_NAME a2) {
    methodCalls.push_back("storeCallStatement " + to_s(a1) + " " + a2);
}

void PKBMock::propagateStmtList(LINE_NUM a1) {
    methodCalls.push_back("propagateStmtList " + to_s(a1));
}
