#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBDesignEntities) {
    public:
        TEST_METHOD(GetAllVariables_ThreeNormalVariables_Success) {
            PKB testPkb = PKB();

            testPkb.storeVariable("x");
            testPkb.storeVariable("y");
            testPkb.storeVariable("count");
            testPkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = testPkb.getAllVariables();
            std::vector<VAR_NAME> expected = {"count", "x", "y"};
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAllProcedures_ThreeNormalProcedures_Success) {
            PKB testPkb = PKB();

            testPkb.storeProcedure("main", 1, 10, {});
            testPkb.storeProcedure("child", 11, 12, {});
            testPkb.storeProcedure("s", 13, 20, {});
            testPkb.sortAllDataStructure();

            std::vector<PROC_NAME> actual = testPkb.getAllProcedures();
            std::vector<PROC_NAME> expected = {"child", "main", "s"};
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAllAssignStatements_ThreeNormalAssignStatements_Success) {
            PKB testPkb = PKB();

            testPkb.storeAssignStatement(1, "x", "1+2");
            testPkb.storeAssignStatement(2, "x", "1+2");
            testPkb.storeAssignStatement(3, "x", "1+2");
            testPkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = testPkb.getAllAssignStatements();
            std::vector<LINE_NUM> expected = {1, 2, 3};
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAllWhileStatements_ThreeNormalWhileStatements_Success) {
            PKB testPkb = PKB();

            testPkb.storeWhileStatement(1, std::list<VAR_NAME>());
            testPkb.storeWhileStatement(2, std::list<VAR_NAME>());
            testPkb.storeWhileStatement(3, std::list<VAR_NAME>());
            testPkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = testPkb.getAllWhileStatements();
            std::vector<LINE_NUM> expected = {1, 2, 3};
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAllIfStatements_ThreeNormalIfStatements_Success) {
            PKB testPkb = PKB();

            testPkb.storeIfElseStatement(1, std::list<VAR_NAME>());
            testPkb.storeIfElseStatement(2, std::list<VAR_NAME>());
            testPkb.storeIfElseStatement(3, std::list<VAR_NAME>());
            testPkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = testPkb.getAllIfStatements();
            std::vector<LINE_NUM> expected = {1, 2, 3};
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetAllConstants_ThreeNormalConstants_Success) {
            PKB testPkb = PKB();

            testPkb.storeConstant("5");
            testPkb.storeConstant("13");
            testPkb.storeConstant("0");
            testPkb.sortAllDataStructure();

            auto actual = testPkb.getAllConstants();
            std::vector<std::string> expected = {"0", "13", "5"};
            Assert::IsTrue(actual == expected);
        }
    };
}  // namespace UnitTesting
