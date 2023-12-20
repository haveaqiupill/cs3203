#include "QueryManager.h"

#include "QueryParser.h"
#include "QueryEvaluator.h"
#include "Optimiser/QueryOptimiser.h"
#include "Evaluator/Answers/IntermediateAnswer.h"
#include "Optimiser/ToggleOptimisation.h"
#include "PQLExceptions.h"

void QueryManager::parseAndEvaluateQuery(std::string& queryString,
    RESULT_LIST resultsList,
    PKB* pkb) {
    // Creates query object
    QueryResultProjector qrp = QueryResultProjector();

    QueryParser qParser = QueryParser();
    Query queryObject;
    try {
        queryObject = qParser.parseQuery(queryString);
    } catch (BooleanException) {
        return qrp.formatResults({ "FALSE" }, resultsList);
    } catch (std::exception) {
        return qrp.formatResults(RAW_ANSWER(), resultsList);
    }

    // Optimise clause order
    QueryOptimiser qOptimiser = QueryOptimiser(pkb);
    queryObject = qOptimiser.optimiseQuery(queryObject);

    // Produces clause answers
    QueryEvaluator qe = QueryEvaluator(pkb);
    IntermediateAnswer intermediateAnswer;
    if (OPTIMISATION_ENABLED) {
        intermediateAnswer = qe.evaluateOptimisedQuery(&queryObject);
    } else {
        intermediateAnswer = qe.evaluateQueryObject(&queryObject);
    }

    // Combines, prints and adds result to file
    RAW_ANSWER rawAnswer = qrp.returnResults(intermediateAnswer);
    qrp.formatResults(rawAnswer, resultsList);
}
