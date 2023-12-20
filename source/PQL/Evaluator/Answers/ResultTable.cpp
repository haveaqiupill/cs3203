#include <algorithm>
#include <utility>

#include "ResultTable.h"

ResultTable::ResultTable() {
    this->synonyms = {};
    this->resultTable = {};
}

std::unordered_map<Declaration, int, dec_hash> ResultTable::getSynonyms() {
    return synonyms;
}

RESULT_TABLE ResultTable::getResultTable() {
    return resultTable;
}

ResultTable ResultTable::merge(ResultTable& resultTableTwo) {
    std::unordered_map<Declaration, int, dec_hash> tableTwoSynonyms = resultTableTwo.getSynonyms();
    std::vector<Declaration> pivots;

    int tableOneSynonymNum = synonyms.size();
    int tableTwoSynonymNum = tableTwoSynonyms.size();

    // Case 0: One table does not have synonyms -> return the other table
    if (tableOneSynonymNum == 0) {
        return resultTableTwo;
    } else if (tableTwoSynonymNum == 0) {
        return *this;
    }

    // Add tableTwo synonyms from unordered map to a vector to enable sorting
    std::vector<std::pair<Declaration, int>> newSyns;
    for (auto it : tableTwoSynonyms) {
        newSyns.push_back({ it.first, it.second });
    }
    // Sort the synonyms according to their indexes
    sort(newSyns.begin(), newSyns.end(),
        [](const std::pair<Declaration, int> a, const std::pair<Declaration, int> b) -> bool {
            return a.second < b.second;
        });

    std::vector<std::pair<Declaration, int>> synsToBeAdded;
    // New synonyms from tableTwo will be added to tableOne from the end of tableOne's synonym list
    int newIdx = tableOneSynonymNum;
    for (auto s : newSyns) {
        if (synonyms.find(s.first) != synonyms.end()) {
            // If synonym from tableTwo is found in tableOne, add it to the list of pivots
            pivots.push_back(s.first);
        } else {
            // Else if synonym from tableTwo is not in tableOne,
            // add it to the list of synonyms to be added
            synsToBeAdded.push_back({s.first, newIdx});
            newIdx++;
        }
    }

    // Add the new synonyms from tableTwo into the unordered map
    // that stores the synonyms in tableOne
    for (auto s : synsToBeAdded) {
        synonyms[s.first] = s.second;
    }

    RESULT_TABLE mergedTable;

    // Case 1: No common synonyms -> append the entire second table
    // Cartesian product... :(
    if (pivots.size() == 0) {
        for each (ROW row1 in resultTable) {
            for each (ROW row2 in resultTableTwo.getResultTable()) {
                ROW newRow;
                newRow.insert(newRow.end(), row1.begin(), row1.end());
                for (auto s : synsToBeAdded) {
                    int idx = tableTwoSynonyms.at(s.first);
                    newRow.push_back(row2.at(idx));
                }
                mergedTable.push_back(newRow);
            }
        }
    }

    // Case 2: Has common synonym(s) -> filter results based on common synonym(s)
    if (pivots.size() >= 1) {
        for each (ROW row1 in resultTable) {
            for each (ROW row2 in resultTableTwo.getResultTable()) {
                ROW newRow = row1;
                bool isValidNewRow = true;
                // Check if common synonyms have the same value to determine if it is
                // a valid new row
                for each (Declaration pivot in pivots) {
                    if (row1[synonyms.at(pivot)] != row2[tableTwoSynonyms.at(pivot)]) {
                        isValidNewRow = false;
                        break;
                    }
                }

                if (isValidNewRow) {
                    for each (std::pair<Declaration, int> newSyn in synsToBeAdded) {
                        newRow.push_back(row2[tableTwoSynonyms.at(newSyn.first)]);
                    }
                    mergedTable.push_back(newRow);
                }
            }
        }
    }

    return ResultTable(synonyms, mergedTable);
}
