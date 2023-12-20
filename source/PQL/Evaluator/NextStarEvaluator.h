#pragma once

#include <utility>
#include <vector>
#include <unordered_map>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class NextStarEvaluator {
 private:
    PKB* pkb;
    std::vector<LINE_NUM> nextStarBreadthFirstSearch(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> nextStarBreadthFirstSearch(DESIGN_ENTITY, LINE_NUM);
 public:
    explicit NextStarEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateNextStar(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
    bool doesLinesNextStar(LINE_NUM, LINE_NUM);
};
