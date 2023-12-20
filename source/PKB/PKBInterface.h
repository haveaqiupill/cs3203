#pragma once

#include <stdio.h>
#include <list>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "../Commons/DesignEntity.h"
#include "../Commons/Types.h"
#include "../Commons/Relationship.h"

class PKBInterface {
 protected:
    // Declaration
    typedef DesignEntity DESIGN_ENTITY;
    typedef Relationship RELATIONSHIP;
    typedef std::string CONSTANT;

 public:
    // methods to store releationships within the PKB
    virtual void storeRelationshipFollows(LINE_NUM, LINE_NUM) = 0;
    virtual void storeRelationshipFollowsStar(LINE_NUM, LINE_NUM) = 0;

    virtual void storeRelationshipParent(LINE_NUM, LINE_NUM) = 0;
    virtual void storeRelationshipParentStar(LINE_NUM, LINE_NUM) = 0;

    virtual void storeRelationshipUsesByProcedure(PROC_NAME, VAR_NAME) = 0;
    virtual void storeRelationshipUsesByLineNum(LINE_NUM, VAR_NAME) = 0;

    virtual void storeRelationshipModifiesByProcedure(PROC_NAME, VAR_NAME) = 0;
    virtual void storeRelationshipModifiesByLineNum(LINE_NUM, VAR_NAME) = 0;

    virtual void storeRelationshipNext(LINE_NUM, LINE_NUM) = 0;

    virtual void storeRelationshipCalls(PROC_NAME, PROC_NAME) = 0;
    virtual void storeRelationshipCallsStar(PROC_NAME, PROC_NAME) = 0;

    virtual void storeRelationshipNextBip(LINE_NUM, LINE_NUM) = 0;
    virtual void storeRelationshipNextBip(LINE_NUM, LINE_NUM, LINE_NUM) = 0;
    virtual void storeRelationshipNextBipStar(LINE_NUM, LINE_NUM) = 0;

    virtual void storeRelationshipAffectsBip(LINE_NUM, LINE_NUM) = 0;
    virtual void storeRelationshipAffectsBipStar(LINE_NUM, LINE_NUM) = 0;

    // methods to store design entities within the PKB
    virtual void storeConstant(CONSTANT) = 0;
    virtual void storeVariable(VAR_NAME) = 0;
    virtual void storeProcedure(PROC_NAME, LINE_NUM, LINE_NUM, std::list<LINE_NUM>) = 0;
    virtual void storeAssignStatement(LINE_NUM, VAR_NAME, PREFIX_EXPR) = 0;
    virtual void storeFullPrefixExpr(LINE_NUM, VAR_NAME, PREFIX_EXPR) = 0;
    virtual void storePartialPrefixExpr(LINE_NUM, VAR_NAME, PREFIX_EXPR) = 0;
    virtual void storePrintStatement(LINE_NUM, VAR_NAME) = 0;
    virtual void storeReadStatement(LINE_NUM, VAR_NAME) = 0;
    virtual void storeIfElseStatement(LINE_NUM, std::list<VAR_NAME>) = 0;
    virtual void storeWhileStatement(LINE_NUM, std::list<VAR_NAME>) = 0;
    virtual void storeCallStatement(LINE_NUM, PROC_NAME) = 0;
    virtual void propagateStmtList(LINE_NUM) = 0;

    virtual void sortAllDataStructure() = 0;

    // API for DE to interact with PKB
    virtual std::pair<VAR_NAME, PREFIX_EXPR> getExpr(LINE_NUM) = 0;
    virtual std::pair<LINE_NUM, LINE_NUM> getProc(PROC_NAME) = 0;
    virtual std::vector<LINE_NUM> getLastStmtsOfProc(PROC_NAME) = 0;

    // GETTERS: API for back-end to interact with PKB
    // Getters for design entities
    virtual std::vector<CONSTANT> getAllConstants() = 0;
    virtual std::vector<VAR_NAME> getAllVariables() = 0;
    virtual std::vector<PROC_NAME> getAllProcedures() = 0;
    virtual std::vector<LINE_NUM> getAllAssignStatements() = 0;
    virtual std::vector<LINE_NUM> getAllWhileStatements() = 0;
    virtual std::vector<LINE_NUM> getAllIfStatements() = 0;
    virtual std::unordered_map<LINE_NUM, PROC_NAME> getCallMap() = 0;
    virtual std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllReadStatementsWithAttribute() = 0;
    virtual std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllPrintStatementsWithAttribute() = 0;
    virtual std::vector<std::pair<LINE_NUM, PROC_NAME>> getAllCallStatementsWithAttribute() = 0;

    // To answer Follows query
    virtual bool doesFollows(LINE_NUM, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getLineAfterFromFollows(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getLineBeforeFromFollows(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getFollowsAfter(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getFollowsAfter(DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getFollowsBefore(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getFollowsBefore(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getFollowsPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // To answer Follows* query
    virtual bool doesFollowsStar(LINE_NUM, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getLineAfterFromFollowsStar(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getLineBeforeFromFollowsStar(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getFollowsStarAfter(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getFollowsStarAfter(DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getFollowsStarBefore(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getFollowsStarBefore(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getFollowsStarPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // Helper functions for Follows relationship
    virtual bool hasFollowsRelationship() = 0;
    virtual bool hasFollowee(LINE_NUM) = 0;
    virtual bool hasFollower(LINE_NUM) = 0;

    // To answer Parent query
    virtual bool doesParent(LINE_NUM, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getChildLineFromParent(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getParentLineFromParent(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getParentAfter(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getParentAfter(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getParentPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getParentBefore(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getParentBefore(DESIGN_ENTITY) = 0;


    // To answer Parent* query
    virtual bool doesParentStar(LINE_NUM, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getChildLineFromParentStar(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getParentLineFromParentStar(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getParentStarAfter(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getParentStarAfter(DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getParentStarBefore(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getParentStarBefore(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getParentStarPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // Helper function for Parent relationship
    virtual bool hasParentRelationship() = 0;
    virtual bool isParent(LINE_NUM) = 0;
    virtual bool isParent(DESIGN_ENTITY) = 0;
    virtual bool isChild(LINE_NUM) = 0;

    // To answer uses query
    virtual bool doesProcedureUses(PROC_NAME, VAR_NAME) = 0;
    virtual bool doesLineNumUses(LINE_NUM, VAR_NAME) = 0;
    virtual std::vector<VAR_NAME> getVariableFromUsesByProcedure(PROC_NAME) = 0;
    virtual std::vector<VAR_NAME> getVariableFromUsesByLineNum(LINE_NUM) = 0;
    virtual std::vector<PROC_NAME> getProcedureFromUsesByProcedure(VAR_NAME) = 0;
    virtual std::vector<LINE_NUM> getLineNumFromUsesByVarName(VAR_NAME) = 0;

    virtual bool doesLineNumUsesWildcard(LINE_NUM) = 0;
    virtual bool doesProcedureUsesWildcard(PROC_NAME) = 0;
    virtual std::vector<std::string> getUsesVector(DESIGN_ENTITY) = 0;
    virtual std::vector<std::string> getUsesByVarName(DESIGN_ENTITY, VAR_NAME) = 0;
    virtual std::vector<std::pair<std::string, VAR_NAME>>
        getUsesPairVector(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // To answer modifies query
    virtual bool doesProcedureModifies(PROC_NAME, VAR_NAME) = 0;
    virtual bool doesLineNumModifies(LINE_NUM, VAR_NAME) = 0;
    virtual std::vector<VAR_NAME> getVariableFromModifiesByProcedure(PROC_NAME) = 0;
    virtual std::vector<VAR_NAME> getVariableFromModifiesByLineNum(LINE_NUM) = 0;
    virtual std::vector<PROC_NAME> getProcedureFromModifiesByProcedure(VAR_NAME) = 0;
    virtual std::vector<LINE_NUM> getLineNumFromModifiesByVarName(VAR_NAME) = 0;

    virtual bool doesLineNumModifiesWildcard(LINE_NUM) = 0;
    virtual bool doesProcedureModifiesWildcard(PROC_NAME) = 0;
    virtual std::vector<std::string> getModifiesVector(DESIGN_ENTITY) = 0;
    virtual std::vector<std::string> getModifiesByVarName(DESIGN_ENTITY, VAR_NAME) = 0;
    virtual std::vector<std::pair<std::string, VAR_NAME>>
        getModifiesPairVector(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // To answer pattern query
    // 1. Assign statement
    virtual std::vector<LINE_NUM> getAssignLineNumByVariable(VAR_NAME) = 0;
    virtual std::vector<std::pair<LINE_NUM, VAR_NAME>>
        getAssignLineNumVariableByPrefixExpr(PREFIX_EXPR) = 0;
    virtual std::vector<LINE_NUM> getAssignLineNumByVarPrefixExpr(VAR_NAME, PREFIX_EXPR) = 0;
    virtual bool isVariableInAssignLineNumMap(VAR_NAME) = 0;
    virtual bool isPrefixExprInLineNumVariableMap(PREFIX_EXPR) = 0;
    virtual std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllAssignLineNumVariable() = 0;
    virtual std::vector<std::pair<LINE_NUM, VAR_NAME>>
        getAssignLineNumVariableByPartialPrefixExpr(PREFIX_EXPR) = 0;
    virtual std::vector<LINE_NUM> getAssignLineNumByVarPartialPrefixExpr(VAR_NAME, PREFIX_EXPR) = 0;
    virtual bool isPartialPrefixExprInLineNumVariableMap(PREFIX_EXPR) = 0;

    virtual std::vector<LINE_NUM> getAllIfLineNumByVariable(VAR_NAME) = 0;
    virtual std::vector< std::pair<LINE_NUM, VAR_NAME >> getAllIfLineNumByVariable() = 0;
    virtual std::vector<LINE_NUM> getAllWhileLineNumByVariable(VAR_NAME) = 0;
    virtual std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllWhileLineNumByVariable() = 0;

    // calls
    virtual bool doesProcedureCalls(PROC_NAME, PROC_NAME) = 0;
    virtual bool hasCallsRelationship() = 0;
    virtual std::vector<PROC_NAME> getAllCallers() = 0;
    virtual std::vector<PROC_NAME> getAllCallees() = 0;
    virtual std::vector<PROC_NAME> getCallers(PROC_NAME) = 0;
    virtual std::vector<PROC_NAME> getCallees(PROC_NAME) = 0;
    virtual bool hasCallers(PROC_NAME) = 0;
    virtual bool hasCallees(PROC_NAME) = 0;
    virtual std::vector<std::pair<PROC_NAME, PROC_NAME>>
        getCallsPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;
    virtual bool doesProcedureCallsStar(PROC_NAME, PROC_NAME) = 0;
    virtual std::vector<PROC_NAME> getCallersStar(PROC_NAME) = 0;
    virtual std::vector<PROC_NAME> getCalleesStar(PROC_NAME) = 0;
    virtual std::vector<std::pair<PROC_NAME, PROC_NAME>>
        getCallsStarPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // next
    virtual bool hasNextRelationship() = 0;
    virtual bool hasNextRelationship(LINE_NUM, LINE_NUM) = 0;
    virtual bool hasNextLine(LINE_NUM) = 0;
    virtual bool hasPrevLine(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getNextLines(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getNextLines(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getNextLines(DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getPrevLines(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getPrevLines(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getNextLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;
    virtual std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* getNextMapKeyLineBefore() = 0;
    virtual std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* getNextMapKeyLineAfter() = 0;

    // nextBip
    virtual bool hasNextBipRelationship() = 0;
    virtual bool hasNextBipRelationship(LINE_NUM, LINE_NUM) = 0;
    virtual bool hasNextBipLine(LINE_NUM) = 0;
    virtual bool hasPrevBipLine(LINE_NUM) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>> getNextBipLines(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getNextBipLines(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getNextBipLines(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>> getPrevBipLines(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getPrevBipLines(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getPrevBipLines(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getNextBipLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;
    virtual bool hasNextBipStarRelationship() = 0;
    virtual bool hasNextBipStarRelationship(LINE_NUM, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getNextBipStarLines(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getNextBipStarLines(DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getPrevBipStarLines(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getPrevBipStarLines(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getNextBipStarLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    virtual bool hasAffectsBipRelationship() = 0;
    virtual bool hasAffectsBipRelationship(LINE_NUM, LINE_NUM) = 0;
    virtual bool hasAffectsBipLine(LINE_NUM) = 0;
    virtual bool hasAffectingBipLine(LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getAffectsBipLines(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getAffectsBipLines(DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getAffectingBipLines(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getAffectingBipLines(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getAffectsBipLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;
    virtual bool hasAffectsBipStarRelationship() = 0;
    virtual bool hasAffectsBipStarRelationship(LINE_NUM, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getAffectsBipStarLines(LINE_NUM, DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getAffectsBipStarLines(DESIGN_ENTITY) = 0;
    virtual std::vector<LINE_NUM> getAffectingBipStarLines(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual std::vector<LINE_NUM> getAffectingBipStarLines(DESIGN_ENTITY) = 0;
    virtual std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getAffectsBipStarLinesPair(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // answer with query
    virtual std::vector <std::pair<LINE_NUM, PROC_NAME>> getCallPairWithLineNum(LINE_NUM) = 0;
    virtual std::vector <std::pair<LINE_NUM, PROC_NAME>> getCallPairWithProcName(PROC_NAME) = 0;
    virtual std::vector <std::pair<LINE_NUM, VAR_NAME>> getReadPairWithLineNum(LINE_NUM) = 0;
    virtual std::vector <std::pair<LINE_NUM, VAR_NAME>> getReadPairWithVarName(VAR_NAME) = 0;
    virtual std::vector <std::pair<LINE_NUM, VAR_NAME>> getPrintPairWithLineNum(LINE_NUM) = 0;
    virtual std::vector <std::pair<LINE_NUM, VAR_NAME>> getPrintPairWithVarName(VAR_NAME) = 0;

    virtual bool isRightDesignEntity(DESIGN_ENTITY, LINE_NUM) = 0;
    virtual bool isValidVar(VAR_NAME) = 0;
    virtual bool isValidProc(PROC_NAME) = 0;
    virtual bool isValidConstant(CONSTANT) = 0;

    virtual std::vector<std::string> findCommonStrItems(DESIGN_ENTITY, DESIGN_ENTITY) = 0;
    virtual std::vector<int> findCommonIntItems(DESIGN_ENTITY, DESIGN_ENTITY) = 0;

    // other helper functions
    virtual std::vector<LINE_NUM>* selectDataStructure(DESIGN_ENTITY) = 0;
    virtual std::unordered_set<LINE_NUM>* selectDataSet(DESIGN_ENTITY) = 0;

    // optimizer
    virtual int getNumOfRelationship(RELATIONSHIP) = 0;
    virtual int getNumOfDesignEntity(DESIGN_ENTITY) = 0;
};
