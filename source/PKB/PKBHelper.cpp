#include <algorithm>

#include "PKB.h"

template <typename T>
std::vector<T> setToSortedVector(std::unordered_set<T>* set) {
    std::vector<T> temp(set->begin(), set->end());
    std::sort(temp.begin(), temp.end());
    return temp;
}

template <typename T>
std::vector<T> removeDuplicateAndSortVector(std::vector<T>* vector) {
    std::unordered_set<T> s(vector->begin(), vector->end());
    std::vector<T> temp = setToSortedVector(&s);
    return temp;
}

void PKB::sortNextBipDataStructure() {
    for (auto const& key : nextBipMapKeyLineBefore) {
        std::vector<std::pair<LINE_NUM, LINE_NUM>> vec = key.second;
        std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> s(vec.begin(), vec.end());
        std::vector<std::pair<LINE_NUM, LINE_NUM>> finalVec(s.begin(), s.end());
        std::sort(finalVec.begin(), finalVec.end());
        nextBipMapKeyLineBefore[key.first] = finalVec;
    }

    for (auto const& key : nextBipMapKeyLineAfter) {
        std::vector<std::pair<LINE_NUM, LINE_NUM>> vec = key.second;
        std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> s(vec.begin(), vec.end());
        std::vector<std::pair<LINE_NUM, LINE_NUM>> finalVec(s.begin(), s.end());
        std::sort(finalVec.begin(), finalVec.end());
        nextBipMapKeyLineAfter[key.first] = finalVec;
    }

    for (auto const& key : nextBipStarMapKeyLineBefore) {
        std::vector<LINE_NUM> vec = key.second;
        nextBipStarMapKeyLineBefore[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : nextBipStarMapKeyLineAfter) {
        std::vector<LINE_NUM> vec = key.second;
        nextBipStarMapKeyLineAfter[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortAffectsBipDataStructure() {
    for (auto const& key : affectsBipMapKeyLineBefore) {
        std::vector<LINE_NUM> vec = key.second;
        affectsBipMapKeyLineBefore[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : affectsBipMapKeyLineAfter) {
        std::vector<LINE_NUM> vec = key.second;
        affectsBipMapKeyLineAfter[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : affectsBipStarMapKeyLineBefore) {
        std::vector<LINE_NUM> vec = key.second;
        affectsBipStarMapKeyLineBefore[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : affectsBipStarMapKeyLineAfter) {
        std::vector<LINE_NUM> vec = key.second;
        affectsBipStarMapKeyLineAfter[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortFollowsDataStructure() {
    // follows is a one-to-one property, so it is guaranteed to have one entry
    for (auto const& key : followsStarMapKeyLineBefore) {
        std::vector<LINE_NUM> vec = key.second;
        followsStarMapKeyLineBefore[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : followsStarMapKeyLineAfter) {
        std::vector<LINE_NUM> vec = key.second;
        followsStarMapKeyLineAfter[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortParentDataStructure() {
    // each child can only have one parent, thus there is no need to sort parentMapKeyChildLine
    for (auto const& key : parentMapKeyParentLine) {
        std::vector<LINE_NUM> vec = key.second;
        parentMapKeyParentLine[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : parentStarMapKeyParentLine) {
        std::vector<LINE_NUM> vec = key.second;
        std::unordered_set<LINE_NUM> s(vec.begin(), vec.end());
        parentStarMapKeyParentLine[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : parentStarMapKeyChildLine) {
        std::vector<LINE_NUM> vec = key.second;
        std::unordered_set<LINE_NUM> s(vec.begin(), vec.end());
        parentStarMapKeyChildLine[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortUsesDataStructure() {
    for (auto const& key : usesMapByProcedureKeyProcedure) {
        std::vector<VAR_NAME> vec = key.second;
        usesMapByProcedureKeyProcedure[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : usesMapByProcedureKeyVariable) {
        std::vector<PROC_NAME> vec = key.second;
        usesMapByProcedureKeyVariable[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : usesMapByLineNumKeyLineNum) {
        std::vector<VAR_NAME> vec = key.second;
        usesMapByLineNumKeyLineNum[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : usesMapByLineNumKeyVariable) {
        std::vector<LINE_NUM> vec = key.second;
        usesMapByLineNumKeyVariable[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortModifiesDataStructure() {
    for (auto const& key : modifiesMapByProcedureKeyProcedure) {
        std::vector<VAR_NAME> vec = key.second;
        modifiesMapByProcedureKeyProcedure[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : modifiesMapByProcedureKeyVariable) {
        std::vector<PROC_NAME> vec = key.second;
        modifiesMapByProcedureKeyVariable[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : modifiesMapByLineNumKeyLineNum) {
        std::vector<VAR_NAME> vec = key.second;
        modifiesMapByLineNumKeyLineNum[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : modifiesMapByLineNumKeyVariable) {
        std::vector<LINE_NUM> vec = key.second;
        modifiesMapByLineNumKeyVariable[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortAssignDataStructure() {
    for (auto const& key : assignLineNumPrefixExprMapKeyVariable) {
        std::vector<std::pair<LINE_NUM, PREFIX_EXPR>> vec = key.second;
        std::unordered_set<std::pair<LINE_NUM, PREFIX_EXPR>, pair_hash> s(vec.begin(), vec.end());
        std::vector<std::pair<LINE_NUM, PREFIX_EXPR>> finalVec(s.begin(), s.end());
        std::sort(finalVec.begin(), finalVec.end());
        assignLineNumPrefixExprMapKeyVariable[key.first] = finalVec;
    }

    for (auto const& key : assignLineNumMapKeyVariable) {
        std::vector<LINE_NUM> vec = key.second;
        assignLineNumMapKeyVariable[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : assignLineNumVariableMapKeyPrefixExpr) {
        std::vector<std::pair<LINE_NUM, VAR_NAME>> vec = key.second;
        std::unordered_set<std::pair<LINE_NUM, VAR_NAME>, pair_hash> s(vec.begin(), vec.end());
        std::vector<std::pair<LINE_NUM, VAR_NAME>> finalVec(s.begin(), s.end());
        std::sort(finalVec.begin(), finalVec.end());
        assignLineNumVariableMapKeyPrefixExpr[key.first] = finalVec;
    }
}

void PKB::sortNextDataStructure() {
    for (auto const& key : nextMapKeyLineBefore) {
        std::vector<LINE_NUM> vec = key.second;
        nextMapKeyLineBefore[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : nextMapKeyLineAfter) {
        std::vector<LINE_NUM> vec = key.second;
        nextMapKeyLineAfter[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortCallsDataStructure() {
    for (auto const& key : callsMapKeyCaller) {
        std::vector<PROC_NAME> vec = key.second;
        callsMapKeyCaller[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : callsMapKeyCallee) {
        std::vector<PROC_NAME> vec = key.second;
        callsMapKeyCallee[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : callsStarMapKeyCaller) {
        std::vector<PROC_NAME> vec = key.second;
        callsStarMapKeyCaller[key.first] = removeDuplicateAndSortVector(&vec);
    }

    for (auto const& key : callsStarMapKeyCallee) {
        std::vector<PROC_NAME> vec = key.second;
        callsStarMapKeyCallee[key.first] = removeDuplicateAndSortVector(&vec);
    }
}

void PKB::sortDesignEntities() {
    sortedAssignList = setToSortedVector(&statementAssignSet);
    sortedPrintList = setToSortedVector(&statementPrintSet);
    sortedReadList = setToSortedVector(&statementReadSet);
    sortedWhileList = setToSortedVector(&statementWhileSet);
    sortedIfList = setToSortedVector(&statementIfSet);
    sortedCallList = setToSortedVector(&statementCallSet);
    sortedParentsList = sortedIfList;
    for (LINE_NUM line : sortedWhileList)
        sortedParentsList.push_back(line);
    std::sort(sortedParentsList.begin(), sortedParentsList.end());
    sortedProcedureList = setToSortedVector(&procedureSet);
    sortedConstantList = setToSortedVector(&constantSet);
    sortedVariableList = setToSortedVector(&variableSet);
}
