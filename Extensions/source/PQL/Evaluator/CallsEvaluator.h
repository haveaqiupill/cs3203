#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class CallsEvaluator {
 private:
    PKB* pkb;
 public:
    explicit CallsEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateCalls(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
