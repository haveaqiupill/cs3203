#include "IntermediateAnswer.h"

std::vector<ClauseAnswer> IntermediateAnswer::getClauseAnsList() {
    return clauseAnsList;
}

SelectionType IntermediateAnswer::getSelectionType() {
    return selectionType;
}

std::vector<Declaration>IntermediateAnswer::getSelection() {
    return selection;
}
