#pragma once
#include <utility>
#include "AbstractParser.h"

class StmtParser;

class StmtListParser : public AbstractStmtListParser {
 public:
    void setStmtParser(AbstractStmtParser* stmtParser);
    PARSE_RESULT parse(CODE_SEGMENT stmtList, LINE_NUM firstLine);
    PARSE_RESULT parse(CODE_SEGMENT stmtList, LINE_NUM firstLine, LINE_NUM parentLine);
    static std::pair<CODE_SEGMENT, CODE_SEGMENT> splitFirstStmt(
        CODE_SEGMENT stmtList);  // return value: firstStmt, restOfStmtList
 private:
    static int findEndOfFirstBlock(CODE_SEGMENT stmtList);
    AbstractStmtParser* stmtParser;
};
