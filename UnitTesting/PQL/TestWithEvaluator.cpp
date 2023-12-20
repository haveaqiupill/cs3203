#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/WithEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestWithEvaluator) {
    public:
        TEST_METHOD(EvalZeroAttrRef_SameInteger_True) {
            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT} };

            // with 1 = 1
            QueryArg q1 = QueryArg((std::string)"1", QueryArgType::NUM);
            QueryArg q2 = QueryArg((std::string)"1", QueryArgType::NUM);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 0);
        }
        TEST_METHOD(EvalZeroAttrRef_DiffInteger_False) {
            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT} };

            // with 1 = 2
            QueryArg q1 = QueryArg((std::string)"1", QueryArgType::NUM);
            QueryArg q2 = QueryArg((std::string)"2", QueryArgType::NUM);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsFalse(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 0);
        }
        TEST_METHOD(EvalZeroAttrRef_SameString_True) {
            DECLARATION_TABLE dc = { {"v", DesignEntity::VARIABLE} };

            // with "x" = "x"
            QueryArg q1 = QueryArg((std::string)"x", QueryArgType::NAME);
            QueryArg q2 = QueryArg((std::string)"x", QueryArgType::NAME);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 0);
        }
        TEST_METHOD(EvalZeroAttrRef_DiffString_False) {
            DECLARATION_TABLE dc = { {"v", DesignEntity::VARIABLE} };

            // with "x" = "y"
            QueryArg q1 = QueryArg((std::string)"x", QueryArgType::NAME);
            QueryArg q2 = QueryArg((std::string)"y", QueryArgType::NAME);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsFalse(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 0);
        }
        TEST_METHOD(EvalOneAttrRef_IntegerRight_Success) {
            pkb.propagateStmtList(1);
            pkb.sortAllDataStructure();
            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT} };

            // with s.stmt# = 1
            std::string syn = "s";
            DECLARATION left = Declaration(syn, ATTRIBUTE::STMT_NUM);
            QueryArg q1 = QueryArg(left);
            QueryArg q2 = QueryArg((std::string)"1", QueryArgType::NUM);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 1);
        }
        TEST_METHOD(EvalOneAttrRef_IntegerLeft_Success) {
            pkb.propagateStmtList(1);
            pkb.sortAllDataStructure();
            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT} };

            // with 1 = s.stmt#
            QueryArg q1 = QueryArg((std::string)"1", QueryArgType::NUM);
            std::string syn = "s";
            DECLARATION right = Declaration(syn, ATTRIBUTE::STMT_NUM);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 1);
        }
        TEST_METHOD(EvalOneAttrRef_StringRight_Success) {
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();
            DECLARATION_TABLE dc = { {"v", DesignEntity::VARIABLE} };

            // with v.varName = x
            std::string syn = "v";
            DECLARATION left = Declaration(syn, ATTRIBUTE::VAR_NAME);
            QueryArg q1 = QueryArg(left);
            QueryArg q2 = QueryArg((std::string)"x", QueryArgType::NAME);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 1);
        }
        TEST_METHOD(EvalOneAttrRef_StringLeft_Success) {
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();
            DECLARATION_TABLE dc = { {"v", DesignEntity::VARIABLE} };

            // with x = v.varName
            QueryArg q1 = QueryArg((std::string)"x", QueryArgType::NAME);
            std::string syn = "v";
            DECLARATION right = Declaration(syn, ATTRIBUTE::VAR_NAME);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 1);
        }
        TEST_METHOD(EvalTwoAttrRef_BothOneAttrBothStmt_Success) {
            pkb.propagateStmtList(5);
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"s1", DesignEntity::STMT}, {"s2", DesignEntity::STMT} };

            // with s1.stmt# = s2.stmt# OR s1 = s2 (attributes filled by Parser)
            std::string syn1 = "s1";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::STMT_NUM);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "s2";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::STMT_NUM);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 5);
        }
        TEST_METHOD(EvalTwoAttrRef_BothOneAttrBothVar_Success) {
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"v1", DesignEntity::VARIABLE},
                                     {"v2", DesignEntity::VARIABLE} };

            // with v1.varName = v2.varName (v1 = v2 is NOT valid)
            std::string syn1 = "v1";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::VAR_NAME);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "v2";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::VAR_NAME);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 3);
        }
        TEST_METHOD(EvalTwoAttrRef_BothOneAttrBothStr_Success) {
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeVariable("main");
            pkb.storeProcedure("main", 1, 2, {});
            pkb.storeProcedure("y", 3, 4, {});
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"v", DesignEntity::VARIABLE},
                                     {"p", DesignEntity::PROCEDURE} };

            // with v.varName = p.procName (v = p is NOT valid)
            std::string syn1 = "v";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::VAR_NAME);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "p";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::PROC_NAME);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 2);
        }
        TEST_METHOD(EvalTwoAttrRef_BothOneAttrBothInt_Success) {
            pkb.propagateStmtList(5);
            pkb.storeConstant("3");
            pkb.storeConstant("4");
            pkb.storeConstant("10");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT}, {"c", DesignEntity::CONSTANT} };

            // with s.stmt# = c.value (s = c is NOT valid)
            std::string syn1 = "s";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::STMT_NUM);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "c";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::VALUE);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 2);
        }
        TEST_METHOD(EvalTwoAttrRef_BothOneAttrBothDiffStmt_Success) {
            pkb.propagateStmtList(5);
            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(5, "y", "+ y 1");
            pkb.storeAssignStatement(2, "z", "+ z 1");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT}, {"a", DesignEntity::ASSIGN} };

            // with s.stmt# = a.stmt# OR s = a (attributes filled by Parser)
            std::string syn1 = "s";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::STMT_NUM);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "a";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::STMT_NUM);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 3);
        }
        TEST_METHOD(EvalTwoAttrRef_BothTwoAttrBothStr_Success) {
            pkb.propagateStmtList(6);
            pkb.storeReadStatement(1, "x");
            pkb.storePrintStatement(2, "x");
            pkb.storeReadStatement(3, "y");
            pkb.storePrintStatement(4, "z");
            pkb.storeReadStatement(5, "a");
            pkb.storePrintStatement(6, "a");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"r", DesignEntity::READ}, {"p", DesignEntity::PRINT} };

            // with r.varName = p.varName
            std::string syn1 = "r";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::VAR_NAME);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "p";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::VAR_NAME);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 2);
        }
        TEST_METHOD(EvalTwoAttrRef_BothTwoAttrBothInt_Success) {
            pkb.propagateStmtList(6);
            pkb.storeReadStatement(1, "x");
            pkb.storePrintStatement(2, "x");
            pkb.storeReadStatement(3, "y");
            pkb.storePrintStatement(4, "z");
            pkb.storeReadStatement(5, "a");
            pkb.storePrintStatement(6, "a");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"r", DesignEntity::READ}, {"p", DesignEntity::PRINT} };

            // with r.stmt# = p.stmt# OR r = p (attributes filled by Parser)
            std::string syn1 = "r";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::STMT_NUM);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "p";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::STMT_NUM);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsFalse(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 0);
        }
        TEST_METHOD(EvalTwoAttrRef_LeftTwoAttrBothStr_Success) {
            pkb.propagateStmtList(6);
            pkb.storeReadStatement(1, "x");
            pkb.storeReadStatement(2, "y");
            pkb.storeReadStatement(3, "a");
            pkb.storeAssignStatement(4, "c", "1");
            pkb.storeAssignStatement(5, "e", "1");
            pkb.storeAssignStatement(6, "b", "1");
            pkb.storeProcedure("x", 1, 3, {});
            pkb.storeProcedure("assign", 4, 6, {});
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"r", DesignEntity::READ}, {"p", DesignEntity::PROCEDURE} };

            // with r.varName = p.procName
            std::string syn1 = "r";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::VAR_NAME);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "p";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::PROC_NAME);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 1);
        }
        TEST_METHOD(EvalTwoAttrRef_LeftTwoAttrBothInt_Success) {
            pkb.propagateStmtList(3);
            pkb.storeReadStatement(1, "x");
            pkb.storePrintStatement(2, "y");
            pkb.storeReadStatement(3, "a");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"r", DesignEntity::READ}, {"s", DesignEntity::STMT} };

            // with r.stmt# = s.stmt# OR r = s (attributes filled by Parser)
            std::string syn1 = "r";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::STMT_NUM);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "s";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::STMT_NUM);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 2);
        }
        TEST_METHOD(EvalTwoAttrRef_RightTwoAttrBothStr_Success) {
            pkb.propagateStmtList(7);
            pkb.storeCallStatement(1, "assign");
            pkb.storeCallStatement(4, "test");
            pkb.storeProcedure("x", 1, 3, {});
            pkb.storeProcedure("assign", 4, 6, {});
            pkb.storeProcedure("test", 7, 7, {});
            pkb.storeVariable("x");
            pkb.storeVariable("test");
            pkb.storeVariable("y");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"v", DesignEntity::VARIABLE}, {"c", DesignEntity::CALL} };

            // with v.varName = c.procName
            std::string syn1 = "v";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::VAR_NAME);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "c";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::PROC_NAME);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 1);
        }
        TEST_METHOD(EvalTwoAttrRef_RightTwoAttrBothInt_Success) {
            pkb.propagateStmtList(7);
            pkb.storeCallStatement(1, "assign");
            pkb.storeCallStatement(4, "test");
            pkb.storeProcedure("x", 1, 3, {});
            pkb.storeProcedure("assign", 4, 6, {});
            pkb.storeProcedure("test", 7, 7, {});
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT}, {"c", DesignEntity::CALL} };

            // with s.stmt# = c.stmt# OR s = c (attributes filled by Parser)
            std::string syn1 = "s";
            DECLARATION left = Declaration(syn1, ATTRIBUTE::STMT_NUM);
            QueryArg q1 = QueryArg(left);
            std::string syn2 = "c";
            DECLARATION right = Declaration(syn2, ATTRIBUTE::STMT_NUM);
            QueryArg q2 = QueryArg(right);
            ClauseAnswer actual = withevaluator.evaluateWithClause(q1, q2, dc, ptr);

            Assert::IsTrue(actual.getIsClauseTrue());
            Assert::IsTrue(actual.getResultTable().getResultTable().size() == 2);
        }
    private:
        PKB pkb;
        PKB *ptr = &pkb;
        WithEvaluator withevaluator = WithEvaluator(ptr);
    };
}  // namespace UnitTesting
