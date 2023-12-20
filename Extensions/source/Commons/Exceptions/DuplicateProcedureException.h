#pragma once
#include <exception>
#include <string>

class DuplicateProcedureException : public std::exception {
 private:
     std::string procedureName;
 public:
    explicit DuplicateProcedureException(std::string procedureName) {
        this->procedureName = procedureName;
    }

    const char* what() const throw() {
        std::string output = "Duplicated procedure " + procedureName + " found.";
        const char* cstr = &output[0];
        return cstr;
    }
};
