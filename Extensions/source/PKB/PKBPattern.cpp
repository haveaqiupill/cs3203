#include "PKB.h"

void PKB::storeFullPrefixExpr(LINE_NUM lineNum, VAR_NAME assignee, PREFIX_EXPR expr) {
    this->assignPrefixExprSet.insert(expr);

    std::pair<LINE_NUM, PREFIX_EXPR> lineExprPair = std::make_pair(lineNum, expr);
    this->assignLineNumPrefixExprSet.insert(lineExprPair);

    std::pair<VAR_NAME, PREFIX_EXPR> varExprPair = std::make_pair(assignee, expr);
    this->assignVariablePrefixExprSet.insert(varExprPair);
    this->assignVariableExpressionKeyLineNum.insert({ lineNum, varExprPair });

    std::pair<LINE_NUM, VAR_NAME> lineVarPair = std::make_pair(lineNum, assignee);
    this->assignLineNumVariableSet.insert(lineVarPair);

    auto searchVar = this->assignLineNumPrefixExprMapKeyVariable.find(assignee);
    if (searchVar == this->assignLineNumPrefixExprMapKeyVariable.end()) {
        this->assignLineNumPrefixExprMapKeyVariable.insert(
            { assignee, std::vector<std::pair<LINE_NUM, PREFIX_EXPR>>() });
    }
    this->assignLineNumPrefixExprMapKeyVariable[assignee].push_back(lineExprPair);

    auto searchVar2 = this->assignLineNumMapKeyVariable.find(assignee);
    if (searchVar2 == this->assignLineNumMapKeyVariable.end()) {
        this->assignLineNumMapKeyVariable.insert(
            { assignee, std::vector<LINE_NUM>() });
    }
    this->assignLineNumMapKeyVariable[assignee].push_back(lineNum);

    auto searchExpr = this->assignLineNumVariableMapKeyPrefixExpr.find(expr);
    if (searchExpr == this->assignLineNumVariableMapKeyPrefixExpr.end()) {
        this->assignLineNumVariableMapKeyPrefixExpr.insert(
            { expr, std::vector<std::pair<LINE_NUM, VAR_NAME>>() });
    }
    this->assignLineNumVariableMapKeyPrefixExpr[expr].push_back(lineVarPair);
}

void PKB::storePartialPrefixExpr(LINE_NUM lineNum, VAR_NAME assignee, PREFIX_EXPR expr) {
    this->assignPartialPrefixExprSet.insert(expr);

    std::pair<LINE_NUM, PREFIX_EXPR> lineExprPair = std::make_pair(lineNum, expr);
    this->assignLineNumPartialPrefixExprSet.insert(lineExprPair);

    std::pair<VAR_NAME, PREFIX_EXPR> varExprPair = std::make_pair(assignee, expr);
    this->assignVariablePartialPrefixExprSet.insert(varExprPair);
    this->assignVariablePartialExpressionKeyLineNum.insert({ lineNum, varExprPair });

    auto searchVar = this->assignLineNumPartialPrefixExprMapKeyVariable.find(assignee);
    if (searchVar == this->assignLineNumPartialPrefixExprMapKeyVariable.end()) {
        this->assignLineNumPartialPrefixExprMapKeyVariable.insert(
            { assignee, std::vector<std::pair<LINE_NUM, PREFIX_EXPR>>() });
    }
    this->assignLineNumPartialPrefixExprMapKeyVariable[assignee].push_back(lineExprPair);

    auto searchExpr = this->assignLineNumVariableMapKeyPartialPrefixExpr.find(expr);
    if (searchExpr == this->assignLineNumVariableMapKeyPartialPrefixExpr.end()) {
        this->assignLineNumVariableMapKeyPartialPrefixExpr.insert(
            { expr, std::vector<std::pair<LINE_NUM, VAR_NAME>>() });
    }
    std::pair<LINE_NUM, VAR_NAME> lineVarPair = std::make_pair(lineNum, assignee);
    this->assignLineNumVariableMapKeyPartialPrefixExpr[expr].push_back(lineVarPair);
}

std::vector<LINE_NUM> PKB::getAssignLineNumByVariable(VAR_NAME variable) {
    // Select a pattern a("x", _)
    if (isVariableInAssignLineNumMap(variable)) {
        return assignLineNumMapKeyVariable[variable];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<std::pair<LINE_NUM, VAR_NAME>>
PKB::getAssignLineNumVariableByPrefixExpr(PREFIX_EXPR prefixExpr) {
    // Select a pattern a(_, "1 + 2")
    if (isPrefixExprInLineNumVariableMap(prefixExpr)) {
        return assignLineNumVariableMapKeyPrefixExpr[prefixExpr];
    } else {
        return std::vector<std::pair<LINE_NUM, VAR_NAME>>();
    }
}

bool PKB::isVariableInAssignLineNumMap(VAR_NAME variable) {
    return assignLineNumMapKeyVariable.find(variable) != assignLineNumMapKeyVariable.end();
}

bool PKB::isPrefixExprInLineNumVariableMap(PREFIX_EXPR prefixExpr) {
    return assignLineNumVariableMapKeyPrefixExpr.find(prefixExpr)
        != assignLineNumVariableMapKeyPrefixExpr.end();
}

std::vector<std::pair<LINE_NUM, VAR_NAME>> PKB::getAllAssignLineNumVariable() {
    // Returns all the <line_num, var_name> pairs in the program as a vector
    std::vector<std::pair<LINE_NUM, VAR_NAME>> lineVarVector;
    lineVarVector.assign(assignLineNumVariableSet.begin(), assignLineNumVariableSet.end());
    return lineVarVector;
}

std::vector<LINE_NUM> PKB::getAssignLineNumByVarPrefixExpr(VAR_NAME variable, PREFIX_EXPR expr) {
    // Select a pattern a("x", "1 + 2")
    auto prefixMatch = getAssignLineNumVariableByPrefixExpr(expr);
    std::vector<LINE_NUM> result;
    for (auto pair : prefixMatch) {
        if (pair.second == variable) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::vector<std::pair<LINE_NUM, VAR_NAME>>
    PKB::getAssignLineNumVariableByPartialPrefixExpr(PREFIX_EXPR prefixExpr) {
    if (isPartialPrefixExprInLineNumVariableMap(prefixExpr)) {
        return assignLineNumVariableMapKeyPartialPrefixExpr[prefixExpr];
    } else {
        return std::vector<std::pair<LINE_NUM, VAR_NAME>>();
    }
}

bool PKB::isPartialPrefixExprInLineNumVariableMap(PREFIX_EXPR prefixExpr) {
    return assignLineNumVariableMapKeyPartialPrefixExpr.find(prefixExpr)
        != assignLineNumVariableMapKeyPartialPrefixExpr.end();
}

std::vector<LINE_NUM> PKB::getAssignLineNumByVarPartialPrefixExpr
    (VAR_NAME variable, PREFIX_EXPR expr) {
    auto prefixMatch = getAssignLineNumVariableByPartialPrefixExpr(expr);
    std::vector<LINE_NUM> result;
    for (auto pair : prefixMatch) {
        if (pair.second == variable) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getAllIfLineNumByVariable(VAR_NAME var) {
    // pattern if("x1", _, _)
    if (ifMapByVarName.find(var) == ifMapByVarName.end()) {
        return std::vector<LINE_NUM>();
    }
    return ifMapByVarName[var];
}

std::vector<std::pair<LINE_NUM, VAR_NAME>> PKB::getAllIfLineNumByVariable() {
    // pattern if(v, _, _)
    std::vector<std::pair<LINE_NUM, VAR_NAME>> result;
    for (auto& pair : ifMapByVarName) {
        auto toSearch = pair.second;
        VAR_NAME var = pair.first;
        for (LINE_NUM lineNum : toSearch) {
            result.push_back({ lineNum, var });
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getAllWhileLineNumByVariable(VAR_NAME var) {
    // pattern while("x1", _)
    if (whileMapByVarName.find(var) == whileMapByVarName.end()) {
        return std::vector<LINE_NUM>();
    }
    return whileMapByVarName[var];
}

std::vector<std::pair<LINE_NUM, VAR_NAME>> PKB::getAllWhileLineNumByVariable() {
    // pattern while(v, _)
    std::vector<std::pair<LINE_NUM, VAR_NAME>> result;
    for (auto& pair : whileMapByVarName) {
        auto toSearch = pair.second;
        VAR_NAME var = pair.first;
        for (LINE_NUM lineNum : toSearch) {
            result.push_back({ lineNum, var });
        }
    }
    return result;
}
