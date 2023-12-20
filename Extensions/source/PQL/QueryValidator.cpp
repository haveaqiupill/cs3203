#include "QueryValidator.h"
/*
* check that the query string is not empty.
* check that it contains declaration and Select statements
*/
void QueryValidator::doInitialCheck(std::string& queryString) {
    if (queryString == "" || isAllWhitespace(queryString)) {  // empty query
        throw InvalidQuerySyntaxException();
    } else {
        std::regex toFind("\\bSelect\\b");
        std::smatch matches;

        std::regex_search(queryString, matches, toFind);

        // no select at all, throw exception
        if (matches.size() < 1) {
            throw InvalidQuerySyntaxException();
        }
    }
}

/*
* General method to check that a given string matches the given regex expression
*/
bool QueryValidator::isSyntaxValid(std::string& str, const std::regex& regex) {
    return std::regex_match(str, regex);
}

bool QueryValidator::areSuchThatArgSyntaxValid(
    std::tuple<Relationship, std::string, std::string>& suchThatArgs) {
    Relationship rs = std::get<0>(suchThatArgs);
    std::string argL = std::get<1>(suchThatArgs);
    std::string argR = std::get<2>(suchThatArgs);
    std::regex leftRegex = relationshipLeftArgLookup.at(rs);
    std::regex rightRegex = relationshipRightArgLookup.at(rs);

    // if both match the required syntax, return true
    return std::regex_match(argL, leftRegex) && std::regex_match(argR, rightRegex);
}

bool QueryValidator::isSuchThatLeftArgDesignEntValid(Relationship rs, DesignEntity designEnt) {
    std::unordered_set<DesignEntity> validDesignEntSet =
            relationshipLeftArgDesignEntLookup.at(rs);

    // if the set has anything specified,
    // need to check that the declaration is of specified type
    if (validDesignEntSet.size() > 0) {
        if (validDesignEntSet.find(designEnt) == validDesignEntSet.end()) {
            // the design entity is not in the set of valid entities
            return false;
        }
    }
    return true;
}

bool QueryValidator::isSuchThatRightArgDesignEntValid(Relationship rs,
    DesignEntity designEnt) {
      std::unordered_set<DesignEntity> validDesignEntSet =
            relationshipRightArgDesignEntLookup.at(rs);

    // if the set has anything specified,
    // need to check that the declaration is of specified type
    if (validDesignEntSet.size() > 0) {
        if (validDesignEntSet.find(designEnt) == validDesignEntSet.end()) {
            // the design entity is not in the set of valid entities
            return false;
        }
    }
    return true;
}


bool QueryValidator::isAllWhitespace(std::string& str) {
    for (size_t i = 0; i < str.length(); i++) {
        // the moment you find any other character, return false
        if (!std::isspace(str.at(i))) {
            return false;
        }
    }
    // if just a lot of whitespace, return true
    return true;
}

// stolen from ParserCommons
bool QueryValidator::isValidPatternExpr(std::string str) {
    str = std::regex_replace(str, std::regex("[1-9]\\d*"), "1");
    str = std::regex_replace(str, std::regex("0"), "1");
    str = std::regex_replace(str, std::regex("[A-Za-z][A-Za-z\\d]*"), "1");
    str = std::regex_replace(str, std::regex("[*/\\-%]"), "+");
    str = std::regex_replace(str, std::regex("\\s"), "");

    while (str.find("1+1") != std::string::npos || str.find("(1)") != std::string::npos) {
        str = std::regex_replace(str, std::regex("1\\+1"), "1");
        str = std::regex_replace(str, std::regex("\\(1\\)"), "1");
    }
    return str == "1";
}

bool QueryValidator::isFoundInDeclrTable(DECLARATION_TABLE& declrTable, SYNONYM& syn) {
    return declrTable.find(syn) != declrTable.end();
}

/*
* Checks that the synonym used has been declared in the declaration table
*/
bool QueryValidator::isFoundInDeclrTable(DECLARATION_TABLE& declrTable, DECLARATION& declr) {
    return declrTable.find(declr.getSynonym()) != declrTable.end();
}

bool QueryValidator::isValidAttributeOfDesignEntity(DesignEntity de, Attribute attr) {
    if (attr != Attribute::NONE &&
        designEntitiesWithNoAttrAllowed.find(de) != designEntitiesWithNoAttrAllowed.end()) {
         // not allowed to have attribute
        return false;
    } else {
        // check that the attribute is allowed for this design entity
        std::unordered_set<Attribute> found = designEntityAllowedAttributesLookup.at(de);
        return found.find(attr) != found.end();
    }
}

