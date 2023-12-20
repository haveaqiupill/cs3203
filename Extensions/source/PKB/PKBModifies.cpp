#include "PKB.h"
#include "../Commons/Exceptions.h"

void PKB::storeRelationshipModifiesByProcedure(PROC_NAME procedure, VAR_NAME variable) {
    std::pair<PROC_NAME, VAR_NAME> procVar = std::make_pair(procedure, variable);
    this->modifiesSetByProcedure.insert(procVar);

    auto searchProc = this->modifiesMapByProcedureKeyProcedure.find(procedure);
    if (searchProc == this->modifiesMapByProcedureKeyProcedure.end()) {
        this->modifiesMapByProcedureKeyProcedure.insert({ procedure, std::vector<VAR_NAME>() });
    }
    this->modifiesMapByProcedureKeyProcedure[procedure].push_back(variable);

    auto searchVar = this->modifiesMapByProcedureKeyVariable.find(variable);
    if (searchVar == this->modifiesMapByProcedureKeyVariable.end()) {
        this->modifiesMapByProcedureKeyVariable.insert({ variable, std::vector<PROC_NAME>() });
    }
    this->modifiesMapByProcedureKeyVariable[variable].push_back(procedure);
}

void PKB::storeRelationshipModifiesByLineNum(LINE_NUM line, VAR_NAME var_name) {
    std::pair<LINE_NUM, VAR_NAME> temp = std::make_pair(line, var_name);
    this->modifiesSetByLineNum.insert(temp);

    auto searchLine = this->modifiesMapByLineNumKeyLineNum.find(line);
    if (searchLine == this->modifiesMapByLineNumKeyLineNum.end()) {  // element not in map yet
        this->modifiesMapByLineNumKeyLineNum.insert({ line, std::vector<VAR_NAME>() });
    }
    this->modifiesMapByLineNumKeyLineNum[line].push_back(var_name);

    auto searchVar = this->modifiesMapByLineNumKeyVariable.find(var_name);
    if (searchVar == this->modifiesMapByLineNumKeyVariable.end()) {  // element not in map yet
        this->modifiesMapByLineNumKeyVariable.insert({ var_name, std::vector<LINE_NUM>() });
    }
    this->modifiesMapByLineNumKeyVariable[var_name].push_back(line);
}

bool PKB::doesProcedureModifies(PROC_NAME procedure, VAR_NAME variable) {
    // Select v such that Modifies("main", "x")
    std::pair<PROC_NAME, VAR_NAME> modifiesByProcedurePair(procedure, variable);
    return modifiesSetByProcedure.find(modifiesByProcedurePair) != modifiesSetByProcedure.end();
}

bool PKB::doesLineNumModifies(LINE_NUM lineNum, VAR_NAME variable) {
    // Select v such that Modifies(1, "x")
    std::pair<LINE_NUM, VAR_NAME> modifiesByLineNumPair(lineNum, variable);
    return modifiesSetByLineNum.find(modifiesByLineNumPair) != modifiesSetByLineNum.end();
}

std::vector<VAR_NAME> PKB::getVariableFromModifiesByProcedure(PROC_NAME procedure) {
    // Select v such that Modifies("main", v)
    if (doesProcedureModifiesWildcard(procedure)) {
        return modifiesMapByProcedureKeyProcedure[procedure];
    } else {
        return std::vector<VAR_NAME>();
    }
}

std::vector<VAR_NAME> PKB::getVariableFromModifiesByLineNum(LINE_NUM lineNum) {
    // Select v such that Modifies(1, v)
    if (doesLineNumModifiesWildcard(lineNum)) {
        return modifiesMapByLineNumKeyLineNum[lineNum];
    } else {
        return std::vector<VAR_NAME>();
    }
}

std::vector<PROC_NAME> PKB::getProcedureFromModifiesByProcedure(VAR_NAME variable) {
    // Select p such that Modifies(p, "x")
    if (isVariableModified(variable)) {
        return modifiesMapByProcedureKeyVariable[variable];
    } else {
        return std::vector<PROC_NAME>();
    }
}

std::vector<LINE_NUM> PKB::getLineNumFromModifiesByVarName(VAR_NAME variable) {
    // Select s such that Modifies(s, "x")
    if (isVariableModified(variable)) {
        return modifiesMapByLineNumKeyVariable[variable];
    } else {
        return std::vector<LINE_NUM>();
    }
}

bool PKB::doesLineNumModifiesWildcard(LINE_NUM lineNum) {
    // Select BOOLEAN such that Modifies(1, _)
    return modifiesMapByLineNumKeyLineNum.find(lineNum) != modifiesMapByLineNumKeyLineNum.end();
}

bool PKB::doesProcedureModifiesWildcard(PROC_NAME procName) {
    // Select BOOLEAN such that Modifies("main", _)
    return modifiesMapByProcedureKeyProcedure.find(procName)
        != modifiesMapByProcedureKeyProcedure.end();
}

std::vector<std::string> PKB::getModifiesVector(DESIGN_ENTITY designEntity) {
    // Select s such that Modifies(s, _)
    std::vector<std::string> stringVector;

    switch (designEntity) {
    case DesignEntity::PRINT:
        throw InvalidDesignEntity();
    case DesignEntity::PROCEDURE:
        for (auto& procName : sortedProcedureList) {
            if (doesProcedureModifiesWildcard(procName)) {
                stringVector.push_back(procName);
            }
        }
        break;
    default:
        std::vector<LINE_NUM>* designEntityList = selectDataStructure(designEntity);
        for (auto& lineNum : *designEntityList) {
            if (doesLineNumModifiesWildcard(lineNum)) {
                stringVector.push_back(std::to_string(lineNum));
            }
        }
    }
    return stringVector;
}

std::vector<std::string> PKB::getModifiesByVarName(DESIGN_ENTITY designEntity, VAR_NAME var) {
    // Select s such that Modifies(s, "x")
    std::vector<std::string> stringVector;

    switch (designEntity) {
    case DesignEntity::PRINT:
        throw InvalidDesignEntity();
        break;
    case DesignEntity::PROCEDURE:
        return getProcedureFromModifiesByProcedure(var);
    default:
        std::unordered_set<LINE_NUM>* designEntityList = selectDataSet(designEntity);
        std::vector<LINE_NUM> modifiesVarList = getLineNumFromModifiesByVarName(var);
        for (auto& lineNum : modifiesVarList) {
            if (designEntityList->find(lineNum) != designEntityList->end()) {
                stringVector.push_back(std::to_string(lineNum));
            }
        }
    }
    return stringVector;
}

std::vector<std::pair<std::string, VAR_NAME>> PKB::getModifiesPairVector(DESIGN_ENTITY left,
    DESIGN_ENTITY right) {
    // Select <s, v> such that Modifies(s, v)
    std::vector<std::pair<std::string, VAR_NAME>> pairVector;

    switch (left) {
    case DesignEntity::PRINT:
        throw InvalidDesignEntity();
    case DesignEntity::PROCEDURE:
        for (auto& pair : modifiesSetByProcedure) {
            pairVector.push_back(pair);
        }
        break;
    default:
        std::unordered_set<LINE_NUM>* designEntitySet = selectDataSet(left);
        for (auto& pair : modifiesSetByLineNum) {
            if (designEntitySet->find(pair.first) != designEntitySet->end()) {
                std::pair<std::string, VAR_NAME> newPair{ std::to_string(pair.first), pair.second };
                pairVector.push_back(newPair);
            }
        }
    }
    return pairVector;
}

bool PKB::isVariableModified(VAR_NAME var) {
    return modifiesMapByLineNumKeyVariable.find(var) != modifiesMapByLineNumKeyVariable.end()
        || modifiesMapByProcedureKeyVariable.find(var) != modifiesMapByProcedureKeyVariable.end();
}
