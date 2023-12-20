#pragma once

#include <string>

#include "QueryResultProjector.h"
#include "../PKB/PKB.h"

/**
 * Manage queries by calling QueryParser, QueryEvaluator and QueryResultProjector.
 */
class QueryManager {
 public:
    static void parseAndEvaluateQuery(std::string& queryString, RESULT_LIST resultsList, PKB *pkb);
};
