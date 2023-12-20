#pragma once

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"
#include "ParentEvaluator.h"
#include "ParentStarEvaluator.h"
#include "FollowsEvaluator.h"
#include "FollowsStarEvaluator.h"
#include "ModifiesEvaluator.h"
#include "UsesEvaluator.h"
#include "CallsEvaluator.h"
#include "CallsStarEvaluator.h"
#include "NextEvaluator.h"
#include "NextStarEvaluator.h"
#include "AffectsEvaluator.h"
#include "AffectsStarEvaluator.h"
#include "NextBipEvaluator.h"
#include "NextBipStarEvaluator.h"
#include "AffectsBipEvaluator.h"
#include "AffectsBipStarEvaluator.h"

class SuchThatEvaluator {
 private:
    PKB *pkb;
    ParentEvaluator parentEval;
    ParentStarEvaluator parentStarEval;
    FollowsEvaluator followsEval;
    FollowsStarEvaluator followsStarEval;
    ModifiesEvaluator modifiesEval;
    UsesEvaluator usesEval;
    CallsEvaluator callsEval;
    CallsStarEvaluator callsStarEval;
    NextEvaluator nextEval;
    NextStarEvaluator nextStarEval;
    AffectsEvaluator affectsEvaluator;
    AffectsStarEvaluator affectsStarEvaluator;
    NextBipEvaluator nextBipEval;
    NextBipStarEvaluator nextBipStarEval;
    AffectsBipEvaluator affectsBipEval;
    AffectsBipStarEvaluator affectsBipStarEval;

    ClauseAnswer evaluateBooleanRelationship(Relationship relationship,
                                             QueryArg& leftArg,
                                             QueryArg& rightArg);
    ClauseAnswer evaluateSynRelationship(Relationship relationship,
                                         QueryArg& leftArg,
                                         QueryArg& rightArg,
                                         DECLARATION_TABLE& dt);

 public:
    explicit SuchThatEvaluator(PKB *pkb) : pkb(pkb),
                                           parentEval(ParentEvaluator(pkb)),
                                           parentStarEval(ParentStarEvaluator(pkb)),
                                           followsEval(FollowsEvaluator(pkb)),
                                           followsStarEval(FollowsStarEvaluator(pkb)),
                                           modifiesEval(ModifiesEvaluator(pkb)),
                                           usesEval(UsesEvaluator(pkb)),
                                           callsEval(CallsEvaluator(pkb)),
                                           callsStarEval(CallsStarEvaluator(pkb)),
                                           nextEval(NextEvaluator(pkb)),
                                           nextStarEval(NextStarEvaluator(pkb)),
                                           affectsEvaluator(AffectsEvaluator(pkb)),
                                           affectsStarEvaluator(AffectsStarEvaluator(pkb)),
                                           nextBipEval(NextBipEvaluator(pkb)),
                                           nextBipStarEval(NextBipStarEvaluator(pkb)),
                                           affectsBipEval(AffectsBipEvaluator(pkb)),
                                           affectsBipStarEval(AffectsBipStarEvaluator(pkb)) {}
    ClauseAnswer evaluateSuchThatClause(Relationship relationship,
                                        QueryArg& leftArg,
                                        QueryArg& rightArg,
                                        DECLARATION_TABLE& declarationTable);
};
