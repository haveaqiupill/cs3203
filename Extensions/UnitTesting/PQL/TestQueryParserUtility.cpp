#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/QueryParserUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestQueryParserUtility) {
    public :
        /*
        * Ensure that the selection synonym is extracted
        * Ensure that the clauses are also captured as an empty string in index 1 of the vector returned 
        */
        TEST_METHOD(Test_ExtractSelect) {
            std::string stmt = "Select s123";
            try {
                auto res = QueryParserUtility::extractSelect(stmt);
                SelectionType selectType = res.first;
                Assert::IsTrue(selectType == SelectionType::SINGLE);
                std::vector<std::string> selections = std::get<0>(res.second);
                Assert::AreEqual(1, static_cast<int>(selections.size()));
                std::string clauses =
                    std::get<1>(res.second);
                Assert::AreEqual((std::string) "s123", selections[0]);
                Assert::AreEqual((std::string)"", clauses);

                stmt = " Select    s   ";
                res = QueryParserUtility::extractSelect(stmt);
                selectType = res.first;
                Assert::IsTrue(selectType == SelectionType::SINGLE);
                selections = std::get<0>(res.second);
                Assert::AreEqual(1, static_cast<int>(selections.size()));
                clauses = std::get<1>(res.second);
                Assert::AreEqual((std::string) "s", selections[0]);
                Assert::AreEqual((std::string)"", clauses);

                stmt = " Select s123 such that Follows(1,2)  ";
                res = QueryParserUtility::extractSelect(stmt);
                selectType = res.first;
                Assert::IsTrue(selectType == SelectionType::SINGLE);
                selections = std::get<0>(res.second);
                Assert::AreEqual(1, static_cast<int>(selections.size()));
                clauses = std::get<1>(res.second);
                Assert::AreEqual((std::string) "s123", selections[0]);
                Assert::AreEqual((std::string)"such that Follows(1,2)", clauses);
            }
            catch (std::exception e) {
                Assert::Fail();
            }
        }

        TEST_METHOD(Test_Remove_Quotations) {
            std::string str = "\"variable\"";
            std::string res = QueryParserUtility::removeQuotationMarks(str);
            Assert::AreEqual(res, (std::string) "variable");
        }

        /*
        * Test that declaration synonyms are properly captured
        */
        TEST_METHOD(Test_ExtractDeclarationSynonyms) {
            std::string declarationStmt = " assign a123 , blah  , cat, dog";
            std::vector<std::string> result = QueryParserUtility::tokenizeByComma(declarationStmt);
            std::vector<std::string> expectedResult = {"assign", "a123", "blah", "cat", "dog"};

            Assert::IsTrue(static_cast<int>(result.size()) == 5);
            Assert::IsTrue(result[0] == "assign");
            Assert::IsTrue(result[1] == "a123");
            Assert::IsTrue(result[2] == "blah");
            Assert::IsTrue(result[3] == "cat");
            Assert::IsTrue(result[4] == "dog");

            declarationStmt = "stmt stmt";
            result = QueryParserUtility::tokenizeByComma(declarationStmt);
            expectedResult = {"stmt", "stmt"};
            Assert::IsTrue(static_cast<int>(result.size()) == 2);
            Assert::IsTrue(result[0] == "stmt");
            Assert::IsTrue(result[1] == "stmt");
        }

        /*
        * Test that clauses are correctly split and classified
        */
        TEST_METHOD(Test_SplitClauses_OneSuchThat) {
            std::string clauses = "such   that Follows ( 1 , 2  ) ";
              //  "pattern a (\"x\", _\"y\"_)";

            std::unordered_map<ClauseType, std::vector<std::string>> result
                = QueryParserUtility::splitClauses(clauses);

            std::vector<std::string> suchThatRes = result.find(ClauseType::SUCHTHAT_CL)->second;
            std::vector<std::string> patternRes = result.find(ClauseType::PATTERN_CL)->second;

            Assert::IsTrue(static_cast<int>(suchThatRes.size()) == 1);

            Assert::IsTrue(static_cast<int>(patternRes.size()) == 0);

            Assert::AreEqual(suchThatRes[0], (std::string)"Follows ( 1 , 2  )");
        }

        TEST_METHOD(Test_SplitClauses_OnePattern) {
            std::string clauses = "pattern a ( 1, 2)";

            std::unordered_map<ClauseType, std::vector<std::string>> result
                = QueryParserUtility::splitClauses(clauses);

            std::vector<std::string> suchThatRes = result.find(ClauseType::SUCHTHAT_CL)->second;
            std::vector<std::string> patternRes = result.find(ClauseType::PATTERN_CL)->second;

            Assert::IsTrue(static_cast<int>(suchThatRes.size()) == 0);

            Assert::IsTrue(static_cast<int>(patternRes.size()) == 1);

            Assert::IsTrue(patternRes[0] == "a ( 1, 2)");
        }

        TEST_METHOD(Test_SplitClauses_OneSuchThatOnePattern) {
            std::string clauses = "such that Parent*(1,2) pattern a ( 1, 2)";

            std::unordered_map<ClauseType, std::vector<std::string>> result
                = QueryParserUtility::splitClauses(clauses);

            std::vector<std::string> suchThatRes = result.find(ClauseType::SUCHTHAT_CL)->second;
            std::vector<std::string> patternRes = result.find(ClauseType::PATTERN_CL)->second;

            Assert::IsTrue(static_cast<int>(suchThatRes.size()) == 1);

            Assert::IsTrue(static_cast<int>(patternRes.size()) == 1);
            Assert::AreEqual((std::string) "Parent*(1,2) ", suchThatRes[0]);
            Assert::AreEqual((std::string) "a ( 1, 2)", patternRes[0]);
        }
        TEST_METHOD(Test_SplitByDelimiter) {
            std::string str =
                " assign a, b, c ;  stmt s; variable    v ; Select s  such that Follows (1 , 2)  ";

            std::string delim = ";";
            std::vector<std::string> res = QueryParserUtility::splitByDelimiter(str, delim);

            std::vector<std::string> expectedRes = {
                " assign a, b, c ", "  stmt s", " variable    v ",
                " Select s  such that "
                "Follows (1 , 2)  "};

            Assert::IsTrue(static_cast<int>(res.size()) == 4);
            Assert::IsTrue(res[0] == " assign a, b, c ");
            Assert::IsTrue(res[1] == "  stmt s");
            Assert::IsTrue(res[2] == " variable    v ");
            Assert::IsTrue(res[3] == " Select s  such that Follows (1 , 2)  ");
        }

        TEST_METHOD(Test_TrimWhitespace) {
            std::string str1 = "    test 123";
            std::string str2 = "test 123  ";
            std::string str3 = "   test 123    ";
            std::string str4 = "       a       ";
            std::string res1 = "test 123";
            std::string res4 = "a";

            str1 = QueryParserUtility::trimWhitespace(str1);
            str2 = QueryParserUtility::trimWhitespace(str2);
            str3 = QueryParserUtility::trimWhitespace(str3);
            str4 = QueryParserUtility::trimWhitespace(str4);

            Assert::IsTrue(str1 == res1);
            Assert::IsTrue(str2 == res1);
            Assert::IsTrue(str3 == res1);
            Assert::IsTrue(str4 == res4);
        }

        TEST_METHOD(Test_TokenizeStatement) {
            std::string str = " assign  a , bbb , c        ";
            std::vector<std::string> expectedRes = {"assign", "a", ",", "bbb",    ",", "c"};
            std::vector<std::string> res = QueryParserUtility::tokenizeByWhitespace(str);

            Assert::IsTrue(static_cast<int>(res.size()) == 6);

            Assert::IsTrue(res[0] == "assign");
            Assert::IsTrue(res[1] == "a");
            Assert::IsTrue(res[2] == ",");
            Assert::IsTrue(res[3] == "bbb");
            Assert::IsTrue(res[4] == ",");
            Assert::IsTrue(res[5] == "c");
        }
        TEST_METHOD(Test_ExtractSuchThatArguments) {
            std::string str1 = "Follows ( 1 , 2  )";
            std::string str2 = "Parent(_,_)";
            std::string str3 = "Modifies(\"x\", _    )";
            std::string str4 = "Uses(s, \"x123\")";
            std::string str5 = "Modifies(s, \"    read\")";
            try {
                std::string expectedRs = "Follows";
                std::string expectedL = "1";
                std::string expectedR = "2";
                extractSuchThatArgsTestHelper(str1, expectedRs, expectedL, expectedR);
            } catch (std::exception e) {
                Assert::Fail();
            }

            try {
                std::string expectedRs = "Parent";
                std::string expectedL = "_";
                std::string expectedR = "_";
                extractSuchThatArgsTestHelper(str2, expectedRs, expectedL, expectedR);
            } catch (std::exception e) {
                Assert::Fail();
            }

            try {
                std::string expectedRs = "Modifies";
                std::string expectedL = "\"x\"";
                std::string expectedR = "_";
                extractSuchThatArgsTestHelper(str3, expectedRs, expectedL, expectedR);
            } catch (std::exception e) {
                Assert::Fail();
            }

            try {
                std::string expectedRs = "Uses";
                std::string expectedL = "s";
                std::string expectedR = "\"x123\"";
                extractSuchThatArgsTestHelper(str4, expectedRs, expectedL, expectedR);
            } catch (std::exception e) {
                Assert::Fail();
            }

            // iter1 bug
            try {
                std::string expectedRs = "Modifies";
                std::string expectedL = "s";
                std::string expectedR = "\"    read\"";
                extractSuchThatArgsTestHelper(str5, expectedRs, expectedL, expectedR);
            } catch (std::exception e) {
                Assert::Fail();
            }
        }

        /*
        * Helper method to test the result tuple
        */
        void extractSuchThatArgsTestHelper(std::string input, std::string expectedRs,
            std::string expectedL, std::string expectedR) {
            std::tuple<Relationship, std::string, std::string> res
                = QueryParserUtility::extractSuchThatArguments(input);

            // check that tuple size is 3
            Assert::IsTrue(static_cast<int>(std::tuple_size<decltype(res)>::value) == 3);

            std::string strRelationship = relationshipEnumToString(std::get<0>(res));

            Assert::IsTrue(strRelationship == expectedRs);
            Assert::IsTrue(std::get<1>(res) == expectedL);
            Assert::IsTrue(std::get<2>(res) == expectedR);
        }

        /*
        * Helper method to convert Relationship to string rep
        */
        std::string relationshipEnumToString(Relationship rs) {
            switch (rs) {
            case Relationship::FOLLOWS:
                return "Follows";
            case Relationship::FOLLOWS_STAR:
                return "Follows\\*";
            case Relationship::MODIFIES:
                return "Modifies";
            case Relationship::PARENT:
                return "Parent";
            case Relationship::PARENT_STAR:
                return "Parent\\*";
            case Relationship::USES:
                return "Uses";
            }
        }

        /*
        * Helper method to convert
        * DesignEntity to string representation
        */
        std::string designEntEnumToString(DesignEntity de) {
            switch (de) {
            case DesignEntity::ASSIGN:
                return "assign";
            case DesignEntity::CONSTANT:
              return "constant";
            case DesignEntity::IF:
              return "if";
            case DesignEntity::PRINT:
              return "print";
            case DesignEntity::PROCEDURE:
              return "procedure";
            case DesignEntity::READ:
              return "read";
            case DesignEntity::STMT:
              return "stmt";
            case DesignEntity::VARIABLE:
              return "variable";
            case DesignEntity::WHILE:
              return "while";
          }
        }

        /*
        * Helper method to convert QueryArgType enum to string rep
        */
        std::string queryArgTypeEnumToString(QueryArgType argType) {
            switch (argType) {
            case QueryArgType::DECLARATION:
                return "declaration";
            case QueryArgType::NUM:
                return "lineNum";
            case QueryArgType::NAME:
                return "name";
            case QueryArgType::PATTERN_EXPR:
                return "patternExpr";
            case QueryArgType::PATTERN_EXPR_WITH_WILDCARDS:
                return "patternExprWithWildcards";
            case QueryArgType::WILDCARD:
                return "wildcard";
            }
        }

        TEST_METHOD(Test_ExtractArgumentType) {
            // positive tests
            std::string wildcard = "_";
            std::string integer = "14";
            std::string declaration = "a12345";
            std::string name = "\"test\"";

            std::string isWildcard = queryArgTypeEnumToString(
                QueryParserUtility::extractArgumentType(wildcard));
            std::string isLineNum = queryArgTypeEnumToString(
                QueryParserUtility::extractArgumentType(integer));

            std::string isDeclaration = queryArgTypeEnumToString(
                QueryParserUtility::extractArgumentType(declaration));

            std::string isName =
                queryArgTypeEnumToString(QueryParserUtility::extractArgumentType(name));

            Assert::IsTrue(isWildcard ==  "wildcard");
            Assert::IsTrue(isLineNum ==  "lineNum");
            Assert::IsTrue(isDeclaration ==  "declaration");
            Assert::IsTrue(isName ==  "name");
        }

        TEST_METHOD(Test_ExtractPatternSynonym) {
            std::string patternStmt = "a ( \"x \", 1)";
            std::tuple<std::string, std::string> res =
                QueryParserUtility::extractPatternSynonym(patternStmt);
            std::string syn = std::get<0>(res);
            std::string unparsedArgs = std::get<1>(res);
            Assert::AreEqual((std::string) "a", syn);
            Assert::AreEqual((std::string) "\"x \", 1", unparsedArgs);
        }
        // ITER 2 TESTS
        TEST_METHOD(Test_ExtractAssignPatternArgs) {
            // test 1 -- assign-pattern with wildcards, single term, no space
            std::string patternArgs = "\"x\", _\"y\"_";
            std::tuple<std::string, std::string> res
                = QueryParserUtility::extractPatternArguments(patternArgs, DesignEntity::ASSIGN);
            Assert::AreEqual((std::string) "\"x\"", std::get<0>(res));
            Assert::AreEqual((std::string) "_\"y\"_", std::get<1>(res));

            // test 2 -- assign-pattern with wildcards, single term and whitespace
            std::string withSpace = "\" x \" , _ \" y \" _ ";

            res
                = QueryParserUtility::extractPatternArguments(withSpace, DesignEntity::ASSIGN);
            Assert::AreEqual((std::string) "\" x \"", std::get<0>(res));
            Assert::AreEqual((std::string) "_ \" y \" _", std::get<1>(res));
        }

        TEST_METHOD(Test_ExtractIfPatternArgs) {
            // test 1
            std::string patternArgs = "\"testing\", _ , _";
            std::tuple<std::string, std::string> res
                = QueryParserUtility::extractPatternArguments(patternArgs, DesignEntity::IF);
            Assert::AreEqual((std::string) "\"testing\"", std::get<0>(res));
            Assert::AreEqual((std::string) "", std::get<1>(res));

            // test 2 - all wildcard
            patternArgs = "_, _ , _";
            res = QueryParserUtility::extractPatternArguments(patternArgs, DesignEntity::IF);
            Assert::AreEqual((std::string) "_", std::get<0>(res));
            Assert::AreEqual((std::string) "", std::get<1>(res));

            // test 3 - synonym
            patternArgs = "x123    , _     , _";
            res = QueryParserUtility::extractPatternArguments(patternArgs, DesignEntity::IF);
            Assert::AreEqual((std::string) "x123", std::get<0>(res));
            Assert::AreEqual((std::string) "", std::get<1>(res));
        }

        TEST_METHOD(Test_ExtractPatternType) {
            // positive tests
            std::string wildcard = "_";
            std::string exprWithWildcard = "_\"y\"_";
            std::string expr = "\"x\"";

            bool res1 = QueryParserUtility::extractPatternTypeByWildcard(wildcard)
                == QueryArgType::WILDCARD
                ? true
                : false;

            bool res2 =
                QueryParserUtility::extractPatternTypeByWildcard(exprWithWildcard)
                == QueryArgType::PATTERN_EXPR_WITH_WILDCARDS
                ? true
                : false;

            bool res3 = QueryParserUtility::extractPatternTypeByWildcard(expr)
                == QueryArgType::PATTERN_EXPR
                ? true
                : false;

            Assert::IsTrue(res1);
            Assert::IsTrue(res2);
            Assert::IsTrue(res3);
        }

        TEST_METHOD(Test_SplitClauses_SuchThatPatternWith) {
            std::string clauses = "such that Parent*(1,2) pattern a ( 1, 2) with s.stmt# = 2";

            std::unordered_map<ClauseType, std::vector<std::string>> result
                = QueryParserUtility::splitClauses(clauses);

            std::vector<std::string> suchThatRes = result.find(ClauseType::SUCHTHAT_CL)->second;
            std::vector<std::string> patternRes = result.find(ClauseType::PATTERN_CL)->second;
            std::vector<std::string> withRes = result.find(ClauseType::WITH_CL)->second;

            Assert::IsTrue(static_cast<int>(suchThatRes.size()) == 1);

            Assert::IsTrue(static_cast<int>(patternRes.size()) == 1);
            Assert::AreEqual((std::string) "Parent*(1,2) ", suchThatRes[0]);
            Assert::AreEqual((std::string) "a ( 1, 2) ", patternRes[0]);

            Assert::AreEqual(1, static_cast<int>(withRes.size()));
            Assert::AreEqual(withRes[0], (std::string) "s.stmt# = 2");
        }

         TEST_METHOD(Test_SplitClauses_SuchThatPatternWith_Jumbled) {
            std::string clauses = "pattern a ( 1, 2) with s.stmt# = 2 such that Parent*(1,2) ";

            std::unordered_map<ClauseType, std::vector<std::string>> result
                = QueryParserUtility::splitClauses(clauses);

            std::vector<std::string> suchThatRes = result.find(ClauseType::SUCHTHAT_CL)->second;
            std::vector<std::string> patternRes = result.find(ClauseType::PATTERN_CL)->second;
            std::vector<std::string> withRes = result.find(ClauseType::WITH_CL)->second;

            Assert::IsTrue(static_cast<int>(suchThatRes.size()) == 1);

            Assert::IsTrue(static_cast<int>(patternRes.size()) == 1);
            Assert::AreEqual((std::string) "Parent*(1,2)", suchThatRes[0]);
            Assert::AreEqual((std::string) "a ( 1, 2) ", patternRes[0]);

            Assert::AreEqual(1, static_cast<int>(withRes.size()));
            Assert::AreEqual((std::string) "s.stmt# = 2 ", withRes[0]);
        }

        TEST_METHOD(Test_SplitClauses_SuchThatJoinedWithAnd) {
            std::string clauses = "such that Parent*(1,2) and Follows(1,2)";

            std::unordered_map<ClauseType, std::vector<std::string>> result
                = QueryParserUtility::splitClauses(clauses);

            std::vector<std::string> suchThatRes = result.find(ClauseType::SUCHTHAT_CL)->second;
            std::vector<std::string> patternRes = result.find(ClauseType::PATTERN_CL)->second;
            std::vector<std::string> withRes = result.find(ClauseType::WITH_CL)->second;

            // such that result size of 2
            Assert::AreEqual(2, static_cast<int>(suchThatRes.size()));
            // pattern result size of 0
            Assert::AreEqual(0, static_cast<int>(patternRes.size()));
            // with result size of 0
            Assert::AreEqual(0, static_cast<int>(withRes.size()));

            // check such that arguments
            std::string st1 = suchThatRes[0];
            std::string st2 = suchThatRes[1];
            Assert::AreEqual((std::string) "Parent*(1,2)", st1);
            Assert::AreEqual((std::string) "Follows(1,2)", st2);
        }

        TEST_METHOD(Test_SplitClauses_SuchThatNotJoinedWithAnd) {
            std::string clauses = "such that Parent*(1,2) such that Follows(1,2)";

            std::unordered_map<ClauseType, std::vector<std::string>> result
                = QueryParserUtility::splitClauses(clauses);

            std::vector<std::string> suchThatRes = result.find(ClauseType::SUCHTHAT_CL)->second;
            std::vector<std::string> patternRes = result.find(ClauseType::PATTERN_CL)->second;
            std::vector<std::string> withRes = result.find(ClauseType::WITH_CL)->second;

            // such that result size of 2
            Assert::AreEqual(2, static_cast<int>(suchThatRes.size()));
            // pattern result size of 0
            Assert::AreEqual(0, static_cast<int>(patternRes.size()));
            // with result size of 0
            Assert::AreEqual(0, static_cast<int>(withRes.size()));

            // check such that arguments
            std::string st1 = suchThatRes[0];
            std::string st2 = suchThatRes[1];
            Assert::AreEqual((std::string) "Parent*(1,2) ", st1);
            Assert::AreEqual((std::string) "Follows(1,2)", st2);
        }

        /*
        * This method is called when splitting multiple selections as well as multiple declarations separated by a comma
        * Test expected behaviour
        */
        TEST_METHOD(Test_Tokenize_By_Comma) {
            // test for declarations 1: multiple declarations with tabs, no space, spaces
            std::string str = "assign a, b,c,d,e  , f,  g";
            std::vector<std::string> tokens =
              QueryParserUtility::tokenizeByComma(str);
            Assert::AreEqual(8, static_cast<int>(tokens.size()));
            std::vector<std::string> expected = {"assign", "a", "b", "c", "d", "e", "f", "g"};
            Assert::IsTrue(expected == tokens);

            // test for declarations 2: no commas
            str = "stmt stmt";
            tokens = QueryParserUtility::tokenizeByComma(str);
            Assert::AreEqual(2, static_cast<int>(tokens.size()));
            expected = {"stmt", "stmt"};
            Assert::IsTrue(expected == tokens);

            // test for selections 1: multiple selections, no space + tabs + spaces
            str = "s.varName,p.procName,procName.procName, var  ,           test.stmt#";
            tokens = QueryParserUtility::tokenizeByComma(str);
            Assert::AreEqual(5, static_cast<int>(tokens.size()));
            expected = {"s.varName", "p.procName", "procName.procName", "var", "test.stmt#"};
            Assert::IsTrue(expected == tokens);
        }

        TEST_METHOD(Test_Extract_Select_BOOLEAN) {
            std::vector<std::string> selections;
            std::vector<std::string> expected;
            std::string unparsedCl;
            std::string expectedCl;
            std::string str;

            // test 1: select BOOLEAN no clause
            str = "Select BOOLEAN";
            auto res = QueryParserUtility::extractSelect(str);
            SelectionType selectType = res.first;
            // query parser util can only differentiate between single / tuple
            Assert::IsTrue(selectType == SelectionType::SINGLE);
            selections = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            expected = {"BOOLEAN"};
            expectedCl = "";
            Assert::IsTrue(expected == selections);
            Assert::AreEqual(expectedCl, unparsedCl);

            // test : edge case where synonym declared as BOOLEAN, used in tuple
            str = "Select <BOOLEAN, var, stmt>";
            res = QueryParserUtility::extractSelect(str);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::TUPLE);
            selections = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            expected = { "BOOLEAN", "var", "stmt" };  // empty result
            expectedCl = "";
            Assert::IsTrue(expected == selections);
            Assert::AreEqual(expectedCl, unparsedCl);

            // test: edge case where synonym declared as BOOLEAN, used as attrRef
            str = "Select BOOLEAN.varName";
            res = QueryParserUtility::extractSelect(str);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::SINGLE);
            selections = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            expected = {"BOOLEAN.varName"};  // non empty result
            expectedCl = "";
            Assert::IsTrue(expected == selections);
            Assert::AreEqual(expectedCl, unparsedCl);

            // test: select BOOLEAN with a such that clause
            str = "Select BOOLEAN such that Follows(1,2)";
            res = QueryParserUtility::extractSelect(str);
            selectType = res.first;
            Assert::IsTrue(selectType == SelectionType::SINGLE);
            selections = std::get<0>(res.second);
            unparsedCl = std::get<1>(res.second);
            expected = {"BOOLEAN"};  // non empty result
            expectedCl = "such that Follows(1,2)";
            Assert::IsTrue(expected == selections);
            Assert::AreEqual(expectedCl, unparsedCl);
        }
    };
}  // namespace UnitTesting

