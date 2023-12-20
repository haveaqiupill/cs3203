#pragma once

#include <list>
#include <vector>
#include <string>

#include "Evaluator/Answers/IntermediateAnswer.h"
#include "Evaluator/Answers/ClauseAnswer.h"
#include "Query/Query.h"

typedef std::vector<std::string> RAW_ANSWER;
typedef std::list<std::string>& RESULT_LIST;

class QueryResultProjector {
 public:
    QueryResultProjector() {}
    RAW_ANSWER returnResults(IntermediateAnswer intermediateAnswer);
    void formatResults(RAW_ANSWER unformattedResults, RESULT_LIST resultsList);
 private:
    RAW_ANSWER getBooleanSelectionResults(std::vector<ClauseAnswer> caList);
    RAW_ANSWER getOptimisedBooleanSelectionResults(std::vector<ClauseAnswer> caList);
    RAW_ANSWER getSingleSelectionResults(ResultTable result, DECLARATION dec);
    RAW_ANSWER getTupleSelectionResults(ResultTable result, std::vector<DECLARATION> dec);
};
