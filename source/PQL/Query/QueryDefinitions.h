#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "QueryClause.h"
#include "SuchThatClause.h"
#include "PatternClause.h"
#include "WithClause.h"
#include "QueryArg.h"
#include "ClauseType.h"
#include "QueryArgType.h"
#include "SelectionType.h"
#include "SharedDefinitions.h"
#include "../../Commons/DesignEntity.h"
#include "../../Commons/Relationship.h"

/*
* definitions of components of Query object
* for use by QueryParser and QueryEvaluator
*/
typedef std::vector<QueryClause> CLAUSE_LIST;
typedef std::vector<CLAUSE_LIST> GROUPED_CLAUSES;
typedef std::vector<DECLARATION> SELECTION_LIST;
typedef std::vector<QueryClause> WITH_CONDITION_LIST;
typedef DesignEntity DESIGN_ENTITY;
typedef std::unordered_map<SYNONYM, DesignEntity> DECLARATION_TABLE;
