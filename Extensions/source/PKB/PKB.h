#pragma once

#include <list>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "../Commons/PairHash.h"
#include "PKBInterface.h"

class PKB : public PKBInterface {
 private:
     // PRIVATE METHODS: For organinzing of code
     void sortFollowsDataStructure();
     void sortParentDataStructure();
     void sortUsesDataStructure();
     void sortModifiesDataStructure();
     void sortAssignDataStructure();
     void sortNextDataStructure();
     void sortCallsDataStructure();
     void sortNextBipDataStructure();
     void sortAffectsBipDataStructure();
     void sortDesignEntities();

     std::vector<LINE_NUM> PKB::nextBreadthFirstSearch(LINE_NUM, DESIGN_ENTITY,
         std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>*);

     bool isVariableModified(VAR_NAME);
     bool isVariableUsed(VAR_NAME);

 protected:
    // LIST OF TABLES STORED IN THE DATABASE
    // follows and follows*
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> followsSet;
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> followsStarSet;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> followsMapKeyLineBefore;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> followsStarMapKeyLineBefore;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> followsMapKeyLineAfter;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> followsStarMapKeyLineAfter;

    // parent and parent*
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> parentSet;
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> parentStarSet;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> parentMapKeyParentLine;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> parentStarMapKeyParentLine;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> parentMapKeyChildLine;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> parentStarMapKeyChildLine;

    // uses
    std::unordered_set<std::pair<PROC_NAME, VAR_NAME>, pair_hash> usesSetByProcedure;
    std::unordered_set<std::pair<LINE_NUM, VAR_NAME>, pair_hash> usesSetByLineNum;
    std::unordered_map<PROC_NAME, std::vector<VAR_NAME>> usesMapByProcedureKeyProcedure;
    std::unordered_map<VAR_NAME, std::vector<PROC_NAME>> usesMapByProcedureKeyVariable;
    std::unordered_map<LINE_NUM, std::vector<VAR_NAME>> usesMapByLineNumKeyLineNum;
    std::unordered_map<VAR_NAME, std::vector<LINE_NUM>> usesMapByLineNumKeyVariable;

    // modifies
    std::unordered_set<std::pair<PROC_NAME, VAR_NAME>, pair_hash> modifiesSetByProcedure;
    std::unordered_set<std::pair<LINE_NUM, VAR_NAME>, pair_hash> modifiesSetByLineNum;
    std::unordered_map<PROC_NAME, std::vector<VAR_NAME>> modifiesMapByProcedureKeyProcedure;
    std::unordered_map<VAR_NAME, std::vector<PROC_NAME>> modifiesMapByProcedureKeyVariable;
    std::unordered_map<LINE_NUM, std::vector<VAR_NAME>> modifiesMapByLineNumKeyLineNum;
    std::unordered_map<VAR_NAME, std::vector<LINE_NUM>> modifiesMapByLineNumKeyVariable;

    // next (can only store next r/s and not next*)
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> nextSet;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> nextMapKeyLineBefore;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> nextMapKeyLineAfter;

    // calls
    std::unordered_set<std::pair<PROC_NAME, PROC_NAME>, pair_hash> callsSet;
    std::unordered_set<std::pair<PROC_NAME, PROC_NAME>, pair_hash> callsStarSet;
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> callsMapKeyCaller;
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> callsStarMapKeyCaller;
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> callsMapKeyCallee;
    std::unordered_map<PROC_NAME, std::vector<PROC_NAME>> callsStarMapKeyCallee;

    // nextBip & nextBip*
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> nextBipSet;
    std::unordered_map<LINE_NUM, std::vector<std::pair<LINE_NUM, LINE_NUM>>>
        nextBipMapKeyLineBefore;
    std::unordered_map<LINE_NUM, std::vector<std::pair<LINE_NUM, LINE_NUM>>>
        nextBipMapKeyLineAfter;
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> nextBipStarSet;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> nextBipStarMapKeyLineBefore;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> nextBipStarMapKeyLineAfter;

    // affectsBip & affectsBip*
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> affectsBipSet;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> affectsBipMapKeyLineBefore;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> affectsBipMapKeyLineAfter;
    std::unordered_set<std::pair<LINE_NUM, LINE_NUM>, pair_hash> affectsBipStarSet;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> affectsBipStarMapKeyLineBefore;
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>> affectsBipStarMapKeyLineAfter;

    // design entities
    std::unordered_set<VAR_NAME> variableSet;
    std::unordered_set<CONSTANT> constantSet;
    std::unordered_set<PROC_NAME> procedureSet;
    std::unordered_set<LINE_NUM> statementSet;
    std::unordered_set<LINE_NUM> statementAssignSet;
    std::unordered_set<LINE_NUM> statementPrintSet;
    std::unordered_set<LINE_NUM> statementReadSet;
    std::unordered_set<LINE_NUM> statementWhileSet;
    std::unordered_set<LINE_NUM> statementIfSet;
    std::unordered_set<LINE_NUM> statementCallSet;
    std::unordered_map<PROC_NAME, std::pair<LINE_NUM, LINE_NUM>> procedureMap;
    std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> procedureEndPointsMapping;
    std::unordered_map<LINE_NUM, PROC_NAME> callMapByLineNum;
    std::unordered_map<PROC_NAME, std::vector<LINE_NUM>> callMapByProcedure;
    std::unordered_map<LINE_NUM, VAR_NAME> readMapByLineNum;
    std::unordered_map<VAR_NAME, std::vector<LINE_NUM>> readMapByVarName;
    std::unordered_map<LINE_NUM, VAR_NAME> printMapByLineNum;
    std::unordered_map<VAR_NAME, std::vector<LINE_NUM>> printMapByVarName;

    std::unordered_map<VAR_NAME, std::vector<LINE_NUM>> whileMapByVarName;
    std::unordered_map<VAR_NAME, std::vector<LINE_NUM>> ifMapByVarName;


    std::vector<LINE_NUM> sortedStatementList;
    std::vector<LINE_NUM> sortedAssignList;
    std::vector<LINE_NUM> sortedPrintList;
    std::vector<LINE_NUM> sortedReadList;
    std::vector<LINE_NUM> sortedWhileList;
    std::vector<LINE_NUM> sortedIfList;
    std::vector<LINE_NUM> sortedCallList;
    std::vector<LINE_NUM> sortedParentsList;
    std::vector<PROC_NAME> sortedProcedureList;
    std::vector<VAR_NAME> sortedVariableList;
    std::vector<CONSTANT> sortedConstantList;

    // TABLES FOR PATTERNS
    // full assignment
    std::unordered_set<PREFIX_EXPR> assignPrefixExprSet;
    std::unordered_set<std::pair<LINE_NUM, PREFIX_EXPR>, pair_hash> assignLineNumPrefixExprSet;
    std::unordered_set<std::pair<VAR_NAME, PREFIX_EXPR>, pair_hash> assignVariablePrefixExprSet;
    std::unordered_set<std::pair<LINE_NUM, VAR_NAME>, pair_hash> assignLineNumVariableSet;
    std::unordered_map<VAR_NAME, std::vector<std::pair<LINE_NUM, PREFIX_EXPR>>>
        assignLineNumPrefixExprMapKeyVariable;
    std::unordered_map<VAR_NAME, std::vector<LINE_NUM>> assignLineNumMapKeyVariable;
    std::unordered_map<PREFIX_EXPR, std::vector<std::pair<LINE_NUM, VAR_NAME>>>
        assignLineNumVariableMapKeyPrefixExpr;
    std::unordered_map<LINE_NUM, std::pair<VAR_NAME, PREFIX_EXPR>>
        assignVariableExpressionKeyLineNum;

    // partial prefix
    std::unordered_set<PREFIX_EXPR> assignPartialPrefixExprSet;
    std::unordered_set<std::pair<LINE_NUM, PREFIX_EXPR>, pair_hash>
        assignLineNumPartialPrefixExprSet;
    std::unordered_set<std::pair<VAR_NAME, PREFIX_EXPR>, pair_hash>
        assignVariablePartialPrefixExprSet;
    std::unordered_map<VAR_NAME, std::vector<std::pair<LINE_NUM, PREFIX_EXPR>>>
        assignLineNumPartialPrefixExprMapKeyVariable;
    std::unordered_map<PREFIX_EXPR, std::vector<std::pair<LINE_NUM, VAR_NAME>>>
        assignLineNumVariableMapKeyPartialPrefixExpr;
    std::unordered_map<LINE_NUM, std::pair<VAR_NAME, PREFIX_EXPR>>
        assignVariablePartialExpressionKeyLineNum;

 public:
    // SETTERS: API for front-end to interact with PKB
    void storeRelationshipFollows(LINE_NUM, LINE_NUM);
    void storeRelationshipFollowsStar(LINE_NUM, LINE_NUM);
    void storeRelationshipParent(LINE_NUM, LINE_NUM);
    void storeRelationshipParentStar(LINE_NUM, LINE_NUM);
    void storeRelationshipUsesByProcedure(PROC_NAME, VAR_NAME);
    void storeRelationshipUsesByLineNum(LINE_NUM, VAR_NAME);
    void storeRelationshipModifiesByProcedure(PROC_NAME, VAR_NAME);
    void storeRelationshipModifiesByLineNum(LINE_NUM, VAR_NAME);
    void storeRelationshipNext(LINE_NUM, LINE_NUM);
    void storeRelationshipCalls(PROC_NAME, PROC_NAME);
    void storeRelationshipCallsStar(PROC_NAME, PROC_NAME);
    void storeRelationshipNextBip(LINE_NUM, LINE_NUM);
    void storeRelationshipNextBip(LINE_NUM, LINE_NUM, LINE_NUM);
    void storeRelationshipNextBipStar(LINE_NUM, LINE_NUM);
    void storeRelationshipAffectsBip(LINE_NUM, LINE_NUM);
    void storeRelationshipAffectsBipStar(LINE_NUM, LINE_NUM);

    void storeConstant(CONSTANT);
    void storeVariable(VAR_NAME);
    void storeProcedure(PROC_NAME, LINE_NUM, LINE_NUM, std::list<LINE_NUM>);
    void storeAssignStatement(LINE_NUM, VAR_NAME, PREFIX_EXPR);
    void storeFullPrefixExpr(LINE_NUM, VAR_NAME, PREFIX_EXPR);
    void storePartialPrefixExpr(LINE_NUM, VAR_NAME, PREFIX_EXPR);
    void storePrintStatement(LINE_NUM, VAR_NAME);
    void storeReadStatement(LINE_NUM, VAR_NAME);
    void storeIfElseStatement(LINE_NUM, std::list<VAR_NAME>);
    void storeWhileStatement(LINE_NUM, std::list<VAR_NAME>);
    void storeCallStatement(LINE_NUM, PROC_NAME);

    std::pair<VAR_NAME, PREFIX_EXPR> getExpr(LINE_NUM);
    std::vector<LINE_NUM> getLastStmtsOfProc(PROC_NAME);
    void sortAllDataStructure();
    void propagateStmtList(LINE_NUM);

    // GETTERS: API for back-end to interact with PKB
    // Getters for design entities
    std::vector<VAR_NAME> getAllVariables();
    std::vector<PROC_NAME> getAllProcedures();
    std::vector<LINE_NUM> getAllAssignStatements();
    std::vector<LINE_NUM> getAllWhileStatements();
    std::vector<LINE_NUM> getAllIfStatements();
    std::vector<CONSTANT> getAllConstants();
    std::unordered_map<LINE_NUM, PROC_NAME> getCallMap();
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllReadStatementsWithAttribute();
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllPrintStatementsWithAttribute();
    std::vector<std::pair<LINE_NUM, PROC_NAME>> getAllCallStatementsWithAttribute();
    std::pair<LINE_NUM, LINE_NUM> getProc(PROC_NAME);

    // To answer follows and follows* query
    bool doesFollows(LINE_NUM, LINE_NUM);
    bool doesFollowsStar(LINE_NUM, LINE_NUM);
    std::vector<LINE_NUM> getLineAfterFromFollows(LINE_NUM);
    std::vector<LINE_NUM> getLineAfterFromFollowsStar(LINE_NUM);
    std::vector<LINE_NUM> getLineBeforeFromFollows(LINE_NUM);
    std::vector<LINE_NUM> getLineBeforeFromFollowsStar(LINE_NUM);
    std::vector<LINE_NUM> getFollowsAfter(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getFollowsAfter(DESIGN_ENTITY);
    std::vector<LINE_NUM> getFollowsBefore(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getFollowsBefore(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getFollowsPair(DESIGN_ENTITY, DESIGN_ENTITY);
    std::vector<LINE_NUM> getFollowsStarAfter(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getFollowsStarAfter(DESIGN_ENTITY);
    std::vector<LINE_NUM> getFollowsStarBefore(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getFollowsStarBefore(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getFollowsStarPair
        (DESIGN_ENTITY, DESIGN_ENTITY);
    bool hasFollowsRelationship();
    bool hasFollowee(LINE_NUM);
    bool hasFollower(LINE_NUM);

    std::unordered_set<LINE_NUM>* selectDataSet(DESIGN_ENTITY);
    std::vector<LINE_NUM>* selectDataStructure(DESIGN_ENTITY);

    // To answer parent and parent* query
    bool doesParent(LINE_NUM, LINE_NUM);
    bool doesParentStar(LINE_NUM, LINE_NUM);
    std::vector<LINE_NUM> getChildLineFromParent(LINE_NUM);
    std::vector<LINE_NUM> getChildLineFromParentStar(LINE_NUM);
    std::vector<LINE_NUM> getParentLineFromParent(LINE_NUM);
    std::vector<LINE_NUM> getParentLineFromParentStar(LINE_NUM);
    std::vector<LINE_NUM> getParentAfter(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getParentAfter(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getParentPair(DESIGN_ENTITY, DESIGN_ENTITY);
    std::vector<LINE_NUM> getParentBefore(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getParentBefore(DESIGN_ENTITY);
    std::vector<LINE_NUM> getParentStarAfter(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getParentStarAfter(DESIGN_ENTITY);
    std::vector<LINE_NUM> getParentStarBefore(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getParentStarBefore(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getParentStarPair(DESIGN_ENTITY, DESIGN_ENTITY);

    bool hasParentRelationship();
    bool isParent(LINE_NUM);
    bool isParent(DESIGN_ENTITY);
    bool isChild(LINE_NUM);


    // To answer uses query
    bool doesProcedureUses(PROC_NAME, VAR_NAME);
    bool doesLineNumUses(LINE_NUM, VAR_NAME);
    std::vector<VAR_NAME> getVariableFromUsesByProcedure(PROC_NAME);
    std::vector<VAR_NAME> getVariableFromUsesByLineNum(LINE_NUM);
    std::vector<PROC_NAME> getProcedureFromUsesByProcedure(VAR_NAME);
    std::vector<LINE_NUM> getLineNumFromUsesByVarName(VAR_NAME);

    bool doesLineNumUsesWildcard(LINE_NUM);
    bool doesProcedureUsesWildcard(PROC_NAME);
    std::vector<std::string> getUsesVector(DESIGN_ENTITY);
    std::vector<std::string> getUsesByVarName(DESIGN_ENTITY, VAR_NAME);
    std::vector<std::pair<std::string, VAR_NAME>> getUsesPairVector(DESIGN_ENTITY, DESIGN_ENTITY);

    // To answer modifies query
    bool doesProcedureModifies(PROC_NAME, VAR_NAME);
    bool doesLineNumModifies(LINE_NUM, VAR_NAME);
    std::vector<VAR_NAME> getVariableFromModifiesByProcedure(PROC_NAME);
    std::vector<VAR_NAME> getVariableFromModifiesByLineNum(LINE_NUM);
    std::vector<PROC_NAME> getProcedureFromModifiesByProcedure(VAR_NAME);
    std::vector<LINE_NUM> getLineNumFromModifiesByVarName(VAR_NAME);

    bool doesLineNumModifiesWildcard(LINE_NUM);
    bool doesProcedureModifiesWildcard(PROC_NAME);
    std::vector<std::string> getModifiesVector(DESIGN_ENTITY);
    std::vector<std::string> getModifiesByVarName(DESIGN_ENTITY, VAR_NAME);
    std::vector<std::pair<std::string, VAR_NAME>>
        getModifiesPairVector(DESIGN_ENTITY, DESIGN_ENTITY);

    // To answer pattern query
    // 1. Assign statement
    std::vector<LINE_NUM> getAssignLineNumByVariable(VAR_NAME);
    std::vector<std::pair<LINE_NUM, VAR_NAME>>
        getAssignLineNumVariableByPrefixExpr(PREFIX_EXPR);
    std::vector<LINE_NUM> getAssignLineNumByVarPrefixExpr(VAR_NAME, PREFIX_EXPR);

    bool isVariableInAssignLineNumMap(VAR_NAME);
    bool isPrefixExprInLineNumVariableMap(PREFIX_EXPR);
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllAssignLineNumVariable();

    std::vector<std::pair<LINE_NUM, VAR_NAME>>
        getAssignLineNumVariableByPartialPrefixExpr(PREFIX_EXPR);
    std::vector<LINE_NUM> getAssignLineNumByVarPartialPrefixExpr(VAR_NAME, PREFIX_EXPR);
    bool isPartialPrefixExprInLineNumVariableMap(PREFIX_EXPR);

    std::vector<LINE_NUM> getAllIfLineNumByVariable(VAR_NAME);
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllIfLineNumByVariable();
    std::vector<LINE_NUM> getAllWhileLineNumByVariable(VAR_NAME);
    std::vector<std::pair<LINE_NUM, VAR_NAME>> getAllWhileLineNumByVariable();


    // calls
    bool doesProcedureCalls(PROC_NAME, PROC_NAME);
    bool hasCallsRelationship();
    std::vector<PROC_NAME> getAllCallers();
    std::vector<PROC_NAME> getAllCallees();
    bool hasCallers(PROC_NAME);
    bool hasCallees(PROC_NAME);
    std::vector<PROC_NAME> getCallers(PROC_NAME);
    std::vector<PROC_NAME> getCallees(PROC_NAME);
    std::vector<std::pair<PROC_NAME, PROC_NAME>> getCallsPair(DESIGN_ENTITY, DESIGN_ENTITY);
    bool doesProcedureCallsStar(PROC_NAME, PROC_NAME);
    std::vector<PROC_NAME> getCallersStar(PROC_NAME);
    std::vector<PROC_NAME> getCalleesStar(PROC_NAME);
    std::vector<std::pair<PROC_NAME, PROC_NAME>> getCallsStarPair(DESIGN_ENTITY, DESIGN_ENTITY);

    // next
    bool hasNextRelationship();
    bool hasNextRelationship(LINE_NUM, LINE_NUM);
    bool hasNextLine(LINE_NUM);
    bool hasPrevLine(LINE_NUM);
    std::vector<LINE_NUM> getNextLines(LINE_NUM);
    std::vector<LINE_NUM> getNextLines(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getNextLines(DESIGN_ENTITY);
    std::vector<LINE_NUM> getPrevLines(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getPrevLines(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getNextLinesPair(DESIGN_ENTITY, DESIGN_ENTITY);
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* getNextMapKeyLineBefore();
    std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* getNextMapKeyLineAfter();

    // nextBip
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getNextBipLines(LINE_NUM);
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getPrevBipLines(LINE_NUM);
    bool hasNextBipRelationship();
    bool hasNextBipRelationship(LINE_NUM, LINE_NUM);
    bool hasNextBipLine(LINE_NUM);
    bool hasPrevBipLine(LINE_NUM);
    std::vector<LINE_NUM> getNextBipLines(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getNextBipLines(DESIGN_ENTITY);
    std::vector<LINE_NUM> getPrevBipLines(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getPrevBipLines(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>> getNextBipLinesPair(DESIGN_ENTITY, DESIGN_ENTITY);
    bool hasNextBipStarRelationship();
    bool hasNextBipStarRelationship(LINE_NUM, LINE_NUM);
    std::vector<LINE_NUM> getNextBipStarLines(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getNextBipStarLines(DESIGN_ENTITY);
    std::vector<LINE_NUM> getPrevBipStarLines(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getPrevBipStarLines(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getNextBipStarLinesPair(DESIGN_ENTITY, DESIGN_ENTITY);

    bool hasAffectsBipRelationship();
    bool hasAffectsBipRelationship(LINE_NUM, LINE_NUM);
    bool hasAffectsBipLine(LINE_NUM);
    bool hasAffectingBipLine(LINE_NUM);
    std::vector<LINE_NUM> getAffectsBipLines(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getAffectsBipLines(DESIGN_ENTITY);
    std::vector<LINE_NUM> getAffectingBipLines(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getAffectingBipLines(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getAffectsBipLinesPair(DESIGN_ENTITY, DESIGN_ENTITY);
    bool hasAffectsBipStarRelationship();
    bool hasAffectsBipStarRelationship(LINE_NUM, LINE_NUM);
    std::vector<LINE_NUM> getAffectsBipStarLines(LINE_NUM, DESIGN_ENTITY);
    std::vector<LINE_NUM> getAffectsBipStarLines(DESIGN_ENTITY);
    std::vector<LINE_NUM> getAffectingBipStarLines(DESIGN_ENTITY, LINE_NUM);
    std::vector<LINE_NUM> getAffectingBipStarLines(DESIGN_ENTITY);
    std::vector<std::pair<LINE_NUM, LINE_NUM>>
        getAffectsBipStarLinesPair(DESIGN_ENTITY, DESIGN_ENTITY);

    // answer with query
    std::vector <std::pair<LINE_NUM, PROC_NAME>> getCallPairWithLineNum(LINE_NUM);
    std::vector <std::pair<LINE_NUM, PROC_NAME>> getCallPairWithProcName(PROC_NAME);
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getReadPairWithLineNum(LINE_NUM);
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getReadPairWithVarName(VAR_NAME);
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getPrintPairWithLineNum(LINE_NUM);
    std::vector <std::pair<LINE_NUM, VAR_NAME>> getPrintPairWithVarName(VAR_NAME);

    bool isRightDesignEntity(DESIGN_ENTITY, LINE_NUM);
    bool isValidVar(VAR_NAME);
    bool isValidProc(PROC_NAME);
    bool isValidConstant(CONSTANT);

    std::vector<std::string> findCommonStrItems(DESIGN_ENTITY, DESIGN_ENTITY);
    std::vector<int> findCommonIntItems(DESIGN_ENTITY, DESIGN_ENTITY);

    // optimizer
    int getNumOfRelationship(RELATIONSHIP);
    int getNumOfDesignEntity(DESIGN_ENTITY);
};
