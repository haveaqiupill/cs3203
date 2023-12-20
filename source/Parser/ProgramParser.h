#pragma once
#include <list>
#include "AbstractParser.h"

class ProcParser;

typedef std::list<CODE_SEGMENT> PROCS;

class ProgramParser : public AbstractProgramParser {
 public:
     PARSE_RESULT parse(CODE_SEGMENT program, LINE_NUM firstLine);
    void setProcParser(AbstractProcParser* procParser);
    static PROCS splitProgramByProcs(CODE_SEGMENT program);
 private:
    AbstractProcParser* procParser;
};
