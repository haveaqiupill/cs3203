#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class NextEvaluator {
 private:
    PKB* pkb;
 public:
    explicit NextEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateNext(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
