#include <regex>
#include "Parser.h"
#include "ProgramParser.h"
#include "ProcParser.h"
#include "StmtListParser.h"
#include "StmtParser.h"
#include "../Commons/Utilities.h"

void Parser::parse(CODE_SEGMENT program) {
    if (!this->pkb) {
        throw std::exception("Cannot parse when no PKB has been provided.");
    }

    ProgramParser programParser = ProgramParser();
    ProcParser procParser = ProcParser();
    StmtListParser stmtListParser = StmtListParser();
    StmtParser stmtParser = StmtParser();

    programParser.setPkb(this->pkb);
    procParser.setPkb(this->pkb);
    stmtListParser.setPkb(this->pkb);
    stmtParser.setPkb(this->pkb);

    programParser.setProcParser(&procParser);
    procParser.setStmtListParser(&stmtListParser);
    stmtListParser.setStmtParser(&stmtParser);
    stmtParser.setStmtListParser(&stmtListParser);

    program = preprocessProgram(program);
    programParser.parse(program, 1);
}

CODE_SEGMENT Parser::preprocessProgram(CODE_SEGMENT program) {
    return Utilities::trim(
        std::regex_replace(
            program,
            std::regex("\t"),
            "    "));
}
