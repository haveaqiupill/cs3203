#pragma once

#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../Query/Query.h"
#include "../../PKB/PKB.h"
#include "../Evaluator/Answers/ClauseAnswer.h"
#include "EvaluatorUtility.h"

class AffectsEvaluator {
 protected:
    PKB* pkb;
    bool initialized = false;
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> pairCache;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> forwardCache;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> reverseCache;
    // store some of the data structure so it does not have to keep calling PKB
    std::unordered_set<LINE_NUM>* assignSet = nullptr;
    std::unordered_set<LINE_NUM>* readSet = nullptr;
    std::unordered_set<LINE_NUM>* callSet = nullptr;
    std::vector<LINE_NUM>* assignList = nullptr;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* nextMapKeyLineBefore = nullptr;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* nextMapKeyLineAfter = nullptr;

    bool doesLineNumAffects(LINE_NUM);
    bool isLineNumAffected(LINE_NUM);
    void initializeDataStructures();

    struct pair_hash_set {
        size_t operator()(const std::pair<LINE_NUM, std::unordered_set<VAR_NAME>>& p) const {
            size_t seed = 0;
            seed ^= std::hash<LINE_NUM>{}(p.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            for (VAR_NAME var : p.second) {
                seed ^= std::hash<VAR_NAME>{}(var)+0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

 private:
    std::vector<LINE_NUM> affectsBfsLineBefore(LINE_NUM);
    std::vector<LINE_NUM> affectsDfsLineAfter(LINE_NUM);

 public:
    explicit AffectsEvaluator(PKB* pkb) : pkb(pkb) {}
    ClauseAnswer evaluateAffects(QueryArg& leftArg, QueryArg& rightArg, DECLARATION_TABLE& dt);
    bool doLinesAffects(LINE_NUM, LINE_NUM);
};
