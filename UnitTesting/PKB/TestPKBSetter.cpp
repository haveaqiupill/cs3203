#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestPKB.h"
#include "Commons/Exceptions/DuplicateProcedureException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBSetter) {
public:
    TEST_METHOD(storeRelationshipFollows_TwoFollowsRelationship_Success) {
        TestPKB pkb = TestPKB();
        pkb.storeRelationshipFollows(1, 2);
        pkb.storeRelationshipFollows(2, 3);

        std::pair<int, int> pair1{ 1, 2 };
        std::pair<int, int> pair2{ 2, 3 };

        // Added to followsSet
        Assert::IsTrue(pkb.getFollowsSet().size() == 2);
        Assert::IsTrue(pkb.getFollowsSet().count(pair1));
        Assert::IsTrue(pkb.getFollowsSet().count(pair2));

        // Added to followsMap
        Assert::IsTrue(pkb.getFollowsMapKeyLineBefore()[1] == std::vector<LINE_NUM> {2});
        Assert::IsTrue(pkb.getFollowsMapKeyLineBefore()[2] == std::vector<LINE_NUM> {3});

        Assert::IsTrue(pkb.getFollowsMapKeyLineAfter()[2] == std::vector<LINE_NUM> {1});
        Assert::IsTrue(pkb.getFollowsMapKeyLineAfter()[3] == std::vector<LINE_NUM> {2});

        // Added to followsStarSet
        Assert::IsTrue(pkb.getFollowsStarSet().size() == 2);

        Assert::IsTrue(pkb.getFollowsStarMapKeyLineBefore()[1] == std::vector<LINE_NUM> {2});
        Assert::IsTrue(pkb.getFollowsStarMapKeyLineBefore()[2] == std::vector<LINE_NUM> {3});

        Assert::IsTrue(pkb.getFollowsStarMapKeyLineAfter()[2] == std::vector<LINE_NUM> {1});
        Assert::IsTrue(pkb.getFollowsStarMapKeyLineAfter()[3] == std::vector<LINE_NUM> {2});
    }

    TEST_METHOD(storeRelationshipFollowsStar_OneFollowsStarRelationship_Success) {
        TestPKB pkb = TestPKB();
        pkb.storeRelationshipFollowsStar(1, 3);

        // Not added to followsSet
        Assert::IsTrue(pkb.getFollowsSet().size() == 0);

        Assert::IsTrue(pkb.getFollowsStarSet().size() == 1);
        std::pair<int, int> pair{ 1, 3 };
        Assert::IsTrue(pkb.getFollowsStarSet().count(pair));

        Assert::IsTrue(pkb.getFollowsStarMapKeyLineBefore()[1] == std::vector<LINE_NUM> {3});

        Assert::IsTrue(pkb.getFollowsStarMapKeyLineAfter()[3] == std::vector<LINE_NUM> {1});
    }

    TEST_METHOD(storeRelationshipNext_TwoNextRelationship_Success) {
        /*
        while (a != 0) {
            a--;}
        a = a + 1;
        */
        TestPKB pkb = TestPKB();
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(1, 3);

        Assert::IsTrue(pkb.getNextSet().size() == 2);

        Assert::IsTrue(pkb.getNextMapKeyLineBefore()->size() == 1);
        Assert::IsTrue((*pkb.getNextMapKeyLineBefore())[1] == std::vector<LINE_NUM> { 2, 3 });

        Assert::IsTrue(pkb.getNextMapKeyLineAfter()->size() == 2);
        Assert::IsTrue((*pkb.getNextMapKeyLineAfter())[2] == std::vector<LINE_NUM> {1});
        Assert::IsTrue((*pkb.getNextMapKeyLineAfter())[3] == std::vector<LINE_NUM> {1});
    }

    TEST_METHOD(storeCallStatement_TwoCallsStatement_Success) {
        TestPKB pkb = TestPKB();
        pkb.storeCallStatement(1, "main");
        pkb.storeCallStatement(2, "hello");

        Assert::IsTrue(pkb.getStatementCallSet().size() == 2);

        Assert::IsTrue(pkb.getCallMap().size() == 2);
        Assert::IsTrue(pkb.getCallMap()[1] == "main");
        Assert::IsTrue(pkb.getCallMap()[2] == "hello");
        Assert::IsFalse(pkb.getCallMap()[1] == "hello");
    }

    TEST_METHOD(storeCallRelationship_twoCallRelationship_success) {
        TestPKB pkb = TestPKB();
        pkb.storeRelationshipCalls("main", "hello");
        pkb.storeRelationshipCalls("hello", "hi");

        Assert::IsTrue(pkb.getCallsSet().size() == 2);

        // check Caller --> Callee data structure
        Assert::IsTrue(pkb.getCallsMapKeyCaller()["main"] == std::vector<PROC_NAME> {"hello"});
        Assert::IsTrue(pkb.getCallsMapKeyCaller()["hello"] == std::vector<PROC_NAME> {"hi"});
        Assert::IsFalse(pkb.getCallsMapKeyCaller()["main"] == std::vector<PROC_NAME> {"hi"});

        // check Callee --> Caller data structure
        Assert::IsTrue(pkb.getCallsMapKeyCallee()["hello"] == std::vector<PROC_NAME> {"main"});
        Assert::IsTrue(pkb.getCallsMapKeyCallee()["hi"] == std::vector<PROC_NAME> {"hello"});

        // check star data structure
        Assert::IsTrue(pkb.getCallsStarMapKeyCaller()["main"] == std::vector<PROC_NAME> {"hello"});
        Assert::IsTrue(pkb.getCallsStarMapKeyCallee()["hi"] == std::vector<PROC_NAME> {"hello"});
    }

    TEST_METHOD(storeDuplicatedProcedure_exceptionThrown) {
        TestPKB pkb = TestPKB();

        pkb.storeProcedure("main", 1, 10, {});
        auto error_expr = [&] {pkb.storeProcedure("main", 11, 12, {}); };
        Assert::ExpectException<DuplicateProcedureException>(error_expr);
    }
};
}  // namespace UnitTesting
