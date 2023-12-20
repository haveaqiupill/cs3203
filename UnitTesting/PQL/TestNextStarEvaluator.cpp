#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/NextStarEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestNextStarEvaluator) {
public:
    TEST_METHOD(doesLinesNextStar_NextStarRelationship_Success) {
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        bool actual = nextStarEvaluator.doesLinesNextStar(1, 3);
        Assert::IsTrue(actual);
    }

    TEST_METHOD(EvaluateBooleanRelationship_NextStarRelationship_Failed) {
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.propagateStmtList(4);
        bool actual = nextStarEvaluator.doesLinesNextStar(1, 4);
        Assert::IsFalse(actual);
    }
    TEST_METHOD(EvaluateNextStar_LineNumAndDeclaration_Success) {
        pkb.storeAssignStatement(1, "x", "+ 1 2");
        pkb.storeWhileStatement(2, { "x" });
        pkb.storeReadStatement(3, "x");
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 2);
        pkb.sortAllDataStructure();
        DECLARATION_TABLE dc = { {"r", DesignEntity::READ} };
        Declaration dec = Declaration(std::string("r"), Attribute::NONE);
        ClauseAnswer actual = nextStarEvaluator.evaluateNextStar(
            QueryArg(std::string("1"), QueryArgType::NUM), QueryArg(dec), dc);
        Assert::IsTrue(actual.getIsClauseTrue());
        Assert::IsTrue(actual.getResultTable().getResultTable()[0] ==
            std::vector<std::string> {"3"});
    }

    TEST_METHOD(EvaluateNextStar_DeclarationAndDeclaration_Success) {
        pkb.storeAssignStatement(1, "x", "+ 1 2");
        pkb.storeWhileStatement(2, { "x" });
        pkb.storeReadStatement(3, "x");
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 2);
        pkb.sortAllDataStructure();
        DECLARATION_TABLE dc = { {"r", DesignEntity::READ}, {"a", DesignEntity::ASSIGN }};
        Declaration dec1 = Declaration(std::string("a"), Attribute::NONE);
        Declaration dec2 = Declaration(std::string("r"), Attribute::NONE);
        ClauseAnswer actual = nextStarEvaluator.evaluateNextStar(
            QueryArg(dec1), QueryArg(dec2), dc);
        Assert::IsTrue(actual.getIsClauseTrue());
        Assert::IsTrue(actual.getResultTable().getResultTable()[0] ==
            std::vector<std::string>{"1", "3"});
    }

    TEST_METHOD(EvaluateNextStar_DeclarationAndLineNum_Success) {
        pkb.storeAssignStatement(1, "x", "+ 1 2");
        pkb.storeWhileStatement(2, { "x" });
        pkb.storeReadStatement(3, "x");
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 2);
        pkb.sortAllDataStructure();
        DECLARATION_TABLE dc = { {"a", DesignEntity::ASSIGN} };
        Declaration dec = Declaration(std::string("a"), Attribute::NONE);
        ClauseAnswer actual = nextStarEvaluator.evaluateNextStar(
             QueryArg(dec), QueryArg(std::string("3"), QueryArgType::NUM), dc);
        Assert::IsTrue(actual.getIsClauseTrue());
        Assert::IsTrue(actual.getResultTable().getResultTable()[0] ==
            std::vector<std::string> {"1"});
    }
private:
    PKB pkb;
    PKB* ptr = &pkb;
    NextStarEvaluator nextStarEvaluator = NextStarEvaluator(ptr);
    };
}  // namespace UnitTesting
