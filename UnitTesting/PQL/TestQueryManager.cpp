#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../source/PQL/QueryManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestQueryManager) {
    public:
        TEST_METHOD(ParseAndEvaluateQuery_SelectSingle_Success) {
            pkb.propagateStmtList(3);
            pkb.sortAllDataStructure();

            std::string queryString = "stmt s; Select s";
            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);
            std::list<std::string> expected = { "1", "2", "3" };
            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(ParseAndEvaluateQuery_SelectTuple_Success) {
            pkb.propagateStmtList(3);
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            std::string queryString = "stmt s; variable v; Select <s, v>";
            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);
            std::list<std::string> expected = { "1 x", "2 x", "3 x" };
            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(ParseAndEvaluateQuery_SelectBoolean_Success) {
            std::string queryString = "Select BOOLEAN";
            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);
            std::list<std::string> expected = { "TRUE" };
            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(ParseAndEvaluateQuery_SelectSingleSuchThat_Success) {
            pkb.propagateStmtList(3);
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.sortAllDataStructure();

            std::string queryString = "stmt s; Select s such that Follows(s, 2)";
            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);
            std::list<std::string> expected = { "1" };
            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(ParseAndEvaluateQuery_SelectTupleSuchThatPattern_Success) {
            pkb.propagateStmtList(3);
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "x", "+ x 1");
            pkb.storeAssignStatement(3, "x", "+ x 1");
            pkb.storeConstant("1");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            std::string queryString = "stmt s; assign a; Select <s, a> such that Follows(s, a) "
                                      "pattern a(\"x\", _)";
            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);
            std::list<std::string> expected = { "1 2", "2 3" };
            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(ParseAndEvaluateQuery_SelectBooleanSuchThatPatternWith_Success) {
            pkb.propagateStmtList(3);
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "x", "+ x 1");
            pkb.storeAssignStatement(3, "x", "+ x 1");
            pkb.storeConstant("1");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            std::string queryString = "stmt s; assign a; Select BOOLEAN such that Follows(s, a) "
                                      "pattern a(\"x\", _) with a.stmt# = 2";
            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);
            std::list<std::string> expected = { "TRUE" };
            Assert::IsTrue(resultsList == expected);
        }
    private:
        PKB pkb;
        PKB* ptr = &pkb;
        std::list<std::string> resultsList;
    };
}  // namespace UnitTesting

