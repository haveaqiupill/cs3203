#include "QueryParserUtility.h"

std::pair<SelectionType, std::tuple<std::vector<std::string>, std::string>>
    QueryParserUtility::extractSelect(std::string& stmt) {
    std::smatch matches;
    std::vector<std::string> synonyms;
    std::string unparsedClauses;
    SelectionType selectType;
    if (std::regex_match(stmt, matches, selectStmtRegexSingle)) {
        if (matches.size() != 3) {
            throw InvalidSelectionSyntaxException();
        } else {
            synonyms.push_back(matches[1].str());
            unparsedClauses = matches[2].str();
            selectType = SelectionType::SINGLE;
        }
    } else if (std::regex_match(stmt, matches, selectStmtRegexTuple)) {
        if (matches.size() != 3) {
            throw InvalidSelectionSyntaxException();
        } else {
            std::string delim = ",";
            synonyms = QueryParserUtility::tokenizeByComma(matches[1].str());
            unparsedClauses = matches[matches.size() - 1].str();
            selectType = SelectionType::TUPLE;
        }
    } else {
        throw InvalidSelectionSyntaxException();
    }
    return std::make_pair(selectType, std::make_tuple(synonyms, unparsedClauses));
}

std::vector<std::string> QueryParserUtility::tokenizeByComma(
    std::string& stmt) {
    stmt = std::regex_replace(stmt, std::regex(","), " ");

    std::string buffer;  // create buffer string
    std::stringstream ss(stmt);  // insert stmt string into stream
    std::vector<std::string> tokens;  // vector containing parsed tokens

    while (ss >> buffer) {
        tokens.push_back(buffer);
    }
    return tokens;
}

// split clauses by such that and pattern
// for ITER 1, there is only maximum of 1 such that / pattern clause
// this will change for future iterations
// for now, just assume and split half half
std::unordered_map<ClauseType, std::vector<std::string>>
QueryParserUtility::splitClauses(std::string str) {
    str = trimWhitespace(str);
    std::unordered_map<ClauseType, std::vector<std::string>> result;
    std::vector<std::string> suchThatClauses;
    std::vector<std::string> patternClauses;
    std::vector<std::string> withClauses;

    std::smatch anyMatch;

    std::regex_search(str, anyMatch, std::regex("\\b(such\\s+that|pattern|with)\\b"));
    if (anyMatch.size() == 0) {
        return result;  // if no matches for such that or with or pattern
    }

    if (anyMatch.position(0) != 0) {
        return result;  // if does not start with such that or with or pattern
    }

    std::vector<std::string> res;  // intermediate results
    auto searchStart(str.cbegin());
    while (std::regex_search(searchStart, str.cend(), anyMatch,
        std::regex("\\b(such\\s+that|pattern|with)\\s+"))) {
        if (anyMatch.prefix() != "") {
            res.push_back(anyMatch.prefix());
        }
        res.push_back(anyMatch[0].str());
        if (std::regex_match(anyMatch.suffix().str(), std::regex("^\\s*$"))) {
            throw InvalidClauseException();
        }
        searchStart = anyMatch.suffix().first;
    }
    res.push_back(std::string(searchStart, str.cend()));

    // must have at least 2 results (clause type + arguments)
    // must have even number of results
    if (res.size() < 2 || res.size() % 2 != 0) {
        throw InvalidClauseException();
    }

    for (size_t i = 0; i < res.size(); i+=2) {
        std::vector<std::string> splitByAnd = splitJointClausesByAnd(res[i+1]);

        if (std::regex_match(res[i], std::regex("\\bsuch\\s+that\\s+"))) {;
            for (std::string str : splitByAnd) {
                suchThatClauses.push_back(str);
            }
        } else if (std::regex_match(res[i], std::regex("\\bpattern\\s+"))) {
            for (std::string str : splitByAnd) {
                patternClauses.push_back(str);
            }
        } else if (std::regex_match(res[i], std::regex("\\bwith\\s+"))) {
            for (std::string str : splitByAnd) {
                withClauses.push_back(str);
            }
        } else {  // does not match a clause
            return result;  // return empty result
        }
    }

    result[ClauseType::SUCHTHAT_CL] = suchThatClauses;
    result[ClauseType::PATTERN_CL] = patternClauses;
    result[ClauseType::WITH_CL] = withClauses;
    return result;
}

// helper method for splitClauses to split clauses of the same clause type,
// joined together by and
std::vector<std::string> QueryParserUtility::splitJointClausesByAnd(std::string str) {
    std::vector<std::string> res;
    // if there is no and, just return the string
    if (str.find("and") == std::string::npos) {
        res.push_back(str);
        return res;
    }

    std::smatch andMatch;
    auto searchStart(str.cbegin());
    while (std::regex_search(searchStart, str.cend(), andMatch, std::regex("\\s*and\\s+"))) {
        res.push_back(andMatch.prefix());
        searchStart = andMatch.suffix().first;
    }
    res.push_back(std::string(searchStart, str.cend()));
    return res;
}

// utility method to split the given string by the given delimiter
// used to split by semicolon
std::vector<std::string> QueryParserUtility::splitByDelimiter(
    std::string& str, std::string& delim) {
    std::size_t start = 0;  // position of start of substring
    std::size_t pos = 0;  // position of found delimiter
    pos = str.find(delim);
    std::vector<std::string> result;

    while (pos != std::string::npos) {
        result.push_back(str.substr(start, pos - start));
        start = pos + delim.length();
        pos = str.find(delim, start);
    }

    result.push_back(str.substr(start, pos - start));  // select statement

    return result;
}

// utility method to trim the front and trailing whitespace
// using regex
std::string QueryParserUtility::trimWhitespace(std::string& str) {
    std::string frontRegex = "^\\s+";
    std::string backRegex = "\\s+$";
    std::string backTrimmed = std::regex_replace(str, std::regex(backRegex), std::string(""));
    std::string result = std::regex_replace(backTrimmed, std::regex(frontRegex), std::string(""));
    return result;
}

  // split the given statement by whitespace
  // Note: multiple whitespaces will also be removed
  // eg. "assign      a   ;" will return "assign a;"
std::vector<std::string> QueryParserUtility::tokenizeByWhitespace(std::string& stmt) {
    std::string buffer;  // create buffer string
    std::stringstream ss(stmt);  // insert stmt string into stream
    std::vector<std::string> tokens;  // vector containing parsed tokens

    while (ss >> buffer) {
        tokens.push_back(buffer);
    }

    return tokens;
}


std::tuple<Relationship, std::string, std::string>
    QueryParserUtility::extractSuchThatArguments(std::string& suchThat) {
    std::smatch matches;
    std::regex regexExpr(suchThatRegex);
    std::regex_search(suchThat, matches, regexExpr);

    if (matches.size() != 4) {
        throw InvalidQuerySyntaxException();
    }

    // throw an error if unable to find the relationship from the enum table
    if (relationshipEnumLookup.find(matches[1].str()) == relationshipEnumLookup.end()) {
        throw InvalidSuchThatArgumentSyntaxException();
    }
    Relationship rs = relationshipEnumLookup.at(matches[1].str());
    std::string argL = matches[2].str();
    std::string argR = matches[3].str();
    return make_tuple(rs, argL, argR);
}

std::tuple<std::string, std::string> QueryParserUtility::extractPatternSynonym(
    std::string& patternStmt) {
    std::smatch matches;
    std::regex regexExpr(patternStmtRegex);

    std::regex_search(patternStmt, matches, regexExpr);

    if (matches.size() != 3) {  // should have a size of 3
        throw InvalidQuerySyntaxException();
    }

    return std::make_tuple<std::string, std::string>(matches[1].str(), matches[2].str());
}

std::tuple<std::string, std::string>
QueryParserUtility::extractPatternArguments(std::string& unparsedArgs, DesignEntity de) {
    std::smatch matches;
    std::regex_search(unparsedArgs, matches, patternArgLookup.at(de));

    if (de == DesignEntity::ASSIGN) {
        if (matches.size() != 3) {
            throw InvalidPatternArgumentSyntaxException();
        }
        return std::make_tuple<std::string, std::string>(matches[1].str(), matches[2].str());
    } else if (de == DesignEntity::IF) {
        if (matches.size() != 2) {
            throw InvalidPatternArgumentSyntaxException();
        }
        return std::make_tuple<std::string, std::string>(matches[1].str(), "");
    } else {
        if (matches.size() != 2) {
            throw InvalidPatternArgumentSyntaxException();
        }
        return std::make_tuple<std::string, std::string>(matches[1].str(), "");
    }
}

std::tuple<std::string, std::string> QueryParserUtility::extractWithRefs(
    std::string& withStmt) {
    std::smatch matches;
    std::regex_search(withStmt, matches, withStmtRegex);

    if (matches.size() != 3) {  // expect 3 matches
        throw InvalidWithSyntaxException();
    }

    return std::make_tuple<std::string, std::string>(matches[1].str(), matches[2].str());
}

std::tuple<std::string, std::string> QueryParserUtility::splitAttrRef(
    std::string& attrRef) {
    std::smatch matches;
    std::regex_search(attrRef, matches, attrRefRegex);

    if (matches.size() != 3) {  // expect 3 matches
        throw InvalidWithAttrRefException();
    }

    return std::make_tuple<std::string, std::string>(matches[1].str(), matches[2].str());
}


QueryArgType QueryParserUtility::extractArgumentType(std::string& arg) {
    if (arg == "_") {
        return QueryArgType::WILDCARD;
    } else if (std::regex_match(arg, intRegex)) {  // if an integer
        return QueryArgType::NUM;
    } else if (std::regex_match(arg, synRegex)) {
        return QueryArgType::DECLARATION;
    } else if (std::regex_match(arg, nameRegex)) {
        return QueryArgType::NAME;
    } else if (std::regex_match(arg, attrRefRegex)) {
        return QueryArgType::DECLARATION_WITH_ATTR;
    } else {
        // matches none of the types
        throw InvalidArgumentSyntaxException();
    }
}

QueryArgType QueryParserUtility::extractPatternTypeByWildcard(std::string& expr) {
    if (std::regex_match(expr, wildcardRegex)) {
        return QueryArgType::WILDCARD;
    } else if (std::regex_match(expr, patternExprWithWildcardsRegex)) {
        return QueryArgType::PATTERN_EXPR_WITH_WILDCARDS;
    } else if (std::regex_match(expr, patternExprRegex)) {
        return QueryArgType::PATTERN_EXPR;
    } else {
        throw InvalidPatternArgumentSyntaxException();
    }
}

std::string QueryParserUtility::removeQuotationMarks(std::string& str) {
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
    return str;
}

// a simplified version of extractArgumentType that only differentiates betweeen 2 types
// used when parsing selections
QueryArgType QueryParserUtility::getDeclarationType(std::string& str) {
    if (std::regex_match(str, synRegex)) {
        return QueryArgType::DECLARATION;
    } else if (std::regex_match(str, attrRefRegex)) {
        return QueryArgType::DECLARATION_WITH_ATTR;
    } else {
        throw InvalidSelectionSyntaxException();
    }
}

/*
* Utility method to convert a string to a DECLARATION object based on the QueryArgType (declaration / declaration with attr)
*/
DECLARATION QueryParserUtility::convertStringToDeclaration(
    std::string& str, QueryArgType declrType) {
    std::smatch matches;

    if (declrType == QueryArgType::DECLARATION) {  // declaration with no attribute
        return Declaration(str, Attribute::NONE);
    } else {  // declaration with attribute
        std::tuple<std::string, std::string> splitDeclrRef = QueryParserUtility::splitAttrRef(str);
        std::string syn = std::get<0>(splitDeclrRef);

        // get the parsed attribute
        Attribute attr = attributeEnumLookup.at(std::get<1>(splitDeclrRef));
        return Declaration(syn, attr);
    }
}

/*
* Used when converting selection string to DECLARATION objects
*/
DECLARATION QueryParserUtility::wrapSelectionInObject(std::string& str) {
    // check to see if declaration / declaration_with_attr
    QueryArgType declrType = QueryParserUtility::getDeclarationType(str);
    return QueryParserUtility::convertStringToDeclaration(str, declrType);
}

/*
* For pure synonyms (i.e. no attribute), need to assign the DEFAULT attribute.
* This default attribute is retrieved from the lookup table.
* This method is called after the synonym has been verified to belong to the declaration table.
*/
void QueryParserUtility::assignDefaultDeclarationAttribute(
    Declaration& declr, DECLARATION_TABLE& declrTable) {
    DesignEntity de = declrTable.at(declr.getSynonym());
    auto defaultAttrFound = defaultDesignEntityAttrLookup.find(de);
    if (defaultAttrFound == defaultDesignEntityAttrLookup.end()) {
        // QueryParserLookup does not contain the information for the default attribute
        throw MissingDataException();
    }
    // get this design entity's default attribute
    Attribute defaultAttr = defaultAttrFound->second;
    // assign this default attribute to the declaration
    declr.assignDefaultAttribute(defaultAttr);
}
