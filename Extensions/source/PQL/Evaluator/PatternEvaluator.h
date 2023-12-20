#pragma once

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"

class PatternEvaluator {
 private:
    PKB *pkb;
    ClauseAnswer evaluatePatternAssign(QueryArg leftArg,
                                       QueryArg rightArg,
                                       Declaration assignSyn);
    ClauseAnswer evaluatePatternIf(QueryArg leftArg, Declaration patternSyn);
    ClauseAnswer evaluatePatternWhile(QueryArg leftArg, Declaration patternSyn);
 public:
    explicit PatternEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluatePatternClause(QueryArg leftArg, QueryArg rightArg,
                                       Declaration patternSyn,
                                       DECLARATION_TABLE declarationTable);
};
