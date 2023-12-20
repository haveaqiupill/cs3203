#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class NextBipStarEvaluator {
 private:
    PKB* pkb;
 public:
    explicit NextBipStarEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateNextBipStar(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
