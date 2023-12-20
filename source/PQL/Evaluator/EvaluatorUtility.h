#pragma once

#include <string>
#include <vector>
#include <utility>

#include "Answers/ResultTable.h"
#include "../../PKB/PKB.h"

class EvaluatorUtility {
 public:
     static ResultTable lineNumVecToResultTable(Declaration selection,
         std::vector<LINE_NUM> lineNumVec);
     static ResultTable lineNumVecPairToResultTable(Declaration selection1, Declaration selection2,
         std::vector<std::pair<LINE_NUM, LINE_NUM>> lineNumVecPair);
     static ResultTable lineNumStrVecPairToResultTable(Declaration selection1,
         Declaration selection2, std::vector<std::pair<LINE_NUM, VAR_NAME>> lineNumStrVecPair);
     static ResultTable strLineNumVecPairToResultTable(Declaration selection1,
         Declaration selection2, std::vector<std::pair<VAR_NAME, LINE_NUM>> lineNumStrVecPair);
     static ResultTable strVecToResultTable(Declaration selection, std::vector<std::string> strVec);
     static ResultTable strVecPairToResultTable(Declaration selection1, Declaration selection2,
         std::vector<std::pair<std::string, std::string>> strVecPair);
};
