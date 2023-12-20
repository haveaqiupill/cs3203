#include <list>
#include <utility>
#include <string>
#include "Parser/AbstractParser.h"
#include "stdafx.h"

typedef std::pair<CODE_SEGMENT, std::string> PARSE_ARGS;

class AbstractParserMock : virtual AbstractParser {
 public:
    explicit AbstractParserMock(int len) : segmentLen(len) {}
    PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine);
    PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine, LINE_NUM parentLine);
    std::list<PARSE_ARGS> getAllMethodCalls();
 private:
    std::list<PARSE_ARGS> methodCalls;
    int segmentLen = 0;
};

// various mock classes; all are functionally the same, just that they extend different parsers

class ProcParserMock : public AbstractParserMock, public AbstractProcParser {
 public:
     explicit ProcParserMock(int x) : AbstractParserMock(x) {}
     PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine)
     { return AbstractParserMock::parse(code, firstLine); }
     PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine, LINE_NUM parentLine)
     { return AbstractParserMock::parse(code, firstLine, parentLine); }
};
class StmtParserMock : public AbstractParserMock, public AbstractStmtParser {
 public:
     explicit StmtParserMock(int x) : AbstractParserMock(x) {}
     PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine)
    { return AbstractParserMock::parse(code, firstLine); }
     PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine, LINE_NUM parentLine)
    { return AbstractParserMock::parse(code, firstLine, parentLine); }
};
class StmtListParserMock : public AbstractParserMock, public AbstractStmtListParser {
 public:
     explicit StmtListParserMock(int x) : AbstractParserMock(x) {}
     PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine)
    { return AbstractParserMock::parse(code, firstLine); }
     PARSE_RESULT parse(CODE_SEGMENT code, LINE_NUM firstLine, LINE_NUM parentLine)
    { return AbstractParserMock::parse(code, firstLine, parentLine); }
};
