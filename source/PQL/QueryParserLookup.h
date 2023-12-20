#pragma once
#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Query/QueryDefinitions.h"
// definitions of reference data structures used by PQL Parser
typedef std::unordered_map<std::string, DesignEntity> DECLARATION_ENUM_LOOKUP;
typedef std::unordered_map<std::string, Relationship> RELATIONSHIP_ENUM_LOOKUP;
typedef std::unordered_map<std::string, ClauseType> CLAUSE_ENUM_LOOKUP;
typedef std::unordered_map<std::string, Attribute> ATTRIBUTE_ENUM_LOOKUP;
typedef std::unordered_set<std::string> RESTRICTED_NAMES;

/*
* Lookup tables to map unprocessed strings to their respective enum values
*/
// map string to design entity enum
const DECLARATION_ENUM_LOOKUP declrEnumLookup = {
    {"stmt", DesignEntity::STMT},
    {"read", DesignEntity::READ},
    {"print", DesignEntity::PRINT},
    {"call", DesignEntity::CALL},
    {"while", DesignEntity::WHILE},
    {"if", DesignEntity::IF},
    {"assign", DesignEntity::ASSIGN},
    {"variable", DesignEntity::VARIABLE},
    {"constant", DesignEntity::CONSTANT},
    {"procedure", DesignEntity::PROCEDURE},
    {"prog_line", DesignEntity::PROG_LINE}
};

// map string to relationship enum
const RELATIONSHIP_ENUM_LOOKUP relationshipEnumLookup = {
    {"Follows", Relationship::FOLLOWS},
    {"Follows*", Relationship::FOLLOWS_STAR},
    {"Parent", Relationship::PARENT},
    {"Parent*", Relationship::PARENT_STAR},
    {"Uses", Relationship::USES},
    {"Modifies", Relationship::MODIFIES},
    {"Calls", Relationship::CALLS},
    {"Calls*", Relationship::CALLS_STAR},
    {"Next", Relationship::NEXT},
    {"Next*", Relationship::NEXT_STAR},
    {"Affects", Relationship::AFFECTS},
    {"Affects*", Relationship::AFFECTS_STAR},
    {"NextBip", Relationship::NEXT_BIP},
    {"NextBip*", Relationship::NEXT_BIP_STAR},
    {"AffectsBip", Relationship::AFFECTS_BIP},
    {"AffectsBip*", Relationship::AFFECTS_BIP_STAR}
};

// map string to clausetype enum
const CLAUSE_ENUM_LOOKUP clauseDefinitions = {
    {"such that", ClauseType::SUCHTHAT_CL},
    {"pattern", ClauseType::PATTERN_CL}
};

// attrName : <procName>| <varName> | <value> | <stmt#>
const ATTRIBUTE_ENUM_LOOKUP attributeEnumLookup{
    {"procName", Attribute::PROC_NAME},
    {"varName", Attribute::VAR_NAME},
    {"value", Attribute::VALUE},
    {"stmt#", Attribute::STMT_NUM}
};

// names not allowed for declarations
const RESTRICTED_NAMES restrictedNames = {"none"};


/*
* Regex expressions for basic types
*/
const std::regex intRegex("^\\s*0|[1-9]\\d*\\s*$");  // integer: DIGIT+
const std::regex synRegex("^\\s*[a-zA-Z][a-zA-Z0-9]*\\s*$");  // synonym: LETTER(LETTER|DIGIT)*
const std::regex nameRegex("^\\s*\"\\s*[a-zA-Z][a-zA-Z0-9]*\\s*\"\\s*$");

const std::regex wildcardRegex("^\\s*_\\s*$");

/*
* Regex expressions for arguments
*/
// stmtRef : synonym | _ | INTEGER
const std::regex stmtRefRegex(
    "^\\s*([a-zA-Z][a-zA-Z0-9]*"  // synonym
    "|_|"  // | _ |
    "0|[1-9]\\d*?)\\s*$");  // INTEGER

// entRef : synonym | _ | <"> IDENT <">
const std::regex entRefRegex(
    "^\\s*([a-zA-Z][a-zA-Z0-9]*"  // synonym
    "|_|"  // | _ |
    "\"\\s*[a-zA-Z][a-zA-Z0-9]*\\s*\"?)\\s*$");  // <"> IDENT <">

// no wildcard option (for left arguments of Modifies and Uses)
const std::regex stmtOrEntRefRegexNoWildcard(
    "^\\s*([a-zA-Z][a-zA-Z0-9]*"  // SYNONYM
    "|0|[1-9]\\d*|"  // INTEGER
    "\"\\s*[a-zA-Z][a-zA-Z0-9]*\\s*\"?)\\s*$");  // <"> IDENT <">

/*
* Regex expressions for query statements
*/
// declaration: design-entity synonym (<,> synonym)* <;>
// note in PQLParser, the <;> has been removed before passing to regex_match
const std::regex declarationRegex(
    "^\\s*(stmt|read|print|call|while|if|assign|"
    "variable|constant|procedure|prog_line)\\s+"  // design-entity
    "([a-zA-Z][a-zA-Z0-9]*)\\s*"  // synonym
    "(,\\s*[a-zA-Z][a-zA-Z0-9]*\\s*)*$");  // (<,> synonym)*
/*
// select-cl : declaration* <Select> synonym [ suchthat-cl ]   [ pattern-cl ]
const std::regex selectStmtRegexBoolean(
    "^\\s*Select\\s+"  // <Select>
    "(BOOLEAN)\\b"   // synonym
    "\\s*((?:such\\s+that|pattern|with).*?)?\\s*$");  // clauses
*/

// select-cl : declaration* <Select> synonym [ suchthat-cl ]   [ pattern-cl ]
const std::regex selectStmtRegexSingle(
    "^\\s*Select\\s+"  // <Select>
    "("  // start of selection capture group
    "[a-zA-Z][a-zA-Z0-9]*(?:[\\.](?:procName|varName|value|stmt#))?"  // elem
    ")"   // end of selection capture group
    "\\s*((?:such\\s+that|pattern|with).*?)?\\s*$");  // clauses

const std::regex selectStmtRegexTuple(
    "^\\s*Select\\s*"  // <Select>
    "<\\s*"  // <
    "("  // start of selection capture group
    "[a-zA-Z][a-zA-Z0-9]*"  // elem
    "(?:[\\.](?:procName|varName|value|stmt#))?"
    "(?:\\s*,\\s*"  // ( , elem )*
    "[a-zA-Z][a-zA-Z0-9]*"
    "(?:[\\.](?:procName|varName|value|stmt#))?)*"
    "?)"  // end of selection capture group
    "\\s*>"   // >
    "\\s*((?:such\\s+that|pattern|with).*?)?\\s*$");  // clauses

// suchthat-cl : <such that> relCond
// relCond : relRef ( <and> relRef )*
// relRef : ModifiesP | ModifiesS | UsesP | UsesS
//          | Calls | CallsT | Parent | ParentT
//          | Follows | FollowsT | Next | NextT | Affects | AffectsT
const std::regex suchThatRegex(
    // "^\\s*such\\s+that\\s+"
    "^(Follows|Follows\\*|Parent|Parent\\*|Uses|Modifies|"
    "Calls|Calls\\*|Next|Next\\*|Affects|Affects\\*"
    "|NextBip|NextBip\\*|AffectsBip|AffectsBip\\*)\\s*"
    "\\(\\s*"
    "([\"]\\s*[a-zA-z\\d]+\\s*[\"]|[a-zA-z\\d]+)"
    "\\s*,\\s*"
    "([\"]\\s*[a-zA-z\\d]+\\s*[\"]|[a-zA-z\\d]+)"
    "\\s*\\)\\s*$");

//  pattern-cl : <pattern> syn-assign <(> entRef <,> expression-spec <)>
//  expression-spec: <"> expr<"> | <_> <"> expr <"> <_> | <_>
//  expr: expr <+> term | expr <-> term | term
//  term: term <*> factor | term </> factor | term <%> factor | factor
//  factor: var_name | const_value | <(> expr <)>
const std::regex patternStmtRegex(
    // "^\\s*pattern\\s+"  // pattern
    "^([a-zA-Z][a-zA-Z0-9]*)\\s*"  // syn-assign or syn-while or syn-if
    "\\(\\s*"  // <(>
    "(.*?)"  // capture everything within the brackets
    "\\s*\\)\\s*$"  // <)>
);

const std::regex ifPatternArgRegex(
    "^([a-zA-Z][a-zA-Z0-9]*|_|\"\\s*[a-zA-Z][a-zA-Z0-9]*\\s*\")"  // entRef
    "\\s*,\\s*"  // <,>
    "_"  // _
    "\\s*,\\s*"  // <,>
    "_"  // _
    "\\s*$"
);

const std::regex whilePatternArgRegex(
    "^([a-zA-Z][a-zA-Z0-9]*|_|\"\\s*[a-zA-Z][a-zA-Z0-9]*\\s*\")"  // entRef
    "\\s*,\\s*"  // <,>
    "_"  // _
    "\\s*$"
);

const std::regex assignPatternArgRegex(
    "^([a-zA-Z][a-zA-Z0-9]*|_|\"\\s*[a-zA-Z][a-zA-Z0-9]*\\s*\")"  // entRef
    "\\s*,\\s*"  // <,>
    "(.*?)\\s*$"  // expression-spec
);

const std::regex patternExprWithWildcardsRegex(
    "^\\s*_\\s*\"\\s*"  // _"
    "(.*?)"  // expression-spec
    "\\s*\"\\s*_\\s*$");  // "_

const std::regex patternExprRegex(
    "^\\s*\"\\s*"  // "
    "(.*?)\\s*"  // expression-spec
    "\"\\s*$");  // "

// with-cl : <with> attrCond
// attrCond : attrCompare ( <and> attrCompare )*
// attrCompare : ref <=> ref
// ref : <'> IDENT <'> | INTEGER | attrRef | synonym
// attrRef : synonym <.> attrName
const std::regex withStmtRegex(
    "^(.*?)"  // ref
    "\\s*=\\s*"  // <=>
    "(.*?)\\s*$"  // ref
);

const std::regex attrRefRegex(
    "^([a-zA-Z][a-zA-Z0-9]*)"  // synonym
    "[\\.]"  // <.>
    "(procName|varName|value|stmt#)\\s*$"
);


/*
* Tables used in with clause
* Used to assign a type group for the argument
* Where 0 corresponds to number
* and 1 corresponds to string
*/
const std::unordered_map<Attribute, int> attributeTypeGroupLookup{
    {Attribute::STMT_NUM, 0},
    {Attribute::PROC_NAME, 1},
    {Attribute::VAR_NAME, 1},
    {Attribute::VALUE, 0},
    {Attribute::NONE, 0}  // prog_line is a number
};

const std::unordered_map<QueryArgType, int> queryArgTypeGroupLookup{
    {QueryArgType::NUM, 0},
    {QueryArgType::NAME, 1}
};

/*
* Lookup tables used to parse and validate attribute references
*/

/*
* Lookup table to check that the attribute is valid for the design entity
*/
const std::unordered_map<
DesignEntity, std::unordered_set<Attribute>>
designEntityAllowedAttributesLookup{
    {DesignEntity::PROCEDURE,
    {Attribute::PROC_NAME}
    },

    {DesignEntity::CALL,
    {Attribute::STMT_NUM,
    Attribute::PROC_NAME}
    },

    {DesignEntity::VARIABLE,
    {Attribute::VAR_NAME}
    },

    {DesignEntity::READ,
    {Attribute::VAR_NAME,
    Attribute::STMT_NUM}
    },

    {DesignEntity::PRINT,
    {Attribute::VAR_NAME,
    Attribute::STMT_NUM}
    },

    {DesignEntity::CONSTANT,
    {Attribute::VALUE}
    },

    {DesignEntity::STMT,
    {Attribute::STMT_NUM}
    },

    {DesignEntity::WHILE,
    {Attribute::STMT_NUM}
    },

    {DesignEntity::IF,
    {Attribute::STMT_NUM}
    },

    {DesignEntity::ASSIGN,
    {Attribute::STMT_NUM}
    },
};

/*
* Lookup table for the default attribute of design entities
* Used to assign an attribute to a declaration that has no explicit attribute specified
* eg. In the query: call c; Select c, c has a default attribute of STMT_NUM
*/
const std::unordered_map<DesignEntity, Attribute> defaultDesignEntityAttrLookup {
    {DesignEntity::PROCEDURE,
    Attribute::PROC_NAME
    },

    {DesignEntity::CALL,
    Attribute::STMT_NUM
    },

    {DesignEntity::VARIABLE,
    Attribute::VAR_NAME
    },

    {DesignEntity::READ,
    Attribute::STMT_NUM
    },

    {DesignEntity::PRINT,
    Attribute::STMT_NUM
    },

    {DesignEntity::CONSTANT,
    Attribute::VALUE
    },

    {DesignEntity::STMT,
    Attribute::STMT_NUM
    },

    {DesignEntity::WHILE,
    Attribute::STMT_NUM
    },

    {DesignEntity::IF,
    Attribute::STMT_NUM
    },

    {DesignEntity::ASSIGN,
    Attribute::STMT_NUM
    },

    {DesignEntity::ASSIGN,
    Attribute::STMT_NUM
    },

    {DesignEntity::PROG_LINE,
    Attribute::NONE
    },
};

/*
* Stores the design entities that can not be used with attributes
*/
const std::unordered_set<DesignEntity> designEntitiesWithNoAttrAllowed = {
    DesignEntity::PROG_LINE
};


/*
* REGEX LOOKUP TABLES
* a lookup to check that argument syntax is valid,
* according to the type of relationship or design entity in the case of pattern.
* eg. for Follows(..,..) in a such that statement, lookup Relationship::Follows for the regex for the arguments
*/
const std::unordered_map<Relationship, std::regex> relationshipLeftArgLookup ={
    {Relationship::FOLLOWS, stmtRefRegex},
    {Relationship::FOLLOWS_STAR, stmtRefRegex},
    {Relationship::PARENT, stmtRefRegex},
    {Relationship::PARENT_STAR, stmtRefRegex},
    {Relationship::MODIFIES, stmtOrEntRefRegexNoWildcard},
    {Relationship::USES, stmtOrEntRefRegexNoWildcard},
    {Relationship::CALLS, entRefRegex},
    {Relationship::CALLS_STAR, entRefRegex},
    {Relationship::NEXT, stmtRefRegex},
    {Relationship::NEXT_STAR, stmtRefRegex},
    {Relationship::AFFECTS, stmtRefRegex},
    {Relationship::AFFECTS_STAR, stmtRefRegex},
    {Relationship::NEXT_BIP, stmtRefRegex},
    {Relationship::NEXT_BIP_STAR, stmtRefRegex},
    {Relationship::AFFECTS_BIP, stmtRefRegex},
    {Relationship::AFFECTS_BIP_STAR, stmtRefRegex}
};

const std::unordered_map<Relationship, std::regex> relationshipRightArgLookup = {
    {Relationship::FOLLOWS, stmtRefRegex},
    {Relationship::FOLLOWS_STAR, stmtRefRegex},
    {Relationship::PARENT, stmtRefRegex},
    {Relationship::PARENT_STAR, stmtRefRegex},
    {Relationship::MODIFIES, entRefRegex},
    {Relationship::USES, entRefRegex},
    {Relationship::CALLS, entRefRegex},
    {Relationship::CALLS_STAR, entRefRegex},
    {Relationship::NEXT, stmtRefRegex},
    {Relationship::NEXT_STAR, stmtRefRegex},
    {Relationship::AFFECTS, stmtRefRegex},
    {Relationship::AFFECTS_STAR, stmtRefRegex},
    {Relationship::NEXT_BIP, stmtRefRegex},
    {Relationship::NEXT_BIP_STAR, stmtRefRegex},
    {Relationship::AFFECTS_BIP, stmtRefRegex},
    {Relationship::AFFECTS_BIP_STAR, stmtRefRegex}
};

const std::unordered_map<DesignEntity, std::regex> patternArgLookup = {
    {DesignEntity::ASSIGN, assignPatternArgRegex},
    {DesignEntity::IF, ifPatternArgRegex},
    {DesignEntity::WHILE, whilePatternArgRegex}
};

/*
* DECLARATION DESIGN ENTITY LOOKUP TABLES
* Tables to indicate specific design entities allowed for arguments that are declarations
* If the set is empty, anything goes
*/
const std::unordered_map<Relationship, std::unordered_set<DesignEntity>>
    relationshipLeftArgDesignEntLookup = {
        {
        Relationship::FOLLOWS,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {
        Relationship::FOLLOWS_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::PARENT,
        {DesignEntity::IF,
        DesignEntity::PROG_LINE,
        DesignEntity::WHILE,
        DesignEntity::STMT}
        },

        {Relationship::PARENT_STAR,
        {DesignEntity::IF,
        DesignEntity::PROG_LINE,
        DesignEntity::WHILE,
        DesignEntity::STMT}
        },

        {Relationship::MODIFIES,
        {DesignEntity::ASSIGN,
        DesignEntity::PROG_LINE,
        DesignEntity::STMT,
        DesignEntity::READ,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PROCEDURE,
        DesignEntity::CALL}
        },

        {Relationship::USES,
        {DesignEntity::ASSIGN,
        DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::PRINT,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PROCEDURE,
        DesignEntity::CALL}
        },

        {Relationship::CALLS,
        {DesignEntity::PROCEDURE}
        },

        {Relationship::CALLS_STAR,
        {DesignEntity::PROCEDURE}
        },

        {Relationship::NEXT,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::NEXT_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::AFFECTS,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },

        {Relationship::AFFECTS_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },

        {Relationship::NEXT_BIP,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::NEXT_BIP_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::AFFECTS_BIP,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },

        {Relationship::AFFECTS_BIP_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },
    };

const std::unordered_map<Relationship, std::unordered_set<DesignEntity>>
    relationshipRightArgDesignEntLookup = {
        {Relationship::FOLLOWS,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::FOLLOWS_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::PARENT,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::READ,
        DesignEntity::PRINT,
        DesignEntity::CALL}
        },

        {Relationship::PARENT_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::READ,
        DesignEntity::PRINT,
        DesignEntity::CALL}
        },

        {Relationship::MODIFIES,
        {DesignEntity::VARIABLE}
        },

        {Relationship::USES,
        {DesignEntity::VARIABLE}
        },

        {Relationship::CALLS,
        {DesignEntity::PROCEDURE}
        },

        {Relationship::CALLS_STAR,
        {DesignEntity::PROCEDURE}
        },

        {Relationship::NEXT,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::NEXT_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::AFFECTS,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },

        {Relationship::AFFECTS_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },

        {Relationship::NEXT_BIP,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::NEXT_BIP_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN,
        DesignEntity::IF,
        DesignEntity::WHILE,
        DesignEntity::PRINT,
        DesignEntity::READ,
        DesignEntity::CALL}
        },

        {Relationship::AFFECTS_BIP,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },

        {Relationship::AFFECTS_BIP_STAR,
        {DesignEntity::STMT,
        DesignEntity::PROG_LINE,
        DesignEntity::ASSIGN}
        },
};
