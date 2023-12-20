#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../source/PQL/QueryEvaluator.h"
#include "../../source/PQL/Query/Query.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestQueryEvaluator) {
    public:
        TEST_METHOD(EvaluateQueryObject_SelectSingle_Success) {
            pkb.propagateStmtList(3);
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dt{ {"s", DesignEntity::STMT} };

            CLAUSE_LIST cl{ };

            SYNONYM syn = "s";
            Declaration dec = Declaration(syn, Attribute::STMT_NUM);
            SELECTION_LIST sl{ dec };

            WITH_CONDITION_LIST wcl{ };

            SelectionType st = SelectionType::SINGLE;

            Query q = Query(dt, cl, sl, wcl, st);
            QueryEvaluator qe = QueryEvaluator(ptr);
            IntermediateAnswer act = qe.evaluateQueryObject(&q);

            Assert::IsTrue(act.getSelection()[0].getSynonym() == "s");
            Assert::IsTrue(act.getSelection()[0].getAttribute() == Attribute::STMT_NUM);

            std::vector<ROW> exp = { {"1"}, {"2"}, {"3"} };
            std::vector<ROW> res = act.getClauseAnsList()[0].getResultTable().getResultTable();
            Assert::IsTrue(exp == res);

            Assert::IsTrue(act.getSelectionType() == st);
        }
        TEST_METHOD(EvaluateQueryObject_SelectTuple_Success) {
            pkb.propagateStmtList(3);
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dt{ {"s", DesignEntity::STMT}, {"v", DesignEntity::VARIABLE} };

            CLAUSE_LIST cl{ };

            SYNONYM syn1 = "s";
            Declaration dec1 = Declaration(syn1, Attribute::STMT_NUM);
            SYNONYM syn2 = "v";
            Declaration dec2 = Declaration(syn2, Attribute::VAR_NAME);
            SELECTION_LIST sl{ dec1, dec2 };

            WITH_CONDITION_LIST wcl{ };

            SelectionType st = SelectionType::TUPLE;

            Query q = Query(dt, cl, sl, wcl, st);
            QueryEvaluator qe = QueryEvaluator(ptr);
            IntermediateAnswer act = qe.evaluateQueryObject(&q);

            Assert::IsTrue(act.getSelection()[0].getSynonym() == "s");
            Assert::IsTrue(act.getSelection()[0].getAttribute() == Attribute::STMT_NUM);
            Assert::IsTrue(act.getSelection()[1].getSynonym() == "v");
            Assert::IsTrue(act.getSelection()[1].getAttribute() == Attribute::VAR_NAME);

            std::vector<ROW> exp1 = { {"1"}, {"2"}, {"3"} };
            std::vector<ROW> res1 = act.getClauseAnsList()[0].getResultTable().getResultTable();
            Assert::IsTrue(exp1 == res1);
            std::vector<ROW> exp2 = { {"x"} };
            std::vector<ROW> res2 = act.getClauseAnsList()[1].getResultTable().getResultTable();
            Assert::IsTrue(exp2 == res2);

            Assert::IsTrue(act.getSelectionType() == st);
        }
        TEST_METHOD(EvaluateQueryObject_SelectBoolean_Success) {
            DECLARATION_TABLE dt{ };

            CLAUSE_LIST cl{ };

            SELECTION_LIST sl{ };

            WITH_CONDITION_LIST wcl{ };

            SelectionType st = SelectionType::BOOLEAN;

            Query q = Query(dt, cl, sl, wcl, st);
            QueryEvaluator qe = QueryEvaluator(ptr);
            IntermediateAnswer act = qe.evaluateQueryObject(&q);

            Assert::IsTrue(act.getClauseAnsList().size() == 0);

            Assert::IsTrue(act.getSelectionType() == st);
        }
        TEST_METHOD(EvaluateQueryObject_SelectSingleSuchThat_Success) {
            pkb.propagateStmtList(3);
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dt{ {"s", DesignEntity::STMT} };

            SYNONYM syn = "s";
            Declaration dec = Declaration(syn, Attribute::STMT_NUM);
            SELECTION_LIST sl{ dec };

            QueryArg l = QueryArg(dec);
            std::string num = "2";
            QueryArg r = QueryArg(num, ARG_TYPE::NUM);
            QueryClause c1 = QueryClause(CLAUSE_TYPE::SUCHTHAT_CL, l, r, Relationship::FOLLOWS);
            CLAUSE_LIST cl{ c1 };

            WITH_CONDITION_LIST wcl{ };

            SelectionType st = SelectionType::SINGLE;

            Query q = Query(dt, cl, sl, wcl, st);
            QueryEvaluator qe = QueryEvaluator(ptr);
            IntermediateAnswer act = qe.evaluateQueryObject(&q);

            Assert::IsTrue(act.getSelection()[0].getSynonym() == "s");
            Assert::IsTrue(act.getSelection()[0].getAttribute() == Attribute::STMT_NUM);

            std::vector<ROW> exp = { {"1"} };
            std::vector<ROW> res = act.getClauseAnsList()[1].getResultTable().getResultTable();
            Assert::IsTrue(exp == res);

            Assert::IsTrue(act.getSelectionType() == st);
        }
        TEST_METHOD(EvaluateQueryObject_SelectTupleSuchThatPattern_Success) {
            pkb.propagateStmtList(3);
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "x", "+ x 1");
            pkb.storeAssignStatement(3, "x", "+ x 1");
            pkb.storeConstant("1");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dt{ {"s", DesignEntity::STMT}, {"a", DesignEntity::ASSIGN} };

            SYNONYM syn1 = "s";
            Declaration dec1 = Declaration(syn1, Attribute::STMT_NUM);
            SYNONYM syn2 = "a";
            Declaration dec2 = Declaration(syn2, Attribute::STMT_NUM);
            SELECTION_LIST sl{ dec1, dec2 };

            // such that Follows(s, a)
            QueryArg l = QueryArg(dec1);
            QueryArg r = QueryArg(dec2);
            QueryClause c1 = QueryClause(CLAUSE_TYPE::SUCHTHAT_CL, l, r, Relationship::FOLLOWS);

            // pattern a("x", _)
            std::string var = "x";
            QueryArg ll = QueryArg(var, ARG_TYPE::NAME);
            std::string emp = "";
            QueryArg rr = QueryArg(emp, ARG_TYPE::WILDCARD);
            QueryClause c2 = QueryClause(CLAUSE_TYPE::PATTERN_CL, ll, rr, syn2);
            CLAUSE_LIST cl{ c1, c2 };

            WITH_CONDITION_LIST wcl{ };

            SelectionType st = SelectionType::TUPLE;

            Query q = Query(dt, cl, sl, wcl, st);
            QueryEvaluator qe = QueryEvaluator(ptr);
            IntermediateAnswer act = qe.evaluateQueryObject(&q);

            Assert::IsTrue(act.getSelection()[0].getSynonym() == "s");
            Assert::IsTrue(act.getSelection()[0].getAttribute() == Attribute::STMT_NUM);
            Assert::IsTrue(act.getSelection()[1].getSynonym() == "a");
            Assert::IsTrue(act.getSelection()[1].getAttribute() == Attribute::STMT_NUM);

            // answer to such that
            std::vector<ROW> exp1 = { {"1", "2"}, {"2", "3"} };
            std::vector<ROW> res1 = act.getClauseAnsList()[2].getResultTable().getResultTable();
            Assert::IsTrue(exp1 == res1);
            // answer to pattern
            std::vector<ROW> exp2 = { {"1"}, {"2"}, {"3"} };
            std::vector<ROW> res2 = act.getClauseAnsList()[3].getResultTable().getResultTable();
            Assert::IsTrue(exp2 == res2);

            Assert::IsTrue(act.getSelectionType() == st);
        }
        TEST_METHOD(EvaluateQueryObject_SelectBooleanSuchThatPatternWith_Success) {
            pkb.propagateStmtList(3);
            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "x", "+ x 1");
            pkb.storeAssignStatement(3, "x", "+ x 1");
            pkb.storeConstant("1");
            pkb.storeVariable("x");
            pkb.sortAllDataStructure();

            DECLARATION_TABLE dt{ {"s", DesignEntity::STMT}, {"a", DesignEntity::ASSIGN} };

            SYNONYM syn1 = "s";
            Declaration dec1 = Declaration(syn1, Attribute::STMT_NUM);
            SYNONYM syn2 = "a";
            Declaration dec2 = Declaration(syn2, Attribute::STMT_NUM);
            SELECTION_LIST sl{ dec1, dec2 };

            // such that Follows(s, a)
            QueryArg l = QueryArg(dec1);
            QueryArg r = QueryArg(dec2);
            QueryClause c1 = QueryClause(CLAUSE_TYPE::SUCHTHAT_CL, l, r, Relationship::FOLLOWS);

            // pattern a("x", _)
            std::string var = "x";
            QueryArg ll = QueryArg(var, ARG_TYPE::NAME);
            std::string emp = "";
            QueryArg rr = QueryArg(emp, ARG_TYPE::WILDCARD);
            QueryClause c2 = QueryClause(CLAUSE_TYPE::PATTERN_CL, ll, rr, syn2);
            CLAUSE_LIST cl{ c1, c2 };

            // with a.stmt# = 2
            QueryArg lll = QueryArg(dec2);
            std::string num = "2";
            QueryArg rrr = QueryArg(num, ARG_TYPE::NUM);
            QueryClause c3 = QueryClause(CLAUSE_TYPE::WITH_CL, lll, rrr);
            WITH_CONDITION_LIST wcl{ c3 };

            SelectionType st = SelectionType::BOOLEAN;

            Query q = Query(dt, cl, sl, wcl, st);
            QueryEvaluator qe = QueryEvaluator(ptr);
            IntermediateAnswer act = qe.evaluateQueryObject(&q);

            Assert::IsTrue(act.getSelection().size() == 0);

            // answer to such that
            std::vector<ROW> exp1 = { {"1", "2"}, {"2", "3"} };
            std::vector<ROW> res1 = act.getClauseAnsList()[1].getResultTable().getResultTable();
            Assert::IsTrue(exp1 == res1);
            // answer to pattern
            std::vector<ROW> exp2 = { {"1"}, {"2"}, {"3"} };
            std::vector<ROW> res2 = act.getClauseAnsList()[2].getResultTable().getResultTable();
            Assert::IsTrue(exp2 == res2);
            // answer to with - added to clauseList first
            std::vector<ROW> exp3 = { {"2"} };
            std::vector<ROW> res3 = act.getClauseAnsList()[0].getResultTable().getResultTable();
            Assert::IsTrue(exp3 == res3);

            Assert::IsTrue(act.getSelectionType() == st);
        }
    private:
        PKB pkb;
        PKB* ptr = &pkb;
        std::list<std::string> resultsList;
    };
}  // namespace UnitTesting
