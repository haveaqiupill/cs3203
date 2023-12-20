#pragma once

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class ModifiesEvaluator {
 private:
    PKB *pkb;
 public:
    explicit ModifiesEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluateModifies(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
