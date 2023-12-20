#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/AffectsEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestAffectsEvaluator) {
public:
    TEST_METHOD(EvaluateAffects_WildcardAndLineNum_Success) {
        /*
        * w = 1 + 2;
        * read w;
        * read x;
        * y = w + x;
        * z = y + w;
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeReadStatement(2, "w");
        pkb.storeReadStatement(3, "x");
        pkb.storeAssignStatement(4, "y", "+ w x");
        pkb.storeAssignStatement(5, "z", "+ y w");
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeRelationshipModifiesByLineNum(5, "z");
        pkb.storeRelationshipUsesByLineNum(4, "w");
        pkb.storeRelationshipUsesByLineNum(4, "x");
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.propagateStmtList(5);
        pkb.sortAllDataStructure();

        QueryArg leftArg = QueryArg(std::string("_"), QueryArgType::WILDCARD);
        DECLARATION_TABLE dt = {};

        // Wrong design entity
        Assert::IsFalse(affectsEvaluator.evaluateAffects(
            leftArg, QueryArg(std::string("2"), QueryArgType::NUM), dt).getIsClauseTrue());

        // Line number not affected by another statement
        Assert::IsFalse(affectsEvaluator.evaluateAffects(
            leftArg, QueryArg(std::string("4"), QueryArgType::NUM), dt).getIsClauseTrue());

        // Line number affected
        Assert::IsTrue(affectsEvaluator.evaluateAffects(
            leftArg, QueryArg(std::string("5"), QueryArgType::NUM), dt).getIsClauseTrue());
    }

    TEST_METHOD(EvaluateAffects_LineNumAndWildCard_Success) {
        /*
        * w = 1 + 2;
        * read w;
        * read x;
        * y = w + x;
        * z = y + w
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeReadStatement(2, "w");
        pkb.storeReadStatement(3, "x");
        pkb.storeAssignStatement(4, "y", "+ w x");
        pkb.storeAssignStatement(5, "z", "+ y w");
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeRelationshipModifiesByLineNum(5, "z");
        pkb.storeRelationshipUsesByLineNum(4, "w");
        pkb.storeRelationshipUsesByLineNum(4, "x");
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.propagateStmtList(5);
        pkb.sortAllDataStructure();

        QueryArg rightArg = QueryArg(std::string("_"), QueryArgType::WILDCARD);
        DECLARATION_TABLE dt = {};

        // Does not affect any line
        Assert::IsFalse(affectsEvaluator.evaluateAffects(
            QueryArg(std::string("1"), QueryArgType::NUM), rightArg, dt).getIsClauseTrue());

        // Wrong design ent
        Assert::IsFalse(affectsEvaluator.evaluateAffects(
            QueryArg(std::string("2"), QueryArgType::NUM), rightArg, dt).getIsClauseTrue());

        // Line number affected
        Assert::IsTrue(affectsEvaluator.evaluateAffects(
            QueryArg(std::string("4"), QueryArgType::NUM), rightArg, dt).getIsClauseTrue());
    }

    TEST_METHOD(EvaluateAffects_LineNumAndDeclaration_Success) {
        /*
        * w = 1 + 2;
        * read w;
        * read x;
        * y = w + x;
        * z = y + w
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeReadStatement(2, "w");
        pkb.storeReadStatement(3, "x");
        pkb.storeAssignStatement(4, "y", "+ w x");
        pkb.storeAssignStatement(5, "z", "+ y w");
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeRelationshipModifiesByLineNum(5, "z");
        pkb.storeRelationshipUsesByLineNum(4, "w");
        pkb.storeRelationshipUsesByLineNum(4, "x");
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.propagateStmtList(5);
        pkb.sortAllDataStructure();

        DECLARATION_TABLE dt = { {"a", DesignEntity::ASSIGN} };

        Declaration dec1 = Declaration(std::string("a"), Attribute::NONE);

        // Does not affect any line
        Assert::IsTrue(affectsEvaluator.evaluateAffects(QueryArg(std::string("1"),
            QueryArgType::NUM), QueryArg(dec1), dt).getResultTable().getResultTable().empty());

        // Line number affected
        Assert::IsTrue(affectsEvaluator.evaluateAffects(QueryArg(std::string("4"),
            QueryArgType::NUM), QueryArg(dec1), dt).getResultTable()
            .getResultTable()[0] == std::vector<std::string> { "5" });
    }

    TEST_METHOD(EvaluateAffects_DeclarationAndDeclaration_Success) {
        /*
        * x = x + 1;
        * while (x != 2) {
        *   x = x + 3;
        *   if (x < 4) then {
        *     x = x + 5;
        *   } else {
        *     x = x + 6;
        *   }
        * }
        * x = x + 7
        */
        pkb.storeAssignStatement(1, "x", "+ x 1");
        pkb.storeWhileStatement(2, { "x" });
        pkb.storeAssignStatement(3, "x", "+ x 3");
        pkb.storeIfElseStatement(4, { "x" });
        pkb.storeAssignStatement(5, "x", "+ x 5");
        pkb.storeAssignStatement(6, "x", "+ x 6");
        pkb.storeAssignStatement(7, "x", "+ x 7");

        // store the next relationship
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(2, 7);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipNext(4, 6);
        pkb.storeRelationshipNext(5, 2);
        pkb.storeRelationshipNext(6, 2);

        // store modifies relationship
        pkb.storeRelationshipModifiesByLineNum(1, "x");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeRelationshipModifiesByLineNum(5, "x");
        pkb.storeRelationshipModifiesByLineNum(6, "x");
        pkb.storeRelationshipModifiesByLineNum(7, "x");

        // store uses relationship
        pkb.storeRelationshipUsesByLineNum(1, "x");
        pkb.storeRelationshipUsesByLineNum(2, "x");
        pkb.storeRelationshipUsesByLineNum(3, "x");
        pkb.storeRelationshipUsesByLineNum(4, "x");
        pkb.storeRelationshipUsesByLineNum(5, "x");
        pkb.storeRelationshipUsesByLineNum(6, "x");
        pkb.storeRelationshipUsesByLineNum(7, "x");

        pkb.propagateStmtList(7);
        pkb.sortAllDataStructure();

        DECLARATION_TABLE dt = { {"a1", DesignEntity::ASSIGN}, {"a2", DesignEntity::ASSIGN} };

        Declaration dec1 = Declaration(std::string("a1"), Attribute::NONE);
        Declaration dec2 = Declaration(std::string("a2"), Attribute::NONE);

        std::vector<ROW> expected{ {"1", "3"}, {"1", "7"},
            {"3", "5"}, {"3", "6"}, {"5", "3"}, {"5", "7"}, {"6", "3"}, {"6", "7"} };
        // Line number affected
        Assert::IsTrue(affectsEvaluator.evaluateAffects(QueryArg(dec1), QueryArg(dec2), dt)
            .getResultTable().getResultTable() == expected);
    }

    TEST_METHOD(DoLinesAffects_AffectsRelationship_Success) {
        /*
        * w = 1 + 2;
        * read w;
        * read x;
        * y = w + x;
        * z = y + w
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeReadStatement(2, "w");
        pkb.storeReadStatement(3, "x");
        pkb.storeAssignStatement(4, "y", "+ w x");
        pkb.storeAssignStatement(5, "z", "+ y w");
        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeRelationshipModifiesByLineNum(5, "z");
        pkb.storeRelationshipUsesByLineNum(4, "w");
        pkb.storeRelationshipUsesByLineNum(4, "x");
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.propagateStmtList(5);
        pkb.sortAllDataStructure();

        DECLARATION_TABLE dt = {};

        // do not affect
        Assert::IsFalse(affectsEvaluator.doLinesAffects(1, 4));

        // affects each other
        Assert::IsTrue(affectsEvaluator.doLinesAffects(4, 5));
    }

private:
    PKB pkb;
    PKB* ptr = &pkb;
    AffectsEvaluator affectsEvaluator = AffectsEvaluator(ptr);
    };
}  // namespace UnitTesting
