#include "PKB.h"
#include "../Commons/Exceptions.h"

int PKB::getNumOfDesignEntity(DESIGN_ENTITY designEnt) {
    switch (designEnt) {
    case DESIGN_ENTITY::CONSTANT:
        return constantSet.size();
    case DESIGN_ENTITY::PROCEDURE:
        return procedureSet.size();
    case DESIGN_ENTITY::VARIABLE:
        return variableSet.size();
    default:
        return selectDataSet(designEnt)->size();
    }
}

int PKB::getNumOfRelationship(RELATIONSHIP relation) {
    switch (relation) {
    case RELATIONSHIP::AFFECTS_BIP:
        return affectsBipSet.size();
    case RELATIONSHIP::CALLS:
        return callsSet.size();
    case RELATIONSHIP::CALLS_STAR:
        return callsStarSet.size();
    case RELATIONSHIP::FOLLOWS:
        return followsSet.size();
    case RELATIONSHIP::FOLLOWS_STAR:
        return followsStarSet.size();
    case RELATIONSHIP::MODIFIES:
        return modifiesSetByLineNum.size();
    case RELATIONSHIP::NEXT:
        return nextSet.size();
    case RELATIONSHIP::NEXT_BIP:
        return nextBipSet.size();
    case RELATIONSHIP::NEXT_BIP_STAR:
        return nextBipStarSet.size();
    case RELATIONSHIP::PARENT:
        return parentSet.size();
    case RELATIONSHIP::PARENT_STAR:
        return parentStarSet.size();
    case RELATIONSHIP::USES:
        return usesSetByLineNum.size();
    default:
        throw InvalidRelationship();
    }
}
