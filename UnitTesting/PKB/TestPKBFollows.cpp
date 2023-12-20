#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBFollows) {
    public:
        TEST_METHOD(DoesFollows_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollows(1, 2));
        }
        TEST_METHOD(DoesFollows_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesFollows(1, 3));
        }
        TEST_METHOD(DoesFollowsStar_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollowsStar(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(1, 2));
        }
        TEST_METHOD(DoesFollowsStar_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollowsStar(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesFollowsStar(1, 3));
        }
        TEST_METHOD(GetLineAfterFromFollows_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineAfterFromFollows(1);
            std::vector<LINE_NUM> expected{ 2 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineAfterFromFollows_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineAfterFromFollows(2);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineAfterFromFollowsStar_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollowsStar(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineAfterFromFollowsStar(1);
            std::vector<LINE_NUM> expected{ 2, 3 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineAfterFromFollowsStar_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollowsStar(1, 2);
            pkb.storeRelationshipFollowsStar(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineAfterFromFollowsStar(2);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineBeforeFromFollows_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineBeforeFromFollows(2);
            std::vector<LINE_NUM> expected{ 1 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineBeforeFromFollows_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineBeforeFromFollows(4);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineBeforeFromFollowsStar_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollowsStar(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actualTwo = pkb.getLineBeforeFromFollowsStar(2);
            std::vector<LINE_NUM> expectedTwo{ 1 };
            Assert::IsTrue(actualTwo == expectedTwo);

            std::vector<LINE_NUM> actualThree = pkb.getLineBeforeFromFollowsStar(3);
            std::vector<LINE_NUM> expectedThree{ 1, 2 };
            Assert::IsTrue(actualThree == expectedThree);
        }
        TEST_METHOD(GetLineBeforeFromFollowsStar_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollowsStar(1, 2);
            pkb.storeRelationshipFollowsStar(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineBeforeFromFollowsStar(4);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(HasFollower_ValidLineNumber_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasFollower(1));
        }
        TEST_METHOD(HasFollower_InvalidLineNumber_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.hasFollower(2));
        }
        TEST_METHOD(HasPrev_ValidLineNumber_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasFollowee(2));
        }
        TEST_METHOD(HasPrev_InvalidLineNumber_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.hasFollowee(1));
        }

        TEST_METHOD(GetFollowsAfter_LineNumWhile_Success) {
            PKB pkb = PKB();
            pkb.storeWhileStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeWhileStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 4);
            pkb.sortAllDataStructure();

            auto actual = pkb.getFollowsAfter(1, DesignEntity::WHILE);
            std::vector<LINE_NUM> expected{ 2 };
            Assert::IsTrue(actual == expected);
        }

        TEST_METHOD(GetFollowsAfter_IncorrectDesignEntity_Empty) {
            PKB pkb = PKB();
            pkb.storeWhileStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeWhileStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 4);
            pkb.sortAllDataStructure();

            auto actual = pkb.getFollowsAfter(1, DesignEntity::IF);
            std::vector<LINE_NUM> expected;
            Assert::IsTrue(actual == expected);
        }

        TEST_METHOD(GetFollowsAfter_While_Success) {
            PKB pkb = PKB();
            pkb.storeWhileStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeWhileStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 4);
            pkb.sortAllDataStructure();

            auto actual = pkb.getFollowsAfter(DesignEntity::WHILE);
            std::vector<LINE_NUM> expected{ 2, 4 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetFollowsBefore_If_Success) {
            PKB pkb = PKB();
            pkb.storeIfElseStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeIfElseStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 4);
            pkb.sortAllDataStructure();

            auto actual = pkb.getFollowsBefore(DesignEntity::IF);
            std::vector<LINE_NUM> expected{ 2 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetFollowsBefore_IfLineNum_Success) {
            PKB pkb = PKB();
            pkb.storeIfElseStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeIfElseStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 4);
            pkb.sortAllDataStructure();

            auto actual = pkb.getFollowsBefore(DesignEntity::IF, 4);
            std::vector<LINE_NUM> expected{ 2 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(HasFollowsRelationship_FollowsExist_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasFollowsRelationship());
        }
        TEST_METHOD(getFollowsStarBefore_ReadLineNum_Success) {
            PKB pkb = PKB();
            pkb.storeReadStatement(1, "x");
            pkb.storePrintStatement(2, "y");
            pkb.storeRelationshipFollowsStar(1, 3);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            auto result = pkb.getFollowsStarBefore(DesignEntity::READ, 3);
            std::vector<LINE_NUM> expected{ 1 };
            Assert::IsTrue(result == expected);
        }
    };
}  // namespace UnitTesting
