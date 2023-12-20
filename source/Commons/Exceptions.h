#pragma once
#include <exception>

struct InvalidDesignEntity : public std::exception {
    const char* what() const throw() {
        return "Invalid design entity";
    }
};

struct InvalidRelationship : public std::exception {
    const char* what() const throw() {
        return "Invalid relationship";
    }
};

struct InvalidRelationshipException : public std::exception {
    const char* what() const throw() {
        return "Invalid relationship found in Query.";
    }
};

struct InvalidParentRelationshipException : public std::exception {
    const char* what() const throw() {
        return "Invalid parent relationship found in Query.";
    }
};

struct InvalidArgumentException : public std::exception {
    const char* what() const throw() {
        return "Invalid argument found in Query.";
    }
};
