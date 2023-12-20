#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/EvaluatorUtility.h"
#include "PQL/Query/Declaration.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestEvaluatorUtility) {
    public:
        TEST_METHOD(LineNumVecToResultTable_SimpleVector_Success) {
            std::vector<LINE_NUM> input = { 1, 2, 3 };
            SYNONYM syn = "s";
            Declaration dec = Declaration(syn, Attribute::STMT_NUM);

            ResultTable actual = EvaluatorUtility::lineNumVecToResultTable(dec, input);
            std::vector<std::vector<std::string>> expected = { {"1"}, {"2"}, {"3"} };

            Assert::IsTrue(actual.getResultTable() == expected);
        }
        TEST_METHOD(LineNumPairVecToResultTable_SimpleVector_Success) {
            std::vector<std::pair<LINE_NUM, LINE_NUM>> input = { {1, 2}, {3, 4}, {5, 6} };
            SYNONYM syn1 = "s";
            Declaration dec1 = Declaration(syn1, Attribute::STMT_NUM);
            SYNONYM syn2 = "a";
            Declaration dec2 = Declaration(syn2, Attribute::STMT_NUM);

            ResultTable actual = EvaluatorUtility::lineNumVecPairToResultTable(dec1, dec2, input);
            std::vector<std::vector<std::string>> expected = { {"1", "2"}, {"3", "4"}, {"5", "6"} };

            Assert::IsTrue(actual.getResultTable() == expected);
        }
        TEST_METHOD(StrVecToResultTable_SimpleVector_Success) {
            std::vector<std::string> input = { "x", "y", "z" };
            SYNONYM syn = "v";
            Declaration dec = Declaration(syn, Attribute::VAR_NAME);

            ResultTable actual = EvaluatorUtility::strVecToResultTable(dec, input);
            std::vector<std::vector<std::string>> expected = { {"x"}, {"y"}, {"z"} };

            Assert::IsTrue(actual.getResultTable() == expected);
        }
        TEST_METHOD(StrPairVecToResultTable_SimpleVector_Success) {
            std::vector<std::pair<std::string, std::string>> input = { {"a", "b"}, {"c", "d"} };
            SYNONYM syn1 = "v";
            Declaration dec1 = Declaration(syn1, Attribute::VAR_NAME);
            SYNONYM syn2 = "p";
            Declaration dec2 = Declaration(syn2, Attribute::PROC_NAME);

            ResultTable actual = EvaluatorUtility::strVecPairToResultTable(dec1, dec2, input);
            std::vector<std::vector<std::string>> expected = { {"a", "b"}, {"c", "d"} };

            Assert::IsTrue(actual.getResultTable() == expected);
        }
        TEST_METHOD(LineNumStrPairVecToResultTable_SimpleVector_Success) {
            std::vector<std::pair<LINE_NUM, std::string>> input = { {1, "a"}, {2, "b"} };
            SYNONYM syn1 = "s";
            Declaration dec1 = Declaration(syn1, Attribute::STMT_NUM);
            SYNONYM syn2 = "v";
            Declaration dec2 = Declaration(syn2, Attribute::VAR_NAME);

            ResultTable act = EvaluatorUtility::lineNumStrVecPairToResultTable(dec1, dec2, input);
            std::vector<std::vector<std::string>> expected = { {"1", "a"}, {"2", "b"} };

            Assert::IsTrue(act.getResultTable() == expected);
        }
        TEST_METHOD(StrLineNumPairVecToResultTable_SimpleVector_Success) {
            std::vector<std::pair<std::string, LINE_NUM>> input = { {"a", 1}, {"b", 2} };
            SYNONYM syn1 = "v";
            Declaration dec1 = Declaration(syn1, Attribute::VAR_NAME);
            SYNONYM syn2 = "s";
            Declaration dec2 = Declaration(syn2, Attribute::STMT_NUM);

            ResultTable act = EvaluatorUtility::strLineNumVecPairToResultTable(dec1, dec2, input);
            std::vector<std::vector<std::string>> expected = { {"a", "1"}, {"b", "2"} };

            Assert::IsTrue(act.getResultTable() == expected);
        }
    };
}  // namespace UnitTesting

