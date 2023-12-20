#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/ParentEvaluator.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestParentEvaluator) {
    public:
        TEST_METHOD(EvaluateParent_LineNumWildCard_Success) {
            // Parent(1, _)
            ClauseAnswer expected = ClauseAnswer(true, ResultTable());
            pkb.propagateStmtList(2);
            pkb.storeRelationshipParent(1, 2);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"1", QueryArgType::NUM);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt;

            ClauseAnswer actual = prevaluator.evaluateParent(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
        }
        TEST_METHOD(EvaluateParent_LineNumDeclaration_Success) {
            // Parent(1, s)
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"3"}, {"5"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.propagateStmtList(5);
            pkb.storeRelationshipParent(1, 3);
            pkb.storeRelationshipParent(1, 5);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"1", QueryArgType::NUM);
            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg rightArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = prevaluator.evaluateParent(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateParent_DeclarationLineNum_Success) {
            // Parent(s, 5)
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"1"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.storeWhileStatement(1, std::list<VAR_NAME>());
            pkb.propagateStmtList(5);
            pkb.storeRelationshipParent(1, 5);
            pkb.sortAllDataStructure();

            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg leftArg = QueryArg(dec);
            QueryArg rightArg = QueryArg((std::string)"5", QueryArgType::NUM);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = prevaluator.evaluateParent(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateParent_DeclarationWildcard_Success) {
            // Parent(s, _)
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"3"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.storeWhileStatement(3, std::list<VAR_NAME>());
            pkb.propagateStmtList(5);
            pkb.storeRelationshipParent(3, 5);
            pkb.sortAllDataStructure();

            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg leftArg = QueryArg(dec);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = prevaluator.evaluateParent(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateParent_WildcardWildcard_Success) {
            // Parent(_, _)
            ClauseAnswer expected = ClauseAnswer(true, ResultTable());
            pkb.propagateStmtList(3);
            pkb.storeRelationshipParent(1, 3);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt;

            ClauseAnswer actual = prevaluator.evaluateParent(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateParent_WildcardLineNum_Success) {
            // Parent(_, 5)
            std::unordered_map<Declaration, int, dec_hash> synonyms = { };
            ResultTable rt = ResultTable(synonyms, { });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.storeWhileStatement(3, std::list<VAR_NAME>());
            pkb.propagateStmtList(5);

            pkb.storeRelationshipParent(3, 5);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            QueryArg rightArg = QueryArg((std::string)"5", QueryArgType::NUM);
            DECLARATION_TABLE dt = { };

            ClauseAnswer actual = prevaluator.evaluateParent(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
    private:
        PKB pkb;
        ParentEvaluator prevaluator = ParentEvaluator(&pkb);
    };
}  // namespace UnitTesting
