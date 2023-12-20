#include "stdafx.h"
#include "CppUnitTest.h"
#include "PQL/Evaluator/SuchThatEvaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestSuchThatEvaluator) {
    public:
        TEST_METHOD(EvaluateBooleanRelationship_FollowsRelationship_Success) {
            pkb.propagateStmtList(2);
            pkb.storeRelationshipFollows(1, 2);
            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT} };
            QueryArg q1 = QueryArg((std::string)"1", QueryArgType::NUM);
            ClauseAnswer actual = stevaluator
                                  .evaluateSuchThatClause(Relationship::FOLLOWS,
                                                          q1,
                                                          QueryArg((std::string)"2",
                                                            QueryArgType::NUM),
                                                          dc);
            Assert::IsTrue(actual.getIsClauseTrue());
        }
        TEST_METHOD(EvaluateBooleanRelationship_ParentRelationship_Success) {
            pkb.propagateStmtList(2);
            pkb.storeRelationshipParent(1, 2);
            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT} };
            ClauseAnswer actual = stevaluator
                                  .evaluateSuchThatClause(Relationship::PARENT,
                                                          QueryArg((std::string)"1",
                                                            QueryArgType::NUM),
                                                          QueryArg((std::string)"2",
                                                            QueryArgType::NUM),
                                                          dc);
            Assert::IsTrue(actual.getIsClauseTrue());
        }

        TEST_METHOD(EvaluateBooleanRelationship_UsesRelationship_Success) {
            pkb.propagateStmtList(1);
            pkb.storeVariable("hello");
            pkb.storeRelationshipUsesByLineNum(1, "hello");
            DECLARATION_TABLE dc = { {"s", DesignEntity::STMT} };
            ClauseAnswer actual = stevaluator
                                  .evaluateSuchThatClause(Relationship::USES,
                                                          QueryArg((std::string)"1",
                                                            QueryArgType::NUM),
                                                          QueryArg((std::string)"hello",
                                                            QueryArgType::NAME),
                                                          dc);
            Assert::IsTrue(actual.getIsClauseTrue());
        }

    private:
        PKB pkb;
        PKB *ptr = &pkb;
        SuchThatEvaluator stevaluator = SuchThatEvaluator(ptr);
    };
}  // namespace UnitTesting
