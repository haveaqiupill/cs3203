#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBNextBip) {
public:
    TEST_METHOD(HasNextBipRelationship_NextBipRelationshipExists_True) {
        /* while (i == j) {
        *     i = i + 1;
        * } j = i * j;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNextBip(1, 2);
        pkb.storeRelationshipNextBip(2, 1);
        pkb.storeRelationshipNextBip(1, 3);

        Assert::IsTrue(pkb.hasNextBipRelationship());
        Assert::IsTrue(pkb.hasNextBipRelationship(1, 2));
        Assert::IsTrue(pkb.hasNextBipRelationship(1, 3));
        Assert::IsFalse(pkb.hasNextBipRelationship(2, 3));
    }

    TEST_METHOD(HasNextBipLine_NextBipLineExists_True) {
        /* while (i == j) {
        *     i = i + 1;
        * } j = i * j;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNextBip(1, 2);
        pkb.storeRelationshipNextBip(2, 1);
        pkb.storeRelationshipNextBip(1, 3);

        Assert::IsTrue(pkb.hasNextBipLine(1));
        Assert::IsTrue(pkb.hasNextBipLine(2));
        Assert::IsFalse(pkb.hasNextBipLine(3));
    }

    TEST_METHOD(HasPrevBipLine_PrevBipLineExists_True) {
        /* while (i == j) {
        *     i = i + 1;
        * } j = i * j;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNextBip(1, 2);
        pkb.storeRelationshipNextBip(2, 1);
        pkb.storeRelationshipNextBip(1, 3);

        Assert::IsTrue(pkb.hasPrevBipLine(1));
        Assert::IsTrue(pkb.hasPrevBipLine(2));
        Assert::IsTrue(pkb.hasPrevBipLine(3));
    }

    TEST_METHOD(getNextBipLines_LineNumAndDesignEntity_Success) {
        /* while (i == j) {
        *     i = i + 1;
        * } print i;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNextBip(1, 2);
        pkb.storeRelationshipNextBip(2, 1);
        pkb.storeRelationshipNextBip(1, 3);
        pkb.storeAssignStatement(2, "i", "+ i 1");
        pkb.storeWhileStatement(1, std::list<VAR_NAME> {"i", "j"});
        pkb.storePrintStatement(3, "i");
        pkb.propagateStmtList(3);
        pkb.sortAllDataStructure();

        auto actual = pkb.getNextBipLines(1, DesignEntity::PRINT);
        std::vector<LINE_NUM> expected{ 3 };
        Assert::IsTrue(actual == expected);

        actual = pkb.getNextBipLines(1, DesignEntity::STMT);
        std::vector<LINE_NUM> expected2{ 2, 3 };
        Assert::IsTrue(actual == expected2);
    }

    TEST_METHOD(getPrevBipLines_LineNumAndDesignEntity_Success) {
        /* while (i == j) {
        *     i = i + 1;
        * } print i;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNextBip(1, 2);
        pkb.storeRelationshipNextBip(2, 1);
        pkb.storeRelationshipNextBip(1, 3);
        pkb.storeAssignStatement(2, "i", "+ i 1");
        pkb.storeWhileStatement(1, std::list<VAR_NAME> {"i", "j"});
        pkb.storePrintStatement(3, "i");
        pkb.propagateStmtList(3);
        pkb.sortAllDataStructure();

        auto actual = pkb.getPrevBipLines(DesignEntity::WHILE, 2);
        std::vector<LINE_NUM> expected{ 1 };
        Assert::IsTrue(actual == expected);

        actual = pkb.getPrevBipLines(DesignEntity::STMT, 3);
        Assert::IsTrue(actual == expected);
    }

    TEST_METHOD(getNextBipLinesPair_DesignEntityAndDesignEntity_Success) {
        /* while (i == j) {
        *     i = i + 1;
        * } print i;
        */
        PKB pkb = PKB();
        pkb.storeRelationshipNextBip(1, 2);
        pkb.storeRelationshipNextBip(2, 1);
        pkb.storeRelationshipNextBip(1, 3);
        pkb.storeAssignStatement(2, "i", "+ i 1");
        pkb.storeWhileStatement(1, std::list<VAR_NAME> {"i", "j"});
        pkb.storePrintStatement(3, "i");
        pkb.propagateStmtList(3);
        pkb.sortAllDataStructure();

        auto actual = pkb.getNextBipLinesPair(DesignEntity::WHILE, DesignEntity::PRINT);
        std::vector<std::pair<LINE_NUM, LINE_NUM>> expected{ std::pair<LINE_NUM, LINE_NUM>(1, 3) };
        Assert::IsTrue(actual == expected);

        auto actual2 = pkb.getNextBipLinesPair(DesignEntity::STMT, DesignEntity::STMT);
        std::vector<std::pair<LINE_NUM, LINE_NUM>> expected2{
            std::pair<LINE_NUM, LINE_NUM>(1, 2),
            std::pair<LINE_NUM, LINE_NUM>(1, 3),
            std::pair<LINE_NUM, LINE_NUM>(2, 1)
        };
        Assert::IsTrue(actual2 == expected2);
    }
    };
}  // namespace UnitTesting
