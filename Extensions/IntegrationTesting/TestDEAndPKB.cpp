#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"
#include "DesignExtractor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PkbAndDeIntegrationTesting {
    std::vector<LINE_NUM> retrieveParentStmt(PKB& pkb) {
        std::vector<LINE_NUM> parentLst = std::vector<LINE_NUM>();
        std::vector<LINE_NUM> whileLst = pkb.getAllWhileStatements();
        std::vector<LINE_NUM> ifLst = pkb.getAllIfStatements();
        parentLst.insert(parentLst.end(), whileLst.begin(), whileLst.end());
        parentLst.insert(parentLst.end(), ifLst.begin(), ifLst.end());
        return parentLst;
    }

    TEST_CLASS(TestExtractFollowsStar) {
        TEST_METHOD(ExtractFollowsStar_OneParentThreeChildren_ThreeFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollows(3, 4);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(2, 3));
            Assert::IsTrue(pkb.doesFollowsStar(2, 4));
            Assert::IsTrue(pkb.doesFollowsStar(3, 4));

            Assert::IsFalse(pkb.doesFollowsStar(3, 2));
            Assert::IsFalse(pkb.doesFollowsStar(4, 2));
            Assert::IsFalse(pkb.doesFollowsStar(4, 3));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3, 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 2 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4) == std::vector<LINE_NUM>{ 2, 3 });
        }

        TEST_METHOD(ExtractFollowsStar_OneParentTwoChildren_OneFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(2, 3));
            Assert::IsFalse(pkb.doesFollowsStar(3, 2));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 2 });
        }

        TEST_METHOD(ExtractFollowsStar_MultipleParents_MutuallyDisjointFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollows(3, 4);
            pkb.storeRelationshipParent(5, 6);
            pkb.storeRelationshipParent(5, 7);
            pkb.storeRelationshipParent(5, 8);
            pkb.storeRelationshipFollows(6, 7);
            pkb.storeRelationshipFollows(7, 8);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.storeWhileStatement(5, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesFollowsStar(2, 6));
            Assert::IsFalse(pkb.doesFollowsStar(2, 7));
            Assert::IsFalse(pkb.doesFollowsStar(2, 8));
            Assert::IsFalse(pkb.doesFollowsStar(3, 6));
            Assert::IsFalse(pkb.doesFollowsStar(3, 7));
            Assert::IsFalse(pkb.doesFollowsStar(3, 8));
            Assert::IsFalse(pkb.doesFollowsStar(4, 6));
            Assert::IsFalse(pkb.doesFollowsStar(4, 7));
            Assert::IsFalse(pkb.doesFollowsStar(4, 8));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3, 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 2 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4) == std::vector<LINE_NUM>{ 2, 3 });

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(5).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(6) == std::vector<LINE_NUM>{ 7, 8 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(7) == std::vector<LINE_NUM>{ 8 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(8).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(5).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(6).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(7) == std::vector<LINE_NUM>{ 6 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(8) == std::vector<LINE_NUM>{ 6, 7 });
        }

        TEST_METHOD(ExtractFollowsStar_OneChildAlsoParent_ThreeFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollows(3, 4);
            pkb.storeRelationshipParent(4, 5);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.storeWhileStatement(4, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(2, 3));
            Assert::IsTrue(pkb.doesFollowsStar(2, 4));
            Assert::IsTrue(pkb.doesFollowsStar(3, 4));

            Assert::IsFalse(pkb.doesFollowsStar(3, 2));
            Assert::IsFalse(pkb.doesFollowsStar(4, 2));
            Assert::IsFalse(pkb.doesFollowsStar(4, 3));
            Assert::IsFalse(pkb.doesFollowsStar(4, 5));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3, 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(5).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 2 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4) == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(5).size() == 0);
        }

        TEST_METHOD(ExtractFollowsStar_NestedParent_ManyFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(3, 4);
            pkb.storeRelationshipParent(3, 5);
            pkb.storeRelationshipParent(3, 6);
            pkb.storeRelationshipParent(1, 7);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollows(4, 5);
            pkb.storeRelationshipFollows(5, 6);
            pkb.storeRelationshipFollows(3, 7);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.storeWhileStatement(3, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(2, 3));
            Assert::IsTrue(pkb.doesFollowsStar(2, 7));
            Assert::IsTrue(pkb.doesFollowsStar(3, 7));
            Assert::IsTrue(pkb.doesFollowsStar(4, 5));
            Assert::IsTrue(pkb.doesFollowsStar(4, 6));
            Assert::IsTrue(pkb.doesFollowsStar(5, 6));

            // Boundary crosses from/into nested parent
            Assert::IsFalse(pkb.doesFollowsStar(3, 4));
            Assert::IsFalse(pkb.doesFollowsStar(6, 7));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3, 7 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3) == std::vector<LINE_NUM>{ 7 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4) == std::vector<LINE_NUM>{ 5, 6 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(5) == std::vector<LINE_NUM>{ 6 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(6).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(7).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 2 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(5) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(6) == std::vector<LINE_NUM>{ 4, 5 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(7) == std::vector<LINE_NUM>{ 2, 3 });
        }

        TEST_METHOD(ExtractFollowsStar_IfStmt_DisjointFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipParent(1, 5);
            pkb.storeRelationshipParent(1, 6);
            pkb.storeRelationshipFollows(2, 3);  // Then block from 2 to 4
            pkb.storeRelationshipFollows(3, 4);
            pkb.storeRelationshipFollows(5, 6);  // Else block from 5 to 6
            pkb.storeIfElseStatement(1, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(2, 3));
            Assert::IsTrue(pkb.doesFollowsStar(2, 4));
            Assert::IsTrue(pkb.doesFollowsStar(3, 4));
            Assert::IsTrue(pkb.doesFollowsStar(5, 6));

            Assert::IsFalse(pkb.doesFollowsStar(4, 5));  // between then and else blocks

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3, 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(5) == std::vector<LINE_NUM>{ 6 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(6).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 2 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4) == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(5).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(6) == std::vector<LINE_NUM>{ 5 });
        }

        TEST_METHOD(ExtractFollowsStar_IfStmtOneLine_NoExceptionsThrown) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeIfElseStatement(1, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesFollowsStar(2, 3));
        }
    };

    TEST_CLASS(TestExtractFollowsStarNoParent) {
        TEST_METHOD(ExtractFollowsStarNoParent_ThreeStmt_ThreeFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeProcedure("main", 1, 3, {});
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStarNoParent();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(1, 2));
            Assert::IsTrue(pkb.doesFollowsStar(1, 3));
            Assert::IsTrue(pkb.doesFollowsStar(2, 3));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1) == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 1, 2 });
        }

        TEST_METHOD(ExtractFollowsStarNoParent_StmtIsParent_ThreeFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipParent(2, 3);  // Statement 2 is a parent
            pkb.storeRelationshipFollows(2, 4);
            pkb.storeProcedure("main", 1, 4, {});
            pkb.storeWhileStatement(2, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStarNoParent();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(1, 2));
            Assert::IsTrue(pkb.doesFollowsStar(1, 4));
            Assert::IsTrue(pkb.doesFollowsStar(2, 4));

            Assert::IsFalse(pkb.doesFollowsStar(2, 3));
            Assert::IsFalse(pkb.doesFollowsStar(3, 4));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1) == std::vector<LINE_NUM>{ 2, 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4) == std::vector<LINE_NUM>{ 1, 2 });
        }

        TEST_METHOD(ExtractFollowsStarNoParent_OnlyOneStmt_OneFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipFollows(1, 4);
            pkb.storeProcedure("main", 1, 4, {});
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStarNoParent();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(1, 4));

            Assert::IsFalse(pkb.doesFollowsStar(1, 2));
            Assert::IsFalse(pkb.doesFollowsStar(1, 3));
            Assert::IsFalse(pkb.doesFollowsStar(2, 4));
            Assert::IsFalse(pkb.doesFollowsStar(3, 4));

            // Don't care about relationships inside parent statement because PKB will remove
            // duplicate entries in the same table

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4) == std::vector<LINE_NUM>{ 1 });
        }

        TEST_METHOD(ExtractFollowsStarNoParent_MultipleProcs_DisjointFollowsStar) {
            PKB pkb;
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollows(4, 5);
            pkb.storeRelationshipFollows(5, 7);
            pkb.storeRelationshipParent(5, 6);
            pkb.storeWhileStatement(5, std::list<VAR_NAME>());
            pkb.storeProcedure("main", 1, 3, {});
            pkb.storeProcedure("secondary", 4, 7, {});
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractFollowsStarNoParent();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesFollowsStar(1, 2));
            Assert::IsTrue(pkb.doesFollowsStar(1, 3));
            Assert::IsTrue(pkb.doesFollowsStar(2, 3));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(1) == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(2) == std::vector<LINE_NUM>{ 3 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(3).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(1).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(2) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(3) == std::vector<LINE_NUM>{ 1, 2 });

            Assert::IsTrue(pkb.doesFollowsStar(4, 5));
            Assert::IsTrue(pkb.doesFollowsStar(4, 7));
            Assert::IsTrue(pkb.doesFollowsStar(5, 7));

            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(4) == std::vector<LINE_NUM>{ 5, 7 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(5) == std::vector<LINE_NUM>{ 7 });
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(6).size() == 0);
            Assert::IsTrue(pkb.getLineAfterFromFollowsStar(7).size() == 0);

            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(4).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(5) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(6).size() == 0);
            Assert::IsTrue(pkb.getLineBeforeFromFollowsStar(7) == std::vector<LINE_NUM>{ 4, 5 });
        }
    };

    TEST_CLASS(TestExtractParentStar) {
        TEST_METHOD(ExtractParentStar_ParentOfParent_OneParentStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(2, 3);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.storeWhileStatement(2, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractParentStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesParentStar(1, 2));
            Assert::IsTrue(pkb.doesParentStar(1, 3));
            Assert::IsTrue(pkb.doesParentStar(2, 3));

            Assert::IsTrue(pkb.getChildLineFromParentStar(1) == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(2) == std::vector<LINE_NUM>{ 3 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(3).size() == 0);

            Assert::IsTrue(pkb.getParentLineFromParentStar(1).size() == 0);
            Assert::IsTrue(pkb.getParentLineFromParentStar(2) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(3) == std::vector<LINE_NUM>{ 1, 2 });
        }

        TEST_METHOD(ExtractParentStar_NestedParents_ManyParentStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(2, 3);
            pkb.storeRelationshipParent(3, 4);
            pkb.storeRelationshipParent(4, 5);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.storeWhileStatement(2, std::list<VAR_NAME>());
            pkb.storeWhileStatement(3, std::list<VAR_NAME>());
            pkb.storeWhileStatement(4, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractParentStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesParentStar(1, 2));
            Assert::IsTrue(pkb.doesParentStar(1, 3));
            Assert::IsTrue(pkb.doesParentStar(1, 4));
            Assert::IsTrue(pkb.doesParentStar(1, 5));
            Assert::IsTrue(pkb.doesParentStar(2, 3));
            Assert::IsTrue(pkb.doesParentStar(2, 4));
            Assert::IsTrue(pkb.doesParentStar(2, 5));
            Assert::IsTrue(pkb.doesParentStar(3, 4));
            Assert::IsTrue(pkb.doesParentStar(3, 5));
            Assert::IsTrue(pkb.doesParentStar(4, 5));

            Assert::IsTrue(pkb.getChildLineFromParentStar(1)
                           == std::vector<LINE_NUM>{ 2, 3, 4, 5 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(2) == std::vector<LINE_NUM>{ 3, 4, 5 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(3) == std::vector<LINE_NUM>{ 4, 5 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(4) == std::vector<LINE_NUM>{ 5 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(5).size() == 0);

            Assert::IsTrue(pkb.getParentLineFromParentStar(1).size() == 0);
            Assert::IsTrue(pkb.getParentLineFromParentStar(2) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(3) == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(4) == std::vector<LINE_NUM>{ 1, 2, 3 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(5)
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4 });
        }

        TEST_METHOD(ExtractParentStar_SplitLineage_AllHaveCommonAncestor) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(2, 3);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipParent(4, 5);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.storeWhileStatement(2, std::list<VAR_NAME>());
            pkb.storeWhileStatement(4, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractParentStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesParentStar(1, 2));
            Assert::IsTrue(pkb.doesParentStar(1, 3));
            Assert::IsTrue(pkb.doesParentStar(1, 4));
            Assert::IsTrue(pkb.doesParentStar(1, 5));
            Assert::IsTrue(pkb.doesParentStar(2, 3));
            Assert::IsTrue(pkb.doesParentStar(4, 5));

            Assert::IsFalse(pkb.doesParentStar(2, 4));
            Assert::IsFalse(pkb.doesParentStar(2, 5));
            Assert::IsFalse(pkb.doesParentStar(3, 4));
            Assert::IsFalse(pkb.doesParentStar(3, 5));

            Assert::IsTrue(pkb.getChildLineFromParentStar(1)
                           == std::vector<LINE_NUM>{ 2, 3, 4, 5 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(2) == std::vector<LINE_NUM>{ 3 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(3).size() == 0);
            Assert::IsTrue(pkb.getChildLineFromParentStar(4) == std::vector<LINE_NUM>{ 5 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(5).size() == 0);

            Assert::IsTrue(pkb.getParentLineFromParentStar(1).size() == 0);
            Assert::IsTrue(pkb.getParentLineFromParentStar(2) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(3) == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(4) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(5) == std::vector<LINE_NUM>{ 1, 4 });
        }

        TEST_METHOD(ExtractParentStar_DisjointParents_DisjointParentStar) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(2, 3);
            pkb.storeRelationshipParent(4, 5);
            pkb.storeRelationshipParent(5, 6);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.storeWhileStatement(2, std::list<VAR_NAME>());
            pkb.storeWhileStatement(4, std::list<VAR_NAME>());
            pkb.storeWhileStatement(5, std::list<VAR_NAME>());
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractParentStar(retrieveParentStmt(pkb));
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesParentStar(1, 2));
            Assert::IsTrue(pkb.doesParentStar(1, 3));
            Assert::IsTrue(pkb.doesParentStar(2, 3));
            Assert::IsTrue(pkb.doesParentStar(4, 5));
            Assert::IsTrue(pkb.doesParentStar(4, 6));
            Assert::IsTrue(pkb.doesParentStar(5, 6));

            Assert::IsTrue(pkb.getChildLineFromParentStar(1) == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(2) == std::vector<LINE_NUM>{ 3 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(3).size() == 0);
            Assert::IsTrue(pkb.getChildLineFromParentStar(4) == std::vector<LINE_NUM>{ 5, 6 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(5) == std::vector<LINE_NUM>{ 6 });
            Assert::IsTrue(pkb.getChildLineFromParentStar(6).size() == 0);

            Assert::IsTrue(pkb.getParentLineFromParentStar(1).size() == 0);
            Assert::IsTrue(pkb.getParentLineFromParentStar(2) == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(3) == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(4).size() == 0);
            Assert::IsTrue(pkb.getParentLineFromParentStar(5) == std::vector<LINE_NUM>{ 4 });
            Assert::IsTrue(pkb.getParentLineFromParentStar(6) == std::vector<LINE_NUM>{ 4, 5 });
        }
    };

    TEST_CLASS(TestExtractModifies) {
        TEST_METHOD(ExtractModifies_OneParentStmt_ParentAlsoModifies) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipModifiesByLineNum(2, "x");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractModifies();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumModifies(1, "x"));

            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x" });

            Assert::IsTrue(pkb.getLineNumFromModifiesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2 });
        }

        TEST_METHOD(ExtractModifies_NestedStmt_AllParentsModifies) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(2, 3);
            pkb.storeRelationshipParent(3, 4);
            pkb.storeRelationshipParent(4, 5);
            pkb.storeRelationshipParentStar(1, 5);
            pkb.storeRelationshipParentStar(2, 5);
            pkb.storeRelationshipParentStar(3, 5);
            pkb.storeRelationshipParentStar(4, 5);
            pkb.storeRelationshipModifiesByLineNum(5, "x");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractModifies();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumModifies(1, "x"));
            Assert::IsTrue(pkb.doesLineNumModifies(2, "x"));
            Assert::IsTrue(pkb.doesLineNumModifies(3, "x"));
            Assert::IsTrue(pkb.doesLineNumModifies(4, "x"));

            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(2)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(3)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(4)
                           == std::vector<VAR_NAME>{ "x" });

            Assert::IsTrue(pkb.getLineNumFromModifiesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5 });
        }

        TEST_METHOD(ExtractModifies_CommonAncestor_AllVarsInModifies) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipModifiesByLineNum(2, "x");
            pkb.storeRelationshipModifiesByLineNum(3, "y");
            pkb.storeRelationshipModifiesByLineNum(4, "z");
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractModifies();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumModifies(1, "x"));
            Assert::IsTrue(pkb.doesLineNumModifies(1, "y"));
            Assert::IsTrue(pkb.doesLineNumModifies(1, "z"));

            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x", "y", "z" });

            Assert::IsTrue(pkb.getLineNumFromModifiesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getLineNumFromModifiesByVarName("y")
                           == std::vector<LINE_NUM>{ 1, 3 });
            Assert::IsTrue(pkb.getLineNumFromModifiesByVarName("z")
                           == std::vector<LINE_NUM>{ 1, 4 });
        }

        TEST_METHOD(ExtractModifies_DisjointParents_DisjointModifies) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(3, 4);
            pkb.storeRelationshipModifiesByLineNum(2, "x");
            pkb.storeRelationshipModifiesByLineNum(4, "z");
            pkb.storeVariable("x");
            pkb.storeVariable("z");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractModifies();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumModifies(1, "x"));
            Assert::IsTrue(pkb.doesLineNumModifies(3, "z"));

            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromModifiesByLineNum(3)
                           == std::vector<VAR_NAME>{ "z" });

            Assert::IsTrue(pkb.getLineNumFromModifiesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getLineNumFromModifiesByVarName("z")
                           == std::vector<LINE_NUM>{ 3, 4 });
        }
    };

    TEST_CLASS(TestExtractUses) {
        TEST_METHOD(ExtractUses_OneParentStmt_ParentAlsoUses) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipUsesByLineNum(2, "x");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractUses();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "x"));

            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x" });

            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2 });
        }

        TEST_METHOD(ExtractUses_OneStmtManyUses_ParentUsesAll) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipUsesByLineNum(2, "x");
            pkb.storeRelationshipUsesByLineNum(2, "y");
            pkb.storeRelationshipUsesByLineNum(2, "z");
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractUses();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "x"));
            Assert::IsTrue(pkb.doesLineNumUses(1, "y"));
            Assert::IsTrue(pkb.doesLineNumUses(1, "z"));

            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x", "y", "z" });

            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("y")
                           == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("z")
                           == std::vector<LINE_NUM>{ 1, 2 });
        }

        TEST_METHOD(ExtractUses_NestedStmt_AllParentsUses) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(2, 3);
            pkb.storeRelationshipParent(3, 4);
            pkb.storeRelationshipParent(4, 5);
            pkb.storeRelationshipParentStar(1, 5);
            pkb.storeRelationshipParentStar(2, 5);
            pkb.storeRelationshipParentStar(3, 5);
            pkb.storeRelationshipParentStar(4, 5);
            pkb.storeRelationshipUsesByLineNum(5, "x");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractUses();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "x"));
            Assert::IsTrue(pkb.doesLineNumUses(2, "x"));
            Assert::IsTrue(pkb.doesLineNumUses(3, "x"));
            Assert::IsTrue(pkb.doesLineNumUses(4, "x"));

            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(2)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(3)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(4)
                           == std::vector<VAR_NAME>{ "x" });

            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5 });
        }

        TEST_METHOD(ExtractUses_CommonAncestor_AllVarsInUses) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipFollows(3, 4);
            pkb.storeRelationshipParent(1, 4);
            pkb.storeRelationshipUsesByLineNum(2, "x");
            pkb.storeRelationshipUsesByLineNum(3, "y");
            pkb.storeRelationshipUsesByLineNum(4, "z");
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractUses();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "x"));
            Assert::IsTrue(pkb.doesLineNumUses(1, "y"));
            Assert::IsTrue(pkb.doesLineNumUses(1, "z"));

            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x", "y", "z" });

            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("y")
                           == std::vector<LINE_NUM>{ 1, 3 });
            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("z")
                           == std::vector<LINE_NUM>{ 1, 4 });
        }

        TEST_METHOD(ExtractUses_DisjointParents_DisjointUses) {
            PKB pkb;
            pkb.storeRelationshipParent(1, 2);
            pkb.storeRelationshipParent(3, 4);
            pkb.storeRelationshipUsesByLineNum(2, "x");
            pkb.storeRelationshipUsesByLineNum(4, "z");
            pkb.storeVariable("x");
            pkb.storeVariable("z");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractUses();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "x"));
            Assert::IsTrue(pkb.doesLineNumUses(3, "z"));

            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(1)
                           == std::vector<VAR_NAME>{ "x" });
            Assert::IsTrue(pkb.getVariableFromUsesByLineNum(3)
                           == std::vector<VAR_NAME>{ "z" });

            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("x")
                           == std::vector<LINE_NUM>{ 1, 2 });
            Assert::IsTrue(pkb.getLineNumFromUsesByVarName("z")
                           == std::vector<LINE_NUM>{ 3, 4 });
        }
    };

    TEST_CLASS(TestExtractAllPrefixExpr) {
        TEST_METHOD(ExtractAllPrefixExpr_OneSimpleExpr_AllPossibleExpr) {
            PKB pkb;
            pkb.storeAssignStatement(1, "x", "+ + x y z");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractAllPrefixExpr();
            pkb.sortAllDataStructure();

            std::vector<std::pair<LINE_NUM, VAR_NAME>> expected;
            expected.push_back({ 1, "x" });

            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("x") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("y") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("z") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ x y") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ + x y z")
                == expected);
        }

        TEST_METHOD(ExtractAllPrefixExpr_LeftHandExpr_AllPossibleExpr) {
            PKB pkb;
            pkb.storeAssignStatement(1, "x", "+ + + + a b c d e");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractAllPrefixExpr();
            pkb.sortAllDataStructure();

            std::vector<std::pair<LINE_NUM, VAR_NAME>> expected;
            expected.push_back({ 1, "x" });

            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("a") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("b") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("c") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("d") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("e") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ a b") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ + a b c")
                == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ + + a b c d")
                == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ + + + a b c d e")
                == expected);
        }

        TEST_METHOD(ExtractAllPrefixExpr_RightHandExpr_AllPossibleExpr) {
            PKB pkb;
            pkb.storeAssignStatement(1, "x", "+ a + b + c + d e");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractAllPrefixExpr();
            pkb.sortAllDataStructure();

            std::vector<std::pair<LINE_NUM, VAR_NAME>> expected;
            expected.push_back({ 1, "x" });

            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("a") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("b") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("c") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("d") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("e") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ d e") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ c + d e")
                == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ b + c + d e")
                == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ a + b + c + d e")
                == expected);
        }

        TEST_METHOD(ExtractAllPrefixExpr_BalancedTreeExpr_AllPossibleExpr) {
            PKB pkb;
            pkb.storeAssignStatement(1, "x", "+ + a b + c d");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractAllPrefixExpr();
            pkb.sortAllDataStructure();

            std::vector<std::pair<LINE_NUM, VAR_NAME>> expected;
            expected.push_back({ 1, "x" });

            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("a") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("b") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("c") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("d") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ a b") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ c d") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ + a b + c d")
                == expected);

            Assert::IsFalse(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ a b + c")
                == expected);
            Assert::IsFalse(pkb.getAssignLineNumVariableByPartialPrefixExpr("b + c") == expected);
        }

        TEST_METHOD(ExtractAllPrefixExpr_MultipleExpr_AllPossibleExpr) {
            PKB pkb;
            pkb.storeAssignStatement(1, "x", "+ a - b c");
            pkb.storeAssignStatement(2, "x", "* w / x % y z");
            pkb.storeAssignStatement(3, "x", "+ 1 / variable % * print read - ABC 1234");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractAllPrefixExpr();
            pkb.sortAllDataStructure();

            std::vector<std::pair<LINE_NUM, VAR_NAME>> expected;
            expected.push_back({ 1, "x" });

            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("- b c") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("+ a - b c")
                == expected);

            expected.clear();
            expected.push_back({ 2, "x" });

            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("% y z") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("/ x % y z")
                == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("* w / x % y z")
                == expected);

            expected.clear();
            expected.push_back({ 3, "x" });

            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("- ABC 1234")
                == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr("* print read")
                == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr(
                "% * print read - ABC 1234") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr(
                "/ variable % * print read - ABC 1234") == expected);
            Assert::IsTrue(pkb.getAssignLineNumVariableByPartialPrefixExpr(
                "+ 1 / variable % * print read - ABC 1234") == expected);
        }
    };

    TEST_CLASS(TestGetCallStmtLineNums) {
        TEST_METHOD(GetCallStmtLineNums_SimpleValidCall_NoException) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(1, "B");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<LINE_NUM> result = dex.getCallStmtLineNums();
            Assert::IsTrue(result == std::vector<LINE_NUM>{ 1 });
        }

        TEST_METHOD(GetCallStmtLineNums_CallToNonexistentProc_ExceptionThrown) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(1, "C");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            try {
                std::vector<LINE_NUM> result = dex.getCallStmtLineNums();
            } catch (const InvalidSimpleException &ise) {
                std::string error = ise.what();
                Assert::IsTrue(error.compare("Call statement does not refer to an existing "
                                             "procedure at line 1") == 0);
            } catch (std::exception) {
                Assert::Fail();
            }
        }

        TEST_METHOD(GetCallStmtLineNums_MultipleValidProc_NoException) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeProcedure("D", 3, 4, {});
            pkb.storeProcedure("C", 5, 7, {});
            pkb.storeCallStatement(1, "C");
            pkb.storeCallStatement(3, "C");
            pkb.storeCallStatement(4, "C");
            pkb.storeCallStatement(7, "B");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<LINE_NUM> result = dex.getCallStmtLineNums();
            Assert::IsTrue(result == std::vector<LINE_NUM>{ 1, 3, 4, 7 });
        }

        TEST_METHOD(GetCallStmtLineNums_CyclicCall_NoException) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "A");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<LINE_NUM> result = dex.getCallStmtLineNums();
            Assert::IsTrue(result == std::vector<LINE_NUM>{ 1, 2 });
        }
    };

    TEST_CLASS(TestExtractProcsIdxAdjList) {
        TEST_METHOD(ExtractProcsIdxAdjList_SimpleExample_ValidResults) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(1, "B");
            pkb.sortAllDataStructure();
            std::vector<LINE_NUM> callStmtLineNums = std::vector<LINE_NUM>{ 1 };

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<std::vector<int>> procsIdxAdjList
                = dex.extractProcsIdxAdjList(callStmtLineNums);

            std::vector<std::vector<int>> expected;
            expected.push_back(std::vector<int> {});
            expected.push_back(std::vector<int> { 0 });

            Assert::IsTrue(expected == procsIdxAdjList);
        }

        TEST_METHOD(ExtractProcsIdxAdjList_MultipleCallsToSameProc_NoDuplicateEntries) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "B");
            pkb.storeCallStatement(3, "B");
            pkb.storeCallStatement(4, "B");
            pkb.sortAllDataStructure();
            std::vector<LINE_NUM> callStmtLineNums = std::vector<LINE_NUM>{ 1, 2, 3, 4 };

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<std::vector<int>> procsIdxAdjList
                = dex.extractProcsIdxAdjList(callStmtLineNums);

            std::vector<std::vector<int>> expected;
            expected.push_back(std::vector<int> {});
            expected.push_back(std::vector<int> { 0 });

            Assert::IsTrue(expected == procsIdxAdjList);
        }

        TEST_METHOD(ExtractProcsIdxAdjList_ChainedProc_ValidEntries) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeProcedure("C", 3, 3, {});
            pkb.storeProcedure("D", 4, 4, {});
            pkb.storeProcedure("E", 5, 5, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "C");
            pkb.storeCallStatement(3, "D");
            pkb.storeCallStatement(4, "E");
            pkb.sortAllDataStructure();
            std::vector<LINE_NUM> callStmtLineNums = std::vector<LINE_NUM>{ 1, 2, 3, 4 };

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<std::vector<int>> procsIdxAdjList
                = dex.extractProcsIdxAdjList(callStmtLineNums);

            std::vector<std::vector<int>> expected;
            expected.push_back(std::vector<int> {});
            expected.push_back(std::vector<int> { 0 });
            expected.push_back(std::vector<int> { 1 });
            expected.push_back(std::vector<int> { 2 });
            expected.push_back(std::vector<int> { 3 });

            Assert::IsTrue(expected == procsIdxAdjList);
        }

        TEST_METHOD(ExtractProcsIdxAdjList_MultipleCyclicProc_NoException) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeProcedure("C", 6, 6, {});
            pkb.storeProcedure("D", 7, 7, {});
            pkb.storeProcedure("E", 8, 8, {});
            pkb.storeProcedure("F", 9, 9, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "C");
            pkb.storeCallStatement(3, "D");
            pkb.storeCallStatement(4, "E");
            pkb.storeCallStatement(5, "F");
            pkb.storeCallStatement(6, "F");
            pkb.storeCallStatement(7, "F");
            pkb.storeCallStatement(8, "F");
            pkb.storeCallStatement(9, "A");  // Note cyclic loop formed!
            pkb.sortAllDataStructure();
            std::vector<LINE_NUM> callStmtLineNums
                = std::vector<LINE_NUM>{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<std::vector<int>> procsIdxAdjList
                = dex.extractProcsIdxAdjList(callStmtLineNums);

            std::vector<std::vector<int>> expected;
            expected.push_back(std::vector<int> { 5 });
            expected.push_back(std::vector<int> { 0 });
            expected.push_back(std::vector<int> { 0 });
            expected.push_back(std::vector<int> { 0 });
            expected.push_back(std::vector<int> { 0 });
            expected.push_back(std::vector<int> { 1, 2, 3, 4 });

            Assert::IsTrue(expected == procsIdxAdjList);
        }
    };

    TEST_CLASS(TestToposortProcsIdx) {
        int getIndex(const std::vector<int> &idxVector, int element) {
            auto result = std::find(idxVector.begin(), idxVector.end(), element);
            return std::distance(idxVector.begin(), result);
        }

        TEST_METHOD(ToposortProcsIdx_SimpleProcs_ValidSort) {
            PKB pkb;
            std::vector<std::vector<int>> AL;
            AL.push_back(std::vector<int> {});
            AL.push_back(std::vector<int> { 0 });

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<int> sortedIdx = dex.toposortProcsIdx(AL);

            // Proc at index 1 must be processed first before 0
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 1));
        }

        TEST_METHOD(ToposortProcsIdx_CyclicProcs_ThrowException) {
            PKB pkb;
            std::vector<std::vector<int>> AL;
            AL.push_back(std::vector<int> { 1 });
            AL.push_back(std::vector<int> { 0 });

            DesignExtractor dex = DesignExtractor(&pkb, false);
            try {
                std::vector<int> sortedIdx = dex.toposortProcsIdx(AL);
            } catch (InvalidSimpleException &ise) {
                std::string error = ise.what();
                Assert::IsTrue(
                    error.compare("Recursive and/or cyclic call(s) detected in SIMPLE program")
                    == 0);
            } catch (std::exception) {
                Assert::Fail();
            }
        }

        TEST_METHOD(ToposortProcsIdx_RecursiveProcs_ThrowException) {
            PKB pkb;
            std::vector<std::vector<int>> AL;
            AL.push_back(std::vector<int> { 1 });
            AL.push_back(std::vector<int> { 1 });

            DesignExtractor dex = DesignExtractor(&pkb, false);
            try {
                std::vector<int> sortedIdx = dex.toposortProcsIdx(AL);
            }
            catch (InvalidSimpleException& ise) {
                std::string error = ise.what();
                Assert::IsTrue(
                    error.compare("Recursive and/or cyclic call(s) detected in SIMPLE program")
                    == 0);
            }
            catch (std::exception) {
                Assert::Fail();
            }
        }

        TEST_METHOD(ToposortProcsIdx_OneToManyProcs_ValidSort) {
            PKB pkb;
            std::vector<std::vector<int>> AL;
            AL.push_back(std::vector<int> {});
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 1, 2, 3, 4 });

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<int> sortedIdx = dex.toposortProcsIdx(AL);
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 1));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 2));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 3));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 4));
            Assert::IsTrue(getIndex(sortedIdx, 1) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 2) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 3) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 4) > getIndex(sortedIdx, 5));
        }

        TEST_METHOD(ToposortProcsIdx_NotCalledProcs_ValidSort) {
            PKB pkb;
            std::vector<std::vector<int>> AL;
            AL.push_back(std::vector<int> {});
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> {});
            AL.push_back(std::vector<int> { 1, 2, 3, 6});
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> {});

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<int> sortedIdx = dex.toposortProcsIdx(AL);
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 1));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 2));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 3));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 6));
            Assert::IsTrue(getIndex(sortedIdx, 1) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 2) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 3) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 6) > getIndex(sortedIdx, 5));
        }

        TEST_METHOD(ToposortProcsIdx_DisjointSets_ValidSort) {
            PKB pkb;
            std::vector<std::vector<int>> AL;
            AL.push_back(std::vector<int> {});
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> {});
            AL.push_back(std::vector<int> { 1, 2, 3, 6 });
            AL.push_back(std::vector<int> { 0 });
            AL.push_back(std::vector<int> { 4 });
            AL.push_back(std::vector<int> {});
            AL.push_back(std::vector<int> { 7, 8 });

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<int> sortedIdx = dex.toposortProcsIdx(AL);
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 1));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 2));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 3));
            Assert::IsTrue(getIndex(sortedIdx, 0) > getIndex(sortedIdx, 6));
            Assert::IsTrue(getIndex(sortedIdx, 1) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 2) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 3) > getIndex(sortedIdx, 5));
            Assert::IsTrue(getIndex(sortedIdx, 6) > getIndex(sortedIdx, 5));

            Assert::IsTrue(getIndex(sortedIdx, 4) > getIndex(sortedIdx, 7));
            Assert::IsTrue(getIndex(sortedIdx, 7) > getIndex(sortedIdx, 9));
            Assert::IsTrue(getIndex(sortedIdx, 8) > getIndex(sortedIdx, 9));
        }
    };

    TEST_CLASS(TestGetProcToCallStmt) {
        TEST_METHOD(GetProcToCallStmt_OneCall_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(1, "B");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> result
                = dex.getProcToCallStmt();

            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> expected;
            expected["B"] = std::vector<LINE_NUM>{ 1 };

            Assert::IsTrue(result == expected);
        }

        TEST_METHOD(GetProcToCallStmt_MultipleCallToSameProc_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "B");
            pkb.storeCallStatement(3, "B");
            pkb.storeCallStatement(4, "B");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> result
                = dex.getProcToCallStmt();

            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> expected;
            expected["B"] = std::vector<LINE_NUM>{ 1, 2, 3, 4 };

            Assert::IsTrue(result == expected);
        }

        TEST_METHOD(GetProcToCallStmt_ChainedProc_ValidEntries) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeProcedure("C", 3, 3, {});
            pkb.storeProcedure("D", 4, 4, {});
            pkb.storeProcedure("E", 5, 5, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "C");
            pkb.storeCallStatement(3, "D");
            pkb.storeCallStatement(4, "E");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> result
                = dex.getProcToCallStmt();

            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> expected;
            expected["B"] = std::vector<LINE_NUM>{ 1 };
            expected["C"] = std::vector<LINE_NUM>{ 2 };
            expected["D"] = std::vector<LINE_NUM>{ 3 };
            expected["E"] = std::vector<LINE_NUM>{ 4 };

            Assert::IsTrue(result == expected);
        }

        TEST_METHOD(GetProcToCallStmt_ComplexProc_ValidEntries) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeProcedure("C", 6, 6, {});
            pkb.storeProcedure("D", 7, 7, {});
            pkb.storeProcedure("E", 8, 8, {});
            pkb.storeProcedure("F", 9, 9, {});
            pkb.storeProcedure("G", 10, 10, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "C");
            pkb.storeCallStatement(3, "D");
            pkb.storeCallStatement(4, "E");
            pkb.storeCallStatement(5, "F");
            pkb.storeCallStatement(6, "F");
            pkb.storeCallStatement(7, "F");
            pkb.storeCallStatement(8, "F");
            pkb.storeCallStatement(10, "F");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> result
                = dex.getProcToCallStmt();

            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> expected;
            expected["B"] = std::vector<LINE_NUM>{ 1 };
            expected["C"] = std::vector<LINE_NUM>{ 2 };
            expected["D"] = std::vector<LINE_NUM>{ 3 };
            expected["E"] = std::vector<LINE_NUM>{ 4 };
            expected["F"] = std::vector<LINE_NUM>{ 5, 6, 7, 8, 10 };

            Assert::IsTrue(result == expected);
        }
    };

    TEST_CLASS(TestUpdateProcedureUsesModifies) {
        TEST_METHOD(UpdateProcedureUsesModifies_OneCall_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeRelationshipUsesByLineNum(2, "uses");
            pkb.storeRelationshipModifiesByLineNum(2, "modifies");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> toposortedProcIdx = dex.toposortProcsIdx(AL);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> procToCallStmtMap;
            procToCallStmtMap = dex.getProcToCallStmt();
            dex.updateProcedureUsesModifies(toposortedProcIdx, procToCallStmtMap);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "uses"));
            Assert::IsTrue(pkb.doesLineNumModifies(1, "modifies"));

            Assert::IsTrue(pkb.doesProcedureUses("A", "uses"));
            Assert::IsTrue(pkb.doesProcedureModifies("A", "modifies"));
        }

        TEST_METHOD(UpdateProcedureUsesModifies_MultipleCallToSameProc_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "B");
            pkb.storeCallStatement(3, "B");
            pkb.storeCallStatement(4, "B");
            pkb.sortAllDataStructure();
            pkb.storeRelationshipUsesByLineNum(5, "uses");
            pkb.storeRelationshipModifiesByLineNum(5, "modifies");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> toposortedProcIdx = dex.toposortProcsIdx(AL);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> procToCallStmtMap;
            procToCallStmtMap = dex.getProcToCallStmt();
            dex.updateProcedureUsesModifies(toposortedProcIdx, procToCallStmtMap);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "uses"));
            Assert::IsTrue(pkb.doesLineNumModifies(1, "modifies"));
            Assert::IsTrue(pkb.doesLineNumUses(2, "uses"));
            Assert::IsTrue(pkb.doesLineNumModifies(2, "modifies"));
            Assert::IsTrue(pkb.doesLineNumUses(3, "uses"));
            Assert::IsTrue(pkb.doesLineNumModifies(3, "modifies"));
            Assert::IsTrue(pkb.doesLineNumUses(4, "uses"));
            Assert::IsTrue(pkb.doesLineNumModifies(4, "modifies"));

            Assert::IsTrue(pkb.doesProcedureUses("A", "uses"));
            Assert::IsTrue(pkb.doesProcedureModifies("A", "modifies"));
            Assert::IsTrue(pkb.doesProcedureUses("B", "uses"));
            Assert::IsTrue(pkb.doesProcedureModifies("B", "modifies"));
        }

        TEST_METHOD(UpdateProcedureUsesModifies_MultipleCallToDiffProc_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeProcedure("C", 6, 6, {});
            pkb.storeProcedure("D", 7, 7, {});
            pkb.storeProcedure("E", 8, 8, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "C");
            pkb.storeCallStatement(3, "D");
            pkb.storeCallStatement(4, "E");
            pkb.sortAllDataStructure();
            pkb.storeRelationshipUsesByLineNum(5, "usesB");
            pkb.storeRelationshipModifiesByLineNum(5, "modifiesB");
            pkb.storeRelationshipUsesByLineNum(6, "usesC");
            pkb.storeRelationshipModifiesByLineNum(6, "modifiesC");
            pkb.storeRelationshipUsesByLineNum(7, "usesD");
            pkb.storeRelationshipModifiesByLineNum(7, "modifiesD");
            pkb.storeRelationshipUsesByLineNum(8, "usesE");
            pkb.storeRelationshipModifiesByLineNum(8, "modifiesE");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> toposortedProcIdx = dex.toposortProcsIdx(AL);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> procToCallStmtMap;
            procToCallStmtMap = dex.getProcToCallStmt();
            dex.updateProcedureUsesModifies(toposortedProcIdx, procToCallStmtMap);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "usesB"));
            Assert::IsTrue(pkb.doesLineNumModifies(1, "modifiesB"));
            Assert::IsTrue(pkb.doesLineNumUses(2, "usesC"));
            Assert::IsTrue(pkb.doesLineNumModifies(2, "modifiesC"));
            Assert::IsTrue(pkb.doesLineNumUses(3, "usesD"));
            Assert::IsTrue(pkb.doesLineNumModifies(3, "modifiesD"));
            Assert::IsTrue(pkb.doesLineNumUses(4, "usesE"));
            Assert::IsTrue(pkb.doesLineNumModifies(4, "modifiesE"));

            Assert::IsTrue(pkb.doesProcedureUses("A", "usesB"));
            Assert::IsTrue(pkb.doesProcedureModifies("A", "modifiesB"));
            Assert::IsTrue(pkb.doesProcedureUses("B", "usesB"));
            Assert::IsTrue(pkb.doesProcedureModifies("B", "modifiesB"));
            Assert::IsTrue(pkb.doesProcedureUses("A", "usesC"));
            Assert::IsTrue(pkb.doesProcedureModifies("A", "modifiesC"));
            Assert::IsTrue(pkb.doesProcedureUses("C", "usesC"));
            Assert::IsTrue(pkb.doesProcedureModifies("C", "modifiesC"));
            Assert::IsTrue(pkb.doesProcedureUses("A", "usesD"));
            Assert::IsTrue(pkb.doesProcedureModifies("A", "modifiesD"));
            Assert::IsTrue(pkb.doesProcedureUses("D", "usesD"));
            Assert::IsTrue(pkb.doesProcedureModifies("D", "modifiesD"));
            Assert::IsTrue(pkb.doesProcedureUses("A", "usesE"));
            Assert::IsTrue(pkb.doesProcedureModifies("A", "modifiesE"));
            Assert::IsTrue(pkb.doesProcedureUses("E", "usesE"));
            Assert::IsTrue(pkb.doesProcedureModifies("E", "modifiesE"));

            Assert::IsFalse(pkb.doesProcedureModifies("B", "modifiesC"));
            Assert::IsFalse(pkb.doesProcedureModifies("C", "modifiesD"));
            Assert::IsFalse(pkb.doesProcedureModifies("D", "modifiesE"));
            Assert::IsFalse(pkb.doesProcedureModifies("E", "modifiesB"));
        }

        TEST_METHOD(UpdateProcedureUsesModifies_ComplexProcChain_ValidResult) {
            /*
            procedure A { call B; }
            procedure B { call E; print x3; }
            procedure C { print x4; call E; }
            procedure D { call F; call G; }
            procedure E { read x8; }
            procedure F { call H; read x10; }
            procedure G { print x11; call H; }
            procedure H { print end; }
            */
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeProcedure("B", 2, 3, {});
            pkb.storeCallStatement(2, "E");
            pkb.storeRelationshipUsesByLineNum(3, "x3");
            pkb.storeProcedure("C", 4, 5, {});
            pkb.storeRelationshipUsesByLineNum(4, "x4");
            pkb.storeCallStatement(5, "E");
            pkb.storeProcedure("D", 6, 7, {});
            pkb.storeCallStatement(6, "F");
            pkb.storeCallStatement(7, "G");
            pkb.storeProcedure("E", 8, 8, {});
            pkb.storeRelationshipModifiesByLineNum(8, "x8");
            pkb.storeProcedure("F", 9, 10, {});
            pkb.storeCallStatement(9, "H");
            pkb.storeRelationshipModifiesByLineNum(10, "x10");
            pkb.storeProcedure("G", 11, 12, {});
            pkb.storeRelationshipUsesByLineNum(11, "x11");
            pkb.storeCallStatement(12, "H");
            pkb.storeProcedure("H", 13, 13, {});
            pkb.storeRelationshipUsesByLineNum(13, "end");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> toposortedProcIdx = dex.toposortProcsIdx(AL);
            std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> procToCallStmtMap;
            procToCallStmtMap = dex.getProcToCallStmt();
            dex.updateProcedureUsesModifies(toposortedProcIdx, procToCallStmtMap);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureUses("A", "x3"));
            Assert::IsTrue(pkb.doesProcedureModifies("A", "x8"));
            Assert::IsTrue(pkb.doesProcedureUses("B", "x3"));
            Assert::IsTrue(pkb.doesProcedureModifies("B", "x8"));
            Assert::IsTrue(pkb.doesLineNumUses(1, "x3"));
            Assert::IsTrue(pkb.doesLineNumModifies(1, "x8"));
            Assert::IsTrue(pkb.doesProcedureUses("C", "x4"));
            Assert::IsTrue(pkb.doesProcedureModifies("C", "x8"));
            Assert::IsTrue(pkb.doesProcedureModifies("E", "x8"));
            Assert::IsTrue(pkb.doesLineNumModifies(2, "x8"));
            Assert::IsTrue(pkb.doesLineNumModifies(5, "x8"));

            Assert::IsTrue(pkb.doesProcedureModifies("D", "x10"));
            Assert::IsTrue(pkb.doesProcedureUses("D", "x11"));
            Assert::IsTrue(pkb.doesProcedureUses("D", "end"));
            Assert::IsTrue(pkb.doesProcedureModifies("F", "x10"));
            Assert::IsTrue(pkb.doesProcedureUses("F", "end"));
            Assert::IsTrue(pkb.doesLineNumModifies(6, "x10"));
            Assert::IsTrue(pkb.doesLineNumUses(6, "end"));
            Assert::IsTrue(pkb.doesProcedureUses("G", "x11"));
            Assert::IsTrue(pkb.doesProcedureUses("G", "end"));
            Assert::IsTrue(pkb.doesLineNumUses(7, "x11"));
            Assert::IsTrue(pkb.doesLineNumUses(7, "end"));
            Assert::IsTrue(pkb.doesProcedureUses("H", "end"));
            Assert::IsTrue(pkb.doesLineNumUses(9, "end"));
            Assert::IsTrue(pkb.doesLineNumUses(12, "end"));
        }
    };

    TEST_CLASS(TestExtractCalls) {
        TEST_METHOD(ExtractCalls_OneCallStmt_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(1, "B");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractCalls();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureCalls("A", "B"));
            Assert::IsFalse(pkb.doesProcedureCalls("B", "A"));

            Assert::IsTrue(pkb.getCallers("A") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallers("B") == std::vector<PROC_NAME>{ "A" });

            Assert::IsTrue(pkb.getCallees("A") == std::vector<PROC_NAME>{ "B" });
            Assert::IsTrue(pkb.getCallees("B") == std::vector<PROC_NAME>{});
        }

        TEST_METHOD(ExtractCalls_MultipleCallsToSameProc_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "B");
            pkb.storeCallStatement(3, "B");
            pkb.storeCallStatement(4, "B");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractCalls();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureCalls("A", "B"));
            Assert::IsFalse(pkb.doesProcedureCalls("B", "A"));

            Assert::IsTrue(pkb.getCallers("A") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallers("B") == std::vector<PROC_NAME>{ "A" });

            Assert::IsTrue(pkb.getCallees("A") == std::vector<PROC_NAME>{ "B" });
            Assert::IsTrue(pkb.getCallees("B") == std::vector<PROC_NAME>{});
        }

        TEST_METHOD(ExtractCalls_CallToDiffProc_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 4, {});
            pkb.storeProcedure("B", 5, 5, {});
            pkb.storeProcedure("C", 6, 6, {});
            pkb.storeProcedure("D", 7, 7, {});
            pkb.storeProcedure("E", 8, 8, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(2, "C");
            pkb.storeCallStatement(3, "D");
            pkb.storeCallStatement(4, "E");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractCalls();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureCalls("A", "B"));
            Assert::IsTrue(pkb.doesProcedureCalls("A", "C"));
            Assert::IsTrue(pkb.doesProcedureCalls("A", "D"));
            Assert::IsTrue(pkb.doesProcedureCalls("A", "E"));
            Assert::IsFalse(pkb.doesProcedureCalls("B", "A"));
            Assert::IsFalse(pkb.doesProcedureCalls("C", "A"));
            Assert::IsFalse(pkb.doesProcedureCalls("D", "A"));
            Assert::IsFalse(pkb.doesProcedureCalls("E", "A"));
            Assert::IsFalse(pkb.doesProcedureCalls("B", "C"));

            Assert::IsTrue(pkb.getCallers("A") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallers("B") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCallers("C") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCallers("D") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCallers("E") == std::vector<PROC_NAME>{ "A" });

            Assert::IsTrue(pkb.getCallees("A") == std::vector<PROC_NAME>{ "B", "C", "D", "E" });
            Assert::IsTrue(pkb.getCallees("B") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallees("C") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallees("D") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallees("E") == std::vector<PROC_NAME>{});
        }

        TEST_METHOD(ExtractCalls_DiamondCall_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(2, "A");
            pkb.storeProcedure("C", 3, 3, {});
            pkb.storeCallStatement(3, "A");
            pkb.storeProcedure("D", 4, 5, {});
            pkb.storeCallStatement(4, "B");
            pkb.storeCallStatement(5, "C");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            dex.extractCalls();
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureCalls("D", "C"));
            Assert::IsTrue(pkb.doesProcedureCalls("D", "B"));
            Assert::IsTrue(pkb.doesProcedureCalls("C", "A"));
            Assert::IsTrue(pkb.doesProcedureCalls("B", "A"));

            Assert::IsTrue(pkb.getCallers("A") == std::vector<PROC_NAME>{ "B", "C" });
            Assert::IsTrue(pkb.getCallers("B") == std::vector<PROC_NAME>{ "D" });
            Assert::IsTrue(pkb.getCallers("C") == std::vector<PROC_NAME>{ "D" });
            Assert::IsTrue(pkb.getCallers("D") == std::vector<PROC_NAME>{});

            Assert::IsTrue(pkb.getCallees("A") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallees("B") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCallees("C") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCallees("D") == std::vector<PROC_NAME>{ "B", "C" });
        }
    };

    TEST_CLASS(TestExtractCallsStar) {
        TEST_METHOD(ExtractCallsStar_SimpleChain_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeCallStatement(1, "B");
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(2, "C");
            pkb.storeProcedure("C", 3, 3, {});
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<int> toposortedProcIdx = std::vector<int>{ 2, 1, 0 };
            dex.extractCalls();
            dex.extractCallsStar(toposortedProcIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureCallsStar("A", "B"));
            Assert::IsTrue(pkb.doesProcedureCallsStar("A", "C"));
            Assert::IsTrue(pkb.doesProcedureCallsStar("B", "C"));
            Assert::IsFalse(pkb.doesProcedureCallsStar("B", "A"));
            Assert::IsFalse(pkb.doesProcedureCallsStar("C", "A"));
            Assert::IsFalse(pkb.doesProcedureCallsStar("C", "B"));

            Assert::IsTrue(pkb.getCallersStar("A") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCallersStar("B") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCallersStar("C") == std::vector<PROC_NAME>{ "A", "B" });

            Assert::IsTrue(pkb.getCalleesStar("A") == std::vector<PROC_NAME>{ "B", "C" });
            Assert::IsTrue(pkb.getCalleesStar("B") == std::vector<PROC_NAME>{ "C" });
            Assert::IsTrue(pkb.getCalleesStar("C") == std::vector<PROC_NAME>{});
        }

        TEST_METHOD(ExtractCallsStar_DiamondCall_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 1, {});
            pkb.storeProcedure("B", 2, 2, {});
            pkb.storeCallStatement(2, "A");
            pkb.storeProcedure("C", 3, 3, {});
            pkb.storeCallStatement(3, "A");
            pkb.storeProcedure("D", 4, 5, {});
            pkb.storeCallStatement(4, "B");
            pkb.storeCallStatement(5, "C");
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, false);
            std::vector<int> toposortedProcIdx = std::vector<int>{ 0, 1, 2, 3 };
            dex.extractCalls();
            dex.extractCallsStar(toposortedProcIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureCallsStar("B", "A"));
            Assert::IsTrue(pkb.doesProcedureCallsStar("C", "A"));
            Assert::IsTrue(pkb.doesProcedureCallsStar("D", "B"));
            Assert::IsTrue(pkb.doesProcedureCallsStar("D", "C"));

            Assert::IsTrue(pkb.getCallersStar("A") == std::vector<PROC_NAME>{ "B", "C", "D" });
            Assert::IsTrue(pkb.getCallersStar("B") == std::vector<PROC_NAME>{ "D" });
            Assert::IsTrue(pkb.getCallersStar("C") == std::vector<PROC_NAME>{ "D" });
            Assert::IsTrue(pkb.getCallersStar("D") == std::vector<PROC_NAME>{});

            Assert::IsTrue(pkb.getCalleesStar("A") == std::vector<PROC_NAME>{});
            Assert::IsTrue(pkb.getCalleesStar("B") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCalleesStar("C") == std::vector<PROC_NAME>{ "A" });
            Assert::IsTrue(pkb.getCalleesStar("D") == std::vector<PROC_NAME>{ "A", "B", "C" });
        }
    };
}  // namespace PkbAndDeIntegrationTesting
