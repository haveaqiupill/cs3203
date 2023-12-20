#include "ProcParser.h"
#include "StmtListParser.h"
#include "../Commons/InvalidSimpleException.h"
#include "ParserCommons.h"
#include "../Commons/Utilities.h"

void ProcParser::setStmtListParser(AbstractStmtListParser* stmtListParser) {
    this->stmtListParser = stmtListParser;
}

PARSE_RESULT ProcParser::parse(CODE_SEGMENT proc, LINE_NUM firstLine) {
    PROC_TOKENS tokens;

    try {
        tokens = tokenizeProc(proc);
    }
    catch (InvalidSimpleException e) {
        throw InvalidSimpleException(e.what(), firstLine);
    }

    PROC_NAME procName = std::get<0>(tokens);
    CODE_SEGMENT stmtList = std::get<1>(tokens);

    PARSE_RESULT parseResult = this->stmtListParser->parse(stmtList, firstLine);
    LINE_NUM lineAfterProc = std::get<0>(parseResult);
    LINE_NUMS procExitPoints = std::get<1>(parseResult);
    this->pkb->storeProcedure(procName, firstLine, lineAfterProc - 1, procExitPoints);
    return parseResult;
}

PROC_TOKENS ProcParser::tokenizeProc(CODE_SEGMENT proc) {
    proc = Utilities::trim(proc);

    if (!ParserCommons::codeSegmentBeginsWith(proc, "procedure ")) {
        throw InvalidSimpleException(InvalidSimpleException::INVALID_PROC_MSG);
    }

    proc = Utilities::trim(proc.substr(10));
    PROC_NAME procName = proc.substr(0, proc.find_first_of('{'));
    procName = Utilities::trim(procName);

    if (!ParserCommons::isValidName(procName)) {
        throw InvalidSimpleException(InvalidSimpleException::INVALID_PROC_NAME_MSG);
    }

    CODE_SEGMENT braceBlock = proc.substr(proc.find_first_of('{'));
    braceBlock = Utilities::trim(braceBlock);

    if (!ParserCommons::codeSegmentBeginsAndEndsWith(braceBlock, "{", "}")) {
        throw InvalidSimpleException(InvalidSimpleException::INVALID_PROC_MSG);
    }

    CODE_SEGMENT stmtList = ParserCommons::extractFromBrackets(braceBlock);
    return PROC_TOKENS(procName, stmtList);
}
