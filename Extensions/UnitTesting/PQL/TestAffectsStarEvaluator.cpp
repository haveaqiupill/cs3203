#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/AffectsStarEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestAffectsStarEvaluator) {
public:
    TEST_METHOD(DoLinesAffectsStar_ValidRelationShip_True) {
        /*
        w = 1 + 2;
        read w;
        x = w + 2;
        y = 1;
        while (w != x) {
	        x = w + x;
	        if (x != y) then {
		        y = x + x;
	        } else {
		        y = x - w;
	        }
        }
        z = y;
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeReadStatement(2, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeAssignStatement(3, "x", "+ w 2");
        pkb.storeRelationshipUsesByLineNum(3, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeAssignStatement(4, "y", "1");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeWhileStatement(5, { "w", "x" });
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "x");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.storeAssignStatement(6, "x", "+ w x");
        pkb.storeRelationshipUsesByLineNum(6, "x");
        pkb.storeRelationshipUsesByLineNum(6, "w");
        pkb.storeRelationshipModifiesByLineNum(6, "x");
        pkb.storeIfElseStatement(7, { "y", "x" });
        pkb.storeRelationshipUsesByLineNum(7, "y");
        pkb.storeRelationshipUsesByLineNum(7, "x");
        pkb.storeAssignStatement(8, "y", "+ x x");
        pkb.storeRelationshipUsesByLineNum(8, "x");
        pkb.storeRelationshipModifiesByLineNum(8, "y");
        pkb.storeAssignStatement(9, "y", "- x w");
        pkb.storeRelationshipUsesByLineNum(9, "x");
        pkb.storeRelationshipUsesByLineNum(9, "w");
        pkb.storeRelationshipModifiesByLineNum(9, "y");
        pkb.storeAssignStatement(10, "z", "y");
        pkb.storeRelationshipUsesByLineNum(10, "y");
        pkb.storeRelationshipModifiesByLineNum(10, "z");

        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipNext(5, 6);
        pkb.storeRelationshipNext(6, 7);
        pkb.storeRelationshipNext(7, 8);
        pkb.storeRelationshipNext(7, 9);
        pkb.storeRelationshipNext(8, 5);
        pkb.storeRelationshipNext(9, 5);
        pkb.storeRelationshipNext(5, 10);

        pkb.sortAllDataStructure();

        Assert::IsTrue(affectsStarEval.doLinesAffectsStar(3, 6));
        Assert::IsTrue(affectsStarEval.doLinesAffectsStar(4, 10));
        Assert::IsTrue(affectsStarEval.doLinesAffectsStar(3, 10));
    }

    TEST_METHOD(DoLinesAffectsStar_InvalidRelationShip_False) {
        /*
        w = 1 + 2;
        read w;
        x = w + 2;
        y = 1;
        while (w != x) {
            x = w + x;
            if (x != y) then {
                y = x + x;
            } else {
                y = x - w;
            }
        }
        z = y;
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeReadStatement(2, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeAssignStatement(3, "x", "+ w 2");
        pkb.storeRelationshipUsesByLineNum(3, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeAssignStatement(4, "y", "1");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeWhileStatement(5, { "w", "x" });
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "x");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.storeAssignStatement(6, "x", "+ w x");
        pkb.storeRelationshipUsesByLineNum(6, "x");
        pkb.storeRelationshipUsesByLineNum(6, "w");
        pkb.storeRelationshipModifiesByLineNum(6, "x");
        pkb.storeIfElseStatement(7, { "y", "x" });
        pkb.storeRelationshipUsesByLineNum(7, "y");
        pkb.storeRelationshipUsesByLineNum(7, "x");
        pkb.storeAssignStatement(8, "y", "+ x x");
        pkb.storeRelationshipUsesByLineNum(8, "x");
        pkb.storeRelationshipModifiesByLineNum(8, "y");
        pkb.storeAssignStatement(9, "y", "- x w");
        pkb.storeRelationshipUsesByLineNum(9, "x");
        pkb.storeRelationshipUsesByLineNum(9, "w");
        pkb.storeRelationshipModifiesByLineNum(9, "y");
        pkb.storeAssignStatement(10, "z", "y");
        pkb.storeRelationshipUsesByLineNum(10, "y");
        pkb.storeRelationshipModifiesByLineNum(10, "z");

        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipNext(5, 6);
        pkb.storeRelationshipNext(6, 7);
        pkb.storeRelationshipNext(7, 8);
        pkb.storeRelationshipNext(7, 9);
        pkb.storeRelationshipNext(8, 5);
        pkb.storeRelationshipNext(9, 5);
        pkb.storeRelationshipNext(5, 10);

        pkb.sortAllDataStructure();

        Assert::IsFalse(affectsStarEval.doLinesAffectsStar(1, 3));
        Assert::IsFalse(affectsStarEval.doLinesAffectsStar(4, 8));
        Assert::IsFalse(affectsStarEval.doLinesAffectsStar(2, 3));
    }

    TEST_METHOD(EvaluateAffectsStar_LineNumAndDeclaration_Success) {
        /*
        w = 1 + 2;
        read w;
        x = w + 2;
        y = 1;
        while (w != x) {
            x = w + x;
            if (x != y) then {
                y = x + x;
            } else {
                y = x - w;
            }
        }
        z = y;
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeReadStatement(2, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeAssignStatement(3, "x", "+ w 2");
        pkb.storeRelationshipUsesByLineNum(3, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeAssignStatement(4, "y", "1");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeWhileStatement(5, { "w", "x" });
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "x");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.storeAssignStatement(6, "x", "+ w x");
        pkb.storeRelationshipUsesByLineNum(6, "x");
        pkb.storeRelationshipUsesByLineNum(6, "w");
        pkb.storeRelationshipModifiesByLineNum(6, "x");
        pkb.storeIfElseStatement(7, { "y", "x" });
        pkb.storeRelationshipUsesByLineNum(7, "y");
        pkb.storeRelationshipUsesByLineNum(7, "x");
        pkb.storeAssignStatement(8, "y", "+ x x");
        pkb.storeRelationshipUsesByLineNum(8, "x");
        pkb.storeRelationshipModifiesByLineNum(8, "y");
        pkb.storeAssignStatement(9, "y", "- x w");
        pkb.storeRelationshipUsesByLineNum(9, "x");
        pkb.storeRelationshipUsesByLineNum(9, "w");
        pkb.storeRelationshipModifiesByLineNum(9, "y");
        pkb.storeAssignStatement(10, "z", "y");
        pkb.storeRelationshipUsesByLineNum(10, "y");
        pkb.storeRelationshipModifiesByLineNum(10, "z");

        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipNext(5, 6);
        pkb.storeRelationshipNext(6, 7);
        pkb.storeRelationshipNext(7, 8);
        pkb.storeRelationshipNext(7, 9);
        pkb.storeRelationshipNext(8, 5);
        pkb.storeRelationshipNext(9, 5);
        pkb.storeRelationshipNext(5, 10);

        pkb.sortAllDataStructure();
        DECLARATION_TABLE dt = {{"a", DesignEntity::ASSIGN} };
        Declaration dec1 = Declaration(std::string("a"), Attribute::NONE);
        auto actual1 = affectsStarEval.evaluateAffectsStar(
            QueryArg(std::string("1"), QueryArgType::NUM), QueryArg(dec1), dt);
        Assert::IsTrue(actual1.getResultTable().getResultTable().empty());

        auto actual2 = affectsStarEval.evaluateAffectsStar(
            QueryArg(std::string("3"), QueryArgType::NUM), QueryArg(dec1), dt);
        RESULT_TABLE expected2{ {"6"}, {"8"}, {"9"}, {"10"} };
        Assert::IsTrue(actual2.getResultTable().getResultTable() == expected2);

        auto actual3 = affectsStarEval.evaluateAffectsStar(
            QueryArg(std::string("6"), QueryArgType::NUM), QueryArg(dec1), dt);
        RESULT_TABLE expected3{ {"8"}, {"9"}, {"6" }, {"10"} };
        Assert::IsTrue(actual3.getResultTable().getResultTable() == expected3);
    }

    TEST_METHOD(EvaluateAffectsStar_DeclarationAndDeclaration_Success) {
        /*
        w = 1 + 2;
        read w;
        x = w + 2;
        y = 1;
        while (w != x) {
            x = w + x;
            if (x != y) then {
                y = x + x;
            } else {
                y = x - w;
            }
        }
        z = y;
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeReadStatement(2, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeAssignStatement(3, "x", "+ w 2");
        pkb.storeRelationshipUsesByLineNum(3, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeAssignStatement(4, "y", "1");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeWhileStatement(5, { "w", "x" });
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "x");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.storeAssignStatement(6, "x", "+ w x");
        pkb.storeRelationshipUsesByLineNum(6, "x");
        pkb.storeRelationshipUsesByLineNum(6, "w");
        pkb.storeRelationshipModifiesByLineNum(6, "x");
        pkb.storeIfElseStatement(7, { "y", "x" });
        pkb.storeRelationshipUsesByLineNum(7, "y");
        pkb.storeRelationshipUsesByLineNum(7, "x");
        pkb.storeAssignStatement(8, "y", "+ x x");
        pkb.storeRelationshipUsesByLineNum(8, "x");
        pkb.storeRelationshipModifiesByLineNum(8, "y");
        pkb.storeAssignStatement(9, "y", "- x w");
        pkb.storeRelationshipUsesByLineNum(9, "x");
        pkb.storeRelationshipUsesByLineNum(9, "w");
        pkb.storeRelationshipModifiesByLineNum(9, "y");
        pkb.storeAssignStatement(10, "z", "y");
        pkb.storeRelationshipUsesByLineNum(10, "y");
        pkb.storeRelationshipModifiesByLineNum(10, "z");

        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipNext(5, 6);
        pkb.storeRelationshipNext(6, 7);
        pkb.storeRelationshipNext(7, 8);
        pkb.storeRelationshipNext(7, 9);
        pkb.storeRelationshipNext(8, 5);
        pkb.storeRelationshipNext(9, 5);
        pkb.storeRelationshipNext(5, 10);

        pkb.sortAllDataStructure();
        DECLARATION_TABLE dt = { {"a", DesignEntity::ASSIGN}, {"s", DesignEntity::STMT} };
        Declaration dec1 = Declaration(std::string("a"), Attribute::NONE);
        Declaration dec2 = Declaration(std::string("s"), Attribute::NONE);
        auto actual = affectsStarEval.evaluateAffectsStar(
            QueryArg(dec2), QueryArg(dec1), dt);
        RESULT_TABLE expected{ {"3", "6"}, {"3", "8"}, {"3", "9"}, {"3", "10"}, {"4", "10"},
            {"6", "8"}, {"6", "9"}, {"6", "6"}, {"6", "10"}, {"8", "10"}, {"9", "10"} };
        Assert::IsTrue(actual.getResultTable().getResultTable() == expected);
    }

    TEST_METHOD(EvaluateAffectsStar_DeclarationAndLineNumber_Success) {
        /*
        w = 1 + 2;
        read w;
        x = w + 2;
        y = 1;
        while (w != x) {
            x = w + x;
            if (x != y) then {
                y = x + x;
            } else {
                y = x - w;
            }
        }
        z = y;
        */
        pkb.storeAssignStatement(1, "w", "+ 1 2");
        pkb.storeRelationshipModifiesByLineNum(1, "w");
        pkb.storeReadStatement(2, "w");
        pkb.storeRelationshipModifiesByLineNum(2, "w");
        pkb.storeAssignStatement(3, "x", "+ w 2");
        pkb.storeRelationshipUsesByLineNum(3, "w");
        pkb.storeRelationshipModifiesByLineNum(3, "x");
        pkb.storeAssignStatement(4, "y", "1");
        pkb.storeRelationshipModifiesByLineNum(4, "y");
        pkb.storeWhileStatement(5, { "w", "x" });
        pkb.storeRelationshipUsesByLineNum(5, "w");
        pkb.storeRelationshipUsesByLineNum(5, "x");
        pkb.storeRelationshipUsesByLineNum(5, "y");
        pkb.storeAssignStatement(6, "x", "+ w x");
        pkb.storeRelationshipUsesByLineNum(6, "x");
        pkb.storeRelationshipUsesByLineNum(6, "w");
        pkb.storeRelationshipModifiesByLineNum(6, "x");
        pkb.storeIfElseStatement(7, { "y", "x" });
        pkb.storeRelationshipUsesByLineNum(7, "y");
        pkb.storeRelationshipUsesByLineNum(7, "x");
        pkb.storeAssignStatement(8, "y", "+ x x");
        pkb.storeRelationshipUsesByLineNum(8, "x");
        pkb.storeRelationshipModifiesByLineNum(8, "y");
        pkb.storeAssignStatement(9, "y", "- x w");
        pkb.storeRelationshipUsesByLineNum(9, "x");
        pkb.storeRelationshipUsesByLineNum(9, "w");
        pkb.storeRelationshipModifiesByLineNum(9, "y");
        pkb.storeAssignStatement(10, "z", "y");
        pkb.storeRelationshipUsesByLineNum(10, "y");
        pkb.storeRelationshipModifiesByLineNum(10, "z");

        pkb.storeRelationshipNext(1, 2);
        pkb.storeRelationshipNext(2, 3);
        pkb.storeRelationshipNext(3, 4);
        pkb.storeRelationshipNext(4, 5);
        pkb.storeRelationshipNext(5, 6);
        pkb.storeRelationshipNext(6, 7);
        pkb.storeRelationshipNext(7, 8);
        pkb.storeRelationshipNext(7, 9);
        pkb.storeRelationshipNext(8, 5);
        pkb.storeRelationshipNext(9, 5);
        pkb.storeRelationshipNext(5, 10);

        pkb.sortAllDataStructure();
        DECLARATION_TABLE dt = { {"a", DesignEntity::ASSIGN} };
        Declaration dec1 = Declaration(std::string("a"), Attribute::NONE);
        auto actual1 = affectsStarEval.evaluateAffectsStar(QueryArg(dec1),
            QueryArg(std::string("3"), QueryArgType::NUM), dt);
        Assert::IsTrue(actual1.getResultTable().getResultTable().empty());

        auto actual2 = affectsStarEval.evaluateAffectsStar(QueryArg(dec1),
            QueryArg(std::string("8"), QueryArgType::NUM), dt);
        RESULT_TABLE expected2{ {"6"}, {"3"} };
        Assert::IsTrue(actual2.getResultTable().getResultTable() == expected2);

        auto actual3 = affectsStarEval.evaluateAffectsStar(QueryArg(dec1),
            QueryArg(std::string("10"), QueryArgType::NUM), dt);
        RESULT_TABLE expected3{ {"4"}, {"8"}, {"9"}, {"6" }, {"3"} };
        Assert::IsTrue(actual3.getResultTable().getResultTable() == expected3);
    }
private:
    PKB pkb;
    AffectsStarEvaluator affectsStarEval = AffectsStarEvaluator(&pkb);
    };
}  // namespace UnitTesting
