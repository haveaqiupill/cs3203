#pragma once
#include <string>
#include "SharedDefinitions.h"
#include "ClauseType.h"
#include "../../Commons/Relationship.h"
#include "../../Commons/DesignEntity.h"
#include "../PQLExceptions.h"
#include "QueryArg.h"

// this is an abstract class for QueryClause
// actual implementation is found inside SuchThatClause and PatternClause
typedef QueryArg QUERY_ARG;
typedef ClauseType CLAUSE_TYPE;
class QueryClause {
 public:
    QueryClause(CLAUSE_TYPE clType, QUERY_ARG &left, QUERY_ARG &right, Relationship r);
    QueryClause(CLAUSE_TYPE clType, QUERY_ARG &left, QUERY_ARG &right, SYNONYM& syn);
    QueryClause::QueryClause(CLAUSE_TYPE clType, QUERY_ARG &left, QUERY_ARG &right);
    // get methods
    CLAUSE_TYPE getClauseType();
    QUERY_ARG getLeftArg();
    QUERY_ARG getRightArg();
    Relationship getRelationship();
    SYNONYM getDesignEntitySynonym();
    void replaceLeftDeclrWithVal(LINE_NUM replacement);
    void replaceLeftDeclrWithVal(ARG_NAME& replacement);
    void replaceRightDeclrWithVal(LINE_NUM replacement);
    void replaceRightDeclrWithVal(ARG_NAME& replacement);
    void replaceLeftDeclrSynonym(SYNONYM& syn);
    void replaceRightDeclrSynonym(SYNONYM& syn);
 protected:
    CLAUSE_TYPE clauseType;
    QUERY_ARG leftArg;
    QUERY_ARG rightArg;
    Relationship rs;
    SYNONYM designEntSyn;
};
