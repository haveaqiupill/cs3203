#pragma once

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class ParentStarEvaluator {
 private:
    PKB *pkb;
 public:
    explicit ParentStarEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluateParentStar(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
