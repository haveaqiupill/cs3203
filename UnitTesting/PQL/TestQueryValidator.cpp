#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/QueryValidator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestQueryValidator) {
    public :
        TEST_METHOD(Test_DoInitialCheck) {
            // negative cases, all should fail
            std::string emptyQuery = "";
            std::string emptyQuery2 = "      ";
            std::string noSelect = " assign a; variable v;";
            std::string noSelect2 = "assign select";

            std::vector<std::string> tests;
            tests.push_back(emptyQuery);
            tests.push_back(emptyQuery2);
            tests.push_back(noSelect);
            tests.push_back(noSelect2);

            for (std::string str : tests) {
                auto err = [&] {QueryValidator::doInitialCheck(str);};
                Assert::ExpectException<InvalidQuerySyntaxException>(err);
            }

            std::string passingString = "assign a ; Select a pattern a(_,_)";
            std::string passingString2 = "Select BOOLEAN such that Follows (1,2)";
            std::string passingString3 = " test Select";  // this will pass the initial check
            try {
                QueryValidator::doInitialCheck(passingString);
                QueryValidator::doInitialCheck(passingString2);
                QueryValidator::doInitialCheck(passingString3);
            }
            catch (InvalidQuerySyntaxException) {
                Assert::Fail();
            }
        }

        TEST_METHOD(Test_SyntaxCheck_Declaration) {
            // test declaration syntax check
            std::string passProcedure = "  procedure   p";
            std::string passConstant = " constant c1, c2";
            std::string passVariedNames = "assign a123, b7a18, variable";

            // results from QueryParserUtility splitByDelimiter test
            std::string passFromSplit = " assign a, b, c ";
            std::string passFromSplit2 = "  stmt s";
            std::string passFromSplit3 = " variable    v ";

            // everything must pass
            Assert::IsTrue(
                QueryValidator::isSyntaxValid(passProcedure, declarationRegex));
            Assert::IsTrue(
                QueryValidator::isSyntaxValid(passConstant, declarationRegex));
            Assert::IsTrue(
                QueryValidator::isSyntaxValid(passVariedNames, declarationRegex));
            Assert::IsTrue(
                QueryValidator::isSyntaxValid(passFromSplit, declarationRegex));
            Assert::IsTrue(
                QueryValidator::isSyntaxValid(passFromSplit2, declarationRegex));
            Assert::IsTrue(
                QueryValidator::isSyntaxValid(passFromSplit3, declarationRegex));

            std::string noCommas = "assign ilove       ashleigh";
            std::string noDesignEntity = "ash ash ash ash";
            std::string designEntityNotFirst = "ashleigh assign";
            std::string oneArg = "assign  ";
            std::string invalidCharacters = "stmt s1,23";
            std::string invalidDesignEntity = "statement s";

            // expect all to fail
            Assert::IsFalse(QueryValidator::isSyntaxValid(noCommas, declarationRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(noDesignEntity, declarationRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(designEntityNotFirst, declarationRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(oneArg, declarationRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(invalidCharacters, declarationRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(invalidDesignEntity, declarationRegex));
        }

        TEST_METHOD(Test_SyntaxCheck_SelectStmt) {
            std::string standard1 = "Select lalala such that "
                "Follows ( 1, 2)  pattern a (_, _\"x\"_)";
            std::string standard2 = " Select  ashleigh123  lalalalala";
            std::string standard3 = "Select   something from variable";

            Assert::IsTrue(QueryValidator::isSyntaxValid(standard1, selectStmtRegexSingle));
            Assert::IsFalse(QueryValidator::isSyntaxValid(standard2, selectStmtRegexSingle));
            Assert::IsFalse(QueryValidator::isSyntaxValid(standard3, selectStmtRegexSingle));
        }

        TEST_METHOD(Test_SyntaxCheck_SuchThat) {
            // negative testing
            std::string noArgsWithComma = "Follows(,)";
            std::string noArgsAndComma = "Modifies()";
            std::string noArgsLeft = "Follows*( , 2)";
            std::string noArgsRight = "Uses (     p, )";
            std::string noBracket = "Uses 1, 2";
            std::string wrongSuchThat = "Such That such that Parent*(1,2)";
            std::string noCommas = "Follows (1 2)";
            std::string wrongRelationship = "follows (1,2)";

            Assert::IsFalse(QueryValidator::isSyntaxValid(noArgsWithComma, suchThatRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(noArgsAndComma, suchThatRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(noArgsLeft, suchThatRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(noArgsRight, suchThatRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(noBracket, suchThatRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(wrongSuchThat, suchThatRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(noCommas, suchThatRegex));
            Assert::IsFalse(QueryValidator::isSyntaxValid(wrongRelationship, suchThatRegex));

            // positive tests
            std::string fromSplit = "Follows ( 1 , 2  )";
            std::string noSpace = "Parent*(1,2)";
            std::string allTabs = "Parent  (   1   ,   2   )   ";
            Assert::IsTrue(QueryValidator::isSyntaxValid(fromSplit, suchThatRegex));
            Assert::IsTrue(QueryValidator::isSyntaxValid(noSpace, suchThatRegex));
            Assert::IsTrue(QueryValidator::isSyntaxValid(allTabs, suchThatRegex));
        }

        TEST_METHOD(Test_SyntaxCheck_SuchThatArgs_Invalid) {
            std::tuple<Relationship, std::string, std::string> invalidLeftArg =
                std::make_tuple(Relationship::MODIFIES, "_", "\"aaaaaa\"");
            std::tuple<Relationship, std::string, std::string> nothingBetweenQuotations =
                std::make_tuple(Relationship::MODIFIES, "a", "\"\"");
            std::tuple<Relationship, std::string, std::string> invalidCharacters =
                std::make_tuple(Relationship::MODIFIES, "a", "\"1+2\"");
            std::tuple<Relationship, std::string, std::string> invalidWildcard =
                std::make_tuple(Relationship::USES, "s", "_\"x123\"_");
            std::tuple<Relationship, std::string, std::string> startWithNumber =
                std::make_tuple(Relationship::FOLLOWS, "1s", "_");
            std::tuple<Relationship, std::string, std::string> numberButStartsWithZero1 =
                std::make_tuple(Relationship::USES, "023", "_");
            std::tuple<Relationship, std::string, std::string> numberButStartsWithZero2 =
                std::make_tuple(Relationship::FOLLOWS, "000", "_");
            std::tuple<Relationship, std::string, std::string> startWithNumber2 =
                std::make_tuple(Relationship::USES, "s", "1s");
            std::tuple<Relationship, std::string, std::string> multipleQuotations =
                std::make_tuple(Relationship::USES, "s", "\"\"a123\"\"");
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(invalidLeftArg));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(nothingBetweenQuotations));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(invalidCharacters));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(invalidWildcard));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(startWithNumber));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(startWithNumber2));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(multipleQuotations));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(numberButStartsWithZero1));
            Assert::IsFalse(QueryValidator::areSuchThatArgSyntaxValid(numberButStartsWithZero2));
        }
    };
}  // using namespace UnitTesting
