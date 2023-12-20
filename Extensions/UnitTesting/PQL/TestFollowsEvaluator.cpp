#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/FollowsEvaluator.h"
#include "PKB/PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestFollowsEvaluator) {
    public:
        TEST_METHOD(EvaluateFollows_LineNumWildCard_Success) {
            // Follows(1, _)
            ClauseAnswer expected = ClauseAnswer(true, ResultTable());
            pkb.propagateStmtList(2);
            pkb.storeRelationshipFollows(1, 2);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"1", QueryArgType::NUM);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt;

            ClauseAnswer actual = fol.evaluateFollows(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
        }
        TEST_METHOD(EvaluateFollows_LineNumDeclaration_Success) {
            // Follows(1, s)
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"3"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.propagateStmtList(5);
            pkb.storeRelationshipFollows(1, 3);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"1", QueryArgType::NUM);
            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg rightArg = QueryArg(dec);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = fol.evaluateFollows(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateFollows_DeclarationLineNum_Success) {
            // Follows(s, 5)
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"1"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.propagateStmtList(5);
            pkb.storeRelationshipFollows(1, 5);
            pkb.sortAllDataStructure();

            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg leftArg = QueryArg(dec);
            QueryArg rightArg = QueryArg((std::string)"5", QueryArgType::NUM);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = fol.evaluateFollows(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateFollows_DeclarationWildcard_Success) {
            // Follows(s, _)
            std::string syn = "s";
            std::unordered_map<Declaration, int, dec_hash> synonyms = {
                {
                    Declaration(syn, ATTRIBUTE::STMT_NUM),
                    0
                }
            };
            ResultTable rt = ResultTable(synonyms, { {"3"} });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.propagateStmtList(5);
            pkb.storeRelationshipFollows(3, 5);
            pkb.sortAllDataStructure();

            std::string synonym = "s";
            DECLARATION dec = Declaration(synonym, ATTRIBUTE::STMT_NUM);
            QueryArg leftArg = QueryArg(dec);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt = { {"s", DesignEntity::STMT} };

            ClauseAnswer actual = fol.evaluateFollows(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateFollows_WildcardWildcard_Success) {
            // Follows(_, _)
            ClauseAnswer expected = ClauseAnswer(true, ResultTable());
            pkb.propagateStmtList(3);
            pkb.storeRelationshipFollows(1, 3);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            QueryArg rightArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            DECLARATION_TABLE dt;

            ClauseAnswer actual = fol.evaluateFollows(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
        TEST_METHOD(EvaluateFollows_WildcardLineNum_Success) {
            // Follows(_, 5)
            std::unordered_map<Declaration, int, dec_hash> synonyms = { };
            ResultTable rt = ResultTable(synonyms, { });
            ClauseAnswer expected = ClauseAnswer(true, rt);
            pkb.propagateStmtList(5);

            pkb.storeRelationshipFollows(3, 5);
            pkb.sortAllDataStructure();

            QueryArg leftArg = QueryArg((std::string)"_", QueryArgType::WILDCARD);
            QueryArg rightArg = QueryArg((std::string)"5", QueryArgType::NUM);
            DECLARATION_TABLE dt = { };

            ClauseAnswer actual = fol.evaluateFollows(leftArg, rightArg, dt);

            Assert::AreEqual(expected.getIsClauseTrue(), actual.getIsClauseTrue());
            Assert::IsTrue(expected.getResultTable().getResultTable() ==
                actual.getResultTable().getResultTable());
            Assert::IsTrue(expected.getResultTable().getSynonyms() ==
                actual.getResultTable().getSynonyms());
        }
    private:
        PKB pkb;
        FollowsEvaluator fol = FollowsEvaluator(&pkb);
    };
}  // namespace UnitTesting
