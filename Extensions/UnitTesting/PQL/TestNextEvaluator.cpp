#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/NextEvaluator.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestNextEvaluator) {
    public:
        TEST_METHOD(EvaluateNext_LineNumDeclaration_Success) {
            // Next(1, s)
            pkb.propagateStmtList(2);
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"1", QueryArgType::NUM);
            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg rightArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = nex.evaluateNext(leftArg, rightArg, dt);

            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"2"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateNext_DeclarationLineNum_Success) {
            // Next(s, 2)
            pkb.propagateStmtList(2);
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            QueryArg rightArg = QueryArg((std::string)"2", QueryArgType::NUM);
            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg leftArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = nex.evaluateNext(leftArg, rightArg, dt);

            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"1"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateNext_WildcardWildcard_Success) {
            // Next(_, _)
            ClauseAnswer expected = ClauseAnswer(true, ResultTable());
            pkb.propagateStmtList(2);
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt;

            ClauseAnswer actual = nex.evaluateNext(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateNext_WildcardLineNum_Success) {
            // Next(_, 2)
            std::unordered_map<Declaration, int, dec_hash> synonyms = { };
            ResultTable rt = ResultTable(synonyms, { });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(2);
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            QueryArg rightArg = QueryArg((std::string)"2", QueryArgType::NUM);
            DECLARATION_TABLE dt = { };

            ClauseAnswer actual = nex.evaluateNext(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateNext_DeclarationWildcard_Success) {
            // Next(s, _)
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"1"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);

            pkb.propagateStmtList(2);
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg leftArg = QueryArg(dec);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = nex.evaluateNext(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateNext_LineNumWildCard_Success) {
            // Next(1, _)
            ClauseAnswer expected = ClauseAnswer(true, ResultTable());
            pkb.propagateStmtList(2);
            pkb.storeRelationshipNext(1, 2);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"1", QueryArgType::NUM);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt;

            ClauseAnswer actual = nex.evaluateNext(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
        }
    private:
        PKB pkb;
        NextEvaluator nex = NextEvaluator(&pkb);
    };
}  // namespace UnitTesting
