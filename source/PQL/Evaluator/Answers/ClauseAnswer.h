#pragma once

#include <string>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "ResultTable.h"

class ClauseAnswer {
 private:
    bool isClauseTrue;
    ResultTable resultTable;

 public:
    // Constructor
    ClauseAnswer();
    ClauseAnswer(bool isClauseTrue,
                 ResultTable resultTable) :
                 isClauseTrue(isClauseTrue),
                 resultTable(resultTable) {}

    // Getters
    bool getIsClauseTrue();
    ResultTable getResultTable();
};
