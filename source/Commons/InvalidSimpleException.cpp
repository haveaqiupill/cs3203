#include "InvalidSimpleException.h"

// Parser Exceptions
const ERR_MSG InvalidSimpleException::MISSING_SEMICOLON_MSG
= "Missing semicolon";
const ERR_MSG InvalidSimpleException::PROGRAM_HAS_NO_PROCS_MSG
= "Program has no procs (syntax spec: program)";
const ERR_MSG InvalidSimpleException::INVALID_STMT_MSG
= "Invalid statement (syntax spec: stmt)";
const ERR_MSG InvalidSimpleException::INVALID_PROC_MSG
= "Invalid proc (syntax spec: procedure)";
const ERR_MSG InvalidSimpleException::INVALID_READ_STMT_MSG
= "Invalid read statement (syntax spec: read)";
const ERR_MSG InvalidSimpleException::INVALID_PRINT_STMT_MSG
= "Invalid print statement (syntax spec: print)";
const ERR_MSG InvalidSimpleException::INVALID_CALL_STMT_MSG
= "Invalid call statement (syntax spec: call)";
const ERR_MSG InvalidSimpleException::INVALID_ASSIGNMENT_STMT_MSG
= "Invalid assignment statement (syntax spec: assign)";
const ERR_MSG InvalidSimpleException::INVALID_IF_ELSE_STMT_MSG
= "Invalid if statement (syntax spec: if)";
const ERR_MSG InvalidSimpleException::INVALID_WHILE_STMT_MSG
= "Invalid while statement (syntax spec: while)";
const ERR_MSG InvalidSimpleException::INVALID_ARITH_EXPR_MSG
= "Invalid arithmetic expression (syntax spec: expr)";
const ERR_MSG InvalidSimpleException::INVALID_COND_EXPR_MSG
= "Invalid conditional expression (syntax spec: cond_expr)";
const ERR_MSG InvalidSimpleException::INVALID_PROC_NAME_MSG
= "Invalid proc name (syntax spec: proc_name)";
const ERR_MSG InvalidSimpleException::INVALID_VAR_NAME_MSG
= "Invalid variable name (syntax spec: var_name)";
const ERR_MSG InvalidSimpleException::INVALID_CONST_VALUE_MSG
= "Invalid constant value (syntax spec: const_value)";

// DE Exceptions
const ERR_MSG InvalidSimpleException::PROC_DOES_NOT_EXIST_MSG
= "Call statement does not refer to an existing procedure";
const ERR_MSG InvalidSimpleException::CYCLIC_CALL_MSG
= "Recursive and/or cyclic call(s) detected in SIMPLE program";

InvalidSimpleException::InvalidSimpleException()
    : InvalidSimpleException("Invalid syntax") {}
InvalidSimpleException::InvalidSimpleException(ERR_MSG msg, LINE_NUM line)
    : InvalidSimpleException(msg + " at line " + std::to_string(line)) {}
InvalidSimpleException::InvalidSimpleException(ERR_MSG msg) {
    this->errorMsg = msg;
}

const char* InvalidSimpleException::what() const noexcept {
    return this->errorMsg.c_str();
}
