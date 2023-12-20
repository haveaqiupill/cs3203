#include "PKB.h"
#include "../Commons/Exceptions.h"
#include "../Commons/Exceptions/DuplicateProcedureException.h"

void PKB::propagateStmtList(LINE_NUM lastline) {
    for (int i = 1; i <= lastline; i++) {
        this->sortedStatementList.push_back(i);
        this->statementSet.insert(i);
    }
}

void PKB::storeConstant(CONSTANT value) {
    this->constantSet.emplace(value);
}

void PKB::storeVariable(VAR_NAME name) {
    this->variableSet.emplace(name);
}

void PKB::storeProcedure(PROC_NAME procedure, LINE_NUM firstLine, LINE_NUM lastLine,
    std::list<LINE_NUM> listEndPoints) {
    if (!(this->procedureSet.insert(procedure).second)) {
        throw DuplicateProcedureException(procedure);
    }
    std::pair<LINE_NUM, LINE_NUM> linePair = std::make_pair(firstLine, lastLine);
    this->procedureMap.insert({ procedure, linePair });

    this->procedureEndPointsMapping.insert(
        { procedure, std::vector<LINE_NUM>(listEndPoints.begin(), listEndPoints.end()) });
}

void PKB::storeReadStatement(LINE_NUM lineNum, VAR_NAME variable) {
    this->statementReadSet.insert(lineNum);
    readMapByLineNum.insert({ lineNum, variable });
    if (readMapByVarName.find(variable) == readMapByVarName.end()) {
        readMapByVarName.insert({ variable, std::vector<LINE_NUM>() });
    }
    readMapByVarName[variable].push_back(lineNum);
}

void PKB::storePrintStatement(LINE_NUM lineNum, VAR_NAME variable) {
    this->statementPrintSet.insert(lineNum);
    printMapByLineNum.insert({ lineNum, variable });
    if (printMapByVarName.find(variable) == printMapByVarName.end()) {
        printMapByVarName.insert({ variable, std::vector<LINE_NUM>() });
    }
    printMapByVarName[variable].push_back(lineNum);
}

void PKB::storeAssignStatement(LINE_NUM lineNum, VAR_NAME assignee, PREFIX_EXPR expr) {
    this->statementAssignSet.insert(lineNum);
    this->storeFullPrefixExpr(lineNum, assignee, expr);
}

void PKB::storeIfElseStatement(LINE_NUM lineNum, std::list<VAR_NAME> variables) {
    this->statementIfSet.insert(lineNum);
    for (VAR_NAME var : variables) {
        if (this->ifMapByVarName.find(var) == ifMapByVarName.end()) {
            ifMapByVarName.insert({ var, std::vector<LINE_NUM>() });
        }
        ifMapByVarName[var].push_back(lineNum);
    }
}

void PKB::storeWhileStatement(LINE_NUM lineNum, std::list<VAR_NAME> variables) {
    this->statementWhileSet.insert(lineNum);
    for (VAR_NAME var : variables) {
        if (this->whileMapByVarName.find(var) == whileMapByVarName.end()) {
            whileMapByVarName.insert({ var, std::vector<LINE_NUM>() });
        }
        whileMapByVarName[var].push_back(lineNum);
    }
}

void PKB::storeCallStatement(LINE_NUM lineNum, PROC_NAME callee) {
    this->statementCallSet.insert(lineNum);
    this->callMapByLineNum.insert({ lineNum, callee });
    if (callMapByProcedure.find(callee) == callMapByProcedure.end()) {
        callMapByProcedure.insert({ callee, std::vector<LINE_NUM>() });
    }
    callMapByProcedure[callee].push_back(lineNum);
}

std::pair<VAR_NAME, PREFIX_EXPR> PKB::getExpr(LINE_NUM lineNum) {
    return this->assignVariableExpressionKeyLineNum[lineNum];
}

// Getter method to interact with back-end (PQL)
std::vector<VAR_NAME> PKB::getAllVariables() {
    // Returns all the variables, sorted alphabetically, in the program as a vector
    return sortedVariableList;
}

std::vector<PROC_NAME> PKB::getAllProcedures() {
    // Returns all the procedures, sorted alphabetically, in the program as a vector
    return sortedProcedureList;
}

std::vector<LINE_NUM> PKB::getAllAssignStatements() {
    // Returns vector of line numbers, sorted in ascending order, of all assign statements
    return sortedAssignList;
}

std::vector<LINE_NUM> PKB::getAllWhileStatements() {
    // Returns vector of line numbers, sorted in ascending order, of all while statements
    return sortedWhileList;
}

std::vector<LINE_NUM> PKB::getAllIfStatements() {
    // Returns vector of line numbers, sorted in ascending order, of all if statements
    return sortedIfList;
}

std::vector<PKB::CONSTANT> PKB::getAllConstants() {
    // Returns all the constants, sorted in ascending order, in the program as a vector
    return sortedConstantList;
}

std::unordered_map<LINE_NUM, PROC_NAME> PKB::getCallMap() {
    return callMapByLineNum;
}

std::pair<LINE_NUM, LINE_NUM> PKB::getProc(PROC_NAME procedure) {
    if (procedureMap.find(procedure) != procedureMap.end()) {
        return procedureMap[procedure];
    } else {
        return std::pair<LINE_NUM, LINE_NUM> {0, 0};
    }
}

std::vector<std::pair<LINE_NUM, VAR_NAME>> PKB::getAllReadStatementsWithAttribute() {
    std::vector<std::pair<LINE_NUM, VAR_NAME>> result;
    for (auto pair : readMapByLineNum) {
        result.push_back(pair);
    }
    return result;
}

std::vector<std::pair<LINE_NUM, VAR_NAME>> PKB::getAllPrintStatementsWithAttribute() {
    std::vector<std::pair<LINE_NUM, VAR_NAME>> result;
    for (auto pair : printMapByLineNum) {
        result.push_back(pair);
    }
    return result;
}

std::vector<std::pair<LINE_NUM, PROC_NAME>> PKB::getAllCallStatementsWithAttribute() {
    std::vector<std::pair<LINE_NUM, PROC_NAME>> result;
    for (auto pair : callMapByLineNum) {
        result.push_back(pair);
    }
    return result;
}

std::vector<LINE_NUM> PKB::getLastStmtsOfProc(PROC_NAME proc) {
    if (procedureEndPointsMapping.find(proc) == procedureEndPointsMapping.end()) {
        return std::vector<LINE_NUM> {};
    }
    return procedureEndPointsMapping[proc];
}

std::unordered_set<LINE_NUM>* PKB::selectDataSet(DESIGN_ENTITY designEntity) {
    std::unordered_set<LINE_NUM>* designEntitySet;
    switch (designEntity) {
    case DesignEntity::PROG_LINE:
    case DesignEntity::STMT:
        designEntitySet = &statementSet;
        break;
    case DesignEntity::ASSIGN:
        designEntitySet = &statementAssignSet;
        break;
    case DesignEntity::PRINT:
        designEntitySet = &statementPrintSet;
        break;
    case DesignEntity::READ:
        designEntitySet = &statementReadSet;
        break;
    case DesignEntity::IF:
        designEntitySet = &statementIfSet;
        break;
    case DesignEntity::WHILE:
        designEntitySet = &statementWhileSet;
        break;
    case DesignEntity::CALL:
        designEntitySet = &statementCallSet;
        break;
    default:
        throw InvalidDesignEntity();
    }
    return designEntitySet;
}

std::vector<LINE_NUM>* PKB::selectDataStructure(DESIGN_ENTITY designEntity) {
    std::vector<LINE_NUM>* designEntityVec;
    switch (designEntity) {
    case DesignEntity::PROG_LINE:
    case DesignEntity::STMT:
        designEntityVec = &sortedStatementList;
        break;
    case DesignEntity::ASSIGN:
        designEntityVec = &sortedAssignList;
        break;
    case DesignEntity::PRINT:
        designEntityVec = &sortedPrintList;
        break;
    case DesignEntity::READ:
        designEntityVec = &sortedReadList;
        break;
    case DesignEntity::IF:
        designEntityVec = &sortedIfList;
        break;
    case DesignEntity::WHILE:
        designEntityVec = &sortedWhileList;
        break;
    case DesignEntity::CALL:
        designEntityVec = &sortedCallList;
        break;
    default:
        throw InvalidDesignEntity();
    }
    return designEntityVec;
}

void PKB::sortAllDataStructure() {
    sortFollowsDataStructure();
    sortParentDataStructure();
    sortUsesDataStructure();
    sortModifiesDataStructure();
    sortAssignDataStructure();
    sortNextDataStructure();
    sortNextBipDataStructure();
    sortAffectsBipDataStructure();
    sortCallsDataStructure();
    sortDesignEntities();
}
