#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "../../PKB/PKB.h"
#include "../Query/Query.h"
#include "OrderByGroupOptimiser.h"

class OrderWithinGroupOptimiser {
 private:
    PKB* pkb;
    OrderByGroupOptimiser obgOpt;
    std::unordered_map<Relationship, double> RELATIONSHIPS_LOOKUP;
    std::unordered_map<DesignEntity, double> DESIGN_ENTITY_LOOKUP;
    bool isInitialised;

 public:
    explicit OrderWithinGroupOptimiser(PKB* pkb) : pkb(pkb),
        obgOpt(OrderByGroupOptimiser(pkb)), isInitialised(false) {}
    Query orderWithinGroups(std::vector<int>& noSyns,
        std::vector<std::vector<int>>& groupings, Query& query);
    double calculateScore(QueryClause& qc, DECLARATION_TABLE& dt);
    double calculateDeScore(QueryClause& qc, DECLARATION_TABLE& dt);
    double calculateNumSynonyms(QueryClause& qc, DECLARATION_TABLE& dt);
    void initialiseLookupTable();
    void checkOverlappingSynonyms(std::vector<int>& affectsQueue,
        std::unordered_set<SYNONYM>& computedSyns, std::unordered_set<int>& visitedClauses,
        std::vector<int>& optimisedOrder, CLAUSE_LIST& clList, std::pair<int, double>& p);
};
