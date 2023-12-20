#pragma once

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class ParentEvaluator {
 private:
    PKB *pkb;
 public:
    explicit ParentEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluateParent(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
