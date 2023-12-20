#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class CallsStarEvaluator {
 private:
    PKB* pkb;
 public:
    explicit CallsStarEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateCallsStar(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
