#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"
#include "Commons/Exceptions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBUses) {
    public:
        TEST_METHOD(DoesProcedureUses_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureUses("main", "x"));
        }
        TEST_METHOD(DoesProcedureUses_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesProcedureUses("main", "y"));
        }
        TEST_METHOD(DoesLineNumUses_PairExists_True) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUses(1, "x"));
        }
        TEST_METHOD(DoesLineNumUses_PairDoesNotExist_False) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            Assert::IsFalse(pkb.doesLineNumUses(1, "y"));
        }
        TEST_METHOD(GetVariableFromUsesByProcedure_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromUsesByProcedure("main");
            std::vector<VAR_NAME> expected{ "x", "y" };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetVariableFromUsesByProcedure_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromUsesByProcedure("test");
            std::vector<VAR_NAME> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetVariableFromUsesByLineNum_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromUsesByLineNum(1);
            std::vector<VAR_NAME> expected{ "x", "y" };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetVariableFromUsesByLineNum_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<VAR_NAME> actual = pkb.getVariableFromUsesByLineNum(2);
            std::vector<VAR_NAME> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetProcedureFromUsesByProcedure_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<PROC_NAME> actualX = pkb.getProcedureFromUsesByProcedure("x");
            std::vector<PROC_NAME> expectedX{ "main" };
            Assert::IsTrue(actualX == expectedX);

            std::vector<PROC_NAME> actualY = pkb.getProcedureFromUsesByProcedure("y");
            std::vector<PROC_NAME> expectedY{ "main" };
            Assert::IsTrue(actualY == expectedY);
        }
        TEST_METHOD(GetProcedureFromUsesByProcedure_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("main", "y");
            pkb.sortAllDataStructure();

            std::vector<PROC_NAME> actual = pkb.getProcedureFromUsesByProcedure("z");
            std::vector<PROC_NAME> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(GetLineNumFromUsesByVarName_KeyValid_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actualX = pkb.getLineNumFromUsesByVarName("x");
            std::vector<LINE_NUM> expectedX{ 1 };
            Assert::IsTrue(actualX == expectedX);

            std::vector<LINE_NUM> actualY = pkb.getLineNumFromUsesByVarName("y");
            std::vector<LINE_NUM> expectedY{ 1 };
            Assert::IsTrue(actualY == expectedY);
        }
        TEST_METHOD(GetLineNumFromUsesByVarName_KeyInvalid_Empty) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(1, "y");
            pkb.sortAllDataStructure();

            std::vector<LINE_NUM> actual = pkb.getLineNumFromUsesByVarName("z");
            std::vector<LINE_NUM> expected{ };
            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(DoesLineNumUsesWildcard_SingleLineNum_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUsesWildcard(1));
            Assert::IsFalse(pkb.doesLineNumUsesWildcard(2));
        }
        TEST_METHOD(DoesLineNumUsesWildcard_MultipleLineNum_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(2, "x");
            pkb.storeRelationshipUsesByLineNum(2, "y");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesLineNumUsesWildcard(1));
            Assert::IsTrue(pkb.doesLineNumUsesWildcard(2));
            Assert::IsFalse(pkb.doesLineNumUsesWildcard(3));
        }
        TEST_METHOD(DoesProcedureUsesWildcard_SingleProcedure_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureUsesWildcard("main"));
            Assert::IsFalse(pkb.doesProcedureUsesWildcard("test"));
        }
        TEST_METHOD(DoesProcedureUsesWildcard_MultipleProcedure_Success) {
            PKB pkb = PKB();
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("test", "x");
            pkb.storeRelationshipUsesByProcedure("test", "y");
            pkb.sortAllDataStructure();

            Assert::IsTrue(pkb.doesProcedureUsesWildcard("main"));
            Assert::IsTrue(pkb.doesProcedureUsesWildcard("test"));
            Assert::IsFalse(pkb.doesProcedureUsesWildcard("hello"));
        }
        TEST_METHOD(GetUsesVector_DistinctVariablePerLine_Success) {
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

            pkb.storeRelationshipUsesByProcedure("main", "b");
            pkb.storeRelationshipUsesByProcedure("main", "c");
            pkb.storeRelationshipUsesByProcedure("main", "d");
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("main", "z");
            pkb.storeRelationshipUsesByLineNum(1, "z");
            pkb.storeRelationshipUsesByLineNum(1, "b");
            pkb.storeRelationshipUsesByLineNum(1, "c");
            pkb.storeRelationshipUsesByLineNum(2, "b");
            pkb.storeRelationshipUsesByLineNum(2, "c");
            pkb.storeRelationshipUsesByLineNum(4, "d");
            pkb.storeRelationshipUsesByLineNum(4, "x");
            pkb.storeRelationshipUsesByLineNum(5, "x");

            pkb.sortAllDataStructure();

            std::vector<std::string> actualProc = pkb.getUsesVector(DesignEntity::PROCEDURE);
            std::vector<std::string> expectedProc{ "main" };
            Assert::IsTrue(actualProc == expectedProc);

            std::vector<std::string> actualAssign = pkb.getUsesVector(DesignEntity::ASSIGN);
            std::vector<std::string> expectedAssign{ "2" };
            Assert::IsTrue(actualAssign == expectedAssign);

            std::vector<std::string> actualIf = pkb.getUsesVector(DesignEntity::IF);
            std::vector<std::string> expectedIf{ "1" };
            Assert::IsTrue(actualIf == expectedIf);

            std::vector<std::string> actualWhile = pkb.getUsesVector(DesignEntity::WHILE);
            std::vector<std::string> expectedWhile{ "4" };
            Assert::IsTrue(actualWhile == expectedWhile);

            auto error_expr = [&] {pkb.getUsesVector(DesignEntity::READ); };
            Assert::ExpectException<InvalidDesignEntity>(error_expr);

            std::vector<std::string> actualPrint = pkb.getUsesVector(DesignEntity::PRINT);
            std::vector<std::string> expectedPrint{ "5" };
            Assert::IsTrue(actualPrint == expectedPrint);

            std::vector<std::string> actualStmt = pkb.getUsesVector(DesignEntity::STMT);
            std::vector<std::string> expectedStmt{ "1", "2", "4", "5" };
            Assert::IsTrue(actualStmt == expectedStmt);
        }
        TEST_METHOD(GetUsesByVarName_DistinctVariablePerLine_Success) {
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

            pkb.storeRelationshipUsesByProcedure("main", "b");
            pkb.storeRelationshipUsesByProcedure("main", "c");
            pkb.storeRelationshipUsesByProcedure("main", "d");
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("main", "z");
            pkb.storeRelationshipUsesByLineNum(1, "z");
            pkb.storeRelationshipUsesByLineNum(1, "b");
            pkb.storeRelationshipUsesByLineNum(1, "c");
            pkb.storeRelationshipUsesByLineNum(2, "b");
            pkb.storeRelationshipUsesByLineNum(2, "c");
            pkb.storeRelationshipUsesByLineNum(4, "d");
            pkb.storeRelationshipUsesByLineNum(4, "x");
            pkb.storeRelationshipUsesByLineNum(5, "x");

            pkb.sortAllDataStructure();

            std::vector<std::string> actualProcPos
                = pkb.getUsesByVarName(DesignEntity::PROCEDURE, "b");
            std::vector<std::string> expectedProcPos{ "main" };
            Assert::IsTrue(actualProcPos == expectedProcPos);

            std::vector<std::string> actualProcNeg
                = pkb.getUsesByVarName(DesignEntity::PROCEDURE, "a");
            std::vector<std::string> expectedProcNeg{ };
            Assert::IsTrue(actualProcNeg == expectedProcNeg);

            std::vector<std::string> actualAssignPos
                = pkb.getUsesByVarName(DesignEntity::ASSIGN, "b");
            std::vector<std::string> expectedAssignPos{ "2" };
            Assert::IsTrue(actualAssignPos == expectedAssignPos);

            std::vector<std::string> actualAssignNeg
                = pkb.getUsesByVarName(DesignEntity::ASSIGN, "a");
            std::vector<std::string> expectedAssignNeg{ };
            Assert::IsTrue(actualAssignNeg == expectedAssignNeg);

            std::vector<std::string> actualIfPos = pkb.getUsesByVarName(DesignEntity::IF, "b");
            std::vector<std::string> expectedIfPos{ "1" };
            Assert::IsTrue(actualIfPos == expectedIfPos);

            std::vector<std::string> actualIfNeg = pkb.getUsesByVarName(DesignEntity::IF, "a");
            std::vector<std::string> expectedIfNeg{ };
            Assert::IsTrue(actualIfNeg == expectedIfNeg);

            std::vector<std::string> actualWhilePos
                = pkb.getUsesByVarName(DesignEntity::WHILE, "x");
            std::vector<std::string> expectedWhilePos{ "4" };
            Assert::IsTrue(actualWhilePos == expectedWhilePos);

            std::vector<std::string> actualWhileNeg
                = pkb.getUsesByVarName(DesignEntity::WHILE, "y");
            std::vector<std::string> expectedWhileNeg{ };
            Assert::IsTrue(actualWhileNeg == expectedWhileNeg);

            // READ will never use any variable
            auto error_expr = [&] {pkb.getUsesByVarName(DesignEntity::READ, "y"); };
            Assert::ExpectException<InvalidDesignEntity>(error_expr);

            std::vector<std::string> actualPrintPos
                = pkb.getUsesByVarName(DesignEntity::PRINT, "x");
            std::vector<std::string> expectedPrintPos{ "5" };
            Assert::IsTrue(actualPrintPos == expectedPrintPos);

            std::vector<std::string> actualPrintNeg
                = pkb.getUsesByVarName(DesignEntity::PRINT, "y");
            std::vector<std::string> expectedPrintNeg{ };
            Assert::IsTrue(actualPrintNeg == expectedPrintNeg);

            std::vector<std::string> actualStmtPos
                = pkb.getUsesByVarName(DesignEntity::STMT, "b");
            std::vector<std::string> expectedStmtPos{ "1", "2" };
            Assert::IsTrue(actualStmtPos == expectedStmtPos);

            std::vector<std::string> actualStmtNeg
                = pkb.getUsesByVarName(DesignEntity::STMT, "a");
            std::vector<std::string> expectedStmtNeg{ };
            Assert::IsTrue(actualStmtNeg == expectedStmtNeg);
        }
        TEST_METHOD(GetUsesPairVector_DistinctVariablePerLine_Success) {
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

            pkb.storeRelationshipUsesByProcedure("main", "b");
            pkb.storeRelationshipUsesByProcedure("main", "c");
            pkb.storeRelationshipUsesByProcedure("main", "d");
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.storeRelationshipUsesByProcedure("main", "z");
            pkb.storeRelationshipUsesByLineNum(1, "z");
            pkb.storeRelationshipUsesByLineNum(1, "b");
            pkb.storeRelationshipUsesByLineNum(1, "c");
            pkb.storeRelationshipUsesByLineNum(2, "b");
            pkb.storeRelationshipUsesByLineNum(2, "c");
            pkb.storeRelationshipUsesByLineNum(4, "d");
            pkb.storeRelationshipUsesByLineNum(4, "x");
            pkb.storeRelationshipUsesByLineNum(5, "x");

            pkb.sortAllDataStructure();

            auto actualProc = pkb.getUsesPairVector(DesignEntity::PROCEDURE,
                                                    DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedProc{
                { "main", "b" },
                { "main", "c" },
                { "main", "d" },
                { "main", "x" },
                { "main", "z" },
            };
            std::set<std::pair<std::string, std::string>>
                actualProcSet(actualProc.begin(), actualProc.end());
            std::set<std::pair<std::string, std::string>>
                expectedProcSet(expectedProc.begin(), expectedProc.end());
            Assert::IsTrue(actualProcSet == expectedProcSet);

            auto actualAssign = pkb.getUsesPairVector(DesignEntity::ASSIGN,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedAssign{
                { "2", "b" },
                { "2", "c" },
            };
            std::set<std::pair<std::string, std::string>>
                actualAssignSet(actualAssign.begin(), actualAssign.end());
            std::set<std::pair<std::string, std::string>>
                expectedAssignSet(expectedAssign.begin(), expectedAssign.end());
            Assert::IsTrue(actualAssignSet == expectedAssignSet);

            auto actualIf = pkb.getUsesPairVector(DesignEntity::IF,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedIf{
                { "1", "b" },
                { "1", "c" },
                { "1", "z" }
            };
            std::set<std::pair<std::string, std::string>>
                actualIfSet(actualIf.begin(), actualIf.end());
            std::set<std::pair<std::string, std::string>>
                expectedIfSet(expectedIf.begin(), expectedIf.end());
            Assert::IsTrue(actualIfSet == expectedIfSet);

            auto actualWhile = pkb.getUsesPairVector(DesignEntity::WHILE,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedWhile{
                { "4", "d" },
                { "4", "x" },
            };
            std::set<std::pair<std::string, std::string>>
                actualWhileSet(actualWhile.begin(), actualWhile.end());
            std::set<std::pair<std::string, std::string>>
                expectedWhileSet(expectedWhile.begin(), expectedWhile.end());
            Assert::IsTrue(actualWhileSet == expectedWhileSet);

            auto error_expr = [&] {pkb.getUsesPairVector(DesignEntity::READ,
                DesignEntity::VARIABLE); };
            Assert::ExpectException<InvalidDesignEntity>(error_expr);

            auto actualPrint = pkb.getUsesPairVector(DesignEntity::PRINT,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedPrint{
                { "5", "x" },
            };
            std::set<std::pair<std::string, std::string>>
                actualPrintSet(actualPrint.begin(), actualPrint.end());
            std::set<std::pair<std::string, std::string>>
                expectedPrintSet(expectedPrint.begin(), expectedPrint.end());
            Assert::IsTrue(actualPrintSet == expectedPrintSet);

            auto actualStmt = pkb.getUsesPairVector(DesignEntity::STMT,
                DesignEntity::VARIABLE);
            std::vector<std::pair<std::string, std::string>> expectedStmt{
                { "1", "b" },
                { "1", "c" },
                { "1", "z" },
                { "2", "b" },
                { "2", "c" },
                { "4", "d" },
                { "4", "x" },
                { "5", "x" },
            };
            std::set<std::pair<std::string, std::string>>
                actualStmtSet(actualStmt.begin(), actualStmt.end());
            std::set<std::pair<std::string, std::string>>
                expectedStmtSet(expectedStmt.begin(), expectedStmt.end());
            Assert::IsTrue(actualStmtSet == expectedStmtSet);
        }
    };
}  // namespace UnitTesting
