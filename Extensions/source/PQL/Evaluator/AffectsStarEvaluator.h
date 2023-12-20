#pragma once

#include <vector>

#include "AffectsEvaluator.h"

class AffectsStarEvaluator : public AffectsEvaluator {
    // inheriting from AffectsEvaluator as a lot of the API calls are the same
 private:
    std::vector<LINE_NUM> affectsStarDfsLineAfter(LINE_NUM);
    std::vector<LINE_NUM> affectsStarDfsLineBefore(LINE_NUM);

 public:
    explicit AffectsStarEvaluator(PKB* pkb) : AffectsEvaluator(pkb) {}
    ClauseAnswer evaluateAffectsStar(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
    bool doLinesAffectsStar(LINE_NUM, LINE_NUM);
};
