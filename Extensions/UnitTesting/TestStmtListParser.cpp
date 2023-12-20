#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser/StmtListParser.h"
#include "AbstractParserMock.h"
#include "PKBMock.h"
#include "Parser/StmtParser.h"
#include "Commons/InvalidSimpleException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestStmtListParser) {
    public:
        TEST_METHOD(Parse_InvalidStmtList_ExceptionThrown) {
            StmtParserMock stmtParser = StmtParserMock(1);
            PKBMock pkb = PKBMock();
            StmtListParser stmtListParser = StmtListParser();
            stmtListParser.setPkb(&pkb);
            stmtListParser.setStmtParser(&stmtParser);

            // didn't want to use loop. rationale: if it fails, want to know which specific line

            auto error_expr = [&] {stmtListParser.parse("", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr);

            auto error_expr2 = [&] {stmtListParser.parse("procedure proc {x=1;}", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr2);
        }

        TEST_METHOD(Parse_SingleNonContainerStmt_Success) {
            StmtParserMock stmtParser = StmtParserMock(1);
            PKBMock pkb = PKBMock();
            StmtListParser stmtListParser = StmtListParser();
            stmtListParser.setPkb(&pkb);
            stmtListParser.setStmtParser(&stmtParser);
            Assert::IsTrue(stmtListParser.parse(
                "x = 1 + 2 * ((3) % x) / 4;", 1) == PARSE_RESULT{ 2, LINE_NUMS{1} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{});
            Assert::IsTrue(
                stmtParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{
                    "x = 1 + 2 * ((3) % x) / 4;", "1" }});
        }

        TEST_METHOD(Parse_SingleContainerStmt_Success) {
            StmtParserMock stmtParser = StmtParserMock(1);
            PKBMock pkb = PKBMock();
            StmtListParser stmtListParser = StmtListParser();
            stmtListParser.setPkb(&pkb);
            stmtListParser.setStmtParser(&stmtParser);
            Assert::IsTrue(
                stmtListParser.parse(
                    "if (x==x)then{y=y;} else{y=y;} ", 1) == PARSE_RESULT{ 2, LINE_NUMS{1} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{});
            Assert::IsTrue(
                stmtParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{
                    "if (x==x)then{y=y;} else{y=y;}", "1" }});
        }

        TEST_METHOD(Parse_MultipleStmtsWithParent_Success) {
            StmtParserMock stmtParser = StmtParserMock(1);
            PKBMock pkb = PKBMock();
            StmtListParser stmtListParser = StmtListParser();
            stmtListParser.setPkb(&pkb);
            stmtListParser.setStmtParser(&stmtParser);
            Assert::IsTrue(
                stmtListParser.parse(
                    "read print; print procedure;\nx = 1;", 2, 1)
                == PARSE_RESULT{ 5, LINE_NUMS{4} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                    "storeRelationshipParent 1 2",
                    "storeRelationshipNext 2 3",
                    "storeRelationshipFollows 2 3",
                    "storeRelationshipParent 1 3",
                    "storeRelationshipNext 3 4",
                    "storeRelationshipFollows 3 4",
                    "storeRelationshipParent 1 4"
            });
            Assert::IsTrue(
                stmtParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                    PARSE_ARGS{"read print;", "2" },
                    PARSE_ARGS{"print procedure;", "3"},
                    PARSE_ARGS{"x = 1;", "4"}});
        }

        TEST_METHOD(SplitFirstStmt_SingleStmt_ReturnsStmtAndBlank) {
            Assert::IsTrue(
                StmtListParser::splitFirstStmt("read read;")
                == std::pair<CODE_SEGMENT, CODE_SEGMENT>("read read;", ""));

            Assert::IsTrue(
                StmtListParser::splitFirstStmt(
                    "\n  if (x == x) then {\n    x = 1;\n  } else {\n    x = 0;\n  }\n")
                == std::pair<CODE_SEGMENT, CODE_SEGMENT>(
                    "if (x == x) then {\n    x = 1;\n  } else {\n    x = 0;\n  }",
                    ""));

            Assert::IsTrue(
                StmtListParser::splitFirstStmt(
                    "if (else == if) then {\n\nx=1;}else    {while(x==1){read read;}}   ")
                == std::pair<CODE_SEGMENT, CODE_SEGMENT>(
                    "if (else == if) then {\n\nx=1;}else    {while(x==1){read read;}}", ""));
        }


        TEST_METHOD(SplitFirstStmt_MultipleStmts_SplitsCorrectly) {
            Assert::IsTrue(
                StmtListParser::splitFirstStmt("read read;\nx = (1 + 2) * 3;")
                == std::pair<CODE_SEGMENT, CODE_SEGMENT>("read read;", "x = (1 + 2) * 3;"));

            Assert::IsTrue(
                StmtListParser::splitFirstStmt(
                    "if (else == if) then {\n\nx=1;}else    {while(x==1){read read;}}   "
                    "X1=x; {while(x==1){read read;}")
                == std::pair<CODE_SEGMENT, CODE_SEGMENT>(
                    "if (else == if) then {\n\nx=1;}else    {while(x==1){read read;}}",
                    "X1=x; {while(x==1){read read;}"));
        }
    };
}  // namespace UnitTesting
