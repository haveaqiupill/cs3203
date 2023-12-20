#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser/ParserCommons.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestParserCommons) {
    public:
        TEST_METHOD(IsValidName_InvalidName_ReturnsFalse) {
            Assert::IsFalse(ParserCommons::isValidName(""));
            Assert::IsFalse(ParserCommons::isValidName("1"));
            Assert::IsFalse(ParserCommons::isValidName("1x"));
            Assert::IsFalse(ParserCommons::isValidName("x_1"));
            Assert::IsFalse(ParserCommons::isValidName("x+"));
            Assert::IsFalse(ParserCommons::isValidName("none"));
            Assert::IsFalse(ParserCommons::isValidName("(x)"));
        }

        TEST_METHOD(IsValidName_ValidName_ReturnsTrue) {
            Assert::IsTrue(ParserCommons::isValidName("xxxxxxxx"));
            Assert::IsTrue(ParserCommons::isValidName("x1y2z345abc"));
            Assert::IsTrue(ParserCommons::isValidName("A"));
            Assert::IsTrue(ParserCommons::isValidName("print"));
        }

        TEST_METHOD(GetVarsInExpr_ArithExpr_ReturnsVarNames) {
            // don't use AreEqual - build error
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("x") ==
                VAR_NAMES {"x"});
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("1") ==
                VAR_NAMES{});
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("(1)") ==
                VAR_NAMES{});
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("1 + x") ==
                VAR_NAMES{ "x" });
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("(x)") ==
                VAR_NAMES{ "x" });;
            Assert::IsTrue(
                ParserCommons::getVarsInExpr(
                    "(1 + 2) *       ((aBc) - (3 /d1)) % 4 + aBc")
                == VAR_NAMES{ "aBc", "d1" });
        }

        TEST_METHOD(GetVarsInExpr_CondExpr_ReturnsVarNames) {
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("1 == 1") ==
                VAR_NAMES{});
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("(x != 1) && !(1 == x)") ==
                VAR_NAMES{ "x" });
            Assert::IsTrue(
                ParserCommons::getVarsInExpr("!(x == x)") ==
                VAR_NAMES{ "x" });
            Assert::IsTrue(
                ParserCommons::getVarsInExpr(
                    "(3<xyzzy123    + y) || !((2 > z)  && (z >= 2))")
                == VAR_NAMES{ "xyzzy123", "y", "z"});
        }

        TEST_METHOD(GetIntsInExpr_ArithExpr_ReturnsInts) {
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("x") ==
                INTEGERS{});
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("1") ==
                INTEGERS{"1"});
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("(1)") ==
                INTEGERS{ "1" });
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("1 + x") ==
                INTEGERS{ "1" });
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("0") ==
                INTEGERS{ "0" });
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("(x0)") ==
                INTEGERS{});;
            Assert::IsTrue(
                ParserCommons::getIntsInExpr(
                    "(1 + 2) *       ((a0Bc) - (3 /d6)) % 450 + aBc123 * 0")
                == INTEGERS{ "0", "1", "2", "3", "450" });
        }

        TEST_METHOD(GetIntsInExpr_CondExpr_ReturnsInts) {
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("1 == 1") ==
                INTEGERS{ "1" });
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("(x != 1) && !(1 == x)") ==
                INTEGERS{ "1" });
            Assert::IsTrue(
                ParserCommons::getIntsInExpr("!(x == x)") ==
                INTEGERS{});
            Assert::IsTrue(
                ParserCommons::getIntsInExpr(
                    "(300<xyzzy123    + y) || !((2 > z)  && (z9 >= 2))")
                == INTEGERS{ "2", "300" });
        }

        TEST_METHOD(IsValidArithExpr_InvalidExpr_ReturnsFalse) {
            Assert::IsFalse(ParserCommons::isValidArithExpr(""));
            Assert::IsFalse(ParserCommons::isValidArithExpr("00"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("x y"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("1 +"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("21xy"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("1 +% x"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("1 + ()"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("1 + (x"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("x (+) 1"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("x (+ 1)"));
            Assert::IsFalse(ParserCommons::isValidArithExpr(")1 + x("));
            Assert::IsFalse(ParserCommons::isValidArithExpr("x == 1"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("x_1"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("(1 + 2) * ((x) - (3 / y)) % 4)"));
            Assert::IsFalse(ParserCommons::isValidArithExpr("(1 + 2) * ((  5x) - (3 / y)) % 4"));
            Assert::IsFalse(ParserCommons::isValidArithExpr(
                "(1 + 02) * ((x1y234) - (3 / y)) % 40"));
        }

        TEST_METHOD(IsValidArithExpr_ValidExpr_ReturnsTrue) {
            Assert::IsTrue(ParserCommons::isValidArithExpr("1"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("0"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("10"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("x"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("x1"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("(x)"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("1       +2  "));
            Assert::IsTrue(ParserCommons::isValidArithExpr("x / x"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("1 + 2 * x - 3 / y % 4"));
            Assert::IsTrue(ParserCommons::isValidArithExpr("(1 + 2) * ((x1y234) - (0 / y)) % 40"));
        }

        TEST_METHOD(IsValidRelExpr_InvalidExpr_ReturnsFalse) {
            Assert::IsFalse(ParserCommons::isValidRelExpr(""));
            Assert::IsFalse(ParserCommons::isValidRelExpr(">"));
            Assert::IsFalse(ParserCommons::isValidRelExpr("1"));
            Assert::IsFalse(ParserCommons::isValidRelExpr("x > y < z"));
            Assert::IsFalse(ParserCommons::isValidRelExpr("x = 1"));
            Assert::IsFalse(ParserCommons::isValidRelExpr("x + 1"));
            Assert::IsFalse(ParserCommons::isValidRelExpr("( ) > (1 + x)"));
            Assert::IsFalse(ParserCommons::isValidRelExpr("x > (2y + 1)"));
        }

        TEST_METHOD(IsValidRelExpr_ValidExpr_ReturnsTrue) {
            Assert::IsTrue(ParserCommons::isValidRelExpr("x > y"));
            Assert::IsTrue(ParserCommons::isValidRelExpr("x == y"));
            Assert::IsTrue(ParserCommons::isValidRelExpr("x != y"));
            Assert::IsTrue(ParserCommons::isValidRelExpr("x < y + 1"));
            Assert::IsTrue(ParserCommons::isValidRelExpr("x <= (1 + y / x)"));
            Assert::IsTrue(ParserCommons::isValidRelExpr("x >= (1 + y2 / (0 + (y)) * x)"));
        }

        TEST_METHOD(IsValidCondExpr_InvalidExpr_ReturnsFalse) {
            Assert::IsFalse(ParserCommons::isValidCondExpr(""));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x = 1"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x =="));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x == 00"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("(x == 1) && !(1 != x)"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("(x == 1 && 1 != x)"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x == 1 && (1 != x)"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("(x >= 1) && (1 != x"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("(x > 1) && !(1 != x) && (x == x)"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("(x == 1)"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("!"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("! xy == 11"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("=="));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x !== y"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x == y == z"));
            Assert::IsFalse(ParserCommons::isValidCondExpr("x <> y"));
            Assert::IsFalse(ParserCommons::isValidCondExpr(
                "(1 + 2) * ((  5x) - (3 / y)) % 4 < z"));
            Assert::IsFalse(ParserCommons::isValidCondExpr(
                "(3 < x + y) || (!((2 > z) && (3 >= 02)))"));
            Assert::IsFalse(ParserCommons::isValidCondExpr(
                "(1 + 2) * ((x1y234) - (3 / y)) % 4"));
            Assert::IsFalse(ParserCommons::isValidCondExpr(
                "((!((1 + 0) * ((x1y234) -       (3/y))%4 == (1 + 2) * ((x1y234) - (3 / y)) % 4)))"
                "&& ((x1y234 < z) || (x1y234<= a))"));
        }

        TEST_METHOD(IsValidCondExpr_ValidExpr_ReturnsTrue) {
            Assert::IsTrue(ParserCommons::isValidCondExpr("1==            1  "));
            Assert::IsTrue(ParserCommons::isValidCondExpr("x == 1"));
            Assert::IsTrue(ParserCommons::isValidCondExpr("x == 0"));
            Assert::IsTrue(ParserCommons::isValidCondExpr("!(1 != x)"));
            Assert::IsTrue(ParserCommons::isValidCondExpr("(x == 1)&&(!(1 != x))"));
            Assert::IsTrue(ParserCommons::isValidCondExpr(
                "(  3 < x + y)|| (!((2 >   z)  &&  (3 >= 2) )  )   "));
            Assert::IsTrue(ParserCommons::isValidCondExpr(
                "(!((1 + 2) * ((x1y234) -         (3/y))%4 == (1 + 2) * ((x1y234) - (3 / y)) % 4))"
                "&& ((x1y234 < z) || (x1y234<= a))"));
        }

        TEST_METHOD(IsValidBracketedCondExpr_Valid_ReturnsTrue) {
            Assert::IsTrue(ParserCommons::isValidBracketedCondExpr("    (   x == x)   "));
            Assert::IsTrue(ParserCommons::isValidBracketedCondExpr("((x == x) || (x == x))"));
        }

        TEST_METHOD(IsValidBracketedCondExpr_Invalid_ReturnsFalse) {
            Assert::IsFalse(ParserCommons::isValidBracketedCondExpr("x == x"));
            Assert::IsFalse(ParserCommons::isValidBracketedCondExpr("(x == x) || (x == x)"));
            Assert::IsFalse(ParserCommons::isValidBracketedCondExpr("(x)"));
        }
        TEST_METHOD(InferStmtTypeFromStart_InvalidStmt_ThrowsException) {
            // TODO
        }

        TEST_METHOD(ExtractFromBrackets_BracketedCode_ReturnsCode) {
            Assert::IsTrue(
                ParserCommons::extractFromBrackets("     ((x== x) && (y==y)    )")
                == "(x== x) && (y==y)");
            Assert::IsTrue(
                ParserCommons::extractFromBrackets("     {x=1 ; }    ")
                == "x=1 ;");
        }

        TEST_METHOD(InferStmtTypeFromStart_SingleStmt_ReturnsType) {
            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart("read print;")
                == StmtType::READ);

            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart("print      x1Y2    ;")
                == StmtType::PRINT);

            /*
            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart("call p;")
                == StmtType::CALL
            );
            */

            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart("if = then + (xyzzy * 2);")
                == StmtType::ASSIGNMENT);

            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart(
                    "while ((then == else) || (!(x==1))){\nx=1;\n}")
                == StmtType::WHILE);

            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart(
                    "if (else == if) then {\n\nx=1;}else    {while(x==1){read read;}}")
                == StmtType::IF_ELSE);
        }

        TEST_METHOD(InferStmtTypeFromStart_StmtList_ReturnsTypeOfFirstStmt) {
            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart("read print;read = 1;read read;")
                == StmtType::READ);

            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart("if = then + (xyzzy * 2);")
                == StmtType::ASSIGNMENT);

            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart(
                    "while ((then == else) || (!(x==1))){\nx=1;\n}")
                == StmtType::WHILE);

            Assert::IsTrue(
                ParserCommons::inferStmtTypeFromStart(
                    "print x;\nif (else == if) then {\n\nx=1;}else    {while(x==1){read read;}}"
                    "while(x==1)\n{read x;\n}")
                == StmtType::PRINT);
        }

        TEST_METHOD(CodeSegmentBeginsAndEndsWith_DoesBeginAndEndWith_ReturnsTrue) {
            Assert::IsTrue(ParserCommons::codeSegmentBeginsAndEndsWith(
                "{print x;}", "{", "}"));
            Assert::IsTrue(ParserCommons::codeSegmentBeginsAndEndsWith(
                "()", "(", ")"));
        }

        TEST_METHOD(CodeSegmentBeginsAndEndWith_DoesNotBeginAndEndWith_ReturnsFalse) {
            Assert::IsFalse(ParserCommons::codeSegmentBeginsAndEndsWith(
                "{print x;", "{", "}"));
            Assert::IsFalse(ParserCommons::codeSegmentBeginsAndEndsWith(
                "", "(", ")"));
            Assert::IsFalse(ParserCommons::codeSegmentBeginsAndEndsWith(
                ")", "(", ")"));
        }
    };
}  // namespace UnitTesting
