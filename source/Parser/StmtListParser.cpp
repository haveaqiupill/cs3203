#include "StmtListParser.h"
#include "StmtParser.h"
#include "../Commons/Utilities.h"
#include "../Commons/InvalidSimpleException.h"

void StmtListParser::setStmtParser(AbstractStmtParser* stmtParser) {
    this->stmtParser = stmtParser;
}

PARSE_RESULT StmtListParser::parse(CODE_SEGMENT stmtList, LINE_NUM firstStmtLine) {
    return parse(stmtList, firstStmtLine, -1);
}

PARSE_RESULT StmtListParser::parse(
CODE_SEGMENT stmtList, LINE_NUM firstStmtLine, LINE_NUM parentLine) {
    stmtList = Utilities::trim(stmtList);
    if (stmtList == "") {
        throw InvalidSimpleException(
            InvalidSimpleException::INVALID_STMT_MSG);
    }

    std::pair<CODE_SEGMENT, CODE_SEGMENT> splitResult = splitFirstStmt(stmtList);

    CODE_SEGMENT firstStmt = std::get<0>(splitResult);
    CODE_SEGMENT restOfStmtList = std::get<1>(splitResult);

    PARSE_RESULT firstStmtParseResult = this->stmtParser->parse(
        firstStmt, firstStmtLine);

    LINE_NUM nextLineAfterFirstStmt = std::get<0>(firstStmtParseResult);
    LINE_NUMS exitLinesOfFirstStmt = std::get<1>(firstStmtParseResult);

    if (parentLine != -1) {
        this->pkb->storeRelationshipParent(parentLine, firstStmtLine);
    }

    if (restOfStmtList == "") {
        return PARSE_RESULT{ nextLineAfterFirstStmt, exitLinesOfFirstStmt };
    } else {
        for (LINE_NUM n : exitLinesOfFirstStmt) {
            this->pkb->storeRelationshipNext(n, nextLineAfterFirstStmt);
        }
        this->pkb->storeRelationshipFollows(firstStmtLine, nextLineAfterFirstStmt);
        return this->parse(restOfStmtList, nextLineAfterFirstStmt, parentLine);
    }
}

std::pair<CODE_SEGMENT, CODE_SEGMENT> StmtListParser::splitFirstStmt(
CODE_SEGMENT stmtList) {  // return value: firstStmt, restOfStmtList
    stmtList = Utilities::trim(stmtList);
    StmtType firstStmtType;

    try {
        firstStmtType = ParserCommons::inferStmtTypeFromStart(stmtList);
    }
    catch (...) {
        throw InvalidSimpleException(InvalidSimpleException::INVALID_STMT_MSG);
    }

    if (firstStmtType == StmtType::READ
    || firstStmtType == StmtType::PRINT
    || firstStmtType == StmtType::ASSIGNMENT
    || firstStmtType == StmtType::CALL) {
        int firstSemicolonPos = stmtList.find_first_of(';');
        CODE_SEGMENT firstStmt = stmtList.substr(0, firstSemicolonPos + 1);
        CODE_SEGMENT restOfStmtList = stmtList.substr(firstSemicolonPos + 1);
        return std::pair<CODE_SEGMENT, CODE_SEGMENT>(
            Utilities::trim(firstStmt),
            Utilities::trim(restOfStmtList));
    }

    // only remaining options: while, if.

    int firstBlockCutoff = StmtListParser::findEndOfFirstBlock(stmtList);
    CODE_SEGMENT firstBlock = stmtList.substr(0, firstBlockCutoff + 1);
    CODE_SEGMENT restOfStmtList = stmtList.substr(firstBlockCutoff + 1);

    if (firstStmtType == StmtType::WHILE) {
        return std::pair<CODE_SEGMENT, CODE_SEGMENT>(
            Utilities::trim(firstBlock),
            Utilities::trim(restOfStmtList));
    } else if (firstStmtType == StmtType::IF_ELSE) {
        int secondBlockCutoff = StmtListParser::findEndOfFirstBlock(restOfStmtList);
        CODE_SEGMENT secondBlock = restOfStmtList.substr(0, secondBlockCutoff + 1);
        restOfStmtList = restOfStmtList.substr(secondBlockCutoff + 1);
        CODE_SEGMENT firstStmt = firstBlock + secondBlock;
        return std::pair<CODE_SEGMENT, CODE_SEGMENT>(
            Utilities::trim(firstStmt),
            Utilities::trim(restOfStmtList));
    }

    throw InvalidSimpleException(InvalidSimpleException::INVALID_STMT_MSG);
}

int StmtListParser::findEndOfFirstBlock(CODE_SEGMENT stmtList) {
    int nestedLevel = 0;

    for (size_t i = 0; i < stmtList.length(); i++) {
        char nextChar = stmtList.at(i);
        if (nextChar == '{') {
            nestedLevel++;
            continue;
        } else if (nextChar == '}') {
            nestedLevel--;

            if (nestedLevel == 0) {
                return i;
            }
        }
    }

    throw InvalidSimpleException(
        InvalidSimpleException::INVALID_STMT_MSG);
}
