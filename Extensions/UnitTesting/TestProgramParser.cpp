#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser/ProgramParser.h"
#include "Commons/Utilities.h"
#include "AbstractParserMock.h"
#include "PKBMock.h"
#include "Commons/InvalidSimpleException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestProgramParser) {
    public:
        TEST_METHOD(Parse_Invalid_ExceptionThrown) {
            ProcParserMock procParser = ProcParserMock(25);
            PKBMock pkb = PKBMock();
            ProgramParser programParser = ProgramParser();
            programParser.setPkb(&pkb);
            programParser.setProcParser(&procParser);

            auto error_expr = [&] {programParser.parse("", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr);
        }

        TEST_METHOD(Parse_ValidSingleProc_CorrectResult) {
            ProcParserMock procParser = ProcParserMock(25);
            PKBMock pkb = PKBMock();
            ProgramParser programParser = ProgramParser();
            programParser.setPkb(&pkb);
            programParser.setProcParser(&procParser);

            Assert::IsTrue(programParser.parse(
                "procedure main {x=1;}", 1) == PARSE_RESULT{ 26, LINE_NUMS{} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                "propagateStmtList 25"});
            Assert::IsTrue(
                procParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{"procedure main {x=1;}", "1"}});
        }

        TEST_METHOD(Parse_ValidMultiProc_CorrectResult) {
            ProcParserMock procParser = ProcParserMock(25);
            PKBMock pkb = PKBMock();
            ProgramParser programParser = ProgramParser();
            programParser.setPkb(&pkb);
            programParser.setProcParser(&procParser);

            Assert::IsTrue(programParser.parse(
                "procedure main {x=1;} procedure procedure {if (procedure == 1) then "
                "{procedure = 1;} else {then = 1;}}  ", 1) == PARSE_RESULT{ 51, LINE_NUMS{} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                "propagateStmtList 50"});
            Assert::IsTrue(
                procParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{ "procedure main {x=1;}", "1" },
                PARSE_ARGS{ "procedure procedure {if (procedure == 1) then {procedure = 1;} else "
                "{then = 1;}}", "26" }});
        }

        TEST_METHOD(SplitProgramByProcs_Empty_ReturnsEmptyList) {
            Assert::IsTrue(
                ProgramParser::splitProgramByProcs("") == PROCS{});
            Assert::IsTrue(
                ProgramParser::splitProgramByProcs("  ") == PROCS{});
        }

        TEST_METHOD(SplitProgramByProcs_OneProc_ReturnsSingletonList) {
            Assert::IsTrue(
                ProgramParser::splitProgramByProcs("   procedure p {procedure = procedure;x=1;}  ")
                == PROCS{ "procedure p {procedure = procedure;x=1;}" });
        }

        TEST_METHOD(SplitProgramByProcs_MultipleProcs_ReturnsListOfProcs) {
            Assert::IsTrue(
                ProgramParser::splitProgramByProcs(
                    "procedure p {procedure = procedure;x=1;}  "
                    "\nprocedure q{if(x==x) then {print x;} else {print x;}}")
                == PROCS{
                    "procedure p {procedure = procedure;x=1;}",
                    "procedure q{if(x==x) then {print x;} else {print x;}}" });
        }
    };
}  // namespace UnitTesting
