#pragma once

#include <utility>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class AffectsBipStarEvaluator {
 private:
    PKB* pkb;
 public:
    explicit AffectsBipStarEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateAffectsBipStar(QueryArg& leftArg, QueryArg& rightArg,
        DECLARATION_TABLE& dt);
};
