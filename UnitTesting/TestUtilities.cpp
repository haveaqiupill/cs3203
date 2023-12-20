#include "stdafx.h"
#include "CppUnitTest.h"
#include "Commons/Utilities.h"
#include "Commons/Types.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestUtilities) {
    public:
        TEST_METHOD(Trim_Whitespace_ReturnsEmptyString) {
            Assert::IsTrue(Utilities::trim("") == "");
            Assert::IsTrue(Utilities::trim("    ") == "");
            Assert::IsTrue(Utilities::trim("\n \t") == "");
        }

        TEST_METHOD(MakePrefixArithExpr_InfixExpr_ReturnsPrefix) {
            Assert::AreEqual(
                Utilities::makePrefixArithExpr("1"),
                PREFIX_EXPR("1"));
            Assert::AreEqual(
                Utilities::makePrefixArithExpr("(1+2)*      x  "),
                PREFIX_EXPR("* + 1 2 x"));
            Assert::AreEqual(
                Utilities::makePrefixArithExpr("abc+ defg"),
                PREFIX_EXPR("+ abc defg"));
            Assert::AreEqual(
                Utilities::makePrefixArithExpr("1 + 2 - 3 + 4"),
                PREFIX_EXPR("+ - + 1 2 3 4"));
            Assert::AreEqual(
                Utilities::makePrefixArithExpr("1 + 2 * 3"),
                PREFIX_EXPR("+ 1 * 2 3"));
            Assert::AreEqual(
                Utilities::makePrefixArithExpr(
                    "((1+2)*3%((4+hello)/(world123 /   1)))"),
                PREFIX_EXPR("% * + 1 2 3 / + 4 hello / world123 1"));
        }


        TEST_METHOD(ReverseArithExpr_InfixArithExpr_ReturnsReversedExpr) {
            Assert::AreEqual(
                Utilities::reverseArithExpr(""),
                CODE_SEGMENT(""));
            Assert::AreEqual(
                Utilities::reverseArithExpr("1"),
                CODE_SEGMENT("1"));
            Assert::AreEqual(
                Utilities::reverseArithExpr("hello"),
                CODE_SEGMENT("hello"));
            Assert::AreEqual(
                Utilities::reverseArithExpr("1 +(2*3) / abc2   % (51 + z)"),
                CODE_SEGMENT("(z + 51) %   abc2 / (3*2)+ 1"));
        }

        TEST_METHOD(ReverseArithExpr_PostfixArithExpr_ReturnsReversedExpr) {
            Assert::AreEqual(
                Utilities::reverseArithExpr("29 1 abc2 4 / 2   * 1 - +"),
                CODE_SEGMENT("+ - 1 *   2 / 4 abc2 1 29"));
        }
    };
}  // namespace UnitTesting
