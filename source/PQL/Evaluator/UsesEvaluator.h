#pragma once

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class UsesEvaluator {
 private:
    PKB *pkb;
 public:
    explicit UsesEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluateUses(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
