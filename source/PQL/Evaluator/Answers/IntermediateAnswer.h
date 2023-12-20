#pragma once

#include <vector>

#include "ClauseAnswer.h"
#include "../../Query/SelectionType.h"
#include "../../Query/Declaration.h"

class IntermediateAnswer {
 private:
    std::vector<Declaration> selection;
    SelectionType selectionType;
    std::vector<ClauseAnswer> clauseAnsList;

 public:
    // Constructor
    IntermediateAnswer() {}
    IntermediateAnswer(std::vector<Declaration> selection,
                       SelectionType selectionType,
                       std::vector<ClauseAnswer> clauseAnsList) :
                       selection(selection),
                       selectionType(selectionType),
                       clauseAnsList(clauseAnsList) {}

    // Getters
    std::vector<Declaration> getSelection();
    std::vector<ClauseAnswer> getClauseAnsList();
    SelectionType getSelectionType();
};
