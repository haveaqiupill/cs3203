#include "QueryParser.h"

// constructor for QueryParser
QueryParser::QueryParser() {
}

/* the only public api call provided
* called by the QueryManager object
* to parse the given query string.*/
Query QueryParser::parseQuery(std::string& queryString) {
    // ensure that there is a 'Select' and declaration statements
    QueryValidator::doInitialCheck(queryString);

    // split the statements by semicolon
    std::string delim = ";";
    std::vector<std::string> statements = QueryParserUtility::splitByDelimiter(queryString, delim);

    // create variables required for Query object
    DECLARATION_TABLE declarationTable;
    CLAUSE_LIST clauseList;
    SELECTION_LIST selectionList;
    WITH_CONDITION_LIST withConditionList;
    SYNONYM selection;
    SelectionType selectionType;

    // validate declarations
    if (statements.size() > 1) {  // has declarations
        parseDeclarations(declarationTable, statements);
    }

    // the last line of the query is the select statement
    std::string selectStmt = statements[statements.size() - 1];
    std::pair<SelectionType, std::tuple<std::vector<DECLARATION>, std::string>>
        parsedSelect = parseSelectStatement(declarationTable, selectStmt);

    selectionType = parsedSelect.first;
    std::tuple<std::vector<DECLARATION>, std::string>
        splitSelectionAndClauses = parsedSelect.second;
    selectionList = std::get<0>(splitSelectionAndClauses);

    std::string unparsedClauses = std::get<1>(splitSelectionAndClauses);

    // there are no clauses, only a selection
    // job is done, create and return query object
    if (unparsedClauses == "" || QueryValidator::isAllWhitespace(unparsedClauses)) {
        return Query(declarationTable, clauseList, selectionList, withConditionList, selectionType);
    }

    // try catch block: from here on out, catch errors
    // and throw BooleanException if selectiontype is BOOLEAN
    // we assume that the test cases used in project evaluation are syntatically valid
    // hence QueryManager is to return false as an answer
    try {
        // if there are clauses
        // split the clauses and separate them into the type of clause
        std::unordered_map<ClauseType, std::vector<std::string>> splitClauses =
            QueryParserUtility::splitClauses(unparsedClauses);


        // stmt contains invalid clauses
        if (splitClauses.size() == 0) {
            throw InvalidClauseException();
        }

        // validate the clauses

        // validate such that clause
        std::vector<std::string> suchThatClauses =
            splitClauses.find(ClauseType::SUCHTHAT_CL)->second;
        for (size_t i = 0; i < suchThatClauses.size(); i++) {
            SuchThatClause suchThatClause = parseSuchThat(declarationTable, suchThatClauses[i]);
            clauseList.push_back(suchThatClause);
        }

        // validate pattern clause
        std::vector<std::string> patternClauses = splitClauses.find(ClauseType::PATTERN_CL)->second;
        for (size_t i = 0; i < patternClauses.size(); i++) {
        PatternClause patternClause =
            parsePattern(declarationTable, patternClauses[i]);
            clauseList.push_back(patternClause);
        }

        // validate with clauses
        std::vector<std::string> withClauses = splitClauses.find(ClauseType::WITH_CL)->second;
        for (size_t i = 0; i < withClauses.size(); i++) {
            WithClause withClause = parseWith(declarationTable, withClauses[i]);
            withConditionList.push_back(withClause);
        }
        return Query(declarationTable, clauseList, selectionList, withConditionList, selectionType);
    } catch (SemanticErrorException) {
        if (selectionType == SelectionType::BOOLEAN) {  // result is FALSE only if semantic error
            throw BooleanException();
        } else {
            throw;
        }
    } catch (SyntaxErrorException) {
        throw;
    }
}


/*
* A declaration is defined as each statement starting from design entity to the semicolon
* i.e. assign a, b, c; is considered one declaration
*/
void QueryParser::parseDeclarations(DECLARATION_TABLE& declrTable,
    std::vector<std::string>& statements) {
    // validate declarations
    for (size_t i = 0; i < statements.size() - 1; i++) {
        // check declaration syntax using the validator
        if (!QueryValidator::isSyntaxValid(statements[i], declarationRegex)) {
            throw InvalidDeclarationException();
        }
    }

    // if passes validation check
    // split into tokens (removing whitespace and commas)
    for (size_t i = 0; i < statements.size() - 1; i++) {
        std::vector<std::string> tokenizedDeclr =
            QueryParserUtility::tokenizeByComma(statements[i]);
        // attempt to add declaration to declaration table
        // if there already exists a synonym of the same name, throw an error
        if (tokenizedDeclr.size() < 2) {
            throw InvalidDeclarationException();
        } else {
            addDeclaration(declrTable, tokenizedDeclr);
        }
    }
}

/*
* adds a declaration to the declaration table in the form of <declaration name,  design entity>
* throws an ERROR if the declaration name already exists
*/
void QueryParser::addDeclaration(DECLARATION_TABLE& declrTable,
    std::vector<std::string>& declrTokens) {
    // check the design entity lookup table defined in QueryParserLookup.h
    auto found = declrEnumLookup.find(declrTokens[0]);

    // get the enums representation of the design entity
    DESIGN_ENTITY entity = found->second;

    // iterate through the declarations and assign them to this design entity
    for (size_t i = 1; i < declrTokens.size(); i++) {
        if (declrTokens[i] == ",") {  // if it's just a comma, skip
            continue;
        } else if (restrictedNames.find(declrTokens[i]) != restrictedNames.end()) {
            // declaration name is invalid
            // restrictedNames is defined in QueryParserLookup.h
            throw InvalidDeclarationException();
        } else if (declrTable.find(declrTokens[i]) != declrTable.end()) {
            // synonym is already in the declaration table
            // duplicate synonyms are not allowed
            throw DuplicateDeclarationException();
        } else {
            // add declaration to declaration table
            declrTable[declrTokens[i]] = entity;
        }
    }
}

/*
* Takes in the select statement (the second line of the query) and checks that the selection portion is valid.
* Create Declaration objects from the selections
* Return a tuple containing a vector of DECLARATION objects and a string of unparsed clauses.
*/
std::pair<SelectionType, std::tuple<std::vector<DECLARATION>, std::string>>
    QueryParser::parseSelectStatement(DECLARATION_TABLE& declrTable, std::string& selectStmt) {
    // if is a boolean selection, no need to check synonyms
    // return an empty vector (no synonyms)
    // split into selection and clauses
    std::pair<SelectionType, std::tuple<std::vector<std::string>, std::string>> res =
        QueryParserUtility::extractSelect(selectStmt);

    SelectionType selectType = res.first;
    std::tuple<std::vector<std::string>, std::string> extracted = res.second;

    std::vector<std::string> selectionsUnparsed = std::get<0>(extracted);
    std::string unparsedClauses = std::get<1>(extracted);
    std::vector<DECLARATION> selectionList;

    // check if select BOOLEAN clause
    if (selectionsUnparsed.size() == 1) {
        if (selectionsUnparsed[0] == "BOOLEAN"
            && !QueryValidator::isFoundInDeclrTable(declrTable, selectionsUnparsed[0])
            && selectType != SelectionType::TUPLE) {
            // "BOOLEAN" is not declared as a synonym
            // is a select BOOLEAN clause
            // do not treat select <BOOLEAN> as a select BOOLEAN clause
            return std::make_pair(SelectionType::BOOLEAN,
                std::make_tuple(selectionList, unparsedClauses));
        }
    }

    for (size_t i = 0; i < selectionsUnparsed.size(); i++) {
        // convert selection string to DECLARATION object
        DECLARATION declr = QueryParserUtility::wrapSelectionInObject(selectionsUnparsed[i]);

        // check that it is contained in the declaration table
        if (!QueryValidator::isFoundInDeclrTable(declrTable, declr)) {
            throw MissingDeclarationException();
        } else {
            // check that the attribute reference, if any, is correctly used
            DesignEntity de = declrTable.at(declr.getSynonym());
            Attribute attr = declr.getAttribute();
            if (attr == Attribute::NONE) {  // if has no attribute, assign default attribute
              QueryParserUtility::assignDefaultDeclarationAttribute(declr, declrTable);
            } else if (!QueryValidator::isValidAttributeOfDesignEntity(de, attr)) {
                // if has attribute, check that it is valid for the design entity
                throw InvalidSelectionAttrRefException();
            }
        }

        // if passes, add it to selectionList
        selectionList.push_back(declr);
    }

    if (selectType == SelectionType::TUPLE && selectionList.size() == 1) {
        selectType = SelectionType::SINGLE;
    }

    return std::make_pair(selectType, std::make_tuple(selectionList, unparsedClauses));
}


/*
* Checks that the such that statement is of valid syntax.
* Extract the such that arguments (relationship, leftArg, rightArg).
* Checks that the arguments are in accordance with the relationship's definitions.
* Ensures that any synonyms used are already declared in the declaration table.
* Returns the SuchThatClause.
* If any of the above validations fail, throw an error.
*/
SuchThatClause QueryParser::parseSuchThat(
DECLARATION_TABLE& declrTable, std::string& suchThatStmt) {
    // check that syntax of such that statement is valid
    /*
    if (!QueryValidator::isSyntaxValid(suchThatStmt, suchThatRegex)) {
        throw InvalidQuerySyntaxException();
    } */

    // checks that general syntax of such that statement is valid
    // extracts the relationship and left and right args
    std::tuple<Relationship, std::string, std::string> suchThatArgs
        = QueryParserUtility::extractSuchThatArguments(suchThatStmt);

    // check that the left and right args have valid syntax according to the relationship
    if (!QueryValidator::areSuchThatArgSyntaxValid(suchThatArgs)) {
        throw InvalidSuchThatArgumentSyntaxException();
    }

    Relationship rs = std::get<0>(suchThatArgs);
    std::string argL = std::get<1>(suchThatArgs);
    std::string argR = std::get<2>(suchThatArgs);

    // check that none of the arguments are blank
    if (QueryValidator::isAllWhitespace(argL) ||
        QueryValidator::isAllWhitespace(argR)) {
        throw InvalidQuerySyntaxException();
    }

    // get the arg type in enum form
    QueryArgType argTypeL = QueryParserUtility::extractArgumentType(argL);
    QueryArgType argTypeR = QueryParserUtility::extractArgumentType(argR);

    // for declarations
    DECLARATION declrL;
    DECLARATION declrR;

    // check that declarations can be found in the declaration table
    if (argTypeL == QueryArgType::DECLARATION) {
        // must already exist in the declaration table
        if (!QueryValidator::isFoundInDeclrTable(declrTable, argL)) {
            throw MissingDeclarationException();
        } else if (
            !QueryValidator::isSuchThatLeftArgDesignEntValid(
                rs, declrTable.at(argL))) {
            // check that the design entity is valid for the relationship given
            throw InvalidSuchThatDesignEntityException();
        }
        declrL = QueryParserUtility::convertStringToDeclaration(argL, argTypeL);
        QueryParserUtility::assignDefaultDeclarationAttribute(declrL, declrTable);
    } else if (argTypeL == QueryArgType::NAME) {
        // remove quotation marks for names
        argL = QueryParserUtility::removeQuotationMarks(argL);
        argL = QueryParserUtility::trimWhitespace(argL);
    }

    if (argTypeR == QueryArgType::DECLARATION) {
        // must already exist in the declaration table
        if (!QueryValidator::isFoundInDeclrTable(declrTable, argR)) {
            throw MissingDeclarationException();
        } else if (!QueryValidator::isSuchThatRightArgDesignEntValid(
                        rs, declrTable.at(argR))) {
            // check that the design entity is valid for the relationship given
            throw InvalidSuchThatDesignEntityException();
        }
        declrR = QueryParserUtility::convertStringToDeclaration(argR, argTypeR);
        QueryParserUtility::assignDefaultDeclarationAttribute(declrR, declrTable);
    } else if (argTypeR == QueryArgType::NAME) {
        // remove quotation marks for names
        argR = QueryParserUtility::removeQuotationMarks(argR);
        argR = QueryParserUtility::trimWhitespace(argR);
    }

    QueryArg argObjL;
    QueryArg argObjR;
    // if everything passes
    // create the query args
    if (argTypeL == QueryArgType::DECLARATION) {
        argObjL = QueryArg(declrL);
    } else {
        argObjL = QueryArg(argL, argTypeL);
    }

    if (argTypeR == QueryArgType::DECLARATION) {
      argObjR = QueryArg(declrR);
    } else {
        argObjR = QueryArg(argR, argTypeR);
    }

    // create the Such That Clause object
    return SuchThatClause(rs, argObjL, argObjR);
}


PatternClause QueryParser::parsePattern(DECLARATION_TABLE& declrTable,
                                        std::string& patternStmt) {
    // extract pattern synonym and the arguments within the brackets
    std::tuple<std::string, std::string> splitSynonymAndArgs =
        QueryParserUtility::extractPatternSynonym(patternStmt);

    // check that the pattern arguments have valid syntax
    SYNONYM syn = std::get<0>(splitSynonymAndArgs);
    std::string unparsedArgs = std::get<1>(splitSynonymAndArgs);


    // synonym must be declared beforehand
    // if declared, retrieve the design entity
    if (declrTable.find(syn) == declrTable.end()) {
        throw MissingDeclarationException();
    }

    DesignEntity synDesignEnt = declrTable.at(syn);

    std::regex argRegex;
    // check the design entity
    if (patternArgLookup.find(synDesignEnt) == patternArgLookup.end()) {
        throw InvalidPatternDesignEntityException();
    } else {
        argRegex = patternArgLookup.at(synDesignEnt);
    }

    // validate the argument syntax
    if (!QueryValidator::isSyntaxValid(unparsedArgs, argRegex)) {
        throw InvalidPatternArgumentSyntaxException();
    }


    // extract the arguments
    std::tuple<std::string, std::string> extractedArgs =
        QueryParserUtility::extractPatternArguments(unparsedArgs, synDesignEnt);

    // the leftmost argument
    std::string argL = std::get<0>(extractedArgs);
    // for assign-patterns, check the pattern expression
    // cannot have empty argument
    if (QueryValidator ::isAllWhitespace(argL)) {
        throw InvalidPatternArgumentSyntaxException();
    }

    // for declarations
    DECLARATION declrL;
    DECLARATION declrR;

    // process the leftmost argument first
    QueryArgType argTypeL = QueryParserUtility::extractArgumentType(argL);
    // if declaration, ensure that it is in declaration table
    if (argTypeL == QueryArgType::DECLARATION) {
        if (!QueryValidator::isFoundInDeclrTable(declrTable, argL)) {
            // unable to find in declaration table
            throw MissingDeclarationException();
        } else if (declrTable.at(argL) != DesignEntity::VARIABLE) {
            // LHS argument must be of type variable
            // if it is a declaration
            throw InvalidPatternDesignEntityException();
        }
    } else if (argTypeL == QueryArgType::NAME) {
        argL = QueryParserUtility::removeQuotationMarks(argL);
        argL = QueryParserUtility::trimWhitespace(argL);
    }

    QueryArg argObjL;

    // if everything passes
    // create the query args
    if (argTypeL == QueryArgType::DECLARATION) {
        DECLARATION declrL = QueryParserUtility::convertStringToDeclaration(argL, argTypeL);
        QueryParserUtility::assignDefaultDeclarationAttribute(declrL, declrTable);
        argObjL = QueryArg(declrL);
    } else {
        argObjL = QueryArg(argL, argTypeL);
    }

    // proceed to create pattern clause if it is if-pattern or while-pattern
    if (synDesignEnt == DesignEntity::IF ||
        synDesignEnt == DesignEntity::WHILE) {
        QueryArg argObjR((std::string) "", QueryArgType::WILDCARD);
        return PatternClause(syn, argObjL, argObjR);
    }

    // process the right argument if it is a pattern-assign
    std::string argR = std::get<1>(extractedArgs);
    // cannot have empty argument
    if (QueryValidator ::isAllWhitespace(argR)) {
        throw InvalidPatternArgumentSyntaxException();
    }
    QueryArgType argTypeR = QueryParserUtility::extractPatternTypeByWildcard(argR);

    // validate expr and convert to prefix expr
    if (argTypeR == QueryArgType::PATTERN_EXPR ||
        argTypeR == QueryArgType::PATTERN_EXPR_WITH_WILDCARDS) {
        argR = std::regex_replace(argR, std::regex("\""), "");
        if (argTypeR == QueryArgType::PATTERN_EXPR_WITH_WILDCARDS) {
            argR = std::regex_replace(argR, std::regex("_"), "");
        }
        if (QueryValidator::isValidPatternExpr(argR)) {
            argR = Utilities::makePrefixArithExpr(argR);
        } else {
            throw InvalidPatternArgumentSyntaxException();
        }
    }

    QueryArg argObjR(argR, argTypeR);

    return PatternClause(syn, argObjL, argObjR);
}

WithClause QueryParser::parseWith(DECLARATION_TABLE& declrTable,
    std::string& withStmt) {
    /*
    if (!QueryValidator::isSyntaxValid(withStmt, withStmtRegex)) {
        throw InvalidWithSyntaxException();
    } */

    std::tuple<std::string, std::string> extractedRefs =
        QueryParserUtility::extractWithRefs(withStmt);

    // check ref argument types
    std::string refL = std::get<0>(extractedRefs);
    std::string refR = std::get<1>(extractedRefs);

    QueryArgType argTypeL = QueryParserUtility::extractArgumentType(refL);
    QueryArgType argTypeR = QueryParserUtility::extractArgumentType(refR);

    // do a check for trivial comparisons
    // (eg. 2 numbers or 2 strings, and throw an error if they are false comparisons)
    if (argTypeL == argTypeR &&
        (argTypeL == QueryArgType::NUM || argTypeL == QueryArgType::NAME)) {
        if (refL != refR) {
            throw InvalidWithComparisonException();
        }
    }

    Declaration declrL;
    Declaration declrR;
    // type group: 0, 1
    // 0: number
    // 1: string
    int typeGroupL;
    int typeGroupR;

    // process the left argument
    if (argTypeL == QueryArgType::DECLARATION) {
        // synonyms must be of type PROG_LINE
        if (!QueryValidator::isFoundInDeclrTable(declrTable, refL)) {
            throw MissingDeclarationException();
        } else if (declrTable.at(refL) != DesignEntity::PROG_LINE) {
            // design entity must be PROG_LINE
            throw InvalidWithDesignEntityException();
        }
        typeGroupL = 0;  // of group 0
        declrL = QueryParserUtility::convertStringToDeclaration(refL, argTypeL);
        QueryParserUtility::assignDefaultDeclarationAttribute(declrL, declrTable);
    } else if (argTypeL == QueryArgType::DECLARATION_WITH_ATTR) {
        // convert to declaration first in order to split the attrRef
        declrL = QueryParserUtility::convertStringToDeclaration(refL, argTypeL);

        // ensure that synonym is already declared
        if (!QueryValidator::isFoundInDeclrTable(declrTable, declrL)) {
            throw MissingDeclarationException();
        } else {
            DesignEntity de = declrTable.at(declrL.getSynonym());
            Attribute attr = declrL.getAttribute();

            // ensure that attribute ref used correctly
            if (!QueryValidator::isValidAttributeOfDesignEntity(de, attr)) {
                throw InvalidWithAttrRefException();
            }

            // get the type group of this attribute
            typeGroupL = attributeTypeGroupLookup.at(attr);
        }
    } else {  // other query arg type (eg. line num, name)
        // check if it is a valid arg type for with clause
        if (queryArgTypeGroupLookup.find(argTypeL) ==
          queryArgTypeGroupLookup.end()) {
            throw InvalidWithArgTypeException();
        }

        typeGroupL = queryArgTypeGroupLookup.at(argTypeL);
    }

    // process the right argument
    if (argTypeR == QueryArgType::DECLARATION) {
        // synonyms must be of type PROG_LINE
        if (!QueryValidator::isFoundInDeclrTable(declrTable, refR)) {
            throw MissingDeclarationException();
        } else if (declrTable.at(refR) != DesignEntity::PROG_LINE) {
            // design entity must be PROG_LINE
            throw InvalidWithDesignEntityException();
        }
        typeGroupR = 0;
        declrR = QueryParserUtility::convertStringToDeclaration(refR, argTypeR);
        QueryParserUtility::assignDefaultDeclarationAttribute(declrR, declrTable);
    } else if (argTypeR == QueryArgType::DECLARATION_WITH_ATTR) {
        declrR = QueryParserUtility::convertStringToDeclaration(refR, argTypeR);

        // ensure that synonym is already declared
        if (!QueryValidator::isFoundInDeclrTable(declrTable, declrR)) {
            throw MissingDeclarationException();
        } else {
            DesignEntity de = declrTable.at(declrR.getSynonym());
            Attribute attr = declrR.getAttribute();
            // ensure that attribute ref used correctly
            if (!QueryValidator::isValidAttributeOfDesignEntity(de, attr)) {
                throw InvalidWithAttrRefException();
            }

            // get the type group of this attribute
            typeGroupR = attributeTypeGroupLookup.at(attr);
        }
    } else {  // other query arg type (line num, name etc)
        // check if it is a valid arg type for with clause
        if (queryArgTypeGroupLookup.find(argTypeR) ==
          queryArgTypeGroupLookup.end()) {
            throw InvalidWithArgTypeException();
        }

        typeGroupR = queryArgTypeGroupLookup.at(argTypeR);
    }

    // throw error if not of same type group
    if (typeGroupL != typeGroupR) {
        throw UnmatchedRefTypeException();
    }

    QueryArg argObjL;
    QueryArg argObjR;
    // if everything passes
    // create the query args
    if (argTypeL == QueryArgType::DECLARATION || argTypeL == QueryArgType::DECLARATION_WITH_ATTR) {
        argObjL = QueryArg(declrL);
    } else {
        if (argTypeL == QueryArgType::NAME) {
            // remove quotation marks in names
            refL = QueryParserUtility::removeQuotationMarks(refL);
            refL = QueryParserUtility::trimWhitespace(refL);
        }
        argObjL = QueryArg(refL, argTypeL);
    }

    if (argTypeR == QueryArgType::DECLARATION || argTypeR == QueryArgType::DECLARATION_WITH_ATTR) {
        argObjR = QueryArg(declrR);
    } else {
        if (argTypeR == QueryArgType::NAME) {
            // remove quotation marks in names
            refR = QueryParserUtility::removeQuotationMarks(refR);
            refR = QueryParserUtility::trimWhitespace(refR);
        }
        argObjR = QueryArg(refR, argTypeR);
    }
    return WithClause(argObjL, argObjR);
}
