#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBParent) {
    public:
        TEST_METHOD(DoesParent_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesParent(1, 2));
        }
        TEST_METHOD(DoesParent_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesParent(1, 3));
        }
        TEST_METHOD(DoesParentStar_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipParentStar(1, 5);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesParentStar(1, 5));
        }
        TEST_METHOD(DoesParentStar_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipParentStar(1, 5);
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesParentStar(1, 6));
        }
        TEST_METHOD(GetChildLineFromParent_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getChildLineFromParent(1);
            std::vector<LINE_NUM> expected{ 2, 3 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetChildLineFromParent_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getChildLineFromParent(2);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetChildLineFromParentStar_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParentStar(1, 5);
            pkb.storeRelationshipParentStar(1, 6);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getChildLineFromParentStar(1);
            std::vector<LINE_NUM> expected{ 2, 5, 6 };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetChildLineFromParentStar_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipParentStar(1, 5);
            pkb.storeRelationshipParentStar(1, 6);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getChildLineFromParentStar(5);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetParentLineFromParent_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actualTwo = pkb.getParentLineFromParent(2);
            std::vector<LINE_NUM> expectedTwo{ 1 };
            Assert::IsTrue(actualTwo == expectedTwo);

            std::vector<LINE_NUM> actualThree = pkb.getParentLineFromParent(3);
            std::vector<LINE_NUM> expectedThree{ 1 };
            Assert::IsTrue(actualThree == expectedThree);
        }
        TEST_METHOD(GetParentLineFromParent_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getParentLineFromParent(1);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetParentLineFromParentStar_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 5);
            pkb.storeRelationshipParent(5, 6);
            pkb.storeRelationshipParentStar(1, 6);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actualFive = pkb.getParentLineFromParentStar(5);
            std::vector<LINE_NUM> expectedFive{ 1 };
            Assert::IsTrue(actualFive == expectedFive);

            std::vector<LINE_NUM> actualSix = pkb.getParentLineFromParentStar(6);
            std::vector<LINE_NUM> expectedSix{ 1, 5 };
            Assert::IsTrue(actualSix == expectedSix);
        }
        TEST_METHOD(GetParentLineFromParentStar_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipParentStar(1, 5);
            pkb.storeRelationshipParentStar(1, 6);
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getParentLineFromParentStar(1);
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(IsParent_ValidParent_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.isParent(1));
        }
        TEST_METHOD(IsParent_InvalidParent_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.isParent(2));
        }
        TEST_METHOD(HasParentRelationship_HasParent_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipParent(1, 2);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasParentRelationship());
        }
        TEST_METHOD(HasParentRelationship_NoParent_False) {
            PKB pkb = PKB();
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.hasParentRelationship());
        }
        TEST_METHOD(GetParentPair_ChildMoreThanParent_Success) {
            // Sample query: Select w such that Parent(w, a)
            PKB pkb = PKB();
            // Populate assignment datastructure
            pkb.storeAssignStatement(2, "x", "+ 1  2");
            pkb.storeAssignStatement(3, "x", "+ x  1");


            // Populate while statement
            pkb.storeWhileStatement(1, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);

            // Some irrelevant data
            pkb.storeIfElseStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeAssignStatement(5, "y", "x");

            pkb.sortAllDataStructure();

            auto result = pkb.getParentPair(DesignEntity::WHILE, DesignEntity::ASSIGN);
            std::vector<std::pair<LINE_NUM, LINE_NUM>> expected;
            expected.push_back({ 1, 2 });
            expected.push_back({ 1, 3 });

            Assert::IsTrue(result == expected);
        }

        TEST_METHOD(GetParentPair_ParentMoreThanChild_Success) {
            // Sample query: Select w such that Parent(w, a)
            PKB pkb = PKB();
            // Populate assignment datastructure
            pkb.storeAssignStatement(2, "x", "+ 1 2");
            pkb.storeAssignStatement(3, "x", "+ x 1");

            // Populate while statement
            pkb.storeWhileStatement(1, std::list<VAR_NAME> {"x"});
            pkb.storeWhileStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeWhileStatement(5, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.getAllWhileStatements().size() >
                pkb.getAllAssignStatements().size());

            auto result = pkb.getParentPair(DesignEntity::WHILE, DesignEntity::ASSIGN);
            std::vector<std::pair<LINE_NUM, LINE_NUM>> expected;
            expected.push_back({ 1, 2 });
            expected.push_back({ 1, 3 });

            Assert::IsTrue(result == expected);
        }

        TEST_METHOD(GetParentAfter_LineNumIf_Success) {
            // Sample query: Select ifs such that Parent(1, ifs)
            PKB pkb = PKB();

            // Populate while statement
            pkb.storeWhileStatement(1, std::list<VAR_NAME> {"x"});

            pkb.storeIfElseStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeIfElseStatement(4, std::list<VAR_NAME> {"x"});
            pkb.storeIfElseStatement(6, std::list<VAR_NAME> {"x"});
            pkb.storeIfElseStatement(8, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipParent(1, 6);
            pkb.sortAllDataStructure();


            auto result = pkb.getParentAfter(1, DesignEntity::IF);
            std::vector<LINE_NUM> expected{ 2, 4, 6 };

            Assert::IsTrue(result == expected);
        }

        TEST_METHOD(GetParentAfter_InvalidParent_EmptyVector) {
            // Sample query: Select ifs such that Parent(1, ifs)
            PKB pkb = PKB();

            // Populate while statement
            pkb.storeReadStatement(1, "hello");

            pkb.storeIfElseStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeIfElseStatement(3, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipParent(2, 3);
            pkb.sortAllDataStructure();


            auto result = pkb.getParentAfter(1, DesignEntity::IF);
            std::vector<LINE_NUM> expected;

            Assert::IsTrue(result == expected);
        }

        TEST_METHOD(GetParentAfter_Read_Success) {
            // Sample query: Select r such that Parent(_, r)
            PKB pkb = PKB();

            // Populate while statement
            pkb.storeWhileStatement(1, std::list<VAR_NAME> {"x"});

            pkb.storeReadStatement(2, "x");
            pkb.storePrintStatement(3, "y");
            pkb.storeReadStatement(4, "z");
            pkb.storePrintStatement(5, "w");
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(1, 4);
            pkb.sortAllDataStructure();


            auto result = pkb.getParentAfter(DesignEntity::READ);
            std::vector<LINE_NUM> expected{ 2, 4 };

            Assert::IsTrue(result == expected);
        }
        TEST_METHOD(GetParentBefore_InvalidLineNum_EmptyVector) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(2, "x", "+ x y");
            pkb.storeIfElseStatement(3, std::list<VAR_NAME> {"x"});
            pkb.sortAllDataStructure();
            auto result = pkb.getParentBefore(DesignEntity::IF, 2);

            Assert::IsTrue(result == std::vector<LINE_NUM>());
        }
        TEST_METHOD(GetParentBefore_LineNumIf_Success) {
            PKB pkb = PKB();
            pkb.storeAssignStatement(3, "x", "+ x y");
            pkb.storeIfElseStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipParent(2, 3);
            pkb.sortAllDataStructure();
            auto result = pkb.getParentBefore(DesignEntity::IF, 3);

            Assert::IsTrue(result == std::vector<LINE_NUM>{2});
        }
        TEST_METHOD(GetParentStarBefore_LineNumIf_Success) {
            PKB pkb = PKB();
            pkb.storeReadStatement(3, "x");
            pkb.storeIfElseStatement(1, std::list<VAR_NAME> {"x"});
            pkb.storeIfElseStatement(2, std::list<VAR_NAME> {"x"});
            pkb.storeRelationshipParentStar(1, 3);
            pkb.storeRelationshipParent(2, 3);
            pkb.sortAllDataStructure();
            auto result = pkb.getParentStarBefore(DesignEntity::IF, 3);

            Assert::IsTrue(result == std::vector<LINE_NUM>{1, 2});
        }
        TEST_METHOD(GetParentStarAfter_LineNumStatement_Success) {
            /* while (x != 0) {
            *     x = x - 1;
            *     if (x % 2 == 0) {
            *          x = x / 2; }}
            *  return x;
            */
            PKB pkb = PKB();
            pkb.propagateStmtList(5);
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParentStar(1, 4);
            pkb.sortAllDataStructure();
            auto result = pkb.getParentStarAfter(1, DesignEntity::STMT);

            Assert::IsTrue(result == std::vector<LINE_NUM>{2, 3, 4});
        }
    };
}  // namespace UnitTesting
