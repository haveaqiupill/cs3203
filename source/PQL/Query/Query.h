#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "QueryDefinitions.h"

class Query {
 public:
    Query();
    Query(
        DECLARATION_TABLE& declrTable, CLAUSE_LIST& clauseList, SELECTION_LIST& selectionList,
        WITH_CONDITION_LIST& withConditionList, SelectionType selectType);  // new representation
    // get methods
    CLAUSE_LIST getClauseList();
    DECLARATION_TABLE getDeclarations();
    SelectionType getSelectionType();
    SELECTION_LIST getSelectionList();
    WITH_CONDITION_LIST getWithConditionList();
    GROUPED_CLAUSES getGroupedClauses();
    // set methods (used by optimiser)
    void setGroupedClauses(GROUPED_CLAUSES& grouped);
    void setClauseList(CLAUSE_LIST& clList);
    void setWithConditionList(CLAUSE_LIST& withConditionList);

 private:
    // variables
    DECLARATION_TABLE declarations;
    CLAUSE_LIST clauseList;
    WITH_CONDITION_LIST withConditionList;
    SELECTION_LIST selectionList;  // new representation
    SelectionType selectionType;
    GROUPED_CLAUSES groupedClauses;  // optimisation
};
