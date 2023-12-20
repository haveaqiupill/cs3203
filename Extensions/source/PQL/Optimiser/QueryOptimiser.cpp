#include "QueryOptimiser.h"

#include "ToggleOptimisation.h"

Query QueryOptimiser::optimiseQuery(Query &queryObject) {
    if (!OPTIMISATION_ENABLED) {
        return queryObject;
    }

    WithOptimiser wOptimsier = WithOptimiser();
    Query optimisedWQuery = wOptimsier.replaceSynonyms(queryObject);

    GroupOptimiser gOptimiser = GroupOptimiser(pkb);
    Query optimisedQuery = gOptimiser.optimiseClauses(optimisedWQuery);

    return optimisedQuery;
}
