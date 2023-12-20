#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/CallsEvaluator.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestCallsEvaluator) {
    public:
        TEST_METHOD(EvaluateCalls_ProcNameDeclaration_Success) {
            // Call("alpha", p)
            std::string syn = "p";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::PROC_NAME),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"beta"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(2);
            pkb.storeProcedure("alpha", 1, 1, {});
            pkb.storeProcedure("beta", 2, 2, {});
            pkb.storeCallStatement(1, "beta");
            pkb.storeRelationshipCalls("alpha", "beta");
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"alpha", QueryArgType::NAME);
            std::string synonym = "p";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::PROC_NAME);
            QueryArg rightArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"p", DesignEntity::PROCEDURE} };

            ClauseAnswer actual = cal.evaluateCalls(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateCalls_DeclarationProcName_Success) {
            // Call(p, "beta")
            std::string syn = "p";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::PROC_NAME),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"alpha"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(2);
            pkb.storeProcedure("alpha", 1, 1, {});
            pkb.storeProcedure("beta", 2, 2, {});
            pkb.storeCallStatement(1, "beta");
            pkb.storeRelationshipCalls("alpha", "beta");
            pkb.sortAllDataStructure();

            QueryArg rightArg = QueryArg((std::string)"beta", QueryArgType::NAME);
            std::string synonym = "p";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::PROC_NAME);
            QueryArg leftArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"p", DesignEntity::PROCEDURE} };

            ClauseAnswer actual = cal.evaluateCalls(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
    private:
        PKB pkb;
        CallsEvaluator cal = CallsEvaluator(&pkb);
    };
}  // namespace UnitTesting
