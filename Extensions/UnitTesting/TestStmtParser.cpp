#include <list>
#include <string>
#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser/StmtParser.h"
#include "PKBMock.h"
#include "AbstractParserMock.h"
#include "Commons/InvalidSimpleException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestStmtParser) {
        TEST_METHOD(Parse_InvalidStmt_ExceptionThrown) {
            StmtListParserMock stmtListParser = StmtListParserMock(3);
            PKBMock pkb = PKBMock();
            StmtParser stmtParser = StmtParser();
            stmtParser.setPkb(&pkb);
            stmtParser.setStmtListParser(&stmtListParser);

            // didn't want to use loop. rationale: if it fails, want to know which specific line

            auto error_expr = [&] {stmtParser.parse("", 1);};
            Assert::ExpectException<InvalidSimpleException>(error_expr);

            auto error_expr1 = [&] {stmtParser.parse(";", 1);};
            Assert::ExpectException<InvalidSimpleException>(error_expr1);

            auto error_expr2 = [&] {stmtParser.parse("x;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr2);

            auto error_expr3 = [&] {stmtParser.parse("1;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr3);

            auto error_expr4 = [&] {stmtParser.parse("read x", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr4);

            auto error_expr5 = [&] {stmtParser.parse("read print x;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr5);

            auto error_expr6 = [&] {stmtParser.parse("print x + 1;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr6);

            auto error_expr7 = [&] {stmtParser.parse("call x+;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr7);

            auto error_expr8 = [&] {stmtParser.parse("if (x == 1) {x = 1;} else {x = 2;}", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr8);

            auto error_expr9 = [&] {stmtParser.parse(
                "if (x == 1) then {x = 1;} else {x = 2;};", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr9);

            auto error_expr10 = [&] {stmtParser.parse(
                "if x == 1 then {x = 1;} else {x = 2;}", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr10);

            auto error_expr11 = [&] {stmtParser.parse("if (x == 1) then {x = 1;}", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr11);

            auto error_expr12 = [&] {stmtParser.parse("while x == 1 {x = 1;}", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr12);

            auto error_expr13 = [&] {stmtParser.parse("1 = 1;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr13);

            auto error_expr14 = [&] {stmtParser.parse("x+=1;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr14);

            auto error_expr15 = [&] {stmtParser.parse("x = read();", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr15);

            auto error_expr16 = [&] {stmtParser.parse("while (x == 1) {x = 1;};", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr16);

            auto error_expr17 = [&] {stmtParser.parse("x = 1; y = 2;", 1); };
            Assert::ExpectException<InvalidSimpleException>(error_expr17);
        }

        TEST_METHOD(Parse_ReadStmt_CorrectResult) {
            StmtListParserMock stmtListParser = StmtListParserMock(3);
            PKBMock pkb = PKBMock();
            StmtParser stmtParser = StmtParser();
            stmtParser.setPkb(&pkb);
            stmtParser.setStmtListParser(&stmtListParser);
            Assert::IsTrue(
                stmtParser.parse("read read;", 1) == PARSE_RESULT{ 2, LINE_NUMS{1} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                    "storeVariable read",
                    "storeReadStatement 1 read",
                    "storeRelationshipModifiesByLineNum 1 read"
                });
        }

        TEST_METHOD(Parse_CallStmt_CorrectResult) {
            StmtListParserMock stmtListParser = StmtListParserMock(3);
            PKBMock pkb = PKBMock();
            StmtParser stmtParser = StmtParser();
            stmtParser.setPkb(&pkb);
            stmtParser.setStmtListParser(&stmtListParser);
            Assert::IsTrue(
                stmtParser.parse("call x01Y234z0;", 11) == PARSE_RESULT{ 12, LINE_NUMS{11} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                    "storeCallStatement 11 x01Y234z0"
                });
        }

        TEST_METHOD(Parse_AssignmentStmt_CorrectResult) {
            StmtListParserMock stmtListParser = StmtListParserMock(3);
            PKBMock pkb = PKBMock();
            StmtParser stmtParser = StmtParser();
            stmtParser.setPkb(&pkb);
            stmtParser.setStmtListParser(&stmtListParser);
            Assert::IsTrue(
                stmtParser.parse(
                    "x = 1 + 2 * ((3) % x) / 4;", 42) == PARSE_RESULT{ 43, LINE_NUMS{42} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                    "storeVariable x",
                    "storeRelationshipUsesByLineNum 42 x",
                    "storeConstant 1",
                    "storeConstant 2",
                    "storeConstant 3",
                    "storeConstant 4",
                    "storeVariable x",
                    "storeRelationshipModifiesByLineNum 42 x",
                    "storeAssignStatement 42 x + 1 / * 2 % 3 x 4"
            });
        }

        TEST_METHOD(Parse_WhileStmt_CorrectResult) {
            StmtListParserMock stmtListParser = StmtListParserMock(3);
            PKBMock pkb = PKBMock();
            StmtParser stmtParser = StmtParser();
            stmtParser.setPkb(&pkb);
            stmtParser.setStmtListParser(&stmtListParser);
            Assert::IsTrue(
                stmtParser.parse(
                "while (x >= (1 + 2)) {if (1 == 1) then {while (1 == x) {print x;}}"
                "else {y = 1; print y;}}", 1) == PARSE_RESULT{ 5, LINE_NUMS{ 1 } });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                    "storeWhileStatement 1 x",
                    "storeVariable x",
                    "storeRelationshipUsesByLineNum 1 x",
                    "storeConstant 1",
                    "storeConstant 2",
                    "storeRelationshipNext 2 1",
                    "storeRelationshipNext 1 2"
            });
            Assert::IsTrue(
                stmtListParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{
                    "if (1 == 1) then {while (1 == x) {print x;}}else {y = 1; print y;}", "2 1" }
            });
        }

        TEST_METHOD(Parse_IfStmt_CorrectResult) {
            StmtListParserMock stmtListParser = StmtListParserMock(3);
            PKBMock pkb = PKBMock();
            StmtParser stmtParser = StmtParser();
            stmtParser.setPkb(&pkb);
            stmtParser.setStmtListParser(&stmtListParser);
            Assert::IsTrue(stmtParser.parse(
                "if (1 == 1) then{ while (1 == x) { print x; } } else {y = 1; print y;}", 1)
                == PARSE_RESULT{ 8, LINE_NUMS{2, 5} });
            Assert::IsTrue(
                pkb.getAllMethodCalls() == std::list<std::string>{
                    "storeConstant 1",
                    "storeRelationshipNext 1 2",
                    "storeRelationshipNext 1 5",
                    "storeIfElseStatement 1"
            });
            Assert::IsTrue(
                stmtListParser.getAllMethodCalls() == std::list<PARSE_ARGS>{
                PARSE_ARGS{ "while (1 == x) { print x; }", "2 1" },
                PARSE_ARGS{"y = 1; print y;", "5 1"}});
        }

        TEST_METHOD(TokenizeAssignmentStmt_ValidAssignment_ReturnsTokens) {
            Assert::IsTrue(
                StmtParser::tokenizeAssignmentStmt("x=1;")
                == ASSIGNMENT_STMT_TOKENS("x", VAR_NAMES{}, INTEGERS{ "1" }, "1"));

            Assert::IsTrue(
                StmtParser::tokenizeAssignmentStmt("X2y = (1+2*x)    + (a2-(xyzzy*35)%2)  ;  ")
                == ASSIGNMENT_STMT_TOKENS(
                    "X2y",
                    VAR_NAMES{ "a2", "x", "xyzzy" },
                    INTEGERS{ "1", "2", "35" },
                    "+ + 1 * 2 x - a2 % * xyzzy 35 2"));
        }

        TEST_METHOD(TokenizeWhileStmt_Valid_ReturnsTokens) {
            Assert::IsTrue(
                StmtParser::tokenizeWhileStmt("while (x==x) {print x;}")
                == WHILE_STMT_TOKENS(
                    VAR_NAMES{ "x" },
                    INTEGERS{},
                    "print x;"));

            Assert::IsTrue(
                StmtParser::tokenizeWhileStmt(
                    " while((x==1)&&(!(X2+1==3))) {\nwhile = x + 1;\nprint read ; } ")
                == WHILE_STMT_TOKENS(
                    VAR_NAMES{ "X2", "x" },
                    INTEGERS{ "1", "3" },
                    "while = x + 1;\nprint read ;"));

            Assert::IsTrue(
                StmtParser::tokenizeWhileStmt(
                    " while(x == x) {while (y == y) {print hello;}}")
                == WHILE_STMT_TOKENS(
                    VAR_NAMES{ "x" },
                    INTEGERS{},
                    "while (y == y) {print hello;}"));

            Assert::IsTrue(
                StmtParser::tokenizeWhileStmt(
                    "while (x == x) {\n  if (x == x) then {\n    x = 1;\n  } "
                    "else {\n    x = 0;\n  }\n}")
                == WHILE_STMT_TOKENS(
                    VAR_NAMES{ "x" },
                    INTEGERS{},
                    "if (x == x) then {\n    x = 1;\n  } else {\n    x = 0;\n  }"));
        }

        TEST_METHOD(TokenizeIfElseStmt_Valid_ReturnsTokens) {
            Assert::IsTrue(
                StmtParser::tokenizeIfElseStmt(
                    "if (x==x) then {print x;}"
                    "else {if (y==y) then {print y;} else {print X2;}}")
                == IF_ELSE_STMT_TOKENS(
                    VAR_NAMES{ "x" },
                    INTEGERS{},
                    "print x;",
                    "if (y==y) then {print y;} else {print X2;}"));

            Assert::IsTrue(
                StmtParser::tokenizeIfElseStmt(
                    " if((x==1)&&(!(X2+1==3))) then\n"
                    "{\nif = x + 1;\nprint read ; }\nelse {y = 1;} ")
                == IF_ELSE_STMT_TOKENS(
                    VAR_NAMES{ "X2", "x" },
                    INTEGERS{ "1", "3" },
                    "if = x + 1;\nprint read ;",
                    "y = 1;"));
        }
    };
}  // namespace UnitTesting
