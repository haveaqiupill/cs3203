#include <unordered_map>

#include "EvaluatorUtility.h"
#include "../QueryParserUtility.h"

ResultTable EvaluatorUtility::lineNumVecToResultTable(Declaration selection,
    std::vector<LINE_NUM> lineNumVec) {
    RESULT_TABLE table;
    for each (LINE_NUM lineNum in lineNumVec) {
        std::vector<std::string> row{ std::to_string(lineNum) };
        table.push_back(row);
    }

    std::unordered_map<Declaration, int, dec_hash> synonym{ {selection, 0} };
    return ResultTable(synonym, table);
}

ResultTable EvaluatorUtility::lineNumVecPairToResultTable(Declaration selection1,
    Declaration selection2, std::vector<std::pair<LINE_NUM, LINE_NUM>> lineNumVecPair) {
    RESULT_TABLE table;
    for each (std::pair<LINE_NUM, LINE_NUM> lineNumPair in lineNumVecPair) {
        std::vector<std::string> row{ std::to_string(lineNumPair.first),
                                      std::to_string(lineNumPair.second) };
        table.push_back(row);
    }

    std::unordered_map<Declaration, int, dec_hash> synonym{
        { selection1, 0 },
        { selection2, 1 }
    };

    return ResultTable(synonym, table);
}

ResultTable EvaluatorUtility::lineNumStrVecPairToResultTable(Declaration selection1,
    Declaration selection2, std::vector<std::pair<LINE_NUM, VAR_NAME>> lineNumStrVecPair) {
    RESULT_TABLE table;
    for each (std::pair<LINE_NUM, VAR_NAME> lineNumPair in lineNumStrVecPair) {
        std::vector<std::string> row{ std::to_string(lineNumPair.first),
                                      lineNumPair.second };
        table.push_back(row);
    }

    std::unordered_map<Declaration, int, dec_hash> synonym{
        {
            selection1,
            0
        },
        {
            selection2,
            1
        }
    };
    return ResultTable(synonym, table);
}

ResultTable EvaluatorUtility::strLineNumVecPairToResultTable(Declaration selection1,
    Declaration selection2, std::vector<std::pair<VAR_NAME, LINE_NUM>> lineNumStrVecPair) {
    RESULT_TABLE table;
    for each (std::pair<VAR_NAME, LINE_NUM> lineNumPair in lineNumStrVecPair) {
        std::vector<std::string> row{ lineNumPair.first,
                                      std::to_string(lineNumPair.second) };
        table.push_back(row);
    }

    std::unordered_map<Declaration, int, dec_hash> synonym{
        {
            selection1,
            0
        },
        {
            selection2,
            1
        }
    };
    return ResultTable(synonym, table);
}

ResultTable EvaluatorUtility::strVecToResultTable(Declaration selection,
    std::vector<std::string> strVec) {
    RESULT_TABLE table;
    for each (std::string str in strVec) {
        std::vector<std::string> row{ str };
        table.push_back(row);
    }

    std::unordered_map<Declaration, int, dec_hash> synonym{
        {
            selection,
            0
        }
    };
    return ResultTable(synonym, table);
}

ResultTable EvaluatorUtility::strVecPairToResultTable(Declaration selection1,
    Declaration selection2, std::vector<std::pair<std::string, std::string>> strVecPair) {
    RESULT_TABLE table;
    for each (std::pair<std::string, std::string> strPair in strVecPair) {
        std::vector<std::string> row{ strPair.first, strPair.second };
        table.push_back(row);
    }

    std::unordered_map<Declaration, int, dec_hash> synonym{
        {
            selection1,
            0
        },
        {
            selection2,
            1
        }
    };
    return ResultTable(synonym, table);
}
