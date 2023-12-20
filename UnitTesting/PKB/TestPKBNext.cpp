#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBNext) {
public:
    TEST_METHOD(HasNextRelationship_NextRelationshipExists_True) {
        /* while (i == j) {
        *     i = i + 1;
        * } j = i * j;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 1);
        pkb.storeRelationshipNext(1, 3);

        Assert::IsTrue(pkb.hasNextRelationship());
        Assert::IsTrue(pkb.hasNextRelationship(1, 2));
        Assert::IsTrue(pkb.hasNextRelationship(1, 3));
        Assert::IsFalse(pkb.hasNextRelationship(2, 3));
    }

    TEST_METHOD(HasNextLine_NextLineExists_True) {
        /* while (i == j) {
        *     i = i + 1;
        * } j = i * j;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 1);
        pkb.storeRelationshipNext(1, 3);

        Assert::IsTrue(pkb.hasNextLine(1));
        Assert::IsTrue(pkb.hasNextLine(2));
        Assert::IsFalse(pkb.hasNextLine(3));
    }

    TEST_METHOD(HasPrevLine_PrevLineExists_True) {
        /* while (i == j) {
        *     i = i + 1;
        * } j = i * j;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 1);
        pkb.storeRelationshipNext(1, 3);

        Assert::IsTrue(pkb.hasPrevLine(1));
        Assert::IsTrue(pkb.hasPrevLine(2));
        Assert::IsTrue(pkb.hasPrevLine(3));
    }

    TEST_METHOD(getNextLines_LineNumAndDesignEntity_Success) {
        /* while (i == j) {
        *     i = i + 1;
        * } print i;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 1);
        pkb.storeRelationshipNext(1, 3);
        pkb.storeAssignStatement(2, "i", "+ i 1");
        pkb.storeWhileStatement(1, std::list<VAR_NAME> {"i", "j"});
        pkb.storePrintStatement(3, "i");
        pkb.propagateStmtList(3);
        pkb.sortAllDataStructure();

        auto actual = pkb.getNextLines(1, DesignEntity::PRINT);
        std::vector<LINE_NUM> expected{ 3 };
        Assert::IsTrue(actual == expected);

        actual = pkb.getNextLines(1, DesignEntity::STMT);
        std::vector<LINE_NUM> expected2{ 2, 3 };
        Assert::IsTrue(actual == expected2);
    }

    TEST_METHOD(getPrevLines_LineNumAndDesignEntity_Success) {
        /* while (i == j) {
        *     i = i + 1;
        * } print i;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 1);
        pkb.storeRelationshipNext(1, 3);
        pkb.storeAssignStatement(2, "i", "+ i 1");
        pkb.storeWhileStatement(1, std::list<VAR_NAME> {"i", "j"});
        pkb.storePrintStatement(3, "i");
        pkb.propagateStmtList(3);
        pkb.sortAllDataStructure();

        auto actual = pkb.getPrevLines(DesignEntity::WHILE, 2);
        std::vector<LINE_NUM> expected{ 1 };
        Assert::IsTrue(actual == expected);

        actual = pkb.getPrevLines(DesignEntity::STMT, 3);
        Assert::IsTrue(actual == expected);
    }

    TEST_METHOD(getNextLinesPair_DesignEntityAndDesignEntity_Success) {
        /* while (i == j) {
        *     i = i + 1;
        * } print i;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 1);
        pkb.storeRelationshipNext(1, 3);
        pkb.storeAssignStatement(2, "i", "+ i 1");
        pkb.storeWhileStatement(1, std::list<VAR_NAME> {"i", "j"});
        pkb.storePrintStatement(3, "i");
        pkb.propagateStmtList(3);
        pkb.sortAllDataStructure();

        auto actual = pkb.getNextLinesPair(DesignEntity::WHILE, DesignEntity::PRINT);
        std::vector<std::pair<LINE_NUM, LINE_NUM>> expected{std::pair<LINE_NUM, LINE_NUM> (1, 3)};
        Assert::IsTrue(actual == expected);

        auto actual2 = pkb.getNextLinesPair(DesignEntity::STMT, DesignEntity::STMT);
        std::vector<std::pair<LINE_NUM, LINE_NUM>> expected2{
            std::pair<LINE_NUM, LINE_NUM>(1, 2),
            std::pair<LINE_NUM, LINE_NUM>(1, 3),
            std::pair<LINE_NUM, LINE_NUM>(2, 1)
        };
        Assert::IsTrue(actual2 == expected2);
    }
};
}  // namespace UnitTesting
