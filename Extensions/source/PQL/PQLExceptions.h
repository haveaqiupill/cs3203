#pragma once
#include <exception>

struct SyntaxErrorException : public std::exception {
};

struct SemanticErrorException : public std::exception {
};

struct BooleanException : public std::exception {
};

struct InvalidInternalUsageException : public std::exception {
};

struct InvalidQuerySyntaxException : SyntaxErrorException {
    const char* what() const throw() {
        return "General syntax error found";
    }
};

struct InvalidSelectionSyntaxException : SyntaxErrorException {
    const char* what() const throw() {
        return "Invalid selection syntax found";
    }
};

struct InvalidSelectionAttrRefException : SemanticErrorException {
    const char* what() const throw() {
        return "Invalid attribute reference";
    }
};

struct MissingDeclarationException : SemanticErrorException {
    const char* what() const throw() {
        return "Unable to find specified synonym in the declaration table";
    }
};

struct InvalidArgumentSyntaxException : SyntaxErrorException {
    const char* what() const throw() {
        return "Argument's syntax is invalid, or there are missing arguments";
    }
};

struct InvalidClauseException : SyntaxErrorException {
    const char* what() const throw() {
        return "Clauses are invalid";
    }
};

struct InvalidDeclarationException : SyntaxErrorException {
    const char* what() const throw() {
        return "Declaration is invalid";
    }
};

struct DuplicateDeclarationException : SemanticErrorException {
    const char* what() const throw() {
        return "The synonym has been declared before";
    }
};

struct UnsupportedMethodCallException : InvalidInternalUsageException {
    const char* what() const throw() {
        return "You are calling an unimplemented method"
            "that should not be used for this base class";
    }
};

struct WrongQueryArgConstructorException : InvalidInternalUsageException {
    const char* what() const throw() {
        return "The wrong constructor is being used for this QueryArgType";
    }
};

struct InvalidSuchThatArgumentSyntaxException : SyntaxErrorException {
    const char* what() const throw() {
        return "Invalid arguments used in such that clause";
    }
};

struct InvalidSuchThatDesignEntityException : SemanticErrorException {
    const char* what() const throw() {
        return "Invalid design entity used in such that clause";
    }
};

struct InvalidPatternArgumentSyntaxException : SyntaxErrorException {
    const char* what() const throw() {
        return "Invalid arguments used in pattern clause";
    }
};

struct InvalidPatternDesignEntityException : SemanticErrorException {
    const char* what() const throw() {
        return "Invalid design entity used for pattern clause";
    }
};

struct InvalidWithSyntaxException : SyntaxErrorException {
    const char* what() const throw() {
        return "Syntax error found in with clause";
    }
};

struct InvalidWithComparisonException : SemanticErrorException {
    const char* what() const throw() {
        return "Invalid trivial comparisons being made in the with clause";
    }
};

struct InvalidWithDesignEntityException : SemanticErrorException {
    const char* what() const throw() {
        return "Synonym used in with clause is not of PROG_LINE design entity";
    }
};

struct InvalidWithAttrRefException : SemanticErrorException {
    const char* what() const throw() {
        return "Attribute reference used in with clause is invalid";
    }
};

struct InvalidWithArgTypeException : SemanticErrorException {
    const char* what() const throw() {
        return "Invalid arg type used (i.e. pattern / wildcard) in with clause";
    }
};

struct UnmatchedRefTypeException : SemanticErrorException {
    const char* what() const throw() {
        return "The two refs in the with clause are not of the same type";
    }
};

struct MissingDataException : InvalidInternalUsageException {
    const char* what() const throw() {
        return "Missing information in QueryParserLookup!";
    }
};

struct InvalidAttrUsageException : public std::exception {
    const char* what() const throw() { return
        "This design entity does not have attributes";
    }
};
