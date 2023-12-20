#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class NextBipEvaluator {
 private:
    PKB* pkb;
 public:
    explicit NextBipEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateNextBip(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
