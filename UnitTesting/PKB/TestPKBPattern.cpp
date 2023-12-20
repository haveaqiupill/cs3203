#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBPattern) {
    public:
        TEST_METHOD(IsPrefixExprInLineNumVariableMap_ExprExists_True) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.isPrefixExprInLineNumVariableMap("+ b c"));
        }
        TEST_METHOD(IsPrefixExprInLineNumVariableMap_ExprDoesExist_False) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.isPrefixExprInLineNumVariableMap("+ c b"));
        }

        TEST_METHOD(GetAssignLineNumByVarPrefixExpr_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.getAssignLineNumByVarPrefixExpr("a", "+ b c")
                == std::vector<LINE_NUM> {1});
        }
        TEST_METHOD(GetAssignLineNumByVarPrefixExpr_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.getAssignLineNumByVarPrefixExpr("a", "+ c b")
                == std::vector<LINE_NUM> {1});
            Assert::IsFalse(pkb.getAssignLineNumByVarPrefixExpr("d", "+ b c")
                == std::vector<LINE_NUM> {1});
        }
        TEST_METHOD(GetAssignLineNumByVariable_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getAssignLineNumByVariable("a");
            std::vector<LINE_NUM> expected{ 1 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAssignLineNumByVariable_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getAssignLineNumByVariable("b");
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAssignLineNumVariableByPrefixExpr_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            std::vector<std::pair<LINE_NUM, VAR_NAME>>
                actual = pkb.getAssignLineNumVariableByPrefixExpr("+ b c");
            std::vector<std::pair<LINE_NUM, VAR_NAME>> expected{ {1, "a"} };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAssignLineNumVariableByPrefixExpr_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(1, "a", "+ b c");
            pkb.sortAllDataStructure();

            std::vector<std::pair<LINE_NUM, VAR_NAME>>
                actual = pkb.getAssignLineNumVariableByPrefixExpr("+ c b");
            std::vector<std::pair<LINE_NUM, VAR_NAME>> expected{ };
            Assert::IsTrue(actual == expected);
        }
    };
}  // namespace UnitTesting
