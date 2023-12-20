#include <list>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "stdafx.h"
#include "PKB/PKBInterface.h"

class PKBMock : public PKBInterface {
 private:
    std::list<std::string> methodCalls;

 public:
    std::list<std::string> getAllMethodCalls();

    // methods to store releationships within the PKB
    void storeRelationshipFollows(LINE_NUM, LINE_NUM);
    void storeRelationshipFollowsStar(LINE_NUM, LINE_NUM) {}

    void storeRelationshipParent(LINE_NUM, LINE_NUM);
    void storeRelationshipParentStar(LINE_NUM, LINE_NUM) {}

    void storeRelationshipUsesByProcedure(PROC_NAME, VAR_NAME) {}
    void storeRelationshipUsesByLineNum(LINE_NUM, VAR_NAME);

    void storeRelationshipModifiesByProcedure(PROC_NAME, VAR_NAME) {}
    void storeRelationshipModifiesByLineNum(LINE_NUM, VAR_NAME);

    void storeRelationshipNext(LINE_NUM, LINE_NUM);

    void storeRelationshipNextBip(LINE_NUM, LINE_NUM);
    void storeRelationshipNextBip(LINE_NUM, LINE_NUM, LINE_NUM);
    void storeRelationshipNextBipStar(LINE_NUM, LINE_NUM) {}

    void storeRelationshipCalls(PROC_NAME, PROC_NAME) {}
    void storeRelationshipCallsStar(PROC_NAME, PROC_NAME) {}

    void storeRelationshipAffectsBip(LINE_NUM, LINE_NUM) {}
    void storeRelationshipAffectsBipStar(LINE_NUM, LINE_NUM) {}

    // methods to store design entities within the PKB
    void storeConstant(CONSTANT);
    void storeVariable(VAR_NAME);
    void storeProcedure(PROC_NAME, LINE_NUM, LINE_NUM, std::list<LINE_NUM>);
    void storeAssignStatement(LINE_NUM, VAR_NAME, PREFIX_EXPR);
    void storeFullPrefixExpr(LINE_NUM, VAR_NAME, PREFIX_EXPR) {}
    void storePartialPrefixExpr(LINE_NUM, VAR_NAME, PREFIX_EXPR) {}
    void storePrintStatement(LINE_NUM, VAR_NAME);
    void storeReadStatement(LINE_NUM, VAR_NAME);
    void storeIfElseStatement(LINE_NUM, std::list<VAR_NAME>);
    void storeWhileStatement(LINE_NUM, std::list<VAR_NAME>);
    void storeCallStatement(LINE_NUM, PROC_NAME);
    void propagateStmtList(LINE_NUM);

    void sortAllDataStructure() {}

    // API for DE to interact with PKB
    std::pair<VAR_NAME, PREFIX_EXPR> getExpr(LINE_NUM) { return {}; }
    std::pair<LINE_NUM, LINE_NUM> getProc(PROC_NAME) { return {}; }
    std::vector<LINE_NUM> getLastStmtsOfProc(PROC_NAME) { return {}; }

    // GETTERS: API for back-end to interact with PKB
    // Getters for design entities
    std::vector<CONSTANT> getAllConstants() {return {};}
    std::vector<VAR_NAME> getAllVariables() {return {};}
    std::vector<PROC_NAME> getAllProcedures() {return {};}
    std::vector<LINE_NUM> getAllStatements() {return {};}
    std::vector<LINE_NUM> getAllAssignStatements() {return {};}
    std::vector<LINE_NUM> getAllPrintStatements() {return {};}
    std::vector<LINE_NUM> getAllReadStatements() {return {};}
    std::vector<LINE_NUM> getAllWhileStatements() {return {};}
    std::vector<LINE_NUM> getAllIfStatements() {return {};}
    std::unordered_map<LINE_NUM, PROC_NAME> getCallMap() {return {};}
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllReadStatementsWithAttribute() { return {}; }
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllPrintStatementsWithAttribute() { return {}; }
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllCallStatementsWithAttribute() { return {}; }

    // To answer Follows query
    bool doesFollows(LINE_NUM, LINE_NUM) {return true;}
    std::vector<LINE_NUM> getLineAfterFromFollows(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getLineBeforeFromFollows(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getFollowsAfter(LINE_NUM, DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getFollowsAfter(DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getFollowsBefore(DESIGN_ENTITY, LINE_NUM) {return {};}
    std::vector<LINE_NUM> getFollowsBefore(DESIGN_ENTITY) {return {};}
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getFollowsPair(DESIGN_ENTITY, DESIGN_ENTITY) {return {};}

    // To answer Follows* query
    bool doesFollowsStar(LINE_NUM, LINE_NUM) { return true; }
    std::vector<LINE_NUM> getLineAfterFromFollowsStar(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getLineBeforeFromFollowsStar(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getFollowsStarAfter(LINE_NUM, DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getFollowsStarAfter(DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getFollowsStarBefore(DESIGN_ENTITY, LINE_NUM) {return {};}
    std::vector<LINE_NUM> getFollowsStarBefore(DESIGN_ENTITY) {return {};}
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getFollowsStarPair(DESIGN_ENTITY, DESIGN_ENTITY) {return {};}

    // Helper functions for Follows relationship
    bool hasFollowsRelationship() {return true;}
    bool hasFollower(LINE_NUM) {return true;}
    bool hasFollowee(LINE_NUM) {return true;}

    // To answer Parent query
    bool doesParent(LINE_NUM, LINE_NUM) {return true;}
    std::vector<LINE_NUM> getChildLineFromParent(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getParentLineFromParent(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getParentAfter(LINE_NUM, DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getParentAfter(DESIGN_ENTITY) {return {};}
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getParentPair(DESIGN_ENTITY, DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getParentBefore(DESIGN_ENTITY, LINE_NUM) {return {};}
    std::vector<LINE_NUM> getParentBefore(DESIGN_ENTITY) {return {};}


    // To answer Parent* query
    bool doesParentStar(LINE_NUM, LINE_NUM) {return true;}
    std::vector<LINE_NUM> getChildLineFromParentStar(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getParentLineFromParentStar(LINE_NUM) {return {};}
    std::vector<LINE_NUM> getParentStarAfter(LINE_NUM, DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getParentStarAfter(DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getParentStarBefore(DESIGN_ENTITY, LINE_NUM) {return {};}
    std::vector<LINE_NUM> getParentStarBefore(DESIGN_ENTITY) {return {};}
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getParentStarPair(DESIGN_ENTITY, DESIGN_ENTITY) {return {};}

    // Helper function for Parent relationship
    bool hasParentRelationship() {return true;}
    bool isParent(LINE_NUM) {return true;}
    bool isParent(DESIGN_ENTITY) {return true;}
    bool isChild(LINE_NUM) {return true;}

    // To answer uses query
    bool doesProcedureUses(PROC_NAME, VAR_NAME) {return true;}
    bool doesLineNumUses(LINE_NUM, VAR_NAME) {return true;}
    std::vector<VAR_NAME> getVariableFromUsesByProcedure(PROC_NAME) {return {};}
    std::vector<VAR_NAME> getVariableFromUsesByLineNum(LINE_NUM) {return {};}
    std::vector<PROC_NAME> getProcedureFromUsesByProcedure(VAR_NAME) {return {};}
    std::vector<LINE_NUM> getLineNumFromUsesByVarName(VAR_NAME) {return {};}

    bool doesLineNumUsesWildcard(LINE_NUM) {return true;}
    bool doesProcedureUsesWildcard(PROC_NAME) {return true;}
    std::vector<std::string> getUsesVector(DESIGN_ENTITY) {return {};}
    std::vector<std::string> getUsesByVarName(DESIGN_ENTITY, VAR_NAME) {return {};}
    std::vector<std::pair<std::string, VAR_NAME>>
        getUsesPairVector(DESIGN_ENTITY, DESIGN_ENTITY) {return {};}

    std::vector<LINE_NUM> getAllIfLineNumByVariable(VAR_NAME) { return {};}
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllIfLineNumByVariable() { return {}; }
    std::vector<LINE_NUM> getAllWhileLineNumByVariable(VAR_NAME) { return {}; }
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllWhileLineNumByVariable() { return {}; }

    // To answer modifies query
    bool doesProcedureModifies(PROC_NAME, VAR_NAME) {return true;}
    bool doesLineNumModifies(LINE_NUM, VAR_NAME) {return true;}
    std::vector<VAR_NAME> getVariableFromModifiesByProcedure(PROC_NAME) {return {};}
    std::vector<VAR_NAME> getVariableFromModifiesByLineNum(LINE_NUM) {return {};}
    std::vector<PROC_NAME> getProcedureFromModifiesByProcedure(VAR_NAME) {return {};}
    std::vector<LINE_NUM> getLineNumFromModifiesByVarName(VAR_NAME) {return {};}

    bool doesLineNumModifiesWildcard(LINE_NUM) {return true;}
    bool doesProcedureModifiesWildcard(PROC_NAME) {return true;}
    std::vector<std::string> getModifiesVector(DESIGN_ENTITY) {return {};}
    std::vector<std::string> getModifiesByVarName(DESIGN_ENTITY, VAR_NAME) {return {};}
    std::vector<std::pair<std::string, VAR_NAME>>
        getModifiesPairVector(DESIGN_ENTITY, DESIGN_ENTITY) {return {};}

    // To answer pattern query
    // 1. Assign statement
    std::vector<LINE_NUM> getAssignLineNumByVariable(VAR_NAME) { return {}; }
    std::vector<std::pair<LINE_NUM, VAR_NAME>>
        getAssignLineNumVariableByPrefixExpr(PREFIX_EXPR) { return {}; }
    std::vector<LINE_NUM> getAssignLineNumByVarPrefixExpr(VAR_NAME, PREFIX_EXPR) { return {}; }

    bool isVariableInAssignLineNumMap(VAR_NAME) {return true;}
    bool isPrefixExprInLineNumVariableMap(PREFIX_EXPR) {return true;}
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllAssignLineNumVariable() { return {}; }

    std::vector<std::pair<LINE_NUM, VAR_NAME>>
        getAssignLineNumVariableByPartialPrefixExpr(PREFIX_EXPR) { return {}; }
    std::vector<LINE_NUM> getAssignLineNumByVarPartialPrefixExpr(VAR_NAME, PREFIX_EXPR) {
        return {};
    }

    bool isPartialPrefixExprInLineNumVariableMap(PREFIX_EXPR) {return true;}
    std::vector<std::pair<LINE_NUM, PREFIX_EXPR>>
        getAllAssignLineNumPartialPrefixExpr() { return {}; }
    std::vector<std::pair<VAR_NAME, PREFIX_EXPR>>
        getAllAssignVariablePartialPrefixExpr() { return {}; }

    // other helper functions
    std::unordered_set<LINE_NUM>* selectDataSet(DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM>* selectDataStructure(DESIGN_ENTITY) { return {}; }

    // calls
    bool doesProcedureCalls(PROC_NAME, PROC_NAME) {return true;}
    bool hasCallsRelationship() { return true; }
    std::vector<PROC_NAME> getAllCallers() { return {}; }
    std::vector<PROC_NAME> getAllCallees() { return {}; }
    bool hasCallers(PROC_NAME) { return true; }
    bool hasCallees(PROC_NAME) { return true; }
    std::vector<PROC_NAME> getCallers(PROC_NAME) {return {};}
    std::vector<PROC_NAME> getCallees(PROC_NAME) {return {};}
    std::vector<std::pair<PROC_NAME, PROC_NAME>> getCallsPair(DESIGN_ENTITY, DESIGN_ENTITY)
        {return {};}
    bool doesProcedureCallsStar(PROC_NAME, PROC_NAME) {return true;}
    std::vector<PROC_NAME> getCallersStar(PROC_NAME) {return {};}
    std::vector<PROC_NAME> getCalleesStar(PROC_NAME) {return {};}
    std::vector<std::pair<PROC_NAME, PROC_NAME>> getCallsStarPair(DESIGN_ENTITY, DESIGN_ENTITY)
        {return {};}

    // next
    bool hasNextRelationship() {return true;}
    bool hasNextRelationship(LINE_NUM, LINE_NUM) {return true;}
    bool hasNextLine(LINE_NUM) {return true;}
    bool hasPrevLine(LINE_NUM) {return true;}
    std::vector<LINE_NUM> getNextLines(LINE_NUM) { return {};  }
    std::vector<LINE_NUM> getNextLines(LINE_NUM, DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getNextLines(DESIGN_ENTITY) {return {};}
    std::vector<LINE_NUM> getPrevLines(DESIGN_ENTITY, LINE_NUM) {return {};}
    std::vector<LINE_NUM> getPrevLines(DESIGN_ENTITY) {return {};}
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getNextLinesPair(DESIGN_ENTITY, DESIGN_ENTITY)
        {return {};}
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* getNextMapKeyLineBefore() { return {}; }
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* getNextMapKeyLineAfter() { return {}; }

    // nextBip
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getNextBipLines(LINE_NUM) { return {}; }
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getPrevBipLines(LINE_NUM) { return {}; }
    bool hasNextBipRelationship() { return true; }
    bool hasNextBipRelationship(LINE_NUM, LINE_NUM) { return true; }
    bool hasNextBipLine(LINE_NUM) { return true; }
    bool hasPrevBipLine(LINE_NUM) { return true; }
    std::vector<LINE_NUM> getNextBipLines(LINE_NUM, DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getNextBipLines(DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getPrevBipLines(DESIGN_ENTITY, LINE_NUM) { return {}; }
    std::vector<LINE_NUM> getPrevBipLines(DESIGN_ENTITY) { return {}; }
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getNextBipLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) {return {};}
    bool hasNextBipStarRelationship() { return true; }
    bool hasNextBipStarRelationship(LINE_NUM, LINE_NUM) { return true; }
    std::vector<LINE_NUM> getNextBipStarLines(LINE_NUM, DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getNextBipStarLines(DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getPrevBipStarLines(DESIGN_ENTITY, LINE_NUM) { return {}; }
    std::vector<LINE_NUM> getPrevBipStarLines(DESIGN_ENTITY) { return {}; }
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getNextBipStarLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) { return {}; }

    bool hasAffectsBipRelationship() { return true; }
    bool hasAffectsBipRelationship(LINE_NUM, LINE_NUM) { return true; }
    bool hasAffectsBipLine(LINE_NUM) { return true; }
    bool hasAffectingBipLine(LINE_NUM) { return true; }
    std::vector<LINE_NUM> getAffectsBipLines(LINE_NUM, DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getAffectsBipLines(DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getAffectingBipLines(DESIGN_ENTITY, LINE_NUM) { return {}; }
    std::vector<LINE_NUM> getAffectingBipLines(DESIGN_ENTITY) { return {}; }
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getAffectsBipLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) { return {}; }
    bool hasAffectsBipStarRelationship() { return true; }
    bool hasAffectsBipStarRelationship(LINE_NUM, LINE_NUM) { return true; }
    std::vector<LINE_NUM> getAffectsBipStarLines(LINE_NUM, DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getAffectsBipStarLines(DESIGN_ENTITY) { return {}; }
    std::vector<LINE_NUM> getAffectingBipStarLines(DESIGN_ENTITY, LINE_NUM) { return {}; }
    std::vector<LINE_NUM> getAffectingBipStarLines(DESIGN_ENTITY) { return {}; }
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getAffectsBipStarLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) { return {}; }

    // answer with query
    std::vector <std::pair<LINE_NUM, PROC_NAME>> getCallPairWithLineNum(LINE_NUM) { return {}; }
    std::vector <std::pair<LINE_NUM, PROC_NAME>> getCallPairWithProcName(PROC_NAME) { return {}; }
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getReadPairWithLineNum(LINE_NUM) { return {}; }
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getReadPairWithVarName(VAR_NAME) { return {}; }
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getPrintPairWithLineNum(LINE_NUM) { return {}; }
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getPrintPairWithVarName(VAR_NAME) { return {}; }

    bool isRightDesignEntity(DESIGN_ENTITY, LINE_NUM) { return true; }
    bool isValidVar(VAR_NAME) { return true; }
    bool isValidProc(PROC_NAME) { return true; }
    bool isValidConstant(CONSTANT) { return true; }

    std::vector<std::string> findCommonStrItems(DESIGN_ENTITY, DESIGN_ENTITY) { return {}; }
    std::vector<int> findCommonIntItems(DESIGN_ENTITY, DESIGN_ENTITY) { return {}; }

    // optimizer
    int getNumOfRelationship(RELATIONSHIP) { return 0; }
    virtual int getNumOfDesignEntity(DESIGN_ENTITY) { return 0; }
};
