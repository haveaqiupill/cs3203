#include "ClauseAnswer.h"

ClauseAnswer::ClauseAnswer() {
    this->isClauseTrue = true;
    this->resultTable = ResultTable();
}

bool ClauseAnswer::getIsClauseTrue() {
    return isClauseTrue;
}

ResultTable ClauseAnswer::getResultTable() {
    return resultTable;
}
