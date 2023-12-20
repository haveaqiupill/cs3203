#pragma once

#include <utility>
#include <string>
#include <vector>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class WithEvaluator {
 private:
    PKB *pkb;
    ClauseAnswer evaluateZeroAttributeRef(QueryArg& leftArg, QueryArg& rightArg,
                                          DECLARATION_TABLE& dt);
    ClauseAnswer evaluateOneAttributeRef(QueryArg& leftArg, QueryArg& rightArg,
                                         DECLARATION_TABLE& dt, PKB *pkb);
    ClauseAnswer evaluateTwoAttributeRef(QueryArg& leftArg, QueryArg& rightArg,
                                         DECLARATION_TABLE& dt, PKB* pkb);

    // Helper functions
    bool isTwoAttrDe(DesignEntity de);
    std::vector<std::string> convertVecIntToVecStr(std::vector<LINE_NUM> vec);
    std::vector<std::pair<std::string, std::string>> convertToVecPair(std::vector<std::string> vec);
    bool checkValInPkb(DesignEntity de, std::string val, PKB* pkb);
    std::vector<std::pair<int, std::string>> checkValInPkbWithAttr(DesignEntity de, Attribute atr,
                                                                   std::string val, PKB* pkb);
    std::vector<std::pair<int, std::string>> getTwoAttrFromStr(DesignEntity de, std::string ele,
                                                               PKB* pkb);
    std::vector<std::pair<int, std::string>> getTwoAttrFromInt(DesignEntity de, std::string ele,
                                                               PKB* pkb);
    Declaration makeLeftDecForTwoAttr(Declaration dec, DesignEntity de);
    Declaration makeRightDecForTwoAttr(Declaration dec, DesignEntity de);

 public:
    explicit WithEvaluator(PKB *pkb) : pkb(pkb) {}
    ClauseAnswer evaluateWithClause(QueryArg& leftArg, QueryArg& rightArg,
                                    DECLARATION_TABLE& dt, PKB* pkb);
};
