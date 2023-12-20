#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/UsesEvaluator.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestUsesEvaluator) {
    public:
        TEST_METHOD(EvaluateUses_LineNumDeclaration_Success) {
            // Uses(1, v)
            std::string syn = "v";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::VAR_NAME),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"x"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(1);
            pkb.storeVariable("x");
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"1", QueryArgType::NUM);
            std::string synonym = "v";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::VAR_NAME);
            QueryArg rightArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"v", DesignEntity::VARIABLE} };

            ClauseAnswer actual = use.evaluateUses(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateUses_ProcNameDeclaration_Success) {
            // Uses("main", v)
            std::string syn = "v";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::VAR_NAME),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"x"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(1);
            pkb.storeProcedure("main", 1, 1, {});
            pkb.storeVariable("x");
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"main", QueryArgType::NAME);
            std::string synonym = "v";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::VAR_NAME);
            QueryArg rightArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"v", DesignEntity::VARIABLE} };

            ClauseAnswer actual = use.evaluateUses(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateUses_LineNumDecVariable_Success) {
            // Uses(s, "x")
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"1"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(1);
            pkb.storeVariable("x");
            pkb.storeRelationshipUsesByLineNum(1, "x");
            pkb.sortAllDataStructure();

            QueryArg rightArg = QueryArg((std::string)"x", QueryArgType::NAME);
            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg leftArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = use.evaluateUses(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateUses_ProcNameDecVariable_Success) {
            // Uses(p, "x")
            std::string syn = "p";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::PROC_NAME),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"main"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(1);
            pkb.storeProcedure("main", 1, 1, {});
            pkb.storeVariable("x");
            pkb.storeRelationshipUsesByProcedure("main", "x");
            pkb.sortAllDataStructure();

            QueryArg rightArg = QueryArg((std::string)"x", QueryArgType::NAME);
            std::string synonym = "p";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::PROC_NAME);
            QueryArg leftArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"p", DesignEntity::PROCEDURE} };

            ClauseAnswer actual = use.evaluateUses(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
    private:
        PKB pkb;
        UsesEvaluator use = UsesEvaluator(&pkb);
    };
}  // namespace UnitTesting
