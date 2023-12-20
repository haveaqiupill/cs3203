#include "PKB.h"

void PKB::storeRelationshipCalls(PROC_NAME caller, PROC_NAME callee) {
    std::pair <PROC_NAME, PROC_NAME> temp = std::make_pair(caller, callee);
    this->callsSet.insert(temp);

    auto searchCaller = this->callsMapKeyCaller.find(caller);
    if (searchCaller == this->callsMapKeyCaller.end()) {
        this->callsMapKeyCaller.insert({ caller, std::vector<PROC_NAME>() });
    }
    this->callsMapKeyCaller[caller].push_back(callee);

    auto searchCallee = this->callsMapKeyCallee.find(callee);
    if (searchCallee == this->callsMapKeyCallee.end()) {
        this->callsMapKeyCallee.insert({ callee, std::vector<PROC_NAME>() });
    }
    this->callsMapKeyCallee[callee].push_back(caller);

    storeRelationshipCallsStar(caller, callee);
}

void PKB::storeRelationshipCallsStar(PROC_NAME caller, PROC_NAME callee) {
    std::pair <PROC_NAME, PROC_NAME> temp = std::make_pair(caller, callee);
    this->callsStarSet.insert(temp);

    auto searchCaller = this->callsStarMapKeyCaller.find(caller);
    if (searchCaller == this->callsStarMapKeyCaller.end()) {
        this->callsStarMapKeyCaller.insert({ caller, std::vector<PROC_NAME>() });
    }
    this->callsStarMapKeyCaller[caller].push_back(callee);

    auto searchCallee = this->callsStarMapKeyCallee.find(callee);
    if (searchCallee == this->callsStarMapKeyCallee.end()) {
        this->callsStarMapKeyCallee.insert({ callee, std::vector<PROC_NAME>() });
    }
    this->callsStarMapKeyCallee[callee].push_back(caller);
}

bool PKB::doesProcedureCalls(PROC_NAME caller, PROC_NAME callee) {
    // Select c such that Calls("Main", "Hi")
    std::pair<PROC_NAME, PROC_NAME> searchPair(caller, callee);
    return callsSet.find(searchPair) != callsSet.end();
}

bool PKB::hasCallsRelationship() {
    return callsSet.size() != 0;
}

bool PKB::hasCallers(PROC_NAME proc) {
    // Select c such that Calls(_, "hello")
    // can also be used for Calls*
    return callsMapKeyCallee.find(proc) != callsMapKeyCallee.end();
}

bool PKB::hasCallees(PROC_NAME proc) {
    // Select c such that Calls("hello", _)
    // can also be used for Calls*
    return callsMapKeyCaller.find(proc) != callsMapKeyCaller.end();
}

std::vector<PROC_NAME> PKB::getAllCallers() {
    // Select p such that Calls(p, _)
    // can also be used for Calls*
    std::vector<PROC_NAME> result;
    for (auto pair : callsMapKeyCaller) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<PROC_NAME> PKB::getAllCallees() {
    // Select p such that Calls(_, p)
    // can also be used for Calls*
    std::vector<PROC_NAME> result;
    for (auto pair : callsMapKeyCallee) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<PROC_NAME> PKB::getCallers(PROC_NAME callee) {
    // Select p such that Calls(p, "second")
    if (hasCallers(callee)) {
        return callsMapKeyCallee[callee];
    } else {
        return std::vector<PROC_NAME>();
    }
}

std::vector<PROC_NAME> PKB::getCallees(PROC_NAME caller) {
    // Select p such that Calls("main", p)
    if (hasCallees(caller)) {
        return callsMapKeyCaller[caller];
    } else {
        return std::vector<PROC_NAME>();
    }
}

std::vector<std::pair<PROC_NAME, PROC_NAME>> PKB::getCallsPair
    (DESIGN_ENTITY caller, DESIGN_ENTITY callee) {
    // Select <p1, p2> such that Calls(p1, p2)
    std::vector<std::pair<PROC_NAME, PROC_NAME>> result;
    for (auto& pair : callsSet) {
        result.push_back(pair);
    }
    return result;
}

bool PKB::doesProcedureCallsStar(PROC_NAME caller, PROC_NAME callee) {
    std::pair<PROC_NAME, PROC_NAME> searchPair(caller, callee);
    return callsStarSet.find(searchPair) != callsStarSet.end();
}

std::vector<PROC_NAME> PKB::getCallersStar(PROC_NAME callee) {
    if (callsStarMapKeyCallee.find(callee) != callsStarMapKeyCallee.end()) {
        return callsStarMapKeyCallee[callee];
    } else {
        return std::vector<PROC_NAME>();
    }
}

std::vector<PROC_NAME> PKB::getCalleesStar(PROC_NAME caller) {
    if (callsStarMapKeyCaller.find(caller) != callsStarMapKeyCaller.end()) {
        return callsStarMapKeyCaller[caller];
    } else {
        return std::vector<PROC_NAME>();
    }
}

std::vector<std::pair<PROC_NAME, PROC_NAME>> PKB::getCallsStarPair
    (DESIGN_ENTITY caller, DESIGN_ENTITY callee) {
    std::vector<std::pair<PROC_NAME, PROC_NAME>> result;
    for (auto& pair : callsStarSet) {
        result.push_back(pair);
    }
    return result;
}
