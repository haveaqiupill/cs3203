#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include "PQLExceptions.h"
#include "QueryParserLookup.h"
#include "QueryParserUtility.h"
#include "Query/Query.h"
#include "Query/QueryDefinitions.h"

class QueryValidator {
 public:
    static void doInitialCheck(std::string& queryString);
    static bool isSyntaxValid(std::string& str, const std::regex& regex);
    static bool isAllWhitespace(std::string& str);

    static bool areSuchThatArgSyntaxValid(
        std::tuple<Relationship, std::string, std::string>& suchThatArgs);
    static bool isSuchThatLeftArgDesignEntValid(
        Relationship rs, DesignEntity designEnt);
    static bool isSuchThatRightArgDesignEntValid(Relationship rs, DesignEntity designEnt);
    static bool isValidPatternExpr(std::string str);
    static bool isFoundInDeclrTable(DECLARATION_TABLE& declrTable, SYNONYM& syn);
    static bool isFoundInDeclrTable(DECLARATION_TABLE& declrTable, DECLARATION& declr);
    static bool isValidAttributeOfDesignEntity(DesignEntity de, Attribute attr);
};
