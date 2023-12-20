#include "GroupOptimiser.h"
#include "ToggleOptimisation.h"

Query GroupOptimiser::optimiseClauses(Query& query) {
    CLAUSE_LIST clList = query.getClauseList();
    CLAUSE_LIST withConditionList = query.getWithConditionList();

    if (!GROUP_OPTIMISATION_ENABLED) {
        std::vector<CLAUSE_LIST> gc = { clList };
        query.setGroupedClauses(gc);
        return query;
    }


    if (withConditionList.size() > 0) {
        for (auto& withCl : withConditionList) {
            clList.push_back(withCl);
        }
    }

    // create union-find disjoint set
    UFDS ufds(clList.size());

    // mappings needed

    // maps index of clause (as represented in clauseList) to synonym(s) used
    // only contains indices of clauses that use synonyms
    std::unordered_map<int, std::unordered_set<SYNONYM>> indexToSynMap;

    // maps synonyms to the indices of the clauses that contain them
    std::unordered_map<SYNONYM, std::vector<int>> synToIndexMap;

    // stores the indices of clauses that do not contain synonyms
    std::vector<int> clausesWithNoSyn;

    // go through the clause list
    for (size_t i = 0; i < clList.size(); i++) {
        QueryClause cl = clList[i];

        // gather the synonyms used in this clause
        std::vector<SYNONYM> clSyns;
        if (cl.getClauseType() == ClauseType::SUCHTHAT_CL ||
            cl.getClauseType() == ClauseType::WITH_CL) {
            if (cl.getLeftArg().getArgType() == QueryArgType::DECLARATION) {
                clSyns.push_back(cl.getLeftArg().getDeclaration().getSynonym());
            }

            if (cl.getRightArg().getArgType() == QueryArgType::DECLARATION) {
               clSyns.push_back(cl.getRightArg().getDeclaration().getSynonym());
            }
        } else if (cl.getClauseType() == ClauseType::PATTERN_CL) {
            // only the left arg in pattern clause can contain a synonym
            if (cl.getLeftArg().getArgType() == QueryArgType::DECLARATION) {
                clSyns.push_back(cl.getLeftArg().getDeclaration().getSynonym());
            }
            // get the design entity synonym
            clSyns.push_back(cl.getDesignEntitySynonym());
        }

        // this clause does not use any synonyms
        if (clSyns.size() == 0) {
            clausesWithNoSyn.push_back(i);
            continue;
        }

        std::unordered_set<int> groupedClausesFound;  // existing groups these syns belong to

        // go through the synonyms
        for (auto& syn : clSyns) {
            if (synToIndexMap.find(syn) != synToIndexMap.end()) {
                // this synonym belongs to an existing group
                int groupedClause = synToIndexMap.at(syn)[0];  // index of a clause in that group
                groupedClausesFound.insert(groupedClause);
            }
            synToIndexMap[syn].push_back(i);  // add index of this clause into synToIndexMap
            indexToSynMap[i].insert(syn);  // track this synonym as used by this index
        }

        // union the groups
        for (int groupedCl : groupedClausesFound) {
            ufds.unionSet(groupedCl, i);
        }
    }

    // do post-processing
    int numGroups = ufds.getNumDisjointSets() - clausesWithNoSyn.size();
    std::vector<std::vector<int>> groups(numGroups);  // result passed to other optimisers
    std::vector<int> intermediateGrouping(clList.size(), -1);  // remaps ufds sets into grps
    int currIntermediateGroup = 0;
    for (auto& element : indexToSynMap) {
        int group = ufds.findSet(element.first);
        int remappedGroup = intermediateGrouping[group];
        if (remappedGroup == -1) {
            remappedGroup = currIntermediateGroup;
            intermediateGrouping[group] = currIntermediateGroup;
            currIntermediateGroup++;
        }
        groups[remappedGroup].push_back(element.first);
    }

    // Store:
    // 1. sequence
    // 2. clause groups
    // 3. clauses with no synonyms
    query.setClauseList(clList);
    Query optimisedQuery = owgOpt.orderWithinGroups(clausesWithNoSyn, groups, query);

    return optimisedQuery;
}

