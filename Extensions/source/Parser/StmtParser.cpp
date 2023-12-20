#include "StmtParser.h"
#include "StmtListParser.h"
#include "ParserCommons.h"
#include "../Commons/InvalidSimpleException.h"
#include "../Commons/Utilities.h"
#include "StmtType.h"

void StmtParser::setStmtListParser(AbstractStmtListParser* stmtListParser) {
    this->stmtListParser = stmtListParser;
}

PARSE_RESULT StmtParser::parse(CODE_SEGMENT stmt, LINE_NUM line) {
    stmt = Utilities::trim(stmt);
    StmtType stmtType;

    if (stmt == "") {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_STMT_MSG, line);
    }

    try {
        stmtType = ParserCommons::inferStmtTypeFromStart(stmt);
    }
    catch (...) {
        throw InvalidSimpleException(InvalidSimpleException::INVALID_STMT_MSG, line);
    }

    if (stmtType == StmtType::WHILE) {
        WHILE_STMT_TOKENS tokenizeResult;

        try {
            tokenizeResult = tokenizeWhileStmt(stmt);
        }
        catch (InvalidSimpleException e) {
            throw InvalidSimpleException(e.what(), line);
        }
        catch (...) {
            throw InvalidSimpleException(InvalidSimpleException::INVALID_WHILE_STMT_MSG, line);
        }

        VAR_NAMES varsInCondition = std::get<0>(tokenizeResult);
        INTEGERS constsInCondition = std::get<1>(tokenizeResult);
        CODE_SEGMENT stmtList = std::get<2>(tokenizeResult);
        PARSE_RESULT parseResult = this->stmtListParser->parse(stmtList, line + 1, line);

        LINE_NUM lineAfterStmtList = std::get<0>(parseResult);
        LINE_NUMS stmtListExits = std::get<1>(parseResult);

        this->pkb->storeWhileStatement(line, varsInCondition);

        for (VAR_NAME varName : varsInCondition) {
            this->pkb->storeVariable(varName);
            this->pkb->storeRelationshipUsesByLineNum(line, varName);
        }

        for (INTEGER constValue : constsInCondition) {
            this->pkb->storeConstant(constValue);
        }

        for (LINE_NUM n : stmtListExits) {
            this->pkb->storeRelationshipNext(n, line);
        }

        this->pkb->storeRelationshipNext(line, line + 1);
        return PARSE_RESULT{ lineAfterStmtList, LINE_NUMS{line} };

    } else if (stmtType == StmtType::IF_ELSE) {
        IF_ELSE_STMT_TOKENS tokenizeResult;

        try {
            tokenizeResult = tokenizeIfElseStmt(stmt);
        }
        catch (InvalidSimpleException e) {
            throw InvalidSimpleException(e.what(), line);
        }
        catch (...) {
            throw InvalidSimpleException(InvalidSimpleException::INVALID_IF_ELSE_STMT_MSG, line);
        }

        VAR_NAMES varsInCondition = std::get<0>(tokenizeResult);
        INTEGERS constsInCondition = std::get<1>(tokenizeResult);
        CODE_SEGMENT firstStmtList = std::get<2>(tokenizeResult);
        CODE_SEGMENT secondStmtList = std::get<3>(tokenizeResult);

        PARSE_RESULT firstParseResult = this->stmtListParser->parse(
            firstStmtList, line + 1, line);
        LINE_NUM lineAfterFirstStmtList = std::get<0>(firstParseResult);
        LINE_NUMS firstStmtListExits = std::get<1>(firstParseResult);

        PARSE_RESULT secondParseResult = this->stmtListParser->parse(
            secondStmtList, lineAfterFirstStmtList, line);
        LINE_NUM lineAfterSecondStmtList = std::get<0>(secondParseResult);
        LINE_NUMS secondStmtListExits = std::get<1>(secondParseResult);

        for (VAR_NAME varName : varsInCondition) {
            this->pkb->storeVariable(varName);
            this->pkb->storeRelationshipUsesByLineNum(line, varName);
        }

        for (INTEGER constValue : constsInCondition) {
            this->pkb->storeConstant(constValue);
        }

        this->pkb->storeRelationshipNext(line, line + 1);
        this->pkb->storeRelationshipNext(line, lineAfterFirstStmtList);
        this->pkb->storeIfElseStatement(line, varsInCondition);

        firstStmtListExits.splice(firstStmtListExits.end(), secondStmtListExits);

        return PARSE_RESULT{
            lineAfterSecondStmtList, firstStmtListExits };
    }

    // past this point, only non-container stmts

    if (stmt.back() != ';') {
        throw InvalidSimpleException(
            InvalidSimpleException::MISSING_SEMICOLON_MSG, line);
    }

    if (stmtType == StmtType::ASSIGNMENT) {
        ASSIGNMENT_STMT_TOKENS tokenizeResult;

        try {
            tokenizeResult = tokenizeAssignmentStmt(stmt);
        }
        catch (InvalidSimpleException e) {
            throw InvalidSimpleException(e.what(), line);
        }
        catch (...) {
            throw InvalidSimpleException(InvalidSimpleException::INVALID_ASSIGNMENT_STMT_MSG, line);
        }

        VAR_NAME assignee = std::get<0>(tokenizeResult);
        VAR_NAMES varsInExpr = std::get<1>(tokenizeResult);
        INTEGERS constsInExpr = std::get<2>(tokenizeResult);
        PREFIX_EXPR prefixExpr = std::get<3>(tokenizeResult);


        for (VAR_NAME varInExpr : varsInExpr) {
            this->pkb->storeVariable(varInExpr);
            this->pkb->storeRelationshipUsesByLineNum(line, varInExpr);
        }

        for (INTEGER constInExpr : constsInExpr) {
            this->pkb->storeConstant(constInExpr);
        }

        this->pkb->storeVariable(assignee);
        this->pkb->storeRelationshipModifiesByLineNum(line, assignee);
        this->pkb->storeAssignStatement(line, assignee, prefixExpr);

        return PARSE_RESULT{ line + 1, LINE_NUMS{line} };
    }

    // past this point, only read/print/call stmts.
    CODE_SEGMENT firstToken = stmt.substr(0, stmt.find(' '));
    CODE_SEGMENT stmtArgs = "";

    if (firstToken == stmt) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_STMT_MSG, line);
    }

    // remove first token and semicolon
    stmtArgs = stmt.substr(firstToken.length() + 1, stmt.length() - firstToken.length() - 2);
    stmtArgs = Utilities::trim(stmtArgs);

    if (stmtType == StmtType::READ) {
        if (!ParserCommons::isValidName(stmtArgs)) {
            throw new InvalidSimpleException(
                InvalidSimpleException::INVALID_READ_STMT_MSG,
                line);
        }

        // syntax ok; can add to pkb
        VAR_NAME varName = stmtArgs;
        this->pkb->storeVariable(varName);
        this->pkb->storeReadStatement(line, varName);
        this->pkb->storeRelationshipModifiesByLineNum(line, varName);

        return PARSE_RESULT{ line + 1, LINE_NUMS{line} };

    } else if (stmtType == StmtType::PRINT) {
        if (!ParserCommons::isValidName(stmtArgs)) {
            throw new InvalidSimpleException(
                InvalidSimpleException::INVALID_PRINT_STMT_MSG,
                line);
        }

        // syntax ok; can add to pkb
        VAR_NAME varName = stmtArgs;
        this->pkb->storeVariable(varName);
        this->pkb->storePrintStatement(line, varName);
        this->pkb->storeRelationshipUsesByLineNum(line, varName);

        return PARSE_RESULT{ line + 1, LINE_NUMS{line} };
    } else if (stmtType == StmtType::CALL) {
        if (!ParserCommons::isValidName(stmtArgs)) {
            throw new InvalidSimpleException(
                InvalidSimpleException::INVALID_CALL_STMT_MSG,
                line);
        }

        // syntax ok; can add to pkb
        PROC_NAME procName = stmtArgs;
        this->pkb->storeCallStatement(line, procName);

        return PARSE_RESULT{ line + 1, LINE_NUMS{line} };
    }
    throw InvalidSimpleException(
        InvalidSimpleException::INVALID_STMT_MSG, line);
}

ASSIGNMENT_STMT_TOKENS StmtParser::tokenizeAssignmentStmt(CODE_SEGMENT stmt) {
    stmt = Utilities::trim(stmt);
    VAR_NAME assignee = stmt.substr(0, stmt.find('='));
    CODE_SEGMENT expr = stmt.substr(assignee.length() + 1, stmt.length() - assignee.length() - 2);

    assignee = Utilities::trim(assignee);
    expr = Utilities::trim(expr);

    if (assignee == stmt || !ParserCommons::isValidName(assignee)) {
        throw new InvalidSimpleException(
            InvalidSimpleException::INVALID_ASSIGNMENT_STMT_MSG);
    }

    if (!ParserCommons::isValidArithExpr(expr)) {
        throw new InvalidSimpleException(
            InvalidSimpleException::INVALID_ARITH_EXPR_MSG);
    }

    VAR_NAMES varsInExpr = ParserCommons::getVarsInExpr(expr);
    INTEGERS constsInExpr = ParserCommons::getIntsInExpr(expr);
    PREFIX_EXPR prefixExpr = Utilities::makePrefixArithExpr(expr);

    return ASSIGNMENT_STMT_TOKENS(assignee, varsInExpr, constsInExpr, prefixExpr);
}

WHILE_STMT_TOKENS StmtParser::tokenizeWhileStmt(CODE_SEGMENT stmt) {
    stmt = Utilities::trim(stmt);
    CODE_SEGMENT whileKeywordAndCondition = stmt.substr(0, stmt.find_first_of('{'));
    whileKeywordAndCondition = Utilities::trim(whileKeywordAndCondition);

    if (!ParserCommons::codeSegmentBeginsWith(whileKeywordAndCondition, "while")) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_WHILE_STMT_MSG);
    }

    CODE_SEGMENT bracketedCondition = whileKeywordAndCondition.substr(5);

    if (!ParserCommons::isValidBracketedCondExpr(bracketedCondition)) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_WHILE_STMT_MSG);
    }

    CODE_SEGMENT condition = ParserCommons::extractFromBrackets(bracketedCondition);
    VAR_NAMES varsInCondition = ParserCommons::getVarsInExpr(condition);
    INTEGERS constsInCondition = ParserCommons::getIntsInExpr(condition);

    CODE_SEGMENT braceBlock = stmt.substr(stmt.find_first_of('{'));
    braceBlock = Utilities::trim(braceBlock);

    if (!ParserCommons::codeSegmentBeginsAndEndsWith(braceBlock, "{", "}")) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_WHILE_STMT_MSG);
    }

    CODE_SEGMENT stmtList = ParserCommons::extractFromBrackets(braceBlock);

    return WHILE_STMT_TOKENS(varsInCondition, constsInCondition, stmtList);
}

IF_ELSE_STMT_TOKENS StmtParser::tokenizeIfElseStmt(CODE_SEGMENT stmt) {
    stmt = Utilities::trim(stmt);
    CODE_SEGMENT ifKeywordAndConditionAndThen = stmt.substr(0, stmt.find_first_of('{'));
    ifKeywordAndConditionAndThen = Utilities::trim(ifKeywordAndConditionAndThen);

    if (!ParserCommons::codeSegmentBeginsAndEndsWith(
        ifKeywordAndConditionAndThen, "if", "then")) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_IF_ELSE_STMT_MSG);
    }

    CODE_SEGMENT bracketedCondition = ifKeywordAndConditionAndThen.substr(
        2, ifKeywordAndConditionAndThen.length() - 6);

    if (!ParserCommons::isValidBracketedCondExpr(bracketedCondition)) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_IF_ELSE_STMT_MSG);
    }

    CODE_SEGMENT condition = ParserCommons::extractFromBrackets(bracketedCondition);
    VAR_NAMES varsInCondition = ParserCommons::getVarsInExpr(condition);
    INTEGERS constsInCondition = ParserCommons::getIntsInExpr(condition);

    CODE_SEGMENT braceBlocks = stmt.substr(stmt.find_first_of('{'));
    braceBlocks = Utilities::trim(braceBlocks);

    int nestedLevel = 0;
    int cutoffPoint = -1;

    for (size_t i = 0; i < braceBlocks.length(); i++) {
        char nextChar = braceBlocks.at(i);

        if (nextChar == '{') {
            nestedLevel++;
        } else if (nextChar == '}') {
            nestedLevel--;
            if (nestedLevel == 0) {
                cutoffPoint = i;
                break;
            }
        }
    }

    if (cutoffPoint == -1) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_IF_ELSE_STMT_MSG);
    }

    CODE_SEGMENT firstBraceBlock = braceBlocks.substr(0, cutoffPoint + 1);
    CODE_SEGMENT elseKeywordAndSecondBraceBlock = braceBlocks.substr(cutoffPoint + 1);
    firstBraceBlock = Utilities::trim(firstBraceBlock);
    elseKeywordAndSecondBraceBlock = Utilities::trim(elseKeywordAndSecondBraceBlock);

    if (!ParserCommons::codeSegmentBeginsWith(elseKeywordAndSecondBraceBlock, "else")) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_IF_ELSE_STMT_MSG);
    }

    CODE_SEGMENT secondBraceBlock = elseKeywordAndSecondBraceBlock.substr(4);
    secondBraceBlock = Utilities::trim(secondBraceBlock);

    if (!ParserCommons::codeSegmentBeginsAndEndsWith(firstBraceBlock, "{", "}")
        || !ParserCommons::codeSegmentBeginsAndEndsWith(secondBraceBlock, "{", "}")) {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_IF_ELSE_STMT_MSG);
    }

    CODE_SEGMENT firstStmtList = ParserCommons::extractFromBrackets(firstBraceBlock);
    CODE_SEGMENT secondStmtList = ParserCommons::extractFromBrackets(secondBraceBlock);

    return IF_ELSE_STMT_TOKENS(varsInCondition, constsInCondition, firstStmtList, secondStmtList);
}
