#pragma once
#include <utility>
#include "AbstractParser.h"

class StmtListParser;

typedef std::pair<PROC_NAME, CODE_SEGMENT> PROC_TOKENS;

class ProcParser : public AbstractProcParser {
 public:
     PARSE_RESULT parse(CODE_SEGMENT proc, LINE_NUM firstLine);
    void setStmtListParser(AbstractStmtListParser* stmtListParser);
    static PROC_TOKENS tokenizeProc(CODE_SEGMENT proc);
 private:
    AbstractStmtListParser* stmtListParser;
};
