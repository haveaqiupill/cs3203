#include "OrderByGroupOptimiser.h"

#include "ToggleOptimisation.h"

Query OrderByGroupOptimiser::orderGroups(std::vector<int>& noSyns, std::vector<std::vector<int>>&
    groupings, std::vector<std::pair<double, int>>& cumulativeScores, Query& query) {
    if (!ORDER_BY_GRP_OPTIMISATION_ENABLED) {
        return query;
    }

    GROUPED_CLAUSES gc;
    CLAUSE_LIST clList = query.getClauseList();

    // Insert clauses with no synonyms first
    CLAUSE_LIST noSynsCl;
    for (auto i : noSyns) {
        noSynsCl.push_back(clList.at(i));
    }
    gc.push_back(noSynsCl);

    for (auto const& cs : cumulativeScores) {
        std::vector<int> grouping = groupings[cs.second];
        CLAUSE_LIST cl;
        for (auto i : grouping) {
            cl.push_back(clList.at(i));
        }
        gc.push_back(cl);
    }

    query.setGroupedClauses(gc);

    return query;
}
