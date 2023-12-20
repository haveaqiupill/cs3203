#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class AffectsBipEvaluator {
 private:
    PKB* pkb;
 public:
    explicit AffectsBipEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateAffectsBip(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
};
