#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/QueryResultProjector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestQueryResultProjector) {
    public:
        TEST_METHOD(ReturnResults_ZeroClauses_Success) {
            RAW_ANSWER expected = { "1", "2", "3", "4", "5" };

            // Create clause answer
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            RESULT_TABLE rt{ {"1"}, {"2"}, {"3"}, {"4"}, {"5"} };
            ResultTable resultTable = ResultTable(synonyms, rt);

            ClauseAnswer clauseAns = ClauseAnswer(true, resultTable);

            // Create combined answer
            std::vector<ClauseAnswer> caList{ clauseAns };
            std::string str = "s";
            DECLARATION dec = Declaration(syn, ATTRIBUTE::STMT_NUM);
            IntermediateAnswer ia = IntermediateAnswer({ dec }, SelectionType::SINGLE, caList);

            RAW_ANSWER actual = qrp.returnResults(ia);

            Assert::IsTrue(actual == expected);
        }

        TEST_METHOD(ReturnResults_HasFalseBoolean_EmptyResult) {
            RAW_ANSWER expected = { };

            // Create clause answer
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            RESULT_TABLE rt{ {"1"}, {"4"}, {"5"} };
            ResultTable resultTable = ResultTable(synonyms, rt);

            ClauseAnswer clauseAns = ClauseAnswer(false, resultTable);

            // Create combined answer
            std::vector<ClauseAnswer> caList{ ClauseAnswer(true,
                ResultTable()), clauseAns };
            std::string str = "s";
            DECLARATION dec = Declaration(syn, ATTRIBUTE::STMT_NUM);
            IntermediateAnswer ia = IntermediateAnswer({ dec }, SelectionType::SINGLE, caList);

            RAW_ANSWER actual = qrp.returnResults(ia);

            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(ReturnResults_HasBooleanOnlySingleClause_AllSelection) {
            // Select s such that Follows(1, 2)
            RAW_ANSWER expected = { "1", "2", "3", "4", "5" };

            // Create clause answer
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            RESULT_TABLE rt{ {"1"}, {"2"}, {"3"}, {"4"}, {"5"} };
            ResultTable resultTable = ResultTable(synonyms, rt);

            ClauseAnswer clauseAns = ClauseAnswer(true, resultTable);

            // Create combined answer
            std::vector<ClauseAnswer> caList{ clauseAns };
            std::string str = "s";
            DECLARATION dec = Declaration(syn, ATTRIBUTE::STMT_NUM);
            IntermediateAnswer ia = IntermediateAnswer({ dec }, SelectionType::SINGLE, caList);

            RAW_ANSWER actual = qrp.returnResults(ia);

            Assert::IsTrue(actual == expected);
        }
        TEST_METHOD(ReturnResults_FirstSynonymSingleClause_Success) {
            // Select s such that Follows(s, a)
            RAW_ANSWER expected = { "1", "3", "5" };

            // Create clause answer
            std::string syn1 = "s";
            std::string syn2 = "a";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, ATTRIBUTE::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, ATTRIBUTE::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE rt{ {"1", "2"}, {"3", "4"}, {"5", "6"} };
            ResultTable resultTable = ResultTable(synonyms, rt);

            ClauseAnswer clauseAns = ClauseAnswer(true, resultTable);

            // Create combined answer
            std::vector<ClauseAnswer> caList{ clauseAns };
            std::string str = "s";
            DECLARATION dec = Declaration(str, ATTRIBUTE::STMT_NUM);
            IntermediateAnswer ia = IntermediateAnswer({ dec }, SelectionType::SINGLE, caList);

            RAW_ANSWER actual = qrp.returnResults(ia);

            Assert::IsTrue(actual == expected);
        }

        TEST_METHOD(ReturnResults_SecondSynonymSingleClause_Success) {
            // Select a such that Follows(s, a)
            RAW_ANSWER expected = { "2", "4", "6" };

            // Create clause answer
            std::string syn1 = "s";
            std::string syn2 = "a";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, ATTRIBUTE::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, ATTRIBUTE::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE rt{ {"1", "2"}, {"3", "4"}, {"5", "6"} };
            ResultTable resultTable = ResultTable(synonyms, rt);

            ClauseAnswer clauseAns = ClauseAnswer(true, resultTable);

            // Create combined answer
            std::vector<ClauseAnswer> caList{ clauseAns };
            std::string str = "a";
            DECLARATION dec = Declaration(str, ATTRIBUTE::STMT_NUM);
            IntermediateAnswer ia = IntermediateAnswer({ dec }, SelectionType::SINGLE, caList);

            RAW_ANSWER actual = qrp.returnResults(ia);

            Assert::IsTrue(actual == expected);
        }

        TEST_METHOD(ReturnResults_HasRepeatedAnswers_Success) {
            // Select s such that Follows(s, a)
            RAW_ANSWER expected = { "1", "3", "5" };

            // Create clause answer
            std::string syn1 = "s";
            std::string syn2 = "a";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, ATTRIBUTE::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, ATTRIBUTE::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE rt1{ {"1", "2"}, {"1", "9"}, {"3", "4"}, {"5", "6"} };
            ResultTable resultTable1 = ResultTable(synonyms, rt1);

            ClauseAnswer clauseAns1 = ClauseAnswer(true, resultTable1);

            // Create combined answer
            std::vector<ClauseAnswer> caList{ clauseAns1 };
            std::string str = "s";
            DECLARATION dec = Declaration(str, ATTRIBUTE::STMT_NUM);
            IntermediateAnswer ia = IntermediateAnswer({ dec }, SelectionType::SINGLE, caList);

            RAW_ANSWER actual = qrp.returnResults(ia);

            Assert::IsTrue(actual == expected);
        }

        TEST_METHOD(ReturnResults_TupleTwoSynonyms_Success) {
            // Select <s, v> such that Follows(s, a) pattern a("x", _) such that Uses(a, v)
            RAW_ANSWER expected = { "1 x", "3 w", "5 p" };

            // Create clause answer
            std::string syn1 = "s";
            std::string syn2 = "a";
            std::string syn3 = "v";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, ATTRIBUTE::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, ATTRIBUTE::STMT_NUM),
                    1
                },
                {
                    Declaration(syn3, ATTRIBUTE::VAR_NAME),
                    2
                }
            };
            RESULT_TABLE rt1{ {"1", "2", "x"}, {"3", "4", "w"}, {"5", "6", "p"} };
            ResultTable resultTable1 = ResultTable(synonyms, rt1);

            ClauseAnswer clauseAns1 = ClauseAnswer(true, resultTable1);

            // Create combined answer
            std::vector<ClauseAnswer> caList{ clauseAns1 };
            std::string str1 = "s";
            std::string str2 = "v";
            DECLARATION dec1 = Declaration(str1, ATTRIBUTE::STMT_NUM);
            DECLARATION dec2 = Declaration(str2, ATTRIBUTE::VAR_NAME);
            IntermediateAnswer ia = IntermediateAnswer({ dec1, dec2 },
                                                       SelectionType::TUPLE, caList);

            RAW_ANSWER actual = qrp.returnResults(ia);

            Assert::IsTrue(actual == expected);
        }

    private:
        QueryResultProjector qrp = QueryResultProjector();
    };
}  // namespace UnitTesting

