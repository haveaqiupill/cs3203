#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser/ProcParser.h"
#include "AbstractParserMock.h"
#include "PKBMock.h"
#include "Commons/InvalidSimpleException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestProcParser) {
    public:
        TEST_METHOD(Parse_ValidSingleStmtProc_CorrectResult) {
            StmtListParserMock stmtListParser = StmtListParserMock(10);
            PKBMock pkb = PKBMock();
            ProcParser procParser = ProcParser();
            procParser.setPkb(&pkb);
            procParser.setStmtListParser(&stmtListParser);

            Assert::IsTrue(procParser.parse(
                "procedure procedure {procedure=1;}", 1) == PARSE_RESULT{ 11, LINE_NUMS{1} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                "storeProcedure procedure 1 10"});
            Assert::IsTrue(
                stmtListParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{
                    "procedure=1;", "1" }});
        }

        TEST_METHOD(Parse_ValidMultiStmtProc_CorrectResult) {
            StmtListParserMock stmtListParser = StmtListParserMock(10);
            PKBMock pkb = PKBMock();
            ProcParser procParser = ProcParser();
            procParser.setPkb(&pkb);
            procParser.setStmtListParser(&stmtListParser);

            Assert::IsTrue(procParser.parse(
                "procedure procedure {if (x==x)\nthen {x=1;} else {while (procedure == 1)"
                " {procedure = 1;}}}", 1) == PARSE_RESULT{ 11, LINE_NUMS{1} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                    "storeProcedure procedure 1 10"});
            Assert::IsTrue(
                stmtListParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{
                    "if (x==x)\nthen {x=1;} else {while (procedure == 1) {procedure = 1;}}",
                    "1" }});
        }

        TEST_METHOD(Parse_Invalid_ExceptionThrown) {
            StmtListParserMock stmtListParser = StmtListParserMock(10);
            PKBMock pkb = PKBMock();
            ProcParser procParser = ProcParser();
            procParser.setPkb(&pkb);
            procParser.setStmtListParser(&stmtListParser);

            auto error_expr = [&] {procParser.parse("", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr);

            auto error_expr1 = [&] {procParser.parse("procedur p1 {x=1;}", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr1);

            auto error_expr2 = [&] {procParser.parse("procedure p1 {x=1;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr2);

            auto error_expr3 = [&] {procParser.parse("procedure 1p {x=1;}", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr3);
        }

        TEST_METHOD(TokenizeProc_Valid_ReturnsTokens) {
            Assert::IsTrue(
                ProcParser::tokenizeProc(
                    "procedure p1 { print x; }") == PROC_TOKENS("p1", "print x;"));

            Assert::IsTrue(
                ProcParser::tokenizeProc(
                    "procedure P1Z2{\n"
                    "  if (x == x) {\n"
                    "    y = y1;\n"
                    "  }\n"
                    "  read x;\n"
                    "}")
                == PROC_TOKENS("P1Z2", "if (x == x) {\n    y = y1;\n  }\n  read x;"));

            Assert::IsTrue(
                ProcParser::tokenizeProc(
                    "procedure main {\n"
                    "    read x1;\n"
                    "    print x2;\n"
                    "    while (x3 > x3) {\n"
                    "        if (x4 > x4) then {\n"
                    "            x5 = 5;\n"
                    "        } else {\n"
                    "            x6 = 6;\n"
                    "        }\n"
                    "    }\n"
                    "}")
                == PROC_TOKENS(
                    "main",
                    "read x1;\n"
                    "    print x2;\n"
                    "    while (x3 > x3) {\n"
                    "        if (x4 > x4) then {\n"
                    "            x5 = 5;\n"
                    "        } else {\n"
                    "            x6 = 6;\n"
                    "        }\n"
                    "    }"));
        }
    };
}  // namespace UnitTesting
