#include <regex>
#include <string>
#include "ProgramParser.h"
#include "ProcParser.h"
#include "../Commons/InvalidSimpleException.h"
#include "../Commons/Utilities.h"

std::regex PROC_OPENING_REGEX("procedure\\s+[A-Za-z][A-Za-z\\d]*\\s*\\{");

void ProgramParser::setProcParser(AbstractProcParser* procParser) {
    this->procParser = procParser;
}

PARSE_RESULT ProgramParser::parse(CODE_SEGMENT program, LINE_NUM firstLine) {
    PROCS procs = splitProgramByProcs(program);
    LINE_NUM curLine = firstLine;

    if (procs.size() == 0) {
        throw InvalidSimpleException(
            InvalidSimpleException::PROGRAM_HAS_NO_PROCS_MSG);
    }

    for (CODE_SEGMENT proc : procs) {
        curLine = std::get<0>(this->procParser->parse(proc, curLine));
    }

    this->pkb->propagateStmtList(curLine - 1);
    return PARSE_RESULT{ curLine, LINE_NUMS{} };
}

// does not attempt to validate procs; that's left to ProcParser
PROCS ProgramParser::splitProgramByProcs(CODE_SEGMENT program) {
    program = Utilities::trim(program);
    std::smatch match;
    PROCS procs;
    CODE_SEGMENT prev_match = "";

    while (std::regex_search(program, match, PROC_OPENING_REGEX)) {
        int pos = match.position();
        CODE_SEGMENT nextProc = prev_match + program.substr(0, pos);
        nextProc = Utilities::trim(nextProc);

        if (nextProc.length() > 0) {
            procs.push_back(nextProc);
        }

        prev_match = match.str();
        program = program.substr(pos + match.length());
    }

    CODE_SEGMENT leftOvers = Utilities::trim(prev_match + program);

    if (leftOvers.length() > 0) {
        procs.push_back(prev_match + program);
    }

    return procs;
}
