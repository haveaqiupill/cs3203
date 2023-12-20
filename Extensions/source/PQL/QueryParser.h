#pragma once
#include <stdio.h>
#include <string>
#include <tuple>
#include <sstream>
#include <exception>
#include <utility>
#include <regex>
#include <unordered_map>
#include <vector>
#include "QueryValidator.h"
#include "QueryParserLookup.h"
#include "QueryParserUtility.h"
#include "PQLExceptions.h"
#include "Query/QueryDefinitions.h"
#include "Query/Query.h"
#include "../Commons/Utilities.h"

class QueryParser {
 public:
    QueryParser();
    Query parseQuery(std::string& queryString);
    void addDeclaration(DECLARATION_TABLE& declrTable, std::vector<std::string>& declrTokens);
    void parseDeclarations(DECLARATION_TABLE& declrTable, std::vector<std::string>& statements);
    std::pair<SelectionType, std::tuple<std::vector<DECLARATION>, std::string>>
        parseSelectStatement(DECLARATION_TABLE& declrTable, std::string& selectStmt);
    SuchThatClause parseSuchThat(DECLARATION_TABLE& declrTable, std::string& suchThatStmt);
    PatternClause parsePattern(DECLARATION_TABLE& declrTable, std::string& patternStmt);
    WithClause parseWith(DECLARATION_TABLE& declrTable, std::string& withStmt);
};
