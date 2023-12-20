#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"
#include "PQL/QueryManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestSelectBoolean) {
public:
    TEST_METHOD(BooleanSelection_NoClause_True) {
        pkb.propagateStmtList(2);
        pkb.storeRelationshipFollows(1, 2);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "TRUE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_OneTrueClause_True) {
        pkb.propagateStmtList(2);
        pkb.storeRelationshipFollows(1, 2);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN such that Follows(1, 2)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "TRUE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_OneAnswerableClause_True) {
        pkb.propagateStmtList(2);
        pkb.storeRelationshipFollows(1, 2);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN such that Follows(1, s)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "TRUE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_OneFalseClause_False) {
        pkb.propagateStmtList(2);
        pkb.storeRelationshipFollows(1, 2);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN such that Follows(1, 3)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "FALSE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_OneUnanswerableClause_False) {
        pkb.propagateStmtList(2);
        pkb.storeRelationshipFollows(1, 2);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN such that Follows(s, 1)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "FALSE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_ThreeTrueClauses_True) {
        pkb.propagateStmtList(4);
        pkb.storeRelationshipFollows(1, 2);
        pkb.storeRelationshipFollows(2, 3);
        pkb.storeRelationshipFollows(3, 4);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN such that Follows(1, 2) ";
        queryString += "such that Follows(2, 3) ";
        queryString += "such that Follows(3, 4)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "TRUE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_ThreeAnswerableClauses_True) {
        pkb.propagateStmtList(4);
        pkb.storeRelationshipFollows(1, 2);
        pkb.storeRelationshipFollows(2, 3);
        pkb.storeRelationshipFollows(3, 4);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s1, s2; Select BOOLEAN such that Follows(1, s1) ";
        queryString += "such that Follows(s1, s2) ";
        queryString += "such that Follows(s2, 4)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "TRUE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_ThreeMixedClauses_False) {
        pkb.propagateStmtList(4);
        pkb.storeRelationshipFollows(1, 2);
        pkb.storeRelationshipFollows(2, 3);
        pkb.storeRelationshipFollows(3, 4);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN such that Follows(1, 2) ";
        queryString += "such that Follows(2, 3) ";
        queryString += "such that Follows(1, 3)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "FALSE" };

        Assert::IsTrue(resultsList == expected);
    }
    TEST_METHOD(BooleanSelection_ThreeUnanswerableClauses_True) {
        pkb.propagateStmtList(4);
        pkb.storeRelationshipFollows(1, 2);
        pkb.storeRelationshipFollows(2, 3);
        pkb.storeRelationshipFollows(3, 4);
        pkb.sortAllDataStructure();

        std::string queryString = "stmt s; Select BOOLEAN such that Follows(1, s) ";
        queryString += "such that Follows(s, 2) ";
        queryString += "such that Follows(s, 3)";
        QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

        std::list<std::string> expected = { "FALSE" };

        Assert::IsTrue(resultsList == expected);
    }
private:
    PKB pkb;
    PKB* ptr = &pkb;
    std::list<std::string> resultsList;
    };
}  // namespace UnitTesting
