#pragma once
#include "AbstractParser.h"

class PKB;

class Parser {
 public:
    explicit Parser(PKBInterface* pkb) : pkb(pkb) {}
    void parse(CODE_SEGMENT program);
 private:
    PKBInterface* pkb;
    static CODE_SEGMENT preprocessProgram(CODE_SEGMENT program);
};
