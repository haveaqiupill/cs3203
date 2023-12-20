#pragma once

#include <string>

#include "Query/Query.h"
#include "../PKB/PKB.h"
#include "Evaluator/EvaluatorUtility.h"
#include "Evaluator/Answers/IntermediateAnswer.h"
#include "Evaluator/Answers/ClauseAnswer.h"
#include "Evaluator/SuchThatEvaluator.h"
#include "Evaluator/PatternEvaluator.h"
#include "Evaluator/WithEvaluator.h"

class QueryEvaluator {
 private:
    SuchThatEvaluator suchThatEvaluator;
    PatternEvaluator patternEvaluator;
    WithEvaluator withEvaluator;
    PKB *pkb;
    ClauseAnswer evaluateClause(QueryClause& queryClause,
                                DECLARATION_TABLE& declarationTable);
 public:
    explicit QueryEvaluator(PKB *pkb) : suchThatEvaluator(SuchThatEvaluator(pkb)),
                                        patternEvaluator(PatternEvaluator(pkb)),
                                        withEvaluator(WithEvaluator(pkb)),
                                        pkb(pkb) {}
    IntermediateAnswer evaluateQueryObject(Query *queryObject);
    IntermediateAnswer evaluateOptimisedQuery(Query *query);
    static ResultTable getAllOfDesignEntity(Declaration selection,
                                            DesignEntity designEntity,
                                            PKB *pkb);
};
