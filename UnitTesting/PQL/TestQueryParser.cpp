#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/QueryParser.h"
#include "QueryTestHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestQueryParser) {
    public:
        // ITER 1 - UNIT TESTS
        TEST_METHOD(Test_ParseTrivialQuery) {
            std::string queryString = "constant c; Select c";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getClauseList().size() == 0);
            Assert::IsTrue(query.getDeclarations().find("c")->second == DesignEntity::CONSTANT);
            Assert::AreEqual(1, static_cast<int>(query.getSelectionList().size()));
            Assert::IsTrue(query.getSelectionList()[0].getSynonym() == "c");
        }

        TEST_METHOD(Test_InvalidCharactersAsArgs) {
            // test 1
            std::string queryString = "stmt s; Select s such that Uses (s, _\"x123\"_)";
            QueryParser parser = QueryParser();
            bool correctException = false;
            try {
                 Query query = parser.parseQuery(queryString);
            } catch (InvalidQuerySyntaxException) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

        TEST_METHOD(Test_ParseQuery_InvalidSelectionSyntax) {
            // test 1
            // this is counted as selection syntax exception as
            // selection is not followed by such that / pattern / with
            // name of the exception can be changed
            std::string queryString = "stmt s; Select s Such That Uses "
                "(s, \"a\") pattern (_, \"a\")";
            QueryParser parser = QueryParser();
            bool correctException = false;
            try {
                 Query query = parser.parseQuery(queryString);
            } catch (InvalidSelectionSyntaxException) {
                correctException = true;
            }
            Assert::IsTrue(correctException);

            // test 2
            queryString = "stmt s; Select s lalala such that Uses (s, \"a\")";
            correctException = false;
            try {
                 Query query = parser.parseQuery(queryString);
            } catch (InvalidSelectionSyntaxException) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

        TEST_METHOD(Test_ParseQuery_InvalidQuery_WrongDesignEntity) {
            std::string queryString = "assign assignment; statement s; Select s";
            QueryParser parser = QueryParser();
            bool correctException = false;
            try {
                 Query query = parser.parseQuery(queryString);
            } catch (InvalidDeclarationException e) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

         TEST_METHOD(Test_InvalidQuery_SelectLumpedTogether) {
            std::string queryString = "assign a; stmt sSuch;Select sSuch That Follows(1,2)";
            QueryParser parser = QueryParser();
            bool correctException = false;
            try {
                 Query query = parser.parseQuery(queryString);
            } catch (InvalidSelectionSyntaxException) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

        TEST_METHOD(Test_ParseSuchThat_InvalidLeftArgForModifies) {
            std::string queryString = "Modifies(p, \"x\")";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {
                {"p", DesignEntity::PRINT},
                {"s", DesignEntity::STMT}
            };
            bool correctException = false;
            try {
                parser.parseSuchThat(declrTable, queryString);
            } catch (InvalidSuchThatDesignEntityException) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

        TEST_METHOD(Test_ParseSuchThat_InvalidStmt) {
            std::string queryString = "Parent*(1,2)pattern a ( 1, 2)";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {
                {"p", DesignEntity::PRINT},
                {"s", DesignEntity::STMT}
            };
            bool correctException = false;
            try {
                parser.parseSuchThat(declrTable, queryString);
            } catch (InvalidQuerySyntaxException) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

        TEST_METHOD(Test_ParseSuchThat_InvalidLeftArgForUses) {
            std::string queryString = "Uses(p, \"x\")";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {
                {"p", DesignEntity::READ},
                {"s", DesignEntity::STMT}
            };
            bool correctException = false;
            try {
                parser.parseSuchThat(declrTable, queryString);
            } catch (InvalidSuchThatDesignEntityException) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

        TEST_METHOD(Test_ParseQuery_OneSuchThat_OnePattern) {
             std::string queryString = "stmt s; variable v; assign assign;"
                "Select s such that Uses(s,v) pattern assign (_, \"x\")";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            Assert::AreEqual(static_cast<int>(query.getClauseList().size()), 2);  // two clauses
            DECLARATION_TABLE declrTable = query.getDeclarations();
            // check declaration table
            Assert::IsTrue(declrTable.find("s")->second == DesignEntity::STMT);
            Assert::IsTrue(declrTable.find("v")->second == DesignEntity::VARIABLE);
            Assert::IsTrue(declrTable.find("assign")->second == DesignEntity::ASSIGN);
            Assert::AreEqual(query.getSelectionList()[0].getSynonym(), (std::string) "s");
            Assert::AreEqual(static_cast<int>(declrTable.size()), 3);  // declrTable has size of 1
            // check clause
            QueryClause cl = query.getClauseList()[0];
            Assert::IsTrue(cl.getClauseType() == ClauseType::SUCHTHAT_CL);  // is a such that clause
            // check arguments
            Assert::IsTrue(cl.getRelationship() == Relationship::USES);
            QueryArg leftArg = cl.getLeftArg();
            QueryArg rightArg = cl.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::AreEqual(leftArg.getDeclaration().getSynonym(), (std::string)"s");
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::DECLARATION);
            Assert::AreEqual(rightArg.getDeclaration().getSynonym(), (std::string)"v");

            // check pattern clause
            QueryClause clP = query.getClauseList()[1];
            QueryArg leftArgP = clP.getLeftArg();
            QueryArg rightArgP = clP.getRightArg();
            Assert::IsTrue(leftArgP.getArgType() == QueryArgType::WILDCARD);
            Assert::IsTrue(rightArgP.getArgType() == QueryArgType::PATTERN_EXPR);
            Assert::AreEqual(rightArgP.getPatternExpr(), (std::string)"x");  // prefix expression
            Assert::AreEqual(clP.getDesignEntitySynonym(), (std::string)"assign");
        }

        TEST_METHOD(Test_ParseQuery_InvalidQuery_MissingSynonym) {
            std::string queryString = "assign assignment; stmt  ; Select assignment";
            QueryParser parser = QueryParser();
            bool correctException = false;
            try {
                 Query query = parser.parseQuery(queryString);
            } catch (InvalidDeclarationException e) {
                correctException = true;
            }
            Assert::IsTrue(correctException);
        }

        TEST_METHOD(Test_ParseQuery_SinglePatternClause) {
            std::string queryString = "assign a; variable v; "
                "Select v pattern a   ( v , _\"x123\"_  )";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            Assert::AreEqual(query.getSelectionList()[0].getSynonym(), (std::string) "v");
            Assert::AreEqual(static_cast<int>(query.getClauseList().size()), 1);
            QueryClause patternCl = query.getClauseList()[0];
            Assert::IsTrue(patternCl.getClauseType() == ClauseType::PATTERN_CL);
            Assert::AreEqual(patternCl.getDesignEntitySynonym(), (std::string) "a");
            QueryArg leftArg = patternCl.getLeftArg();
            QueryArg rightArg = patternCl.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR_WITH_WILDCARDS);
            Assert::AreEqual(rightArg.getPatternExpr(), (std::string) "x123");
        }

        TEST_METHOD(Test_ParseQuery_ParentStarRelationship) {
            std::string queryString = "stmt s; Select s such that Parent*(s,3)";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            Assert::AreEqual(static_cast<int>(query.getClauseList().size()), 1);  // one clause
            DECLARATION_TABLE declrTable = query.getDeclarations();
            // check declaration table
            Assert::IsTrue(declrTable.find("s")->second == DesignEntity::STMT);
            Assert::AreEqual(query.getSelectionList()[0].getSynonym(), (std::string) "s");
            Assert::AreEqual(static_cast<int>(declrTable.size()), 1);  // declrTable has size of 1
            // check clause
            QueryClause cl = query.getClauseList()[0];
            Assert::IsTrue(cl.getClauseType() == ClauseType::SUCHTHAT_CL);  // is a such that clause
            // check arguments
            Assert::IsTrue(cl.getRelationship() == Relationship::PARENT_STAR);
            QueryArg leftArg = cl.getLeftArg();
            QueryArg rightArg = cl.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::AreEqual(leftArg.getDeclaration().getSynonym(), (std::string)"s");
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::NUM);
            Assert::AreEqual(rightArg.getLineNum(), 3);
        }

        TEST_METHOD(Test_ParsePattern_NameAsLeftArg) {
            std::string patternStmt = "a (\"x\", _)";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {{"a", DesignEntity::ASSIGN}};
            PatternClause cl = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(cl.getClauseType() == ClauseType::PATTERN_CL);
            QueryArg leftArg = cl.getLeftArg();
            QueryArg rightArg = cl.getRightArg();
            Assert::AreEqual(leftArg.getName(), (std::string) "x");
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::NAME);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::WILDCARD);
        }

        TEST_METHOD(Test_ParsePattern_TabsBetweenQuotations) {
            std::string patternStmt = "a (\"x\", \"     read\")";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {{"a", DesignEntity::ASSIGN}};
            PatternClause cl = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(cl.getClauseType() == ClauseType::PATTERN_CL);
            QueryArg leftArg = cl.getLeftArg();
            QueryArg rightArg = cl.getRightArg();
            Assert::AreEqual(leftArg.getName(), (std::string) "x");
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::NAME);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR);
            Assert::AreEqual(rightArg.getPatternExpr(), (std::string) "read");
        }

        TEST_METHOD(Test_ParseDeclaration_InvalidDeclaration_Number) {
            DECLARATION_TABLE declrTable;  // start with empty declaration table
            QueryParser parser = QueryParser();
            std::vector<std::string> statements = { " assign assignment",
                "stmt 1", "Select assignment"};
            try {
                parser.parseDeclarations(declrTable, statements);
                Assert::Fail();  // code should not reach this point
            }
            catch (InvalidDeclarationException e) {
                Assert::IsTrue(true);
            }
        }
        TEST_METHOD(Test_ParseDeclaration_InvalidDeclaration_DesignEntity) {
            DECLARATION_TABLE declrTable;  // start with empty declaration table
            QueryParser parser = QueryParser();
            std::vector<std::string> statements = { " assign assignment",
                "statement s", "Select assignment" };
            auto err = [&] {parser.parseDeclarations(declrTable, statements);};
            Assert::ExpectException<InvalidDeclarationException>(err);
        }
        TEST_METHOD(Test_ParseDeclaration_InvalidDeclaration_Blank) {
            DECLARATION_TABLE declrTable;  // start with empty declaration table
            QueryParser parser = QueryParser();
            std::vector<std::string> statements = {
                " assign assignment", "stmt  ", "Select assignment"
            };
            auto err = [&] {parser.parseDeclarations(declrTable, statements);};
            Assert::ExpectException<InvalidDeclarationException>(err);
        }
        TEST_METHOD(Test_ParsePattern_SynonymNotDeclared) {
            // negative test
            std::string patternStmt = "s (\"x\", _)";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {{"a", DesignEntity::ASSIGN}};
            auto err = [&] {parser.parsePattern(declrTable, patternStmt);};
            Assert::ExpectException<MissingDeclarationException>(err);
        }

        TEST_METHOD(Test_ParsePattern_ValidPatternExpr) {
            DECLARATION_TABLE declrTable = {
                {"a", DesignEntity::ASSIGN},
                {"assign", DesignEntity::ASSIGN},
                {"var", DesignEntity::VARIABLE}
            };

            // test 1 - name, single term with spaces
            std::string patternStmt = "a (\"test\", \" a123    \")";
            QueryParser parser = QueryParser();
            PatternClause pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            QueryArg leftArg = pc.getLeftArg();
            QueryArg rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::NAME);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR);
            // should have quotations removed
            Assert::AreEqual((std::string) "test", leftArg.getName());
            // should have quotations, spaces removed
            Assert::AreEqual((std::string) "a123", rightArg.getPatternExpr());

            // test 2 - name, multiple terms no spaces
            patternStmt = "a (\"te1st123\", \"x+y+a1-a2/a3\")";
            pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            leftArg = pc.getLeftArg();
            rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::NAME);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR);
            Assert::AreEqual((std::string) "te1st123", leftArg.getName());
            Assert::AreEqual((std::string) "- + + x y a1 / a2 a3", rightArg.getPatternExpr());

            // test 3 - name, multiple terms with tabs
            patternStmt = "a (\"test\", \"xa +      10 +        a1  / 3\")";
            pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            leftArg = pc.getLeftArg();
            rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::NAME);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR);
            Assert::AreEqual((std::string) "test", leftArg.getName());
            Assert::AreEqual((std::string) "+ + xa 10 / a1 3", rightArg.getPatternExpr());

            // test 4 - synonym, multiple terms with brackets no spaces
            patternStmt = "a (var, \"(a123+2+3+testing-x/y%2)\")";
            pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            leftArg = pc.getLeftArg();
            rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR);
            Assert::AreEqual((std::string) "var", leftArg.getDeclaration().getSynonym());
            Assert::AreEqual((std::string) "- + + + a123 2 3 testing % / x y 2",
                rightArg.getPatternExpr());

            // test 5 - synonym, multiple terms with multiple brackets and tabs and spaces mixed
            patternStmt = "a (var, \"a145 +    x1  - (x2)"
                "  * (x3 + (x4-x5))    \")";
            pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            leftArg = pc.getLeftArg();
            rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR);
            Assert::AreEqual((std::string) "var", leftArg.getDeclaration().getSynonym());
            Assert::AreEqual((std::string) "- + a145 x1 * x2 + x3 - x4 x5",
                rightArg.getPatternExpr());

            // test 6 - synonym, multiple terms all same operator
            patternStmt = "a (  var  , \" a123 / 2 / 3/ 4/ (5 / 6)  \")";
            pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            leftArg = pc.getLeftArg();
            rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR);
            Assert::AreEqual((std::string) "var", leftArg.getDeclaration().getSynonym());
            Assert::AreEqual((std::string) "/ / / / a123 2 3 4 / 5 6", rightArg.getPatternExpr());

            // test 7 - synonym, multiple terms with wildcard
            patternStmt = "a (var, _    \"a145 +    x1  - (x2)  "
                "* (x3 + (x4-x5))    \"_)";
            pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            leftArg = pc.getLeftArg();
            rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::PATTERN_EXPR_WITH_WILDCARDS);
            Assert::AreEqual((std::string) "var", leftArg.getDeclaration().getSynonym());
            Assert::AreEqual((std::string) "- + a145 x1 * x2 + x3 - x4 x5",
                rightArg.getPatternExpr());

            // test 8 - synonym, wildcard
            patternStmt = "a (var, _) ";
            pc = parser.parsePattern(declrTable, patternStmt);
            Assert::IsTrue(pc.getClauseType() == ClauseType::PATTERN_CL);
            leftArg = pc.getLeftArg();
            rightArg = pc.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::WILDCARD);
            Assert::AreEqual((std::string) "var", leftArg.getDeclaration().getSynonym());
        }

        TEST_METHOD(Test_ParsePattern_InvalidPatternExpr) {
            DECLARATION_TABLE declrTable = {
            {"a", DesignEntity::ASSIGN},
            {"stmt", DesignEntity::STMT},
            {"expr", DesignEntity::CALL},
            };
            QueryParser parser = QueryParser();

            // test 1 - synonym undeclared
            std::string patternExpr = "ab ( s, \" a123    \")";
            auto err1 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<MissingDeclarationException>(err1);

            // test 2 - patternexpr no quotation marks
            patternExpr = "a (\"test\", a123)";
            auto err2 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err2);

            // test 3 - patternexpr term starts with a number
            patternExpr = "a (\"test\", \"1a23\")";
            auto err3 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err3);

            // test 4 - missing left arg
            patternExpr = "a (  , \"a23\")";
            auto err4 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err4);

            // test 5 - missing right arg
            patternExpr = "a (, \"a23\")";
            auto err5 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err5);

            // test 6 - missing both args
            patternExpr = "a (      )";
            auto err6 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err6);

            // test 7 - unclosed brackets
            patternExpr = "a (\"test\", \"a123 / ( 1 - 2\")";
            auto err7 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err7);

            // test 8 - invalid expr
            patternExpr = "a (\"test\", \"a12 +/ 3\")";
            auto err8 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err8);

            // test 9 - missing one wildcard
            patternExpr = "a (\"test\", _\"a23 + 2 + 3\")";
            auto err9 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err9);

            // test 10 - invalid characters in pattern
            patternExpr = "a (\"test\", \"a_23 + 3/4\")";
            auto err10 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err10);

            // test 11 - wrong left arg synonym
            patternExpr = "a (stmt, \"1a23\")";
            auto err11 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternDesignEntityException>(err11);

            // test 12 - wrong pattern synonym
            patternExpr = "expr (\"test\", \"1a23\")";
            auto err12 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternDesignEntityException>(err12);

            // test 13 - invalid pattern
            patternExpr = "a (\"test\", \"a23 + 3 astef ef / 4\")";
            auto err13 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err13);

            // test 14 - wrong pattern syntax
            patternExpr = "a (((\"test\", \"a23 + 3 astef ef / 4\")";
            auto err14 = [&] {parser.parsePattern(declrTable, patternExpr);};
            Assert::ExpectException<InvalidPatternArgumentSyntaxException>(err14);
        }

        TEST_METHOD(Test_ParseQueryWithSuchThat) {
            // query 1
            std::string queryString = "constant c; Select c such that Follows(1, 2)";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            Assert::IsTrue(static_cast<int>(query.getClauseList().size()) == 1);
            QueryClause suchThat = query.getClauseList()[0];
            Assert::IsTrue(suchThat.getClauseType() == ClauseType::SUCHTHAT_CL);
            Assert::IsTrue(suchThat.getRelationship() == Relationship::FOLLOWS);
            QueryArg argL = suchThat.getLeftArg();
            QueryArg argR = suchThat.getRightArg();
            Assert::IsTrue(argL.getArgType() == QueryArgType::NUM);
            Assert::IsTrue(argR.getArgType() == QueryArgType::NUM);
            Assert::IsTrue(argL.getLineNum() == 1);
            Assert::IsTrue(argR.getLineNum() == 2);

            // query 2
            queryString = "stmt s; Select s such that Modifies(s, \"    read\")";
            query = parser.parseQuery(queryString);
            Assert::IsTrue(static_cast<int>(query.getClauseList().size()) == 1);
            suchThat = query.getClauseList()[0];
            Assert::IsTrue(suchThat.getClauseType() == ClauseType::SUCHTHAT_CL);
            Assert::IsTrue(suchThat.getRelationship() == Relationship::MODIFIES);
            argL = suchThat.getLeftArg();
            argR = suchThat.getRightArg();
            Assert::IsTrue(argL.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(argR.getArgType() == QueryArgType::NAME);
            Assert::AreEqual(argL.getDeclaration().getSynonym(), (std::string)"s");
            Assert::AreEqual(argR.getName(), (std::string) "read");
        }

        TEST_METHOD(Test_ParseSuchThat_MultipleTabsAndSpaces) {
            std::string queryString = "Follows     (1  ,   3   )";
            DECLARATION_TABLE declrTable = {
                {"p", DesignEntity::PRINT},
                {"s", DesignEntity::STMT}
            };
            QueryParser parser = QueryParser();
            SuchThatClause stCl = parser.parseSuchThat(declrTable, queryString);
            Assert::IsTrue(stCl.getClauseType() == ClauseType::SUCHTHAT_CL);
            Assert::IsTrue(stCl.getLeftArg().getArgType() == QueryArgType::NUM);
            Assert::IsTrue(stCl.getRightArg().getArgType() == QueryArgType::NUM);
            Assert::AreEqual(stCl.getLeftArg().getLineNum(), 1);
            Assert::AreEqual(stCl.getRightArg().getLineNum(), 3);
        }

        // ITER 2 - TESTS
        TEST_METHOD(Test_ParsePattern_Invalid_NonAssignDesignEnt) {
            std::string patternStmt = "a (\"x\", _)";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {{"a", DesignEntity::VARIABLE}};
            auto err = [&] {parser.parsePattern(declrTable, patternStmt);};
            Assert::ExpectException<InvalidPatternDesignEntityException>(err);
        }

        TEST_METHOD(Test_ParsePattern_Valid_IfDesignEnt) {
            std::string patternStmt = "ifs (\"x\", _, _)";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {{"ifs", DesignEntity::IF}};

            QueryClause patternCl = parser.parsePattern(declrTable, patternStmt);

            Assert::IsTrue(patternCl.getClauseType() == ClauseType::PATTERN_CL);
            Assert::AreEqual((std::string) "ifs", patternCl.getDesignEntitySynonym());
            QueryArg argL = patternCl.getLeftArg();
            QueryArg argR = patternCl.getRightArg();

            Assert::IsTrue(argL.getArgType() == QueryArgType::NAME);
            Assert::IsTrue(argR.getArgType() == QueryArgType::WILDCARD);
            Assert::AreEqual((std::string) "x", argL.getName());  // quotations removed
        }

        TEST_METHOD(Test_ParsePattern_Valid_WhileDesignEnt) {
            std::string patternStmt = "w (\"    x   \", _)";
            QueryParser parser = QueryParser();
            DECLARATION_TABLE declrTable = {{"w", DesignEntity::WHILE}};

            QueryClause patternCl = parser.parsePattern(declrTable, patternStmt);

            Assert::IsTrue(patternCl.getClauseType() == ClauseType::PATTERN_CL);
            Assert::AreEqual((std::string) "w", patternCl.getDesignEntitySynonym());
            QueryArg argL = patternCl.getLeftArg();
            QueryArg argR = patternCl.getRightArg();

            Assert::IsTrue(argL.getArgType() == QueryArgType::NAME);
            Assert::IsTrue(argR.getArgType() == QueryArgType::WILDCARD);
            Assert::AreEqual((std::string) "x", argL.getName());  // quotations removed
        }

        TEST_METHOD(Test_ParseQuery_MultipleDeclarations_OneSTClause) {
            std::string queryString = "if ifs; while w; Select ifs such that Parent(ifs, w) ";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            Assert::AreEqual(static_cast<int>(query.getClauseList().size()), 1);  // one clause
            DECLARATION_TABLE declrTable = query.getDeclarations();
            // check declaration table
            Assert::AreEqual(static_cast<int>(declrTable.size()), 2);  // declrTable has size of 2
            Assert::IsTrue(declrTable.find("ifs")->second == DesignEntity::IF);
            Assert::IsTrue(declrTable.find("w")->second == DesignEntity::WHILE);
            // check selections
            SELECTION_LIST selectionList = query.getSelectionList();
            Assert::AreEqual(1, static_cast<int>(selectionList.size()));
            checkDeclaration((std::string) "ifs", Attribute::STMT_NUM, selectionList[0]);
            Assert::AreEqual(query.getSelectionList()[0].getSynonym(), (std::string) "ifs");
            // check clauses
            CLAUSE_LIST clList = query.getClauseList();
            verifySuchThatArgs(
                clList[0], QueryArgType::DECLARATION, QueryArgType::DECLARATION,
                (std::string) "ifs", (std::string) "w", 0, 0, Relationship::PARENT);
        }

        TEST_METHOD(Test_ParseQuery_Valid_MultipleSelectionMultipleClause) {
            std::string queryString = "if ifs; while w; variable v1, v2; procedure p1;call c;"
                "Select     < ifs,w, v1.varName, v2,p1.procName, "
                    "c.stmt#, c.procName> pattern ifs(v1,_,_) "
                    "such that Next     (ifs, w) with v1.varName=\"no\" and v2.varName=\"yes\" "
                    "such that Uses(c,\"v2\") and Calls*   (p1,\"proc\")";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            DECLARATION_TABLE declrTable = query.getDeclarations();
            // check declaration table
            Assert::AreEqual(6, static_cast<int>(declrTable.size()));  // declrTable has size of 2
            Assert::IsTrue(declrTable.find("ifs")->second == DesignEntity::IF);
            Assert::IsTrue(declrTable.find("w")->second == DesignEntity::WHILE);
            Assert::IsTrue(declrTable.find("v1")->second == DesignEntity::VARIABLE);
            Assert::IsTrue(declrTable.find("v2")->second == DesignEntity::VARIABLE);
            Assert::IsTrue(declrTable.find("p1")->second == DesignEntity::PROCEDURE);
            Assert::IsTrue(declrTable.find("c")->second == DesignEntity::CALL);
            // check selections
            SELECTION_LIST selectionList = query.getSelectionList();
            Assert::AreEqual(7, static_cast<int>(selectionList.size()));
            std::string expectedSyn;
            Attribute expectedAttr = Attribute::STMT_NUM;
            expectedSyn = "ifs";
            checkDeclaration(expectedSyn, expectedAttr, selectionList[0]);
            expectedSyn = "w";
            expectedAttr = Attribute::STMT_NUM;
            checkDeclaration(expectedSyn, expectedAttr, selectionList[1]);
            expectedSyn = "v1";
            expectedAttr = Attribute::VAR_NAME;
            checkDeclaration(expectedSyn, expectedAttr, selectionList[2]);
            expectedSyn = "v2";
            expectedAttr = Attribute::VAR_NAME;
            checkDeclaration(expectedSyn, expectedAttr, selectionList[3]);
            expectedSyn = "p1";
            expectedAttr = Attribute::PROC_NAME;
            checkDeclaration(expectedSyn, expectedAttr, selectionList[4]);
            expectedSyn = "c";
            expectedAttr = Attribute::STMT_NUM;
            checkDeclaration(expectedSyn, expectedAttr, selectionList[5]);
            expectedSyn = "c";
            expectedAttr = Attribute::PROC_NAME;
            checkDeclaration(expectedSyn, expectedAttr, selectionList[6]);
            // check clauses
            std::string strL;
            std::string strR;
            std::string designEnt;
            QueryArgType leftType;
            QueryArgType rightType;
            CLAUSE_LIST clList = query.getClauseList();
            Assert::AreEqual(4, static_cast<int>(query.getClauseList().size()));
            // cl 1 - such that Next     (ifs, w)
            leftType = QueryArgType::DECLARATION;
            rightType = QueryArgType::DECLARATION;
            strL = "ifs";
            strR = "w";
            Relationship rs = Relationship::NEXT;
            verifySuchThatArgs(clList[0], leftType, rightType, strL, strR, 0, 0, rs);
            // cl 2 - such that Uses(c, "v2")
            leftType = QueryArgType::DECLARATION;
            rightType = QueryArgType::NAME;
            rs = Relationship::USES;
            strL = "c";
            strR = "v2";
            verifySuchThatArgs(clList[1], leftType, rightType, strL, strR, 0, 0, rs);
            // with clause is not part of clause list
            // cl 3 - pattern if(v1,_,_)"
            leftType = QueryArgType::DECLARATION;
            rightType = QueryArgType::WILDCARD;
            strL = "v1";
            strR = "";
            designEnt = "ifs";
            verifyPatternArgs(
                clList[3], leftType, rightType, strL, strR, declrTable, designEnt);
            // cl 3 - Calls*   (p1,\"proc\")
            leftType = QueryArgType::DECLARATION;
            rightType = QueryArgType::NAME;
            rs = Relationship::CALLS_STAR;
            strL = "p1";
            strR = "proc";
            verifySuchThatArgs(clList[2], leftType, rightType, strL, strR, 0, 0, rs);

            CLAUSE_LIST withConditionList = query.getWithConditionList();
            verifyWithArgs(
                withConditionList[0], QueryArgType::DECLARATION, QueryArgType::NAME,
                (std::string)"v1", Attribute::VAR_NAME, (std::string)"no", Attribute::NONE, 0, 0);
            verifyWithArgs(
                withConditionList[1], QueryArgType::DECLARATION, QueryArgType::NAME,
                (std::string)"v2", Attribute::VAR_NAME, (std::string)"yes", Attribute::NONE, 0, 0);
        }

        TEST_METHOD(Test_ParseSelection_Valid_TuplePureSynonym) {
            DECLARATION_TABLE declrTable = {
                {"p", DesignEntity::PROCEDURE},
                {"call", DesignEntity::CALL},
                {"c", DesignEntity::CONSTANT},
                {"stmt", DesignEntity::STMT},
                {"a1", DesignEntity::STMT},
                {"a2", DesignEntity::STMT},
                {"a3", DesignEntity::STMT},
                {"a4", DesignEntity::STMT},
                {"a5", DesignEntity::STMT},
            };
            QueryParser parser = QueryParser();
            // test 1 - valid query with 2 selections, Follows, irregular space
            std::string selectStmt = "Select < call, stmt>  such that Follows (1, 2)";
            auto res = parser.parseSelectStatement(declrTable, selectStmt);
            SelectionType selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::TUPLE);
            SELECTION_LIST selectionList = std::get<0>(res.second);
            // 2 selections
            Assert::AreEqual(2, static_cast<int>(selectionList.size()));
            // selections have no attributes
            checkDeclaration((std::string) "call", Attribute::STMT_NUM, selectionList[0]);
            checkDeclaration((std::string) "stmt", Attribute::STMT_NUM, selectionList[1]);
            // unparsed clauses are correct
            std::string unparsedClauses = std::get<1>(res.second);
            Assert::AreEqual((std::string) "such that Follows (1, 2)", unparsedClauses);

            // test 2 - valid query with 5 selections, Calls, no space
            selectStmt = "Select <a1,a2,a3,a4,a5> such that Follows (1, 2)";
            res = parser.parseSelectStatement(declrTable, selectStmt);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::TUPLE);
            selectionList = std::get<0>(res.second);
            // 5 selections
            Assert::AreEqual(5, static_cast<int>(selectionList.size()));
            // selections have no attributes, check declarations
            checkDeclaration((std::string) "a1", Attribute::STMT_NUM, selectionList[0]);
            checkDeclaration((std::string) "a2", Attribute::STMT_NUM, selectionList[1]);
            checkDeclaration((std::string) "a3", Attribute::STMT_NUM, selectionList[2]);
            checkDeclaration((std::string) "a4", Attribute::STMT_NUM, selectionList[3]);
            checkDeclaration((std::string) "a5", Attribute::STMT_NUM, selectionList[4]);
            // unparsed clauses are correct
            unparsedClauses = std::get<1>(res.second);
            Assert::AreEqual((std::string) "such that Follows (1, 2)", unparsedClauses);

            // test 3 - valid query with 1 selections, Next*, tabs
            selectStmt = "Select < a1   > such that Follows (1,2)";
            res = parser.parseSelectStatement(declrTable, selectStmt);
            selectionList = std::get<0>(res.second);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::SINGLE);
            // 1 selection
            Assert::AreEqual(1, static_cast<int>(selectionList.size()));
            // selections have no attributes, check declarations
            checkDeclaration((std::string) "a1", Attribute::STMT_NUM, selectionList[0]);
            // unparsed clauses are correct
            unparsedClauses = std::get<1>(res.second);
            Assert::AreEqual((std::string) "such that Follows (1,2)", unparsedClauses);
        }

        TEST_METHOD(Test_ParseSelect_SemanticallyInvalid_Boolean) {
            // incorrect with ref types (integer and string)
            std::string queryString = "stmt s; Select BOOLEAN with s = \"test\"";
            QueryParser parser = QueryParser();
            auto err = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<BooleanException>(err);
        }

        TEST_METHOD(Test_ParseSelection_Valid_SingleWithAttribute) {
          DECLARATION_TABLE declrTable = {{"s", DesignEntity::STMT}, {"v", DesignEntity::VARIABLE}};
            std::string queryString = "Select v.varName such that Follows (s, 1)";
            QueryParser parser = QueryParser();
            auto res =
                parser.parseSelectStatement(declrTable, queryString);
            SelectionType selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::SINGLE);
            SELECTION_LIST selections = std::get<0>(res.second);
            std::string clauses = std::get<1>(res.second);
            Assert::AreEqual(1, static_cast<int>(selections.size()));
            Assert::IsTrue(selections[0].getAttribute() == Attribute::VAR_NAME);
            Assert::AreEqual((std::string) "v", selections[0].getSynonym());
        }

        TEST_METHOD(Test_ParseSelection_Invalid_Tuple) {
            DECLARATION_TABLE declrTable = {
                {"s", DesignEntity::STMT}, {"v", DesignEntity::VARIABLE}
            };
            QueryParser parser = QueryParser();
            // ----- SYNTAX ERRORS -----
            // test 1 - tuple has no closing <>
            std::string queryString = "Select <s, v such that Follows (1, 2)";
            auto err1 = [&] {parser.parseSelectStatement(declrTable, queryString);};
            Assert::ExpectException<InvalidSelectionSyntaxException>(err1);

            // test 2 - multiple selection but no <>
            // expected behaviour: assumes s is the selection
            // error will be detected within the clauses
            queryString = "Select s, v.varName such that Follows (1, 2)";
            auto err2 = [&] {parser.parseSelectStatement(declrTable, queryString);};
            Assert::ExpectException<InvalidSelectionSyntaxException>(err2);

            // test 3 - multiple selection but no comma
            queryString = "Select <s v.varName> such that Follows (1, 2)";
            auto err3 = [&] {parser.parseSelectStatement(declrTable, queryString);};
            Assert::ExpectException<InvalidSelectionSyntaxException>(err3);

            // test 4 - wrong attrRef
        }

        TEST_METHOD(Test_ParseSelection_Valid_BOOLEAN) {
            std::string str;
            SELECTION_LIST selectionList;
            SELECTION_LIST expectedSList;
            std::string unparsedCl;
            std::string expectedCl;
            QueryParser parser = QueryParser();
            DECLARATION_TABLE dtBoolSyn =
            {{"BOOLEAN", DesignEntity::VARIABLE},
              {"stmt", DesignEntity::STMT}};
            DECLARATION_TABLE dtGeneral =
                {{"variable", DesignEntity::VARIABLE}, {"s", DesignEntity::STMT}
            };

            // test 1: select BOOLEAN with a such that clause
            str = "Select BOOLEAN such that Follows(1,2)";
            auto res = parser.parseSelectStatement(dtGeneral, str);
            SelectionType selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::BOOLEAN);
            selectionList = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            Assert::AreEqual(0, static_cast<int>(selectionList.size()));
            Assert::AreEqual((std::string) "such that Follows(1,2)", unparsedCl);

            // test 2: (edge case) BOOLEAN is a declared synonym
            str = "Select BOOLEAN such that Follows(1,2)";
            res = parser.parseSelectStatement(dtBoolSyn, str);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::SINGLE);
            selectionList = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            Assert::AreEqual(1, static_cast<int>(selectionList.size()));
            Assert::AreEqual((std::string) "BOOLEAN", selectionList[0].getSynonym());
            Assert::AreEqual((std::string) "such that Follows(1,2)", unparsedCl);
        }

        TEST_METHOD(Test_ParseSelection_Valid_Tuple) {
            std::string str;
            SELECTION_LIST selectionList;
            std::string unparsedCl;
            std::string expectedCl;
            std::vector<std::string> expectedSyns;
            std::vector<Attribute> expectedAttrs;
            QueryParser parser = QueryParser();
            DECLARATION_TABLE dtBoolSyn =
                {{"BOOLEAN", DesignEntity::VARIABLE},
                {"stmt", DesignEntity::STMT}};
            DECLARATION_TABLE dtGeneral =
                {{"s", DesignEntity::STMT}, {"a123", DesignEntity::ASSIGN},
                {"vVaR", DesignEntity::VARIABLE}, {"c", DesignEntity::CONSTANT},
                {"var", DesignEntity::VARIABLE}, {"while", DesignEntity::WHILE}};

            // test 1: 3 selections, all with attrRef
            str = "Select < s.stmt#, a123.stmt#, vVaR.varName > with a123.stmt# = 1";
            auto res = parser.parseSelectStatement(dtGeneral, str);
            SelectionType selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::TUPLE);
            selectionList = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            Assert::AreEqual(3, static_cast<int>(selectionList.size()));
            expectedSyns = {"s", "a123", "vVaR"};
            expectedAttrs = {Attribute::STMT_NUM, Attribute::STMT_NUM, Attribute::VAR_NAME};
            for (size_t i = 0; i < selectionList.size(); i++) {
                checkDeclaration(expectedSyns[i], expectedAttrs[i], selectionList[i]);
            }
            Assert::AreEqual((std::string) "with a123.stmt# = 1", unparsedCl);

            // test 2: 6 selections, mix attr ref and tuple
            str = "Select <     s.stmt#, a123.stmt#, vVaR,c,var.varName,    "
                "while   > pattern if (_, \"x + 1\") = 1";
            res = parser.parseSelectStatement(dtGeneral, str);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::TUPLE);
            selectionList = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            Assert::AreEqual(6, static_cast<int>(selectionList.size()));
            expectedSyns = {"s", "a123", "vVaR", "c", "var", "while"};
            expectedAttrs =
                {Attribute::STMT_NUM, Attribute::STMT_NUM, Attribute::VAR_NAME,
                Attribute::VALUE, Attribute::VAR_NAME, Attribute::STMT_NUM};
            for (size_t i = 0; i < selectionList.size(); i++) {
                checkDeclaration(expectedSyns[i], expectedAttrs[i], selectionList[i]);
            }
            Assert::AreEqual((std::string) "pattern if (_, \"x + 1\") = 1", unparsedCl);

            // test 2: only one synonym
            str = "Select < s.stmt#  > with a123.stmt# = 1";
            res = parser.parseSelectStatement(dtGeneral, str);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::SINGLE);
            selectionList = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            Assert::AreEqual(1, static_cast<int>(selectionList.size()));
            expectedSyns = {"s"};
            expectedAttrs = {Attribute::STMT_NUM};
            for (size_t i = 0; i < selectionList.size(); i++) {
                checkDeclaration(expectedSyns[i], expectedAttrs[i], selectionList[i]);
            }
            Assert::AreEqual((std::string) "with a123.stmt# = 1", unparsedCl);
        }

        TEST_METHOD(Test_ParseWith_Valid) {
            DECLARATION_TABLE declrTable = {
                {"p", DesignEntity::PROG_LINE},
                {"c", DesignEntity::CONSTANT},
                {"stmt", DesignEntity::STMT},
                {"var", DesignEntity::VARIABLE},
                {"proc", DesignEntity::PROCEDURE}
            };
            // test 1 - with stmt with prog_line synonym
            std::string withStmt = "p=1";
            QueryParser parser = QueryParser();
            WithClause wcl = parser.parseWith(declrTable, withStmt);
            Assert::IsTrue(wcl.getClauseType() == ClauseType::WITH_CL);
            QueryArg argL = wcl.getLeftArg();
            QueryArg argR = wcl.getRightArg();
            // check argument types
            Assert::IsTrue(argL.getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(argR.getArgType() == QueryArgType::NUM);
        }

        TEST_METHOD(Test_ParseQuery_MultipleSuchThat_NoAnd) {
            std::string queryString = "stmt s;Select s such that Parent*(1,2) "
                "such that Follows(1,2)    such that Next(3,5)";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            CLAUSE_LIST clList = query.getClauseList();
            DECLARATION_TABLE declrTable = query.getDeclarations();
            // verify declarations
            Assert::IsTrue(declrTable.at("s") == DesignEntity::STMT);
            // verify clauses
            Assert::AreEqual(3, static_cast<int>(clList.size()));
            // verify each clause type and arguments
            verifySuchThatArgs(clList[0], QueryArgType::NUM,
                QueryArgType::NUM, "", "", 1, 2, Relationship::PARENT_STAR);
            verifySuchThatArgs(clList[1], QueryArgType::NUM,
                QueryArgType::NUM, "", "", 1, 2, Relationship::FOLLOWS);
            verifySuchThatArgs(clList[2], QueryArgType::NUM,
                QueryArgType::NUM, "", "", 3, 5, Relationship::NEXT);
        }

        // ITER 3 - TESTS
        TEST_METHOD(Test_ParseQuery_NoDeclarations) {
            QueryParser parser = QueryParser();
            // test 1 - passes
            std::string queryString = "Select BOOLEAN such that Follows (1,2)";
            Query query = parser.parseQuery(queryString);
            // check declarations
            DECLARATION_TABLE declrTable = query.getDeclarations();
            Assert::AreEqual(0, static_cast<int>(declrTable.size()));
            // check clauses
            CLAUSE_LIST clList = query.getClauseList();
            Assert::AreEqual(1, static_cast<int>(clList.size()));
            verifySuchThatArgs(clList[0], QueryArgType::NUM,
                                     QueryArgType::NUM, "", "", 1, 2, Relationship::FOLLOWS);

            // test 2 - does not pass
            queryString = "Select BOOLEAN such that Follows (s, 2)";
            auto err2 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<BooleanException>(err2);

            // test 3 - does not pass
            queryString = "Select s such that Follows (s, 2)";
            auto err3 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<MissingDeclarationException>(err3);
        }

        TEST_METHOD(Test_ParseQuery_ExtensionRelationships_GeneralTest) {
            std::string queryString = "stmt s; assign a; prog_line p; Select s "
                "such that NextBip*(s, 1) such that NextBip(a,s)"
                " and Affects*(p,2) and AffectsBip(a,p)";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            CLAUSE_LIST clList = query.getClauseList();
            SELECTION_LIST sList = query.getSelectionList();
            Assert::AreEqual(1, static_cast<int>(sList.size()));
            Assert::AreEqual(4, static_cast<int>(clList.size()));
            verifySuchThatArgs(clList[0], QueryArgType::DECLARATION,
                                    QueryArgType::NUM, (std::string) "s", "",
                                    0, 1, Relationship::NEXT_BIP_STAR);
            verifySuchThatArgs(clList[1], QueryArgType::DECLARATION,
                                    QueryArgType::DECLARATION, (std::string) "a", (std::string) "s",
                                    0, 0, Relationship::NEXT_BIP);
            verifySuchThatArgs(clList[2], QueryArgType::DECLARATION,
                                    QueryArgType::NUM, (std::string) "p", "",
                                    0, 2, Relationship::AFFECTS_STAR);
            verifySuchThatArgs(clList[3], QueryArgType::DECLARATION,
                                    QueryArgType::DECLARATION, (std::string) "a", (std::string) "p",
                                    0, 0, Relationship::AFFECTS_BIP);
        }

        TEST_METHOD(Test_ParseQuery_WithClause) {
            std::string queryString = "procedure p; read r; print i; Select <p.procName, r.stmt#, "
                                     "i.varName> with r.varName = \"x\"";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);
            CLAUSE_LIST clList = query.getWithConditionList();
            Assert::IsTrue(clList[0].getLeftArg().getArgType() == QueryArgType::DECLARATION);
            Assert::IsTrue(
                clList[0].getLeftArg().getDeclaration().getAttribute() == Attribute::VAR_NAME);
            Assert::IsTrue(
                clList[0].getLeftArg().getDeclaration().getSynonym() ==
                (std::string) "r");
            Assert::IsTrue(clList[0].getRightArg().getArgType() == QueryArgType::NAME);
            Assert::IsTrue(clList[0].getRightArg().getName() == (std::string) "x");
        }

        TEST_METHOD(Test_ParseQuery_InvalidWith) {
            std::string queryString = "procedure p; Select p with \"y\" = \"x\"";
            QueryParser parser = QueryParser();

            // test 1
            // does not pass due to invalid comparison
            auto err1 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<InvalidWithComparisonException>(err1);

            // test 2
            // does not pass due to invalid comparison
            queryString = "read r; Select r such that Follows(1,r) with 1 = 2";
            auto err2 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<InvalidWithComparisonException>(err2);
        }

        TEST_METHOD(Test_ParseQuery_EmptyClause) {
            std::string queryString = "procedure p; Select p such that";
            QueryParser parser = QueryParser();

            // test 1
            auto err1 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<InvalidClauseException>(err1);

            // test 2
            queryString = "stmt s; Select s with";
            auto err2 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<InvalidClauseException>(err2);

            // test 3
            queryString = "stmt s; Select s pattern";
            auto err3 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<InvalidClauseException>(err3);

            // test 4
            queryString = "while w; Select w with pattern";
            auto err4 = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<InvalidWithSyntaxException>(err4);
        }

        TEST_METHOD(Test_Parser_Killer) {
            QueryParser parser = QueryParser();

            // test 1
            std::string queryString = "stmt s, stmt;variable v, varName;"
                "constant value;   assign a; procedure procName  ; Select a with pattern a(_,_)";
            auto err = [&] {parser.parseQuery(queryString);};
            Assert::ExpectException<InvalidClauseException>(err);

            // test 2
            queryString = "stmt s, stmt;variable v, varName;constant value;   assign a; "
                "procedure procName  ; Select varName with  varName.varName  =  \" varName  \"";
            Query query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getDeclarations().size() == 7);
            Assert::IsTrue(query.getClauseList().size() == 0);
            Assert::IsTrue(query.getSelectionList().size() == 1);
            Assert::IsTrue(query.getWithConditionList().size() == 1);
            checkDeclaration(
                (std::string) "varName", Attribute::VAR_NAME, query.getSelectionList()[0]);
            QueryClause withCl = query.getWithConditionList()[0];
            QueryArg leftArg = withCl.getLeftArg();
            QueryArg rightArg = withCl.getRightArg();
            Assert::IsTrue(leftArg.getArgType() == QueryArgType::DECLARATION);
            checkDeclaration(
                (std::string) "varName", Attribute::VAR_NAME, leftArg.getDeclaration());
            Assert::IsTrue(rightArg.getArgType() == QueryArgType::NAME);
            Assert::AreEqual(rightArg.getName(), (std::string)"varName");

            // test 3
            queryString = "stmt s, stmt;variable v, varName;constant value;   "
                "assign a; procedure procName  ; Select<v>";
            query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getDeclarations().size() == 7);
            Assert::IsTrue(query.getClauseList().size() == 0);
            Assert::IsTrue(query.getSelectionList().size() == 1);
            Assert::IsTrue(query.getWithConditionList().size() == 0);
            checkDeclaration((std::string) "v", Attribute::VAR_NAME, query.getSelectionList()[0]);
        }

        TEST_METHOD(Test_EdgeCase_Brackets) {
            std::string queryString = "Select BOOLEAN such that Follows(1,2)such that Follows(2,3)";
            QueryParser parser = QueryParser();
            Query query = parser.parseQuery(queryString);

            queryString = "Select BOOLEAN such that Follows ( 1,2)and Follows(2   ,   3 )";
            query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getSelectionType() == SelectionType::BOOLEAN);
            Assert::IsTrue(query.getClauseList().size() == 2);

            queryString = "Select BOOLEAN   such    that  Follows(1,2)such that Follows(2,3)";
            query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getSelectionType() == SelectionType::BOOLEAN);
            Assert::IsTrue(query.getClauseList().size() == 2);

            queryString = "print p; procedure proc; Select<proc, p>such that NextBip*(p, p)";
            query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getSelectionType() == SelectionType::TUPLE);
            Assert::IsTrue(query.getClauseList().size() == 1);

            queryString = "stmt s1, s2; prog_line n1, n2; assign a; Select<n2, s1.stmt#>such"
                " that Next*(n1, n2) with s1.stmt# = a.stmt# such that "
                "Parent*(s2, n1) pattern a(_, _\"x8\"_) such that Follows*(s1,s2)";
            query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getSelectionList().size() == 2);


            queryString = "stmt s;prog_line n1,n2;assign a;"
                "Select<s>such that Next(n1,n2)pattern a(\"x\",_)";
            query = parser.parseQuery(queryString);
            Assert::IsTrue(query.getSelectionList().size() == 1);
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

        void verifyWithArgs(QueryClause cl, QueryArgType expectedTypeL,
            QueryArgType expectedTypeR, std::string strL, Attribute attrL, std::string strR,
            Attribute attrR, int intL, int intR) {
            Assert::IsTrue(cl.getClauseType() == ClauseType::WITH_CL);
            Assert::IsTrue(cl.getLeftArg().getArgType() == expectedTypeL);
            Assert::IsTrue(cl.getRightArg().getArgType() == expectedTypeR);

            switch (expectedTypeL) {
                case QueryArgType::DECLARATION:
                    checkDeclaration(strL, attrL, cl.getLeftArg().getDeclaration());
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
                    checkDeclaration(strR, attrR, cl.getRightArg().getDeclaration());
                    break;
                case QueryArgType::NAME:
                    Assert::AreEqual(strR, cl.getRightArg().getName());
                    break;
                case QueryArgType::NUM:
                    Assert::AreEqual(intR, cl.getRightArg().getLineNum());
                    break;
            }
        }

        void checkDeclaration(std::string& syn, Attribute attr, Declaration& actual) {
            Assert::IsTrue(attr == actual.getAttribute());
            Assert::AreEqual(syn, actual.getSynonym());
        }
    };
}  // using namespace UnitTesting
