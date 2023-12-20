#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class FollowsEvaluator {
 private:
    PKB *pkb;
 public:
    explicit FollowsEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluateFollows(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
