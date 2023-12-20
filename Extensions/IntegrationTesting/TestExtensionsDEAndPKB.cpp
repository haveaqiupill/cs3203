#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"
#include "DesignExtractor.h"
#include "Commons/DesignEntity.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PkbAndDeExtensionTesting {
    std::vector<LINE_NUM> retrieveParentStmt(PKB& pkb) {
        std::vector<LINE_NUM> parentLst = std::vector<LINE_NUM>();
        std::vector<LINE_NUM> whileLst = pkb.getAllWhileStatements();
        std::vector<LINE_NUM> ifLst = pkb.getAllIfStatements();
        parentLst.insert(parentLst.end(), whileLst.begin(), whileLst.end());
        parentLst.insert(parentLst.end(), ifLst.begin(), ifLst.end());
        return parentLst;
    }
    TEST_CLASS(TestReverseAdjList) {
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

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<std::vector<int>> reverseAl = dex.reverseAdjList(AL);

            std::vector<std::vector<int>> expected;
            expected.push_back(std::vector<LINE_NUM> { 1, 2, 3, 4 });
            expected.push_back(std::vector<LINE_NUM> { 5 });
            expected.push_back(std::vector<LINE_NUM> { 5 });
            expected.push_back(std::vector<LINE_NUM> { 5 });
            expected.push_back(std::vector<LINE_NUM> { 5 });
            expected.push_back(std::vector<LINE_NUM> { 0 });

            Assert::IsTrue(expected == reverseAl);
        }
    };

    TEST_CLASS(TestExtractNextBip) {
        TEST_METHOD(ExtractNextBip_CrossOneProc_Valid) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 3, { 3 });
            pkb.storeCallStatement(1, "B");
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipRelationship(1, 3));
            Assert::IsTrue(pkb.hasNextBipRelationship(3, 2));

            Assert::IsTrue(pkb.hasNextBipLine(1));
            Assert::IsFalse(pkb.hasNextBipLine(2));
            Assert::IsTrue(pkb.hasNextBipLine(3));

            Assert::IsFalse(pkb.hasPrevBipLine(1));
            Assert::IsTrue(pkb.hasPrevBipLine(2));
            Assert::IsTrue(pkb.hasPrevBipLine(3));

            // returns a pair of {restriction, next line}
            Assert::IsTrue(pkb.getNextBipLines(1)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 3 } });
            Assert::IsTrue(pkb.getNextBipLines(2) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getNextBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 2 } });

            Assert::IsTrue(pkb.getPrevBipLines(1) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getPrevBipLines(2)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 3 } });
            Assert::IsTrue(pkb.getPrevBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 1 } });
        }

        TEST_METHOD(ExtractNextBip_CallStmtAsLastStmt_Valid) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 3, { 3 });
            pkb.storeCallStatement(2, "B");
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipRelationship(1, 2));
            Assert::IsTrue(pkb.hasNextBipRelationship(2, 3));

            Assert::IsTrue(pkb.hasNextBipLine(1));
            Assert::IsTrue(pkb.hasNextBipLine(2));
            Assert::IsFalse(pkb.hasNextBipLine(3));

            Assert::IsFalse(pkb.hasPrevBipLine(1));
            Assert::IsTrue(pkb.hasPrevBipLine(2));
            Assert::IsTrue(pkb.hasPrevBipLine(3));

            // returns a pair of {restriction, next line}
            Assert::IsTrue(pkb.getNextBipLines(1)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 2 } });
            Assert::IsTrue(pkb.getNextBipLines(2)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 3 } });
            Assert::IsTrue(pkb.getNextBipLines(3) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});

            Assert::IsTrue(pkb.getPrevBipLines(1) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getPrevBipLines(2)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 1 } });
            Assert::IsTrue(pkb.getPrevBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 2 } });
        }

        TEST_METHOD(ExtractNextBip_MultipleProcCallStmtAsLastStmt_Valid) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 3, { 3 });
            pkb.storeProcedure("C", 4, 4, { 4 });
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(3, "C");
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipRelationship(1, 3));
            Assert::IsTrue(pkb.hasNextBipRelationship(3, 4));
            Assert::IsTrue(pkb.hasNextBipRelationship(4, 2));

            Assert::IsTrue(pkb.hasNextBipLine(1));
            Assert::IsFalse(pkb.hasNextBipLine(2));
            Assert::IsTrue(pkb.hasNextBipLine(3));
            Assert::IsTrue(pkb.hasNextBipLine(4));

            Assert::IsFalse(pkb.hasPrevBipLine(1));
            Assert::IsTrue(pkb.hasPrevBipLine(2));
            Assert::IsTrue(pkb.hasPrevBipLine(3));
            Assert::IsTrue(pkb.hasPrevBipLine(4));

            // returns a pair of {restriction, next line}
            Assert::IsTrue(pkb.getNextBipLines(1)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 3 } });
            Assert::IsTrue(pkb.getNextBipLines(2) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getNextBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 4 } });
            Assert::IsTrue(pkb.getNextBipLines(4)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 2 } });

            Assert::IsTrue(pkb.getPrevBipLines(1) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getPrevBipLines(2)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 4 } });
            Assert::IsTrue(pkb.getPrevBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 1 } });
            Assert::IsTrue(pkb.getPrevBipLines(4)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 3 } });
        }

        TEST_METHOD(ExtractNextBip_MultipleCallToSameProc_Valid) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 4, { 4 });
            pkb.storeProcedure("C", 5, 5, { 5 });
            pkb.storeCallStatement(1, "C");
            pkb.storeCallStatement(3, "C");
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(3, 4);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipRelationship(1, 5));
            Assert::IsTrue(pkb.hasNextBipRelationship(5, 2));
            Assert::IsTrue(pkb.hasNextBipRelationship(3, 5));
            Assert::IsTrue(pkb.hasNextBipRelationship(5, 4));

            Assert::IsTrue(pkb.hasNextBipLine(1));
            Assert::IsFalse(pkb.hasNextBipLine(2));
            Assert::IsTrue(pkb.hasNextBipLine(3));
            Assert::IsFalse(pkb.hasNextBipLine(4));
            Assert::IsTrue(pkb.hasNextBipLine(5));

            Assert::IsFalse(pkb.hasPrevBipLine(1));
            Assert::IsTrue(pkb.hasPrevBipLine(2));
            Assert::IsFalse(pkb.hasPrevBipLine(3));
            Assert::IsTrue(pkb.hasPrevBipLine(4));
            Assert::IsTrue(pkb.hasPrevBipLine(5));

            // returns a pair of {restriction, next line}
            Assert::IsTrue(pkb.getNextBipLines(1)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 5 } });
            Assert::IsTrue(pkb.getNextBipLines(2) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getNextBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 3, 5 } });
            Assert::IsTrue(pkb.getNextBipLines(4)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getNextBipLines(5)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 2 }, { 3, 4 }});

            Assert::IsTrue(pkb.getPrevBipLines(1) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getPrevBipLines(2)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 5 } });
            Assert::IsTrue(pkb.getPrevBipLines(3) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getPrevBipLines(4)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 3, 5 } });
            Assert::IsTrue(pkb.getPrevBipLines(5)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 1 }, { 3, 3 } });
        }

        TEST_METHOD(ExtractNextBip_MultipleLastStmt_Valid) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 5, { 5 });
            pkb.storeProcedure("C", 6, 8, { 7, 8 });
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(3, "C");
            pkb.storeCallStatement(5, "C");
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(3, 4);
            pkb.storeRelationshipNext(4, 5);
            pkb.storeRelationshipNext(6, 7);
            pkb.storeRelationshipNext(6, 8);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipRelationship(1, 3));
            Assert::IsTrue(pkb.hasNextBipRelationship(3, 6));
            Assert::IsTrue(pkb.hasNextBipRelationship(6, 7));
            Assert::IsTrue(pkb.hasNextBipRelationship(6, 8));
            Assert::IsTrue(pkb.hasNextBipRelationship(7, 4));
            Assert::IsTrue(pkb.hasNextBipRelationship(8, 4));
            Assert::IsTrue(pkb.hasNextBipRelationship(4, 5));
            Assert::IsTrue(pkb.hasNextBipRelationship(5, 6));
            Assert::IsTrue(pkb.hasNextBipRelationship(7, 2));
            Assert::IsTrue(pkb.hasNextBipRelationship(8, 2));

            Assert::IsTrue(pkb.getNextBipLines(1)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 3 } });
            Assert::IsTrue(pkb.getNextBipLines(2) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getNextBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 3, 6 } });
            Assert::IsTrue(pkb.getNextBipLines(4)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 5 } });
            Assert::IsTrue(pkb.getNextBipLines(5)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 6 } });
            Assert::IsTrue(pkb.getNextBipLines(6)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 7 }, { 0, 8 } });
            Assert::IsTrue(pkb.getNextBipLines(7)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 2 }, { 3, 4 } });
            Assert::IsTrue(pkb.getNextBipLines(8)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 2 }, { 3, 4 } });

            Assert::IsTrue(pkb.getPrevBipLines(1) == std::vector<std::pair<LINE_NUM, LINE_NUM>>{});
            Assert::IsTrue(pkb.getPrevBipLines(2)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 7 }, { 1, 8 } });
            Assert::IsTrue(pkb.getPrevBipLines(3)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 1 } });
            Assert::IsTrue(pkb.getPrevBipLines(4)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 3, 7 }, { 3, 8 } });
            Assert::IsTrue(pkb.getPrevBipLines(5)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 4 } });
            Assert::IsTrue(pkb.getPrevBipLines(6)
                          == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 1, 5 }, { 3, 3 } });
            Assert::IsTrue(pkb.getPrevBipLines(7)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 6 } });
            Assert::IsTrue(pkb.getPrevBipLines(8)
                           == std::vector<std::pair<LINE_NUM, LINE_NUM>>{ { 0, 6 } });
        }
    };

    TEST_CLASS(TestGetStmtToProcMap) {
        TEST_METHOD(GetStmtToProcMap_MultipleProcs_ValidResult) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 5, { 5 });
            pkb.storeProcedure("C", 6, 8, { 7, 8 });
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::unordered_map<LINE_NUM, PROC_NAME> stmtToProcMap = dex.getStmtToProcMap();

            Assert::IsTrue(stmtToProcMap[1] == "A");
            Assert::IsTrue(stmtToProcMap[2] == "A");
            Assert::IsTrue(stmtToProcMap[3] == "B");
            Assert::IsTrue(stmtToProcMap[4] == "B");
            Assert::IsTrue(stmtToProcMap[5] == "B");
            Assert::IsTrue(stmtToProcMap[6] == "C");
            Assert::IsTrue(stmtToProcMap[7] == "C");
            Assert::IsTrue(stmtToProcMap[8] == "C");
        }
    };

    TEST_CLASS(TestTraverseNextBip) {
        TEST_METHOD(TraverseNextBip_OneProcNoCall_SameAsNextStar) {
            // a = z;
            // b = a;
            // while (b > b) {
            //   a = d;
            //   e = e + b; }
            // if (e > e)
            //   then { g = a; }
            //   else { h = z; }
            PKB pkb;
            pkb.storeProcedure("A", 1, 8, { 7, 8 });
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(2, 3);
            pkb.storeRelationshipNext(3, 4);
            pkb.storeRelationshipNext(3, 6);
            pkb.storeRelationshipNext(4, 5);
            pkb.storeRelationshipNext(5, 3);
            pkb.storeRelationshipNext(6, 7);
            pkb.storeRelationshipNext(6, 8);
            pkb.storeAssignStatement(1, "a", "");
            pkb.storeRelationshipUsesByLineNum(1, "z");
            pkb.storeRelationshipModifiesByLineNum(1, "a");
            pkb.storeAssignStatement(2, "b", "");
            pkb.storeRelationshipUsesByLineNum(2, "a");
            pkb.storeRelationshipModifiesByLineNum(2, "b");
            pkb.storeWhileStatement(3, {});
            pkb.storeRelationshipUsesByLineNum(3, "b");
            pkb.storeRelationshipUsesByLineNum(3, "d");
            pkb.storeRelationshipUsesByLineNum(3, "e");
            pkb.storeRelationshipModifiesByLineNum(3, "a");
            pkb.storeRelationshipModifiesByLineNum(3, "e");
            pkb.storeAssignStatement(4, "a", "");
            pkb.storeRelationshipUsesByLineNum(4, "d");
            pkb.storeRelationshipModifiesByLineNum(4, "a");
            pkb.storeAssignStatement(5, "e", "");
            pkb.storeRelationshipUsesByLineNum(5, "b");
            pkb.storeRelationshipUsesByLineNum(5, "e");
            pkb.storeRelationshipModifiesByLineNum(5, "e");
            pkb.storeIfElseStatement(6, {});
            pkb.storeRelationshipUsesByLineNum(6, "a");
            pkb.storeRelationshipUsesByLineNum(6, "e");
            pkb.storeRelationshipUsesByLineNum(6, "z");
            pkb.storeRelationshipModifiesByLineNum(6, "g");
            pkb.storeRelationshipModifiesByLineNum(6, "h");
            pkb.storeAssignStatement(7, "g", "");
            pkb.storeRelationshipUsesByLineNum(7, "a");
            pkb.storeRelationshipModifiesByLineNum(7, "g");
            pkb.storeAssignStatement(8, "h", "");
            pkb.storeRelationshipUsesByLineNum(8, "z");
            pkb.storeRelationshipModifiesByLineNum(8, "h");
            pkb.propagateStmtList(8);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();
            dex.traverseNextBip(callStmtLineNum, retrieveParentStmt(pkb), callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 2));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 3));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 5));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 6));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 7));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 8));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(2, 3));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(2, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(2, 5));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(2, 6));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(2, 7));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(2, 8));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 3));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 5));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 6));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 7));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 8));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(4, 3));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(4, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(4, 5));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(4, 6));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(4, 7));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(4, 8));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 3));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 5));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 6));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 7));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 8));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(6, 7));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(6, 8));

            Assert::IsFalse(pkb.hasNextBipStarRelationship(6, 3));
            Assert::IsFalse(pkb.hasNextBipStarRelationship(7, 8));

            Assert::IsTrue(pkb.getNextBipStarLines(1, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(2, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(3, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(4, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(5, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(6, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(7, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getNextBipStarLines(8, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});

            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 1)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 2)
                           == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 3)
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 4)
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 5)
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 6)
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 7)
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5, 6 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 8)
                           == std::vector<LINE_NUM>{ 1, 2, 3, 4, 5, 6 });

            Assert::IsTrue(pkb.hasAffectsBipRelationship(1, 2));
            Assert::IsTrue(pkb.hasAffectsBipRelationship(1, 7));
            Assert::IsTrue(pkb.hasAffectsBipRelationship(2, 5));
            Assert::IsTrue(pkb.hasAffectsBipRelationship(4, 7));
            Assert::IsTrue(pkb.hasAffectsBipRelationship(5, 5));

            Assert::IsTrue(pkb.getAffectsBipLines(1, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 2, 7 });
            Assert::IsTrue(pkb.getAffectsBipLines(2, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 5 });
            Assert::IsTrue(pkb.getAffectsBipLines(3, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectsBipLines(4, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 7 });
            Assert::IsTrue(pkb.getAffectsBipLines(5, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 5 });
            Assert::IsTrue(pkb.getAffectsBipLines(6, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectsBipLines(7, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectsBipLines(8, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});

            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 1)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 2)
                           == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 3)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 4)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 5)
                           == std::vector<LINE_NUM>{ 2, 5 });
            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 6)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 7)
                           == std::vector<LINE_NUM>{ 1, 4 });
            Assert::IsTrue(pkb.getAffectingBipLines(DesignEntity::PROG_LINE, 8)
                           == std::vector<LINE_NUM>{});

            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(1, 5));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(1, 2));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(1, 7));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(2, 5));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(4, 7));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(5, 5));

            Assert::IsTrue(pkb.getAffectsBipStarLines(1, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 2, 5, 7 });
            Assert::IsTrue(pkb.getAffectsBipStarLines(2, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 5 });
            Assert::IsTrue(pkb.getAffectsBipStarLines(3, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectsBipStarLines(4, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 7 });
            Assert::IsTrue(pkb.getAffectsBipStarLines(5, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{ 5 });
            Assert::IsTrue(pkb.getAffectsBipStarLines(6, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectsBipStarLines(7, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectsBipStarLines(8, DesignEntity::PROG_LINE)
                           == std::vector<LINE_NUM>{});

            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 1)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 2)
                           == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 3)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 4)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 5)
                           == std::vector<LINE_NUM>{ 1, 2, 5 });
            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 6)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 7)
                           == std::vector<LINE_NUM>{ 1, 4 });
            Assert::IsTrue(pkb.getAffectingBipStarLines(DesignEntity::PROG_LINE, 8)
                           == std::vector<LINE_NUM>{});
        }

        TEST_METHOD(TraverseNextBip_AcrossOneProc_ValidResult) {
            // procedure A { call B; b = a; }
            // procedure B { a = a; }
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 3, { 3 });
            pkb.storeCallStatement(1, "B");
            pkb.storeRelationshipNext(1, 2);
            pkb.storeAssignStatement(2, "b", "a");
            pkb.storeRelationshipModifiesByLineNum(2, "b");
            pkb.storeRelationshipUsesByLineNum(2, "a");
            pkb.storeAssignStatement(3, "a", "a");
            pkb.storeRelationshipModifiesByLineNum(3, "a");
            pkb.storeRelationshipUsesByLineNum(3, "a");
            pkb.propagateStmtList(3);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();
            dex.traverseNextBip(callStmtLineNum, retrieveParentStmt(pkb), callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 3));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 2));

            Assert::IsFalse(pkb.hasNextBipStarRelationship(2, 3));

            Assert::IsTrue(pkb.getNextBipStarLines(1, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getNextBipStarLines(2, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getNextBipStarLines(3, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2 });

            Assert::IsTrue(pkb.hasAffectsBipRelationship(3, 2));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(3, 2));

            Assert::IsFalse(pkb.hasAffectsBipRelationship(2, 3));
            Assert::IsFalse(pkb.hasAffectsBipStarRelationship(2, 3));
        }

        TEST_METHOD(TraverseNextBip_CallStmtAsLastStmt_Valid) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 3, { 3 });
            pkb.storeAssignStatement(1, "a", "b");
            pkb.storeRelationshipModifiesByLineNum(1, "a");
            pkb.storeRelationshipUsesByLineNum(1, "b");
            pkb.storeCallStatement(2, "B");
            pkb.storeAssignStatement(3, "b", "a");
            pkb.storeRelationshipModifiesByLineNum(3, "b");
            pkb.storeRelationshipUsesByLineNum(3, "a");
            pkb.storeRelationshipNext(1, 2);
            pkb.propagateStmtList(3);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();
            dex.traverseNextBip(callStmtLineNum, retrieveParentStmt(pkb), callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 2));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(2, 3));

            Assert::IsTrue(pkb.getNextBipStarLines(1, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 3 });
            Assert::IsTrue(pkb.getNextBipStarLines(2, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 3 });
            Assert::IsTrue(pkb.getNextBipStarLines(3, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});

            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 1)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 2)
                           == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 3)
                           == std::vector<LINE_NUM>{ 1, 2 });

            Assert::IsTrue(pkb.hasAffectsBipRelationship(1, 3));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(1, 3));

            Assert::IsFalse(pkb.hasAffectsBipRelationship(3, 1));
            Assert::IsFalse(pkb.hasAffectsBipStarRelationship(3, 1));
        }

        TEST_METHOD(TraverseNextBip_MultipleCallToSameProc_Valid) {
            // procedure A { call C; b = a; }
            // procedure B { call C; c = a; }
            // procedure C { a = b + c; }
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 4, { 4 });
            pkb.storeProcedure("C", 5, 5, { 5 });
            pkb.storeCallStatement(1, "C");
            pkb.storeCallStatement(3, "C");
            pkb.storeAssignStatement(2, "b", "a");
            pkb.storeRelationshipModifiesByLineNum(2, "b");
            pkb.storeRelationshipUsesByLineNum(2, "a");
            pkb.storeAssignStatement(4, "c", "a");
            pkb.storeRelationshipModifiesByLineNum(4, "c");
            pkb.storeRelationshipUsesByLineNum(4, "a");
            pkb.storeAssignStatement(5, "a", "+ b c");
            pkb.storeRelationshipModifiesByLineNum(5, "a");
            pkb.storeRelationshipUsesByLineNum(5, "b");
            pkb.storeRelationshipUsesByLineNum(5, "c");
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(3, 4);
            pkb.propagateStmtList(5);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();
            dex.traverseNextBip(callStmtLineNum, retrieveParentStmt(pkb), callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 2));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 5));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 5));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 2));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(5, 4));

            Assert::IsFalse(pkb.hasNextBipStarRelationship(1, 4));
            Assert::IsFalse(pkb.hasNextBipStarRelationship(3, 2));

            Assert::IsTrue(pkb.getNextBipStarLines(1, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 5 });
            Assert::IsTrue(pkb.getNextBipStarLines(2, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getNextBipStarLines(3, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 4, 5 });
            Assert::IsTrue(pkb.getNextBipStarLines(4, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getNextBipStarLines(5, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 4 });

            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 1)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 2)
                           == std::vector<LINE_NUM>{ 1, 5 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 3)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 4)
                           == std::vector<LINE_NUM>{ 3, 5 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 5)
                           == std::vector<LINE_NUM>{ 1, 3 });

            Assert::IsTrue(pkb.hasAffectsBipRelationship(5, 2));
            Assert::IsTrue(pkb.hasAffectsBipRelationship(5, 4));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(5, 2));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(5, 4));
            Assert::IsFalse(pkb.hasAffectsBipRelationship(2, 5));
            Assert::IsFalse(pkb.hasAffectsBipRelationship(4, 5));
        }

        TEST_METHOD(TraverseNextBip_MultipleProcCallStmtAsLastStmt_Valid) {
            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 3, { 3 });
            pkb.storeProcedure("C", 4, 4, { 4 });
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(3, "C");
            pkb.storeRelationshipNext(1, 2);
            pkb.propagateStmtList(4);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();
            dex.traverseNextBip(callStmtLineNum, retrieveParentStmt(pkb), callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 2));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 3));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(1, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 4));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(3, 2));
            Assert::IsTrue(pkb.hasNextBipStarRelationship(4, 2));

            Assert::IsTrue(pkb.getNextBipStarLines(1, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 3, 4 });
            Assert::IsTrue(pkb.getNextBipStarLines(2, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getNextBipStarLines(3, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 4 });
            Assert::IsTrue(pkb.getNextBipStarLines(4, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2 });

            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 1)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 2)
                           == std::vector<LINE_NUM>{ 1, 3, 4 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 3)
                           == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 4)
                           == std::vector<LINE_NUM>{ 1, 3 });
        }

        TEST_METHOD(TraverseNextBip_MultipleLastStmt_Valid) {
            // procedure A {
            //   call B;
            //   d = c; }
            // procedure B {
            //   call C;
            //   a = z;
            //   call C; }
            // procedure C {
            //   if (x6 > x6)
            //     then { b = a; }
            //     else { c = b; }}

            PKB pkb;
            pkb.storeProcedure("A", 1, 2, { 2 });
            pkb.storeProcedure("B", 3, 5, { 5 });
            pkb.storeProcedure("C", 6, 8, { 7, 8 });
            pkb.storeCallStatement(1, "B");
            pkb.storeCallStatement(3, "C");
            pkb.storeCallStatement(5, "C");
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(3, 4);
            pkb.storeRelationshipNext(4, 5);
            pkb.storeRelationshipNext(6, 7);
            pkb.storeRelationshipNext(6, 8);
            pkb.storeAssignStatement(2, "d", "c");
            pkb.storeRelationshipModifiesByLineNum(2, "d");
            pkb.storeRelationshipUsesByLineNum(2, "c");
            pkb.storeAssignStatement(4, "a", "z");
            pkb.storeRelationshipModifiesByLineNum(4, "a");
            pkb.storeRelationshipUsesByLineNum(4, "z");
            pkb.storeIfElseStatement(6, {});
            pkb.storeAssignStatement(7, "b", "a");
            pkb.storeRelationshipModifiesByLineNum(7, "b");
            pkb.storeRelationshipUsesByLineNum(7, "a");
            pkb.storeAssignStatement(8, "c", "b");
            pkb.storeRelationshipModifiesByLineNum(8, "c");
            pkb.storeRelationshipUsesByLineNum(8, "b");
            pkb.propagateStmtList(8);
            pkb.sortAllDataStructure();

            DesignExtractor dex = DesignExtractor(&pkb, true);
            std::vector<LINE_NUM> callStmtLineNum = dex.getCallStmtLineNums();
            std::vector<std::vector<int>> AL = dex.extractProcsIdxAdjList(callStmtLineNum);
            std::vector<int> callToposortIdx = dex.toposortProcsIdx(dex.reverseAdjList(AL));
            dex.extractNextBip(callStmtLineNum, callToposortIdx);
            pkb.sortAllDataStructure();
            dex.traverseNextBip(callStmtLineNum, retrieveParentStmt(pkb), callToposortIdx);
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.getNextBipStarLines(1, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(2, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getNextBipStarLines(3, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(4, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(5, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(6, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(7, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getNextBipStarLines(8, DesignEntity::STMT)
                           == std::vector<LINE_NUM>{ 2, 4, 5, 6, 7, 8 });

            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 1)
                           == std::vector<LINE_NUM>{});
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 2)
                           == std::vector<LINE_NUM>{ 1, 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 3)
                           == std::vector<LINE_NUM>{ 1 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 4)
                           == std::vector<LINE_NUM>{ 1, 3, 6, 7, 8 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 5)
                           == std::vector<LINE_NUM>{ 1, 3, 4, 6, 7, 8 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 6)
                           == std::vector<LINE_NUM>{ 1, 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 7)
                           == std::vector<LINE_NUM>{ 1, 3, 4, 5, 6, 7, 8 });
            Assert::IsTrue(pkb.getPrevBipStarLines(DesignEntity::STMT, 8)
                           == std::vector<LINE_NUM>{ 1, 3, 4, 5, 6, 7, 8 });

            Assert::IsTrue(pkb.hasAffectsBipRelationship(4, 7));
            Assert::IsTrue(pkb.hasAffectsBipRelationship(7, 8));
            Assert::IsTrue(pkb.hasAffectsBipRelationship(8, 2));

            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(4, 7));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(7, 8));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(8, 2));
            Assert::IsTrue(pkb.hasAffectsBipStarRelationship(7, 2));

            Assert::IsFalse(pkb.hasAffectsBipStarRelationship(4, 8));
        }
    };
}  // namespace PkbAndDeExtensionTesting
