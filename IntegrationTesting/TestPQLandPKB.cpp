#include <list>

#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB/PKB.h"
#include "PQL/QueryManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PqlAndPkbIntegrationTesting {
    TEST_CLASS(TestQueryEvaluatorAndPkb) {
    public:
        TEST_METHOD(EvaluateQuery_InvalidQuery_EmptyResult) {
            pkb.propagateStmtList(2);
            pkb.sortAllDataStructure();

            // Invalid query string
            std::string queryString = "stmt s Select s";

            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

            std::list<std::string> expected = { };

            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(EvaluateQuery_NoClauses_Success) {
            pkb.propagateStmtList(3);
            pkb.sortAllDataStructure();

            std::string queryString = "stmt s; Select s";

            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

            std::list<std::string> expected = { "1", "2", "3" };

            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(EvaluateQuery_NoDeclaration_EmptyResult) {
            pkb.propagateStmtList(2);
            pkb.sortAllDataStructure();

            // No declaration in query
            std::string queryString = "Select s";

            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

            std::list<std::string> expected = { };

            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(EvaluateQuery_SuchThatClauseFollows_Success) {
            pkb.propagateStmtList(2);

            pkb.sortAllDataStructure();

            std::string queryString = "assign a; stmt s; Select s such that Follows(s, a)";

            QueryManager::parseAndEvaluateQuery(queryString, resultsList, ptr);

            std::list<std::string> expected = { };

            Assert::IsTrue(resultsList == expected);
        }
        TEST_METHOD(SelectSynonym_SingleSuchThatClause_Success) {
            /*
                procedure main {
                    x = x + 1;  // 1
                    y = y + 1;  // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);

            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeProcedure("main", 1, 3, {});

            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "y", "+ y 1");
            pkb.storeAssignStatement(3, "z", "+ z 1");

            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(2, 3);
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(2, "y");
            pkb.storeRelationshipUsesByLineNum(3, "z");
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(2, "y");
            pkb.storeRelationshipModifiesByLineNum(3, "z");

            pkb.sortAllDataStructure();

            std::string queryOne = "assign a; stmt s; Select s such that Follows(s, a)";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "1", "2" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "stmt s; Select s such that Modifies(s, \"x\")";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "1" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "stmt s; Select s such that Uses(2, \"y\")";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "1", "2", "3" };
            Assert::IsTrue(resultsThree == expectThree);

            std::string queryFour = "prog_line n1, n2; Select n2 such that Next(n1, n2)";
            std::list<std::string> resultsFour;
            QueryManager::parseAndEvaluateQuery(queryFour, resultsFour, ptr);
            std::list<std::string> expectFour = { "2", "3" };
            Assert::IsTrue(resultsFour == expectFour);
        }
        TEST_METHOD(SelectTuple_SingleSuchThatClause_Success) {
            /*
                procedure main {
                    x = x + 1;  // 1
                    y = y + 1;  // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);

            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeProcedure("main", 1, 3, {});

            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "y", "+ y 1");
            pkb.storeAssignStatement(3, "z", "+ z 1");

            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(2, 3);
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(2, "y");
            pkb.storeRelationshipUsesByLineNum(3, "z");
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(2, "y");
            pkb.storeRelationshipModifiesByLineNum(3, "z");

            pkb.sortAllDataStructure();

            std::string queryOne = "assign a; stmt s; Select <s, a> such that Follows(s, a)";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "1 2", "2 3" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "stmt s; variable v; Select <s, v> such that Modifies(1, \"x\")";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "1 x", "1 y", "1 z", "2 x", "2 y", "2 z",
                                                 "3 x", "3 y", "3 z" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "stmt s1, s2; Select <s1, s2> such that Uses(3, \"y\")";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectBoolean_SingleSuchThatClause_Success) {
            /*
                procedure main {
                    x = x + 1;  // 1
                    y = y + 1;  // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);

            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeProcedure("main", 1, 3, {});

            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "y", "+ y 1");
            pkb.storeAssignStatement(3, "z", "+ z 1");

            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(2, 3);
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(2, "y");
            pkb.storeRelationshipUsesByLineNum(3, "z");
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(2, "y");
            pkb.storeRelationshipModifiesByLineNum(3, "z");

            pkb.sortAllDataStructure();

            std::string queryOne = "assign a; stmt s; Select BOOLEAN such that Follows(s, a)";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "TRUE" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "stmt s; Select BOOLEAN such that Modifies(1, \"x\")";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "TRUE" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "stmt s; Select BOOLEAN such that Uses(3, \"y\")";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "FALSE" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectSynonym_SinglePatternClause_Success) {
            /*
                procedure main {
                    while (x1 > y1) {  // 1
                        x2 = x2 + 1;   // 2
                    }
                    if (x3 > y3) {     // 3
                        x4 = x4 + 1;   // 4
                    } else {
                        x5 = x5 + 1;   // 5
                    }
                }
            */
            pkb.propagateStmtList(5);

            pkb.storeVariable("x1");
            pkb.storeVariable("y1");
            pkb.storeVariable("x2");
            pkb.storeVariable("x3");
            pkb.storeVariable("y3");
            pkb.storeVariable("x4");
            pkb.storeVariable("x5");
            pkb.storeConstant("1");
            pkb.storeProcedure("main", 1, 5, {});

            pkb.storeAssignStatement(2, "x2", "+ x2 1");
            pkb.storeAssignStatement(4, "x4", "+ x4 1");
            pkb.storeAssignStatement(5, "x5", "+ x5 1");
            pkb.storeIfElseStatement(3, { "x3", "y3" });
            pkb.storeWhileStatement(1, { "x1", "y1" });

            pkb.storePartialPrefixExpr(2, "x2", "x2");
            pkb.storePartialPrefixExpr(2, "x2", "1");
            pkb.storePartialPrefixExpr(4, "x4", "x4");
            pkb.storePartialPrefixExpr(4, "x4", "1");
            pkb.storePartialPrefixExpr(5, "x5", "x5");
            pkb.storePartialPrefixExpr(5, "x5", "1");

            pkb.sortAllDataStructure();

            std::string queryOne = "assign a; Select a pattern a(_, _\"1\"_)";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "2", "4", "5" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "if ifs; Select ifs pattern ifs(\"y3\", _, _)";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "3" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "while w; variable v; Select v pattern w(v, _)";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "x1", "y1" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectTuple_SinglePatternClause_Success) {
            /*
                procedure main {
                    while (x1 > y1) {  // 1
                        x2 = x2 + 1;   // 2
                    }
                    if (x3 > y3) {     // 3
                        x4 = x4 + 1;   // 4
                    } else {
                        x5 = x5 + 1;   // 5
                    }
                    while (x6 > y6) {  // 6
                        x7 = x7 + 1;   // 7
                    }
                }
            */
            pkb.propagateStmtList(5);

            pkb.storeVariable("x1");
            pkb.storeVariable("y1");
            pkb.storeVariable("x2");
            pkb.storeVariable("x3");
            pkb.storeVariable("y3");
            pkb.storeVariable("x4");
            pkb.storeVariable("x5");
            pkb.storeVariable("x6");
            pkb.storeVariable("y6");
            pkb.storeVariable("x7");
            pkb.storeConstant("1");
            pkb.storeProcedure("main", 1, 7, {});

            pkb.storeAssignStatement(2, "x2", "+ x2 1");
            pkb.storeAssignStatement(4, "x4", "+ x4 1");
            pkb.storeAssignStatement(5, "x5", "+ x5 1");
            pkb.storeAssignStatement(7, "x7", "+ x7 1");
            pkb.storeIfElseStatement(3, { "x3", "y3" });
            pkb.storeWhileStatement(1, { "x1", "y1" });
            pkb.storeWhileStatement(6, { "x6", "y6" });

            pkb.storePartialPrefixExpr(2, "x2", "x2");
            pkb.storePartialPrefixExpr(2, "x2", "1");
            pkb.storePartialPrefixExpr(4, "x4", "x4");
            pkb.storePartialPrefixExpr(4, "x4", "1");
            pkb.storePartialPrefixExpr(5, "x5", "x5");
            pkb.storePartialPrefixExpr(5, "x5", "1");
            pkb.storePartialPrefixExpr(7, "x7", "x7");
            pkb.storePartialPrefixExpr(7, "x7", "1");

            pkb.sortAllDataStructure();

            std::string queryOne = "assign a; constant c; Select <a, c> pattern a(_, _\"1\"_)";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "2 1", "4 1", "5 1", "7 1" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "if ifs; procedure p; Select <p, ifs> pattern ifs(\"y3\", _, _)";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "main 3" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "while w; variable v; Select <w, v> pattern w(v, _)";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "1 x1", "1 y1", "6 x6", "6 y6" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectBoolean_SinglePatternClause_Success) {
            /*
                procedure main {
                    while (x1 > y1) {  // 1
                        x2 = x2 + 1;   // 2
                    }
                }
            */
            pkb.propagateStmtList(2);

            pkb.storeVariable("x1");
            pkb.storeVariable("y1");
            pkb.storeVariable("x2");
            pkb.storeConstant("1");
            pkb.storeProcedure("main", 1, 2, {});

            pkb.storeAssignStatement(2, "x2", "+ x2 1");
            pkb.storeWhileStatement(1, { "x1", "y1" });

            pkb.storePartialPrefixExpr(2, "x2", "x2");
            pkb.storePartialPrefixExpr(2, "x2", "1");

            pkb.sortAllDataStructure();

            std::string queryOne = "assign a; Select BOOLEAN pattern a(_, \"1\")";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "FALSE" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "if ifs; Select BOOLEAN pattern ifs(_, _, _)";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "FALSE" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "while w; Select BOOLEAN pattern w(\"y1\", _)";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "TRUE" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectSynonym_MultipleSuchThatClause_Success) {
            /*
                procedure main {
                    x = x + 1;  // 1
                    y = y + 1;  // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);

            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeProcedure("main", 1, 3, {});

            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeAssignStatement(2, "y", "+ y 1");
            pkb.storeAssignStatement(3, "z", "+ z 1");

            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipNext(1, 2);
            pkb.storeRelationshipNext(2, 3);
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.storeRelationshipUsesByLineNum(2, "y");
            pkb.storeRelationshipUsesByLineNum(3, "z");
            pkb.storeRelationshipModifiesByLineNum(1, "x");
            pkb.storeRelationshipModifiesByLineNum(2, "y");
            pkb.storeRelationshipModifiesByLineNum(3, "z");

            pkb.sortAllDataStructure();

            std::string queryOne = "stmt s; assign a; Select s such that Follows(s, 2) "
                                   "such that Modifies(a, \"x\")";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "1" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "assign a; variable v; Select a such that Follows(1, 2) "
                                   "such that Follows(2, 3) such that Uses(a, v) "
                                   "such that Modifies(a, v)";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "3", "1", "2" };  // Different sorting
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "assign a; variable v; Select a such that Follows(1, 3) "
                                     "such that Uses(a, v) such that Modifies(a, v)";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { };
            Assert::IsTrue(resultsThree == expectThree);

            std::string queryFour = "prog_line n1, n2, n3; variable v; Select v "
                                    "such that Next(n1, 2) such that Uses(n3, v) "
                                    "such that Next(n2, 3) such that Follows(n1, n3)";
            std::list<std::string> resultsFour;
            QueryManager::parseAndEvaluateQuery(queryFour, resultsFour, ptr);
            std::list<std::string> expectFour = { "y" };
            Assert::IsTrue(resultsFour == expectFour);
        }
        TEST_METHOD(SelectSynonym_SingleWithClause_Success) {
            /*
                procedure x {
                    x = x + 1;  // 1
                    read x;  // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);

            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeProcedure("x", 1, 3, {});

            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeReadStatement(2, "x");
            pkb.storeAssignStatement(3, "z", "+ z 1");

            pkb.sortAllDataStructure();

            std::string queryOne = "stmt s; Select s with s.stmt# = 3";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "3" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "stmt s; assign a; Select s with s.stmt# = a.stmt#";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "1", "3" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "prog_line n; assign a; Select n with n = a.stmt#";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "1", "3" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectBoolean_EdgeCase_Success) {
            std::string queryOne = "Select BOOLEAN";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "TRUE" };
            Assert::IsTrue(resultsOne == expectOne);
        }
        TEST_METHOD(SelectBoolean_SingleWithClause_Success) {
            /*
                procedure x {
                    x = x + 1;  // 1
                    read x;  // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);

            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeProcedure("x", 1, 3, {});

            pkb.storeAssignStatement(1, "x", "+ x 1");
            pkb.storeReadStatement(2, "x");
            pkb.storeAssignStatement(3, "z", "+ z 1");

            pkb.sortAllDataStructure();

            std::string queryOne = "read r; Select BOOLEAN with r.varName = \"y\"";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "FALSE" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "stmt s; assign a; Select BOOLEAN with s.stmt# = a.stmt#";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "TRUE" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "stmt s; Select BOOLEAN with 2 = 3";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "FALSE" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectAttribute_NoClause_Success) {
            pkb.propagateStmtList(3);
            pkb.storeVariable("x");
            pkb.storeVariable("test");
            pkb.storeVariable("a");
            pkb.sortAllDataStructure();

            std::string queryOne = "stmt s; Select s.stmt#";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "1", "2", "3" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "variable v; Select v.varName";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "a", "test", "x" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "stmt s; variable v; Select <s.stmt#, v.varName>";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "1 a", "1 test", "1 x", "2 a", "2 test", "2 x",
                                                   "3 a", "3 test", "3 x" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectAttribute_OneClause_Success) {
            /*
                procedure simple {
                    read x;     // 1
                    print y;    // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeReadStatement(1, "x");
            pkb.storePrintStatement(2, "y");
            pkb.storeAssignStatement(3, "z", "+ z 1");
            pkb.storeProcedure("simple", 1, 3, {});

            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollowsStar(1, 3);
            pkb.sortAllDataStructure();

            std::string queryOne = "read r; stmt s; Select s.stmt# such that Follows*(r, s)";
            std::list<std::string> resultsOne;
            QueryManager::parseAndEvaluateQuery(queryOne, resultsOne, ptr);
            std::list<std::string> expectOne = { "2", "3" };
            Assert::IsTrue(resultsOne == expectOne);

            std::string queryTwo = "variable v; assign a; Select <a.stmt#, v.varName> "
                                   "pattern a(v, _)";
            std::list<std::string> resultsTwo;
            QueryManager::parseAndEvaluateQuery(queryTwo, resultsTwo, ptr);
            std::list<std::string> expectTwo = { "3 z" };
            Assert::IsTrue(resultsTwo == expectTwo);

            std::string queryThree = "procedure p; read r; print i; Select <p.procName, r.stmt#, "
                                     "i.varName> with r.varName = \"x\"";
            std::list<std::string> resultsThree;
            QueryManager::parseAndEvaluateQuery(queryThree, resultsThree, ptr);
            std::list<std::string> expectThree = { "simple 1 y" };
            Assert::IsTrue(resultsThree == expectThree);
        }
        TEST_METHOD(SelectAttribute_CompoundClause_Success) {
            /*
                procedure simple {
                    read x;     // 1
                    print y;    // 2
                    z = z + 1;  // 3
                }
            */
            pkb.propagateStmtList(3);
            pkb.storeVariable("x");
            pkb.storeVariable("y");
            pkb.storeVariable("z");
            pkb.storeConstant("1");
            pkb.storeReadStatement(1, "x");
            pkb.storePrintStatement(2, "y");
            pkb.storeAssignStatement(3, "z", "+ z 1");
            pkb.storeProcedure("simple", 1, 3, {});

            pkb.storeRelationshipFollows(1, 2);
            pkb.storeRelationshipFollows(2, 3);
            pkb.storeRelationshipFollowsStar(1, 3);
            pkb.sortAllDataStructure();

            std::string query = "read r; print p; stmt s; assign a; Select <r, p.varName, s> "
                                "such that Follows*(r, s) pattern a(_, _) with p.stmt# = 2";
            std::list<std::string> results;
            QueryManager::parseAndEvaluateQuery(query, results, ptr);
            std::list<std::string> expect = { "1 y 2", "1 y 3" };
            Assert::IsTrue(results == expect);
        }
    private:
        PKB pkb;
        PKB* ptr = &pkb;
        std::list<std::string> resultsList;
    };
}  // namespace PqlAndPkbIntegrationTesting
