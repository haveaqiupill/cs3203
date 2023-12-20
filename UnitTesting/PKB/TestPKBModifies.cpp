#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"
#include "Commons/Exceptions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBModifies) {
    public:
        TEST_METHOD(DoesProcedureModifies_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureModifies("main", "x"));
        }
        TEST_METHOD(DoesProcedureModifies_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesProcedureModifies("main", "y"));
        }
        TEST_METHOD(DoesLineNumModifies_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumModifies(1, "x"));
        }
        TEST_METHOD(DoesLineNumModifies_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesLineNumModifies(1, "y"));
        }
        TEST_METHOD(GetVariableFromModifiesByProcedure_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.storeRelationshipModifiesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromModifiesByProcedure("main");
            std::vector<VAR_NAME> expected{ "x", "y" };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetVariableFromModifiesByProcedure_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.storeRelationshipModifiesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromModifiesByProcedure("test");
            std::vector<VAR_NAME> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetVariableFromModifiesByLineNum_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromModifiesByLineNum(1);
            std::vector<VAR_NAME> expected{ "x", "y" };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetVariableFromModifiesByLineNum_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromModifiesByLineNum(2);
            std::vector<VAR_NAME> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetProcedureFromModifiesByProcedure_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.storeRelationshipModifiesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<PROC_NAME> actualX = pkb.getProcedureFromModifiesByProcedure("x");
            std::vector<PROC_NAME> expectedX{ "main" };
            Assert::IsTrue(actualX == expectedX);

            std::vector<PROC_NAME> actualY = pkb.getProcedureFromModifiesByProcedure("y");
            std::vector<PROC_NAME> expectedY{ "main" };
            Assert::IsTrue(actualY == expectedY);
        }
        TEST_METHOD(GetProcedureFromModifiesByProcedure_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.storeRelationshipModifiesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<PROC_NAME> actual = pkb.getProcedureFromModifiesByProcedure("z");
            std::vector<PROC_NAME> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineNumFromModifiesByVarName_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actualX = pkb.getLineNumFromModifiesByVarName("x");
            std::vector<LINE_NUM> expectedX{ 1 };
            Assert::IsTrue(actualX == expectedX);

            std::vector<LINE_NUM> actualY = pkb.getLineNumFromModifiesByVarName("y");
            std::vector<LINE_NUM> expectedY{ 1 };
            Assert::IsTrue(actualY == expectedY);
        }
        TEST_METHOD(GetLineNumFromModifiesByVarName_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineNumFromModifiesByVarName("z");
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(DoesLineNumModifiesWildcard_SingleLineNum_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumModifiesWildcard(1));
            Assert::IsFalse(pkb.doesLineNumModifiesWildcard(2));
        }
        TEST_METHOD(DoesLineNumModifiesWildcard_MultipleLineNum_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(2, "x");
            pkb.storeRelationshipModifiesByLineNum(2, "y");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumModifiesWildcard(1));
            Assert::IsTrue(pkb.doesLineNumModifiesWildcard(2));
            Assert::IsFalse(pkb.doesLineNumModifiesWildcard(3));
        }
        TEST_METHOD(DoesProcedureModifiesWildcard_SingleProcedure_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureModifiesWildcard("main"));
            Assert::IsFalse(pkb.doesProcedureModifiesWildcard("test"));
        }
        TEST_METHOD(DoesProcedureModifiesWildcard_MultipleProcedure_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipModifiesByProcedure("main", "x");
            pkb.storeRelationshipModifiesByProcedure("test", "x");
            pkb.storeRelationshipModifiesByProcedure("test", "y");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureModifiesWildcard("main"));
            Assert::IsTrue(pkb.doesProcedureModifiesWildcard("test"));
            Assert::IsFalse(pkb.doesProcedureModifiesWildcard("hello"));
        }
        TEST_METHOD(GetModifiesVector_DistinctVariablePerLine_Success) {
            /*
                procedure main {
                    if (z == 0) {      // 1
                        a = b + c;     // 2
                    } else {
                        read y;}       // 3
                    while (d > 0) {    // 4
                        print x;}      // 5
                }
            */
            PKB pkb = PKB();
            pkb.propagateStmtList(5);
            pkb.storeVariable("a");
            pkb.storeVariable("b");
            pkb.storeVariable("c");
            pkb.storeVariable("d");
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeProcedure("main", 1, 5, {});
            pkb.storeReadStatement(3, "y");
            pkb.storePrintStatement(5, "x");
            pkb.storeIfElseStatement(1, { "z" });
            pkb.storeWhileStatement(4, { "d" });
            pkb.storeAssignStatement(2, "a", "+ b c");

            pkb.storeRelationshipModifiesByProcedure("main", "a");
            pkb.storeRelationshipModifiesByProcedure("main", "y");
            pkb.storeRelationshipModifiesByLineNum(1, "a");
            pkb.storeRelationshipModifiesByLineNum(1, "y");
            pkb.storeRelationshipModifiesByLineNum(2, "a");
            pkb.storeRelationshipModifiesByLineNum(3, "y");

            pkb.sortAllDataStructure();

            std::vector<std::string> actualProc = pkb.getModifiesVector(DesignEntity::PROCEDURE);
            std::vector<std::string> expectedProc{ "main" };
            Assert::IsTrue(actualProc == expectedProc);

            std::vector<std::string> actualAssign = pkb.getModifiesVector(DesignEntity::ASSIGN);
            std::vector<std::string> expectedAssign{ "2" };
            Assert::IsTrue(actualAssign == expectedAssign);

            std::vector<std::string> actualIf = pkb.getModifiesVector(DesignEntity::IF);
            std::vector<std::string> expectedIf{ "1" };
            Assert::IsTrue(actualIf == expectedIf);

            std::vector<std::string> actualWhile = pkb.getModifiesVector(DesignEntity::WHILE);
            std::vector<std::string> expectedWhile{ };
            Assert::IsTrue(actualWhile == expectedWhile);

            std::vector<std::string> actualRead = pkb.getModifiesVector(DesignEntity::READ);
            std::vector<std::string> expectedRead{ "3" };
            Assert::IsTrue(actualRead == expectedRead);

            auto error_expr = [&] {pkb.getModifiesVector(DesignEntity::PRINT); };
            Assert::ExpectException<InvalidDesignEntity>(error_expr);

            std::vector<std::string> actualStmt = pkb.getModifiesVector(DesignEntity::STMT);
            std::vector<std::string> expectedStmt{ "1", "2", "3" };
            Assert::IsTrue(actualStmt == expectedStmt);
        }
        TEST_METHOD(GetModifiesByVarName_DistinctVariablePerLine_Success) {
            /*
                procedure main {
                    if (z == 0) {      // 1
                        a = b + c;     // 2
                    } else {
                        read y;}       // 3
                    while (d > 0) {    // 4
                        print x;}      // 5
                }
            */
            PKB pkb = PKB();
            pkb.propagateStmtList(5);
            pkb.storeVariable("a");
            pkb.storeVariable("b");
            pkb.storeVariable("c");
            pkb.storeVariable("d");
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeProcedure("main", 1, 5, {});
            pkb.storeReadStatement(3, "y");
            pkb.storePrintStatement(5, "x");
            pkb.storeIfElseStatement(1, { "z" });
            pkb.storeWhileStatement(4, { "d" });
            pkb.storeAssignStatement(2, "a", "+ b c");

            pkb.storeRelationshipModifiesByProcedure("main", "a");
            pkb.storeRelationshipModifiesByProcedure("main", "y");
            pkb.storeRelationshipModifiesByLineNum(1, "a");
            pkb.storeRelationshipModifiesByLineNum(1, "y");
            pkb.storeRelationshipModifiesByLineNum(2, "a");
            pkb.storeRelationshipModifiesByLineNum(3, "y");

            pkb.sortAllDataStructure();

            std::vector<std::string> actualProcPos
                = pkb.getModifiesByVarName(DesignEntity::PROCEDURE, "a");
            std::vector<std::string> expectedProcPos{ "main" };
            Assert::IsTrue(actualProcPos == expectedProcPos);

            std::vector<std::string> actualProcNeg
                = pkb.getModifiesByVarName(DesignEntity::PROCEDURE, "b");
            std::vector<std::string> expectedProcNeg{ };
            Assert::IsTrue(actualProcNeg == expectedProcNeg);

            std::vector<std::string> actualAssignPos
                = pkb.getModifiesByVarName(DesignEntity::ASSIGN, "a");
            std::vector<std::string> expectedAssignPos{ "2" };
            Assert::IsTrue(actualAssignPos == expectedAssignPos);

            std::vector<std::string> actualAssignNeg
                = pkb.getModifiesByVarName(DesignEntity::ASSIGN, "b");
            std::vector<std::string> expectedAssignNeg{ };
            Assert::IsTrue(actualAssignNeg == expectedAssignNeg);

            std::vector<std::string> actualIfPos = pkb.getModifiesByVarName(DesignEntity::IF, "a");
            std::vector<std::string> expectedIfPos{ "1" };
            Assert::IsTrue(actualIfPos == expectedIfPos);

            std::vector<std::string> actualIfNeg = pkb.getModifiesByVarName(DesignEntity::IF, "b");
            std::vector<std::string> expectedIfNeg{ };
            Assert::IsTrue(actualIfNeg == expectedIfNeg);

            // No variable is modified in WHILE for this case
            std::vector<std::string> actualWhileNeg
                = pkb.getModifiesByVarName(DesignEntity::WHILE, "b");
            std::vector<std::string> expectedWhileNeg{ };
            Assert::IsTrue(actualWhileNeg == expectedWhileNeg);

            std::vector<std::string> actualReadPos
                = pkb.getModifiesByVarName(DesignEntity::READ, "y");
            std::vector<std::string> expectedReadPos{ "3" };
            Assert::IsTrue(actualReadPos == expectedReadPos);

            std::vector<std::string> actualReadNeg
                = pkb.getModifiesByVarName(DesignEntity::READ, "x");
            std::vector<std::string> expectedReadNeg{ };
            Assert::IsTrue(actualReadNeg == expectedReadNeg);

            // PRINT will never modify any variable
            auto error_expr = [&] {pkb.getModifiesByVarName(DesignEntity::PRINT, "x"); };
            Assert::ExpectException<InvalidDesignEntity>(error_expr);

            std::vector<std::string> actualStmtPos
                = pkb.getModifiesByVarName(DesignEntity::STMT, "a");
            std::vector<std::string> expectedStmtPos{ "1", "2" };
            Assert::IsTrue(actualStmtPos == expectedStmtPos);

            std::vector<std::string> actualStmtNeg
                = pkb.getModifiesByVarName(DesignEntity::STMT, "b");
            std::vector<std::string> expectedStmtNeg{ };
            Assert::IsTrue(actualStmtNeg == expectedStmtNeg);
        }
        TEST_METHOD(GetModifiesPairVector_DistinctVariablePerLine_Success) {
            /*
                procedure main {
                    if (z == 0) {      // 1
                        a = b + c;     // 2
                    } else {
                        read y;}       // 3
                    while (d > 0) {    // 4
                        print x;}      // 5
                }
            */
            PKB pkb = PKB();
            pkb.propagateStmtList(5);
            pkb.storeVariable("a");
            pkb.storeVariable("b");
            pkb.storeVariable("c");
            pkb.storeVariable("d");
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeProcedure("main", 1, 5, {});
            pkb.storeReadStatement(3, "y");
            pkb.storePrintStatement(5, "x");
            pkb.storeIfElseStatement(1, { "z" });
            pkb.storeWhileStatement(4, { "d" });
            pkb.storeAssignStatement(2, "a", "+ b c");

            pkb.storeRelationshipModifiesByProcedure("main", "a");
            pkb.storeRelationshipModifiesByProcedure("main", "y");
            pkb.storeRelationshipModifiesByLineNum(1, "a");
            pkb.storeRelationshipModifiesByLineNum(1, "y");
            pkb.storeRelationshipModifiesByLineNum(2, "a");
            pkb.storeRelationshipModifiesByLineNum(3, "y");

            pkb.sortAllDataStructure();

            auto actualProc = pkb.getModifiesPairVector(DesignEntity::PROCEDURE,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedProc{
                { "main", "a" },
                { "main", "y" },
            };
            std::set<std::pair<std::string, std::string>>
                actualProcSet(actualProc.begin(), actualProc.end());
            std::set<std::pair<std::string, std::string>>
                expectedProcSet(expectedProc.begin(), expectedProc.end());
            Assert::IsTrue(actualProcSet == expectedProcSet);

            auto actualAssign = pkb.getModifiesPairVector(DesignEntity::ASSIGN,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedAssign{
                { "2", "a" },
            };
            std::set<std::pair<std::string, std::string>>
                actualAssignSet(actualAssign.begin(), actualAssign.end());
            std::set<std::pair<std::string, std::string>>
                expectedAssignSet(expectedAssign.begin(), expectedAssign.end());
            Assert::IsTrue(actualAssignSet == expectedAssignSet);

            auto actualIf = pkb.getModifiesPairVector(DesignEntity::IF,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedIf{
                { "1", "a" },
                { "1", "y" },
            };
            std::set<std::pair<std::string, std::string>>
                actualIfSet(actualIf.begin(), actualIf.end());
            std::set<std::pair<std::string, std::string>>
                expectedIfSet(expectedIf.begin(), expectedIf.end());
            Assert::IsTrue(actualIfSet == expectedIfSet);

            auto actualWhile = pkb.getModifiesPairVector(DesignEntity::WHILE,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedWhile{ };
            Assert::IsTrue(actualWhile == expectedWhile);

            auto actualRead = pkb.getModifiesPairVector(DesignEntity::READ,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedRead{
                { "3", "y" },
            };
            std::set<std::pair<std::string, std::string>>
                actualReadSet(actualRead.begin(), actualRead.end());
            std::set<std::pair<std::string, std::string>>
                expectedReadSet(expectedRead.begin(), expectedRead.end());
            Assert::IsTrue(actualReadSet == expectedReadSet);

            auto error_expr = [&] {pkb.getModifiesPairVector(DesignEntity::PRINT,
                DesignEntity::VARIABLE); };
            Assert::ExpectException<InvalidDesignEntity>(error_expr);

            auto actualStmt = pkb.getModifiesPairVector(DesignEntity::STMT,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedStmt{
                { "1", "a" },
                { "1", "y" },
                { "2", "a" },
                { "3", "y" },
            };
            std::set<std::pair<std::string, std::string>>
                actualStmtSet(actualStmt.begin(), actualStmt.end());
            std::set<std::pair<std::string, std::string>>
                expectedStmtSet(expectedStmt.begin(), expectedStmt.end());
            Assert::IsTrue(actualStmtSet == expectedStmtSet);
        }
    };
}  // namespace UnitTesting
