#pragma once
#include <exception>
#include <string>
#include "Types.h"

typedef std::string ERR_MSG;

class InvalidSimpleException : public std::exception {
 public:
    static const ERR_MSG MISSING_SEMICOLON_MSG;
    static const ERR_MSG PROGRAM_HAS_NO_PROCS_MSG;
    static const ERR_MSG INVALID_STMT_MSG;
    static const ERR_MSG INVALID_PROC_MSG;
    static const ERR_MSG INVALID_READ_STMT_MSG;
    static const ERR_MSG INVALID_PRINT_STMT_MSG;
    static const ERR_MSG INVALID_CALL_STMT_MSG;
    static const ERR_MSG INVALID_ASSIGNMENT_STMT_MSG;
    static const ERR_MSG INVALID_IF_ELSE_STMT_MSG;
    static const ERR_MSG INVALID_WHILE_STMT_MSG;
    static const ERR_MSG INVALID_ARITH_EXPR_MSG;
    static const ERR_MSG INVALID_COND_EXPR_MSG;
    static const ERR_MSG INVALID_PROC_NAME_MSG;
    static const ERR_MSG INVALID_VAR_NAME_MSG;
    static const ERR_MSG INVALID_CONST_VALUE_MSG;

    static const ERR_MSG PROC_DOES_NOT_EXIST_MSG;
    static const ERR_MSG CYCLIC_CALL_MSG;

    InvalidSimpleException();
    explicit InvalidSimpleException(ERR_MSG errorMsg);
    InvalidSimpleException(ERR_MSG errorMsg, LINE_NUM line);
    const char* what() const noexcept;

 private:
    ERR_MSG errorMsg;
};
