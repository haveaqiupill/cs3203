#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/Answers/ResultTable.h"
#include "PQL/QueryParserUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestResultTable) {
    public:
        TEST_METHOD(MergeTables_EmptyHeaders_Success) {
            // Merge table with empty table and (a, s)
            std::string syn1 = "a";
            std::string syn2 = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable{ {"1", "2"}, {"8", "9"} };
            ResultTable expected = ResultTable(synonyms, resTable);

            std::unordered_map<Declaration, int, dec_hash> synonyms1 = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable1{ {"1", "2"}, {"8", "9"} };
            ResultTable table1 = ResultTable(synonyms1, resTable1);

            std::unordered_map<Declaration, int, dec_hash> synonyms2{ };
            RESULT_TABLE resTable2{ };
            ResultTable table2 = ResultTable(synonyms2, resTable2);

            ResultTable actual = table1.merge(table2);

            Assert::IsTrue(expected.getSynonyms() == actual.getSynonyms());
            Assert::IsTrue(expected.getResultTable() == actual.getResultTable());
        }
        TEST_METHOD(MergeTables_BooleanClause_Success) {
            // Merge table with synonyms (a, s) and s
            RESULT_TABLE resTable{ {"1", "2"}, {"8", "9"} };
            std::string syn1 = "a";
            std::string syn2 = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            ResultTable expected = ResultTable(synonyms, resTable);

            std::unordered_map<Declaration, int, dec_hash> synonyms1 = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable1{ {"1", "2"}, {"8", "9"} };
            ResultTable table1 = ResultTable(synonyms1, resTable1);

            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms2 = {
                {
                    Declaration(syn, Attribute::STMT_NUM),
                    0
                }
            };
            RESULT_TABLE resTable2{ {"1"}, {"2"}, {"3"}, {"4"}, {"5"}, {"6"}, {"7"}, {"8"}, {"9"} };
            ResultTable table2 = ResultTable(synonyms2, resTable2);

            ResultTable actual = table1.merge(table2);

            Assert::IsTrue(expected.getSynonyms() == actual.getSynonyms());
            Assert::IsTrue(expected.getResultTable() == actual.getResultTable());
        }

        TEST_METHOD(MergeTables_NoCommonSynonym_Success) {
            // Merge table with synonyms (a, s) and (w, v)
            std::string syn1 = "a";
            std::string syn2 = "s";
            std::string syn3 = "w";
            std::string syn4 = "v";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                },
                {
                    Declaration(syn3, Attribute::STMT_NUM),
                    2
                },
                {
                    Declaration(syn4, Attribute::VAR_NAME),
                    3
                }
            };
            RESULT_TABLE resTable{ {"1", "2", "8", "x"},
                                   {"1", "2", "7", "y"},
                                   {"3", "4", "8", "x"},
                                   {"3", "4", "7", "y"} };
            ResultTable expected = ResultTable(synonyms, resTable);

            std::unordered_map<Declaration, int, dec_hash> synonyms1 = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable1{ {"1", "2"}, {"3", "4"} };
            ResultTable table1 = ResultTable(synonyms1, resTable1);

            std::unordered_map<Declaration, int, dec_hash> synonyms2 = {
                {
                    Declaration(syn3, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn4, Attribute::VAR_NAME),
                    1
                }
            };
            RESULT_TABLE resTable2{ {"8", "x"}, {"7", "y"} };
            ResultTable table2 = ResultTable(synonyms2, resTable2);

            ResultTable actual = table1.merge(table2);

            Assert::IsTrue(expected.getSynonyms() == actual.getSynonyms());
            Assert::IsTrue(expected.getResultTable() == actual.getResultTable());
        }
        TEST_METHOD(MergeTables_OneCommonSynonym_Success) {
            // Merge table with synonyms (a, s) and (s, v)
            std::string syn1 = "a";
            std::string syn2 = "s";
            std::string syn3 = "v";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                },
                {
                    Declaration(syn3, Attribute::VAR_NAME),
                    2
                }
            };
            RESULT_TABLE resTable{ {"1", "2", "x"} };
            ResultTable expected = ResultTable(synonyms, resTable);

            std::unordered_map<Declaration, int, dec_hash> synonyms1 = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable1{ {"1", "2"}, {"3", "4"} };
            ResultTable table1 = ResultTable(synonyms1, resTable1);

            std::unordered_map<Declaration, int, dec_hash> synonyms2 = {
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn3, Attribute::VAR_NAME),
                    1
                }
            };
            RESULT_TABLE resTable2{ {"2", "x"}, {"5", "y"} };
            ResultTable table2 = ResultTable(synonyms2, resTable2);

            ResultTable actual = table1.merge(table2);

            Assert::IsTrue(expected.getSynonyms() == actual.getSynonyms());
            Assert::IsTrue(expected.getResultTable() == actual.getResultTable());
        }

        TEST_METHOD(MergeTables_TwoCommonSynonyms_Success) {
            // Merge table with synonyms (a, s) and (s, a)
            std::string syn1 = "a";
            std::string syn2 = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable{ {"3", "4"}, {"7", "8"} };
            ResultTable expected = ResultTable(synonyms, resTable);

            std::unordered_map<Declaration, int, dec_hash> synonyms1 = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable1{ {"1", "2"}, {"3", "4"}, {"5", "6"}, {"7", "8"} };
            ResultTable table1 = ResultTable(synonyms1, resTable1);

            std::unordered_map<Declaration, int, dec_hash> synonyms2 = {
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    1
                }
            };
            RESULT_TABLE resTable2{ {"8", "7"}, {"6", "1"}, {"4", "3"} };
            ResultTable table2 = ResultTable(synonyms2, resTable2);

            ResultTable actual = table1.merge(table2);

            Assert::IsTrue(expected.getSynonyms() == actual.getSynonyms());
            Assert::IsTrue(expected.getResultTable() == actual.getResultTable());
        }

        TEST_METHOD(MergeTables_FiveSynonymsTwoCommonSynonyms_Success) {
            // Merge table with synonyms (a, s, v, w) and (s, a, i)
            std::string syn1 = "a";
            std::string syn2 = "s";
            std::string syn3 = "v";
            std::string syn4 = "w";
            std::string syn5 = "i";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                },
                {
                    Declaration(syn3, Attribute::VAR_NAME),
                    2
                },
                {
                    Declaration(syn4, Attribute::STMT_NUM),
                    3
                },
                {
                    Declaration(syn5, Attribute::STMT_NUM),
                    4
                }
            };
            RESULT_TABLE resTable{ {"3", "4", "x", "10", "18"}, {"7", "8", "k", "2", "5"} };
            ResultTable expected = ResultTable(synonyms, resTable);

            // (a, s, v, w)
            std::unordered_map<Declaration, int, dec_hash> synonyms1 = {
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    1
                },
                {
                    Declaration(syn3, Attribute::VAR_NAME),
                    2
                },
                {
                    Declaration(syn4, Attribute::STMT_NUM),
                    3
                },
            };
            RESULT_TABLE resTable1{
                {"1", "2", "test", "20"},
                {"3", "4", "x", "10"},
                {"5", "6", "hello", "11"},
                {"7", "8", "k", "2"}
            };
            ResultTable table1 = ResultTable(synonyms1, resTable1);

            // (s, a, i)
            std::unordered_map<Declaration, int, dec_hash> synonyms2 = {
                {
                    Declaration(syn2, Attribute::STMT_NUM),
                    0
                },
                {
                    Declaration(syn1, Attribute::STMT_NUM),
                    1
                },
                {
                    Declaration(syn5, Attribute::STMT_NUM),
                    2
                }
            };
            RESULT_TABLE resTable2{ {"8", "7", "5"}, {"6", "1", "100"}, {"4", "3", "18"} };
            ResultTable table2 = ResultTable(synonyms2, resTable2);

            ResultTable actual = table1.merge(table2);

            Assert::IsTrue(expected.getSynonyms() == actual.getSynonyms());
            Assert::IsTrue(expected.getResultTable() == actual.getResultTable());
        }
    };
}  // namespace UnitTesting

