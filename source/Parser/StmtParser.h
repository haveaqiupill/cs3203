#pragma once
#include <utility>
#include <tuple>
#include "AbstractParser.h"

class StmtListParser;

// assignee, vars in expr, consts in expr, expr
typedef std::tuple<VAR_NAME, VAR_NAMES, INTEGERS, PREFIX_EXPR> ASSIGNMENT_STMT_TOKENS;

// vars in cond, consts in cond, stmtlist
typedef std::tuple<VAR_NAMES, INTEGERS, CODE_SEGMENT> WHILE_STMT_TOKENS;

// vars in cond, consts in cond, stmtlist, stmtlist
typedef std::tuple<VAR_NAMES, INTEGERS, CODE_SEGMENT, CODE_SEGMENT> IF_ELSE_STMT_TOKENS;

class StmtParser : public AbstractStmtParser {
 public:
    void setStmtListParser(AbstractStmtListParser* stmtListParser);
    PARSE_RESULT parse(CODE_SEGMENT stmt, LINE_NUM firstLine);
    static ASSIGNMENT_STMT_TOKENS tokenizeAssignmentStmt(CODE_SEGMENT assignmentStmt);
    static WHILE_STMT_TOKENS tokenizeWhileStmt(CODE_SEGMENT whileStmt);
    static IF_ELSE_STMT_TOKENS tokenizeIfElseStmt(CODE_SEGMENT ifElseStmt);
 private:
    AbstractStmtListParser* stmtListParser;
};
