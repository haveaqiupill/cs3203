#pragma once
#include "ParserCommons.h"
#include "../PKB/PKBInterface.h"

class AbstractParser {
 public:
     virtual PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine) {
         return PARSE_RESULT{ 0, LINE_NUMS{} };
     }
    virtual PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine, LINE_NUM parentLine) {
        return PARSE_RESULT{ 0, LINE_NUMS{} };
    }
    void setPkb(PKBInterface* pkb);
 protected:
    PKBInterface* pkb;
};

class AbstractProgramParser : public AbstractParser {
};

class AbstractProcParser : public AbstractParser {
};

class AbstractStmtListParser : public AbstractParser {
};

class AbstractStmtParser : public AbstractParser {
};
