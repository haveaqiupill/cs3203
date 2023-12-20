#include <algorithm>

#include "PKB.h"
#include "../Commons/Exceptions.h"

std::vector < std::pair<LINE_NUM, PROC_NAME>> PKB::getCallPairWithLineNum(LINE_NUM lineNum) {
    std::vector < std::pair<LINE_NUM, PROC_NAME>> result;
    if (callMapByLineNum.find(lineNum) != callMapByLineNum.end()) {
        result.push_back({ lineNum, callMapByLineNum[lineNum] });
    }
    return result;
}

std::vector < std::pair<LINE_NUM, PROC_NAME>> PKB::getCallPairWithProcName(PROC_NAME procedure) {
    std::vector < std::pair<LINE_NUM, PROC_NAME>> result;
    if (callMapByProcedure.find(procedure) != callMapByProcedure.end()) {
        auto list = callMapByProcedure[procedure];
        for (auto lineNum : list) {
            result.push_back({ lineNum, procedure });
        }
    }
    return result;
}

std::vector < std::pair<LINE_NUM, VAR_NAME>> PKB::getReadPairWithLineNum(LINE_NUM lineNum) {
    std::vector < std::pair<LINE_NUM, VAR_NAME>> result;
    if (readMapByLineNum.find(lineNum) != readMapByLineNum.end()) {
        result.push_back({ lineNum, readMapByLineNum[lineNum] });
    }
    return result;
}

std::vector < std::pair<LINE_NUM, VAR_NAME>> PKB::getReadPairWithVarName(VAR_NAME variable) {
    std::vector < std::pair<LINE_NUM, VAR_NAME>> result;
    if (readMapByVarName.find(variable) != readMapByVarName.end()) {
        auto list = readMapByVarName[variable];
        for (auto lineNum : list) {
            result.push_back({ lineNum, variable });
        }
    }
    return result;
}

std::vector < std::pair<LINE_NUM, VAR_NAME>> PKB::getPrintPairWithLineNum(LINE_NUM lineNum) {
    std::vector < std::pair<LINE_NUM, VAR_NAME>> result;
    if (printMapByLineNum.find(lineNum) != printMapByLineNum.end()) {
        result.push_back({ lineNum, printMapByLineNum[lineNum] });
    }
    return result;
}

std::vector < std::pair<LINE_NUM, VAR_NAME>> PKB::getPrintPairWithVarName(VAR_NAME variable) {
    std::vector < std::pair<LINE_NUM, VAR_NAME>> result;
    if (printMapByVarName.find(variable) != printMapByVarName.end()) {
        auto list = printMapByVarName[variable];
        for (auto lineNum : list) {
            result.push_back({ lineNum, variable });
        }
    }
    return result;
}

bool PKB::isRightDesignEntity(DESIGN_ENTITY designEntity, LINE_NUM lineNum) {
    auto designEntitySet = selectDataSet(designEntity);
    return designEntitySet->find(lineNum) != designEntitySet->end();
}
bool PKB::isValidVar(VAR_NAME variable) {
    return variableSet.find(variable) != variableSet.end();
}
bool PKB::isValidProc(PROC_NAME procedure) {
    return procedureSet.find(procedure) != procedureSet.end();
}
bool PKB::isValidConstant(CONSTANT constant) {
    return constantSet.find(constant) != constantSet.end();
}

std::vector<std::string> PKB::findCommonStrItems(DESIGN_ENTITY left, DESIGN_ENTITY right) {
    // can only be used for design entity that uses var or proc i.e. call, read, print
    std::vector<std::string> leftList, result;
    std::unordered_set<std::string> rightSet{}, *rightPtr;
    rightPtr = &rightSet;

    switch (left) {
    case DesignEntity::VARIABLE:
        leftList = sortedVariableList;
        break;
    case DesignEntity::PROCEDURE:
        leftList = sortedProcedureList;
        break;
    case DesignEntity::READ:
        for (auto pair : readMapByVarName) {
            leftList.push_back(pair.first);
        }
        break;
    case DESIGN_ENTITY::PRINT:
        for (auto pair : printMapByVarName) {
            leftList.push_back(pair.first);
        }
        break;
    case DESIGN_ENTITY::CALL:
        for (auto pair : callMapByProcedure) {
            leftList.push_back(pair.first);
        }
        break;
    default:
        throw InvalidDesignEntity();
    }

    if (left == right) {
        return leftList;
    }

    switch (right) {
    case DesignEntity::VARIABLE:
        rightPtr = &variableSet;
        break;
    case DesignEntity::PROCEDURE:
        rightPtr = &procedureSet;
        break;
    case DesignEntity::READ:
        for (auto pair : readMapByVarName) {
            rightPtr->insert(pair.first);
        }
        break;
    case DESIGN_ENTITY::PRINT:
        for (auto pair : printMapByVarName) {
            rightPtr->insert(pair.first);
        }
        break;
    case DESIGN_ENTITY::CALL:
        for (auto pair : callMapByProcedure) {
            rightPtr->insert(pair.first);
        }
        break;
    default:
        throw InvalidDesignEntity();
    }

    for (std::string leftString : leftList) {
        if (rightPtr->find(leftString) != rightPtr->end()) {
            result.push_back(leftString);
        }
    }
    return result;
}

std::vector<int> PKB::findCommonIntItems(DESIGN_ENTITY left, DESIGN_ENTITY right) {
    // can either be constant or synonym
    std::vector<int> leftList, result;
    std::unordered_set<int> rightSet{}, *rightPtr;
    rightPtr = &rightSet;
    switch (left) {
    case DesignEntity::CONSTANT:
        // current implementation of constant is a string
        for (CONSTANT c : sortedConstantList) {
            int value = std::stoi(c);
            leftList.push_back(value);
        }
        break;
    default:
        leftList = *selectDataStructure(left);
    }

    if (left == right) {
        return leftList;
    }

    switch (right) {
    case DesignEntity::CONSTANT:
        // current implementation of constant is a string
        for (CONSTANT c : sortedConstantList) {
            int value = std::stoi(c);
            rightPtr->insert(value);
        }
        break;
    default:
        rightPtr = selectDataSet(right);
    }

    for (int leftInt : leftList) {
        if (rightPtr->find(leftInt) != rightPtr->end()) {
            result.push_back(leftInt);
        }
    }
    return result;
}
