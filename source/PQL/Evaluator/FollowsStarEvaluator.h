#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class FollowsStarEvaluator {
 private:
    PKB *pkb;
 public:
    explicit FollowsStarEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluateFollowsStar(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
