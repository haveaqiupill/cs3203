#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBCalls) {
public:
    TEST_METHOD(doesProcedureCalls_validRelationship_true) {
        PKB pkb = PKB();
        pkb.storeRelationshipCalls("main", "second");

        Assert::IsTrue(pkb.doesProcedureCalls("main", "second"));
    }

    TEST_METHOD(doesProcedureCalls_invalidRelationship_false) {
        PKB pkb = PKB();
        pkb.storeRelationshipCalls("main", "second");

        Assert::IsFalse(pkb.doesProcedureCalls("second", "main"));
    }

    TEST_METHOD(getCallers_twoCallers_Success) {
        PKB pkb = PKB();
        pkb.storeRelationshipCalls("main", "third");
        pkb.storeRelationshipCalls("second", "third");

        auto actual = pkb.getCallers("third");
        std::vector<PROC_NAME> expected{ "main", "second" };

        Assert::IsTrue(actual == expected);
    }

    TEST_METHOD(getCallees_twoCallees_Success) {
        PKB pkb = PKB();
        pkb.storeRelationshipCalls("main", "second");
        pkb.storeRelationshipCalls("main", "third");

        auto actual = pkb.getCallees("main");
        std::vector<PROC_NAME> expected{ "second", "third" };

        Assert::IsTrue(actual == expected);
    }

    TEST_METHOD(getCallersStar_twoCallers_Success) {
        PKB pkb = PKB();
        pkb.storeRelationshipCallsStar("main", "third");
        pkb.storeRelationshipCalls("second", "third");

        auto actual = pkb.getCallersStar("third");
        std::vector<PROC_NAME> expected{ "main", "second" };

        Assert::IsTrue(actual == expected);
    }

    TEST_METHOD(getCalleesStar_twoCallees_Success) {
        PKB pkb = PKB();
        pkb.storeRelationshipCalls("main", "second");
        pkb.storeRelationshipCallsStar("main", "third");

        auto actual = pkb.getCalleesStar("main");
        std::vector<PROC_NAME> expected{ "second", "third" };

        Assert::IsTrue(actual == expected);
    }

    TEST_METHOD(getCallsStarPair_oneCallsStarOneCalls_Success) {
        PKB pkb = PKB();
        pkb.storeRelationshipCalls("main", "second");
        pkb.storeRelationshipCallsStar("main", "third");

        auto actual = pkb.getCallsStarPair(DesignEntity::PROCEDURE, DesignEntity::PROCEDURE);
        std::vector<std::pair<PROC_NAME, PROC_NAME>> expected;
        expected.push_back(std::pair<PROC_NAME, PROC_NAME>("main", "second"));
        expected.push_back(std::pair<PROC_NAME, PROC_NAME>("main", "third"));

        Assert::IsTrue(actual == expected);
    }

    TEST_METHOD(getCallsPair_oneCallsStarOneCalls_CallsPairOnly) {
        PKB pkb = PKB();
        pkb.storeRelationshipCalls("main", "second");
        pkb.storeRelationshipCallsStar("main", "third");

        auto actual = pkb.getCallsPair(DesignEntity::PROCEDURE, DesignEntity::PROCEDURE);
        std::vector<std::pair<PROC_NAME, PROC_NAME>> expected;
        expected.push_back(std::pair<PROC_NAME, PROC_NAME>("main", "second"));

        Assert::IsTrue(actual == expected);
    }
};
}   // namespace UnitTesting
