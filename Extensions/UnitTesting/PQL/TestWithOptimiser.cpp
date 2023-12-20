#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Optimiser/WithOptimiser.h"
#include "QueryTestHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestWithOptimiser) {
    public:
        TEST_METHOD(Test_WithReplacement_SingleSuchThat) {
            //  stmt s; procedure proc, call;
            //  Select s such that Calls(proc, call) with proc.procName = "test"
            CLAUSE_LIST clList;
            CLAUSE_LIST withClList;
            DECLARATION_TABLE declrTable = {
                {"proc", DesignEntity::PROCEDURE},
                {"call", DesignEntity::PROCEDURE},
                {"s", DesignEntity::STMT}
            };
            SELECTION_LIST selectionList = {
                Declaration((std::string) "s", Attribute::STMT_NUM)
            };
            WithOptimiser opt;
            SuchThatClause cl1 = QueryTestHelper::createSuchThatCl(
                QueryArgType::DECLARATION, (std::string)"proc", Attribute::PROC_NAME,
                QueryArgType::DECLARATION, (std::string)"call",
                Attribute::PROC_NAME, Relationship::CALLS);
            clList.push_back(cl1);
            WithClause withCl1 = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"proc", Attribute::PROC_NAME, QueryArgType::NAME,
                (std::string)"test", Attribute::NONE);

            withClList.push_back(withCl1);
            Query query(declrTable, clList, selectionList, withClList, SelectionType::SINGLE);
            Query optimised = opt.replaceSynonyms(query);
            QueryClause optimisedCl =
                optimised.getClauseList()[0];

            // check that such that clause details replaced:
            // leftArg: "test"
            // rightArg: still calls (unchanged)
            verifySuchThatArgs(
                optimisedCl, QueryArgType::NAME, QueryArgType::DECLARATION,
                (std::string) "test", (std::string) "call", 0, 0, Relationship::CALLS);
            // check that the there is 1 with clause remaining
            Assert::IsTrue(optimised.getWithConditionList().size() == 1);
        }

        TEST_METHOD(Test_WithReplacement_TwoDeclarationReplacement) {
            // stmt s; if ifs;
            // Select s such that Follows (s,p) with s.stmt# = p.stmt#

            // one in selection
            CLAUSE_LIST clList;
            CLAUSE_LIST withClList;
            DECLARATION_TABLE declrTable = {
                {"s", DesignEntity::STMT},
                {"ifs", DesignEntity::IF}
            };

            SELECTION_LIST selectionList = {
                Declaration((std::string) "s", Attribute::STMT_NUM)
            };
            WithOptimiser opt;
            SuchThatClause cl1 = QueryTestHelper::createSuchThatCl(
                QueryArgType::DECLARATION, (std::string)"s", Attribute::STMT_NUM,
                QueryArgType::DECLARATION, (std::string)"ifs",
                Attribute::STMT_NUM, Relationship::FOLLOWS);
            clList.push_back(cl1);
            WithClause withCl1 = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"s", Attribute::STMT_NUM, QueryArgType::DECLARATION,
                (std::string)"ifs", Attribute::STMT_NUM);

            withClList.push_back(withCl1);
            Query query(declrTable, clList, selectionList, withClList, SelectionType::SINGLE);
            Query optimised = opt.replaceSynonyms(query);
            QueryClause optimisedCl =
                optimised.getClauseList()[0];

            // check that such that clause details replaced:
            // leftArg: "s"
            // rightArg: "s"
            verifySuchThatArgs(
                optimisedCl, QueryArgType::DECLARATION, QueryArgType::DECLARATION,
                (std::string) "s", (std::string) "s", 0, 0, Relationship::FOLLOWS);
            // check that the there are no with clauses remaining
            Assert::IsTrue(optimised.getWithConditionList().size() == 1);
        }

        TEST_METHOD(Test_WithOptimiser_NoReplacement) {
            // stmt s; procedure proc, call;
            // Select s such that Calls(proc, call) with proc.procName = "test"
            CLAUSE_LIST clList;
            CLAUSE_LIST withClList;
            DECLARATION_TABLE declrTable = {
                {"proc", DesignEntity::PROCEDURE},
                {"call", DesignEntity::PROCEDURE},
                {"s", DesignEntity::STMT}
            };
            SELECTION_LIST selectionList = {
                Declaration((std::string) "s", Attribute::STMT_NUM)
            };
            WithOptimiser opt;
            SuchThatClause cl1 = QueryTestHelper::createSuchThatCl(
                QueryArgType::DECLARATION, (std::string)"proc", Attribute::PROC_NAME,
                QueryArgType::DECLARATION, (std::string)"call",
                Attribute::PROC_NAME, Relationship::CALLS);
            clList.push_back(cl1);
            WithClause withCl1 = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"proc", Attribute::PROC_NAME, QueryArgType::NAME,
                (std::string)"test", Attribute::NONE);

            withClList.push_back(withCl1);
            Query query(declrTable, clList, selectionList, withClList, SelectionType::SINGLE);
        }

        TEST_METHOD(Test_MultipleSelection_Replacement) {
            // stmt s; read r1, r2; print p; call c;
            // Select <s, r1.stmt#, c.procName>
            // such that Follows(r1, r2) and Uses (c, p) with c.stmt# = 2 and 3 = 3
            WithOptimiser opt;
            CLAUSE_LIST clList;
            CLAUSE_LIST withClList;
            DECLARATION_TABLE declrTable = {
                {"s", DesignEntity::STMT},
                {"r1", DesignEntity::READ},
                {"r2", DesignEntity::READ},
                {"p", DesignEntity::PRINT},
                {"c", DesignEntity::CALL},
            };
            // build query
            SELECTION_LIST selectionList = {
                Declaration((std::string) "s", Attribute::STMT_NUM),
                Declaration((std::string) "r1", Attribute::STMT_NUM),
                Declaration((std::string)"c",
                Attribute::PROC_NAME)
            };
            SuchThatClause cl = QueryTestHelper::createSuchThatCl(
                QueryArgType::DECLARATION, (std::string)"r1", Attribute::STMT_NUM,
                QueryArgType::DECLARATION, (std::string)"r2",
                Attribute::STMT_NUM, Relationship::FOLLOWS);
            clList.push_back(cl);
            cl =  QueryTestHelper::createSuchThatCl(
                QueryArgType::DECLARATION, (std::string)"c", Attribute::STMT_NUM,
                QueryArgType::DECLARATION, (std::string)"p",
                Attribute::STMT_NUM, Relationship::USES);
            clList.push_back(cl);
            WithClause withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"c", Attribute::STMT_NUM, QueryArgType::NUM,
                (std::string)"2", Attribute::NONE);
            withClList.push_back(withCl);
            withCl = QueryTestHelper::createWithCl(QueryArgType::NUM,
                (std::string)"3", Attribute::NONE, QueryArgType::NUM,
                (std::string)"3", Attribute::NONE);
            withClList.push_back(withCl);
            Query query(declrTable, clList, selectionList, withClList, SelectionType::TUPLE);

            // expected optimised query: trivial with clause discarded
            // stmt s; read r1, r2; print p; call c;
            // Select <s, r1.stmt#, c.procName>
            // such that Follows(r1, r2) and Uses (c, p) with c.stmt# = 2
            Query optimised = opt.replaceSynonyms(query);
            // 1 with clause remaining
            Assert::IsTrue(optimised.getWithConditionList().size() == 1);
            // selection list still has 3
            Assert::IsTrue(optimised.getSelectionList().size() == 3);
            // selection list not modified
            Assert::IsTrue(optimised.getSelectionList() == selectionList);
            CLAUSE_LIST optimisedClList = optimised.getClauseList();
            Assert::IsTrue(optimisedClList.size() == 2);
            verifySuchThatArgs(
                optimisedClList[0], QueryArgType::DECLARATION, QueryArgType::DECLARATION,
                "r1", "r2", 0, 0, Relationship::FOLLOWS);
            verifySuchThatArgs(
                optimisedClList[1], QueryArgType::DECLARATION, QueryArgType::DECLARATION,
                "c", "p", 0, 0, Relationship::USES);

            // stmt s; read r1, r2; print p; call c;
            // Select <s, r1.stmt#, r2>
            // such that Follows(r1, r2) and Uses (c, p) with c.stmt# = 2 and 3 = 3
            // uses a different selection from the previous test
            selectionList = {
                Declaration((std::string) "s", Attribute::STMT_NUM),
                Declaration((std::string) "r1", Attribute::STMT_NUM),
                Declaration((std::string)"r2", Attribute::STMT_NUM)
            };
            opt = WithOptimiser();
            query = Query(declrTable, clList, selectionList, withClList, SelectionType::TUPLE);

            // expected optimised query: c is not replaced as the replacement is not a string
            // stmt s; read r1, r2; print p; call c;
            // Select <s, r1.stmt#, c.procName>
            // such that Follows(r1, r2) and Uses (c, p) with c.stmt# = 2
            optimised = opt.replaceSynonyms(query);
            // 1 with clause remaining
            Assert::IsTrue(optimised.getWithConditionList().size() == 1);
            // selection list still has 3
            Assert::IsTrue(optimised.getSelectionList().size() == 3);
            // selection list not modified
            Assert::IsTrue(optimised.getSelectionList() == selectionList);
            optimisedClList = optimised.getClauseList();
            Assert::IsTrue(optimisedClList.size() == 2);
            verifySuchThatArgs(
                optimisedClList[0], QueryArgType::DECLARATION, QueryArgType::DECLARATION,
                "r1", "r2", 0, 0, Relationship::FOLLOWS);
            verifySuchThatArgs(
                optimisedClList[1], QueryArgType::DECLARATION, QueryArgType::DECLARATION,
                "c", "p", 0, 0, Relationship::USES);
        }

        TEST_METHOD(Test_ComplicatedReplacement) {
            // stmt s; read r; print p; call c; assign a; variable v;
            // Select <s, r.stmt# >
            // such that Follows (c, p) and pattern a("x", "y")
            // such that Uses(s,v)
            // with v.varName = "hello" and a.stmt# = 3 and c.procName = "byebye"
            // with c.stmt# = 2 with p.varName = "x"

            WithOptimiser opt;
            CLAUSE_LIST clList;
            CLAUSE_LIST withClList;
            DECLARATION_TABLE declrTable = {
                {"s", DesignEntity::STMT},
                {"r", DesignEntity::READ},
                {"a", DesignEntity::ASSIGN},
                {"p", DesignEntity::PRINT},
                {"c", DesignEntity::CALL},
                {"v", DesignEntity::VARIABLE},
            };
            // build query
            SELECTION_LIST selectionList = {
                Declaration((std::string) "s", Attribute::STMT_NUM),
                Declaration((std::string) "r", Attribute::STMT_NUM)
            };
            SuchThatClause cl = QueryTestHelper::createSuchThatCl(
                QueryArgType::DECLARATION, (std::string)"c", Attribute::STMT_NUM,
                QueryArgType::DECLARATION, (std::string)"p",
                Attribute::STMT_NUM, Relationship::FOLLOWS);
            clList.push_back(cl);
            cl =  QueryTestHelper::createSuchThatCl(
                QueryArgType::DECLARATION, (std::string)"s", Attribute::STMT_NUM,
                QueryArgType::DECLARATION, (std::string)"v",
                Attribute::VAR_NAME, Relationship::USES);
            clList.push_back(cl);
            PatternClause patternCl = QueryTestHelper::createPatternCl(
                QueryArgType::DECLARATION, (std::string) "v", Attribute::VAR_NAME,
                QueryArgType::PATTERN_EXPR, (std::string) "y", (std::string)"", (std::string) "a");
            clList.push_back(patternCl);
            WithClause withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"v", Attribute::VAR_NAME, QueryArgType::NAME,
                (std::string)"hello", Attribute::NONE);
            withClList.push_back(withCl);
            withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"a", Attribute::STMT_NUM, QueryArgType::NUM,
                (std::string)"3", Attribute::NONE);
            withClList.push_back(withCl);
            withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"c", Attribute::PROC_NAME, QueryArgType::NAME,
                (std::string)"byebye", Attribute::NONE);
            withClList.push_back(withCl);
            withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"c", Attribute::STMT_NUM, QueryArgType::NUM,
                (std::string)"2", Attribute::NONE);
            withClList.push_back(withCl);
            withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"p", Attribute::VAR_NAME, QueryArgType::NAME,
                (std::string)"x", Attribute::NONE);
            withClList.push_back(withCl);
            Query query(declrTable, clList, selectionList, withClList, SelectionType::TUPLE);

            // original query:
            // Select <s, r.stmt# >
            // such that Follows (c, p) and pattern a(v, "y")
            // such that Uses(s,v)
            // with v.varName = "hello" and a.stmt# = 3 and c.procName = "byebye"
            // with c.stmt# = 2 with p.varName = "x"
            // optimised query:
            // Select <s, r.stmt# >
            // such that Follows (2, p) and pattern a("hello", "y")
            // such that Uses(s,"hello")
            // with a.stmt# = 3 and c.procName = "byebye" with p.varName = "x"
            Query optimised = opt.replaceSynonyms(query);
            Assert::IsTrue(optimised.getWithConditionList().size() == 5);
            // selection list still has 2
            Assert::IsTrue(optimised.getSelectionList().size() == 2);
            // selection list not modified
            Assert::IsTrue(optimised.getSelectionList() == selectionList);
            CLAUSE_LIST optimisedClList = optimised.getClauseList();
            Assert::IsTrue(optimisedClList.size() == 3);
            verifySuchThatArgs(
                optimisedClList[0], QueryArgType::NUM, QueryArgType::DECLARATION,
                (std::string)"", "p", 2, 0, Relationship::FOLLOWS);
            verifySuchThatArgs(
                optimisedClList[1], QueryArgType::DECLARATION, QueryArgType::NAME,
                "s", "hello", 0, 0, Relationship::USES);
            verifyPatternArgs(
                optimisedClList[2], QueryArgType::NAME, QueryArgType::PATTERN_EXPR,
                (std::string)"hello", (std::string)"y", declrTable, (std::string)"a");
        }
        TEST_METHOD(Test_EmptyClauseList_And_BothDeclaration_With) {
            // procedure p; call c; constant co; stmt s;
            // Select p with p.procName = c.procName and
            // c.stmt# = s.stmt# and s.stmt# = co.value
            WithOptimiser opt;
            CLAUSE_LIST clList;
            CLAUSE_LIST withClList;
            DECLARATION_TABLE declrTable = {
                {"p", DesignEntity::PROCEDURE},
                {"c", DesignEntity::CALL},
                {"co", DesignEntity::CONSTANT},
                {"s", DesignEntity::STMT}
            };
            // build query
            SELECTION_LIST selectionList = {
                Declaration((std::string) "p", Attribute::PROC_NAME)
            };
            WithClause withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"p", Attribute::PROC_NAME, QueryArgType::DECLARATION,
                (std::string)"c", Attribute::PROC_NAME);
            withClList.push_back(withCl);
            withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"c", Attribute::STMT_NUM, QueryArgType::DECLARATION,
                (std::string)"s", Attribute::STMT_NUM);
            withClList.push_back(withCl);
            withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"s", Attribute::STMT_NUM, QueryArgType::DECLARATION,
                (std::string)"co", Attribute::VALUE);
            withClList.push_back(withCl);
            Query query(declrTable, clList, selectionList, withClList, SelectionType::TUPLE);

            // original:
            // procedure p; call c; constant co; stmt s;
            // Select p with p.procName = c.procName and
            // c.stmt# = s.stmt# and s.stmt# = co.value
            // nothing changed
            Query optimisedQuery = opt.replaceSynonyms(query);
            Assert::IsTrue(optimisedQuery.getClauseList().size() == 0);
            Assert::IsTrue(optimisedQuery.getWithConditionList().size() == 3);
        }

        TEST_METHOD(Test_EmptyClauseList_OneWithClause) {
            // stmt s; Select s with s.stmt# = 42
            WithOptimiser opt;
            CLAUSE_LIST clList;
            CLAUSE_LIST withClList;
            DECLARATION_TABLE declrTable = {
                {"s", DesignEntity::STMT}
            };
            // build query
            SELECTION_LIST selectionList = {
                Declaration((std::string) "s", Attribute::STMT_NUM)
            };
            WithClause withCl = QueryTestHelper::createWithCl(QueryArgType::DECLARATION,
                (std::string)"s", Attribute::STMT_NUM, QueryArgType::NUM,
                (std::string)"42", Attribute::PROC_NAME);
            withClList.push_back(withCl);
            Query query(declrTable, clList, selectionList, withClList, SelectionType::TUPLE);

            // original:
            // procedure p; call c; constant co; stmt s;
            // Select p with p.procName = c.procName and
            // c.stmt# = s.stmt# and s.stmt# = co.value
            // nothing changed
            Query optimisedQuery = opt.replaceSynonyms(query);
            Assert::IsTrue(optimisedQuery.getClauseList().size() == 0);
            Assert::IsTrue(optimisedQuery.getWithConditionList().size() == 1);
        }

        /*
        * Helper to test the clause type, left and right arg types of the specified QueryClause     
        */
        void verifySuchThatArgs(QueryClause cl, QueryArgType expectedTypeL,
            QueryArgType expectedTypeR, std::string strL, std::string strR,
            int intL, int intR, Relationship rs) {
            Assert::IsTrue(cl.getClauseType() == ClauseType::SUCHTHAT_CL);
            Assert::IsTrue(cl.getLeftArg().getArgType() == expectedTypeL);
            Assert::IsTrue(cl.getRightArg().getArgType() == expectedTypeR);
            Assert::IsTrue(cl.getRelationship() == rs);

            switch (expectedTypeL) {
                case QueryArgType::DECLARATION:
                    Assert::AreEqual(strL, cl.getLeftArg().getDeclaration().getSynonym());
                    break;
                case QueryArgType::NAME:
                    Assert::AreEqual(strL, cl.getLeftArg().getName());
                    break;
                case QueryArgType::NUM:
                    Assert::AreEqual(intL, cl.getLeftArg().getLineNum());
                    break;
            }

            switch (expectedTypeR) {
                case QueryArgType::DECLARATION:
                    Assert::AreEqual(strR, cl.getRightArg().getDeclaration().getSynonym());
                    break;
                case QueryArgType::NAME:
                    Assert::AreEqual(strR, cl.getRightArg().getName());
                    break;
                case QueryArgType::NUM:
                    Assert::AreEqual(intR, cl.getRightArg().getLineNum());
                    break;
            }
        }

        void verifyPatternArgs(QueryClause cl, QueryArgType expectedTypeL,
            QueryArgType expectedTypeR, std::string strL, std::string strR,
            DECLARATION_TABLE& declrTable, std::string designEnt) {
            Assert::IsTrue(cl.getClauseType() == ClauseType::PATTERN_CL);
            Assert::IsTrue(cl.getLeftArg().getArgType() == expectedTypeL);
            Assert::IsTrue(cl.getRightArg().getArgType() == expectedTypeR);
            Assert::AreEqual(designEnt, cl.getDesignEntitySynonym());


            std::string syn;
            switch (cl.getLeftArg().getArgType()) {
                case QueryArgType::DECLARATION:
                    syn = cl.getLeftArg().getDeclaration().getSynonym();
                    Assert::IsTrue(
                        cl.getLeftArg().getDeclaration().getAttribute() ==
                        defaultDesignEntityAttrLookup.at(declrTable.at(syn)));
                    Assert::AreEqual(strL, syn);
                    break;
                case QueryArgType::NAME:
                    Assert::AreEqual(strL, cl.getLeftArg().getName());
                    break;
                case QueryArgType::DECLARATION_WITH_ATTR:
                    Assert::Fail();  // no declr w attr allowed
                    break;
                default:
                    Assert::Fail();
            }

            switch (cl.getRightArg().getArgType()) {
                case QueryArgType::PATTERN_EXPR:
                    Assert::AreEqual(strR, cl.getRightArg().getPatternExpr());
                    break;
                case QueryArgType::PATTERN_EXPR_WITH_WILDCARDS:
                    Assert::AreEqual(strR, cl.getRightArg().getPatternExpr());
                    break;
                case QueryArgType::WILDCARD:
                    break;
                default:
                    Assert::Fail();
            }
        }

        void checkDeclaration(std::string& syn, Attribute attr, Declaration& actual) {
            Assert::IsTrue(attr == actual.getAttribute());
            Assert::AreEqual(syn, actual.getSynonym());
        }
    };
}  // using namespace UnitTesting
