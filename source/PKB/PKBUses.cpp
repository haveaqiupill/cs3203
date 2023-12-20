#include "PKB.h"
#include "../Commons/Exceptions.h"

void PKB::storeRelationshipUsesByProcedure(PROC_NAME procedure, VAR_NAME variable) {
    std::pair<PROC_NAME, VAR_NAME> temp = std::make_pair(procedure, variable);
    this->usesSetByProcedure.insert(temp);

    auto searchProc = this->usesMapByProcedureKeyProcedure.find(procedure);
    if (searchProc == this->usesMapByProcedureKeyProcedure.end()) {
        this->usesMapByProcedureKeyProcedure.insert({ procedure, std::vector<VAR_NAME>() });
    }
    this->usesMapByProcedureKeyProcedure[procedure].push_back(variable);

    auto searchVar = this->usesMapByProcedureKeyVariable.find(variable);
    if (searchVar == this->usesMapByProcedureKeyVariable.end()) {
        this->usesMapByProcedureKeyVariable.insert({ variable, std::vector<PROC_NAME>() });
    }
    this->usesMapByProcedureKeyVariable[variable].push_back(procedure);
}

void PKB::storeRelationshipUsesByLineNum(LINE_NUM line, VAR_NAME var_name) {
    std::pair<LINE_NUM, VAR_NAME> temp = std::make_pair(line, var_name);
    this->usesSetByLineNum.insert(temp);

    auto searchLine = this->usesMapByLineNumKeyLineNum.find(line);
    if (searchLine == this->usesMapByLineNumKeyLineNum.end()) {
        this->usesMapByLineNumKeyLineNum.insert({ line, std::vector<VAR_NAME>() });
    }
    this->usesMapByLineNumKeyLineNum[line].push_back(var_name);

    auto searchVar = this->usesMapByLineNumKeyVariable.find(var_name);
    if (searchVar == this->usesMapByLineNumKeyVariable.end()) {
        this->usesMapByLineNumKeyVariable.insert({ var_name, std::vector<LINE_NUM>() });
    }
    this->usesMapByLineNumKeyVariable[var_name].push_back(line);
}

bool PKB::doesProcedureUses(PROC_NAME procedure, VAR_NAME variable) {
    // Select v such that Uses("main", "x")
    std::pair<PROC_NAME, VAR_NAME> usesByProcedurePair(procedure, variable);
    return usesSetByProcedure.find(usesByProcedurePair) != usesSetByProcedure.end();
}

bool PKB::doesLineNumUses(LINE_NUM lineNum, VAR_NAME variable) {
    // Select v such that Uses(1, "x")
    std::pair<LINE_NUM, VAR_NAME> usesByLineNumPair(lineNum, variable);
    return usesSetByLineNum.find(usesByLineNumPair) != usesSetByLineNum.end();
}

std::vector<VAR_NAME> PKB::getVariableFromUsesByProcedure(PROC_NAME procedure) {
    // Select v such that Uses("main", v)
    if (doesProcedureUsesWildcard(procedure)) {
        return usesMapByProcedureKeyProcedure[procedure];
    } else {
        return std::vector<VAR_NAME>();
    }
}

std::vector<VAR_NAME> PKB::getVariableFromUsesByLineNum(LINE_NUM lineNum) {
    // Select v such that Uses(1, v)
    if (doesLineNumUsesWildcard(lineNum)) {
        return usesMapByLineNumKeyLineNum[lineNum];
    } else {
        return std::vector<VAR_NAME>();
    }
}

std::vector<PROC_NAME> PKB::getProcedureFromUsesByProcedure(VAR_NAME variable) {
    // Select p such that Uses(p, "x")
    if (usesMapByProcedureKeyVariable.find(variable) != usesMapByProcedureKeyVariable.end()) {
        return usesMapByProcedureKeyVariable[variable];
    } else {
        return std::vector<PROC_NAME>();
    }
}

std::vector<LINE_NUM> PKB::getLineNumFromUsesByVarName(VAR_NAME variable) {
    // Select s such that Uses(s, "x")
    if (usesMapByLineNumKeyVariable.find(variable) != usesMapByLineNumKeyVariable.end()) {
        return usesMapByLineNumKeyVariable[variable];
    } else {
        return std::vector<LINE_NUM>();
    }
}

bool PKB::doesLineNumUsesWildcard(LINE_NUM lineNum) {
    // Select BOOLEAN such that Uses(1, _)
    return usesMapByLineNumKeyLineNum.find(lineNum) != usesMapByLineNumKeyLineNum.end();
}

bool PKB::doesProcedureUsesWildcard(PROC_NAME procName) {
    // Select BOOLEAN such that Uses("main", _)
    return usesMapByProcedureKeyProcedure.find(procName) != usesMapByProcedureKeyProcedure.end();
}

std::vector<std::string> PKB::getUsesVector(DESIGN_ENTITY designEntity) {
    // Select s such that Uses(s, _)
    std::vector<std::string> stringVector;
    std::vector<LINE_NUM>* designEntityList;

    switch (designEntity) {
    case DesignEntity::READ:
        throw InvalidDesignEntity();
    case DesignEntity::PROCEDURE:
        for (auto& procName : sortedProcedureList) {
            if (doesProcedureUsesWildcard(procName)) {
                stringVector.push_back(procName);
            }
        }
        break;
    default:
        designEntityList = selectDataStructure(designEntity);
        for (auto& lineNum : *designEntityList) {
            if (doesLineNumUsesWildcard(lineNum)) {
                stringVector.push_back(std::to_string(lineNum));
            }
        }
    }

    return stringVector;
}

std::vector<std::string> PKB::getUsesByVarName(DESIGN_ENTITY designEntity, VAR_NAME var) {
    // Select s such that Uses(s, "x")
    std::vector<std::string> stringVector;

    switch (designEntity) {
    case DesignEntity::READ:
        throw InvalidDesignEntity();
    case DesignEntity::PROCEDURE:
        return getProcedureFromUsesByProcedure(var);
    default:
        if (isVariableUsed(var)) {
            std::unordered_set<LINE_NUM>* designEntitySet = selectDataSet(designEntity);
            for (LINE_NUM lineNum : usesMapByLineNumKeyVariable[var]) {
                if (designEntitySet->find(lineNum) != designEntitySet->end()) {
                    stringVector.push_back(std::to_string(lineNum));
                }
            }
        }
    }
    return stringVector;
}

std::vector<std::pair<std::string, VAR_NAME>> PKB::getUsesPairVector(DESIGN_ENTITY left,
    DESIGN_ENTITY right) {
    // Select <s, v> such that uses (s, v)
    std::vector<std::pair<std::string, VAR_NAME>> pairVector;

    switch (left) {
    case DesignEntity::READ:
        throw InvalidDesignEntity();
    case DesignEntity::PROCEDURE:
        for (auto& pair : usesSetByProcedure) {
            pairVector.push_back(pair);
        }
        break;
    default:
        std::unordered_set<LINE_NUM>* designEntitySet = selectDataSet(left);
        for (auto& pair : usesSetByLineNum) {
            if (designEntitySet->find(pair.first) != designEntitySet->end()) {
                std::pair<std::string, VAR_NAME> newPair{std::to_string(pair.first), pair.second};
                pairVector.push_back(newPair);
            }
        }
    }
    return pairVector;
}

bool PKB::isVariableUsed(VAR_NAME var) {
    return usesMapByLineNumKeyVariable.find(var) != usesMapByLineNumKeyVariable.end()
        || usesMapByProcedureKeyVariable.find(var) != usesMapByProcedureKeyVariable.end();
}
