#pragma once
#include "Types.h"

class Utilities {
 public:
    static PREFIX_EXPR makePrefixArithExpr(CODE_SEGMENT arithExpr);
    static CODE_SEGMENT trim(CODE_SEGMENT s);
    static CODE_SEGMENT reverseArithExpr(CODE_SEGMENT expr);
 private:
    static CODE_SEGMENT trimByCharType(CODE_SEGMENT s, char c);
    static bool operatorHasPrecedence(char op1, char op2);
    static int getOperatorPrecedence(char op);
    static CODE_SEGMENT addSpaceIfNone(CODE_SEGMENT str);
};
