#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "../../Query/Declaration.h"

typedef std::string SYNONYM;
typedef std::vector<std::string> ROW;
typedef std::vector<ROW> RESULT_TABLE;

class ResultTable {
 private:
    std::unordered_map<Declaration, int, dec_hash> synonyms;
    RESULT_TABLE resultTable;

 public:
    // Constructor
    ResultTable();
    ResultTable(std::unordered_map<Declaration, int, dec_hash> synonyms,
                RESULT_TABLE resultTable) :
                synonyms(synonyms),
                resultTable(resultTable) {}

    // Getters
    std::unordered_map<Declaration, int, dec_hash> getSynonyms();
    RESULT_TABLE getResultTable();

    // Merge methods
    ResultTable merge(ResultTable& resultTableTwo);
};
