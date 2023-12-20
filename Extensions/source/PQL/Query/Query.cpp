#include "Query.h"

Query::Query() {
}

Query::Query(
    DECLARATION_TABLE& declrTable, CLAUSE_LIST& clauseList, SELECTION_LIST& selectionList,
    WITH_CONDITION_LIST& withConditionList, SelectionType selectionType) {
    this->declarations = declrTable;
    this->clauseList = clauseList;
    this->selectionList = selectionList;
    this->withConditionList = withConditionList;
    this->selectionType = selectionType;
}

CLAUSE_LIST Query::getClauseList() {
    return clauseList;
}

DECLARATION_TABLE Query::getDeclarations() {
    return declarations;
}

SELECTION_LIST Query::getSelectionList() {
    return selectionList;
}

WITH_CONDITION_LIST Query::getWithConditionList() {
    return withConditionList;
}

SelectionType Query::getSelectionType() {
    return selectionType;
}

GROUPED_CLAUSES Query::getGroupedClauses() {
    return groupedClauses;
}

// used by QueryOptimiser
void Query::setGroupedClauses(GROUPED_CLAUSES& grouped) {
    this->groupedClauses = grouped;
}

void Query::setClauseList(CLAUSE_LIST& clList) {
    this->clauseList = clList; }

void Query::setWithConditionList(CLAUSE_LIST& withClList) {
    this->withConditionList = withClList;
}
