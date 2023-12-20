#pragma once
#include <string>
#include <regex>
#include <tuple>
#include <utility>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "PQLExceptions.h"
#include "QueryParserLookup.h"

class QueryParserUtility {
 public:
    static std::pair<SelectionType, std::tuple<std::vector<std::string>, std::string>>
        extractSelect(std::string& stmt);
    static std::vector<std::string> tokenizeByComma(std::string& stmt);
    static std::vector<std::string> splitByDelimiter(std::string& str, std::string& delim);
    static std::string trimWhitespace(std::string& str);
    static std::vector<std::string> tokenizeByWhitespace(std::string& stmt);
    static std::unordered_map<ClauseType, std::vector<std::string>> splitClauses(std::string str);
    static std::vector<std::string> splitJointClausesByAnd(std::string str);
    static std::tuple<Relationship, std::string, std::string>
        extractSuchThatArguments(std::string& suchThat);
    static std::tuple<std::string, std::string> extractPatternSynonym(std::string& patternStmt);
    static std::tuple<std::string, std::string> extractPatternArguments(
        std::string& unparsedArgs, DesignEntity designEnt);
    static std::tuple<std::string, std::string> extractWithRefs(std::string& withStmt);
    static std::tuple<std::string, std::string> splitAttrRef(std::string& attrRef);
    static QueryArgType extractArgumentType(std::string& arg);
    static QueryArgType extractPatternTypeByWildcard(std::string& arg);
    static std::string removeQuotationMarks(std::string& str);
    static DECLARATION convertStringToDeclaration(std::string& str, QueryArgType declrType);
    static DECLARATION wrapSelectionInObject(std::string& str);
    static QueryArgType getDeclarationType(std::string& str);
    static void assignDefaultDeclarationAttribute(
        Declaration& declr, DECLARATION_TABLE& declrTable);
};
