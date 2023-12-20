#include "SuchThatEvaluator.h"

#include <queue>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../Commons/Exceptions.h"
#include "../QueryEvaluator.h"

ClauseAnswer SuchThatEvaluator::evaluateSuchThatClause(Relationship relationship,
                                                       QueryArg& leftArg,
                                                       QueryArg& rightArg,
                                                       DECLARATION_TABLE& declarationTable) {
    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if ((leftArgType == QueryArgType::NUM || leftArgType == QueryArgType::NAME) &&
        (rightArgType == QueryArgType::NUM || rightArgType == QueryArgType::NAME)) {
        // Case where it is a boolean evaluation
        // Eg such that Follows(1, 2) OR such that Uses(1, "a")
        return evaluateBooleanRelationship(relationship, leftArg, rightArg);
    } else {
        // Case where arguments contain wildcards or synonyms
        // Eg such that Follows(1, v) OR such that Uses(p, _)
        return evaluateSynRelationship(relationship, leftArg, rightArg, declarationTable);
    }
}

ClauseAnswer SuchThatEvaluator::evaluateBooleanRelationship(Relationship relationship,
                                                            QueryArg& leftArg,
                                                            QueryArg& rightArg) {
    // Left argument type is the determining factor for Uses and Modifies
    ARG_TYPE leftArgType = leftArg.getArgType();

    bool isClauseTrue =  true;

    switch (relationship) {
        case Relationship::FOLLOWS:
            // Follows can only take in LINE_NUM and not NAME
            isClauseTrue = pkb->doesFollows(leftArg.getLineNum(), rightArg.getLineNum());
            break;
        case Relationship::FOLLOWS_STAR:
            // Follows can only take in LINE_NUM and not NAME
            isClauseTrue = pkb->doesFollowsStar(leftArg.getLineNum(), rightArg.getLineNum());
            break;
        case Relationship::PARENT:
            // Parent can only take in LINE_NUM and not NAME
            isClauseTrue = pkb->doesParent(leftArg.getLineNum(), rightArg.getLineNum());
            break;
        case Relationship::PARENT_STAR:
            // ParentStar can only take in LINE_NUM and not NAME
            isClauseTrue = pkb->doesParentStar(leftArg.getLineNum(), rightArg.getLineNum());
            break;
        case Relationship::USES:
            // First arg can take in both LINE_NUM and NAME. Second arg can only take in NAME.
            switch (leftArgType) {
                case QueryArgType::NUM:
                    // Uses(1, "hello") where the LINE_NUM can be an assignment, print or container
                    isClauseTrue = pkb->doesLineNumUses(leftArg.getLineNum(), rightArg.getName());
                    break;
                case QueryArgType::NAME:
                    // Uses("main", "x")
                    isClauseTrue = pkb->doesProcedureUses(leftArg.getName(), rightArg.getName());
                    break;
            }
            break;
        case Relationship::MODIFIES:
            // First arg can take in both LINE_NUM and NAME. Second arg can only take in NAME.
            switch (leftArgType) {
                case QueryArgType::NUM:
                    // Modifies(1, "hello") where the LINE_NUM can be assignment, read or container
                    isClauseTrue = pkb->doesLineNumModifies(leftArg.getLineNum(),
                                                            rightArg.getName());
                    break;
                case QueryArgType::NAME:
                    // Modifies("main", "x")
                    isClauseTrue = pkb->doesProcedureModifies(leftArg.getName(),
                                                              rightArg.getName());
                    break;
            }
            break;
        case Relationship::CALLS:
            isClauseTrue = pkb->doesProcedureCalls(leftArg.getName(), rightArg.getName());
            break;
        case Relationship::CALLS_STAR:
            isClauseTrue = pkb->doesProcedureCallsStar(leftArg.getName(), rightArg.getName());
            break;
        case Relationship::NEXT:
            isClauseTrue = pkb->hasNextRelationship(leftArg.getLineNum(), rightArg.getLineNum());
            break;
        case Relationship::NEXT_STAR:
            isClauseTrue = nextStarEval.doesLinesNextStar(leftArg.getLineNum(),
                rightArg.getLineNum());
            break;
        case Relationship::NEXT_BIP:
            isClauseTrue = pkb->hasNextBipRelationship(leftArg.getLineNum(),
                rightArg.getLineNum());
            break;
        case Relationship::NEXT_BIP_STAR:
            isClauseTrue = pkb->hasNextBipStarRelationship(leftArg.getLineNum(),
                rightArg.getLineNum());
            break;
        case Relationship::AFFECTS_BIP:
            isClauseTrue = pkb->hasAffectsBipRelationship(leftArg.getLineNum(),
                rightArg.getLineNum());
            break;
        case Relationship::AFFECTS_BIP_STAR:
            isClauseTrue = pkb->hasAffectsBipStarRelationship(leftArg.getLineNum(),
                rightArg.getLineNum());
            break;
        case Relationship::AFFECTS:
            isClauseTrue = affectsEvaluator.doLinesAffects(leftArg.getLineNum(),
                rightArg.getLineNum());
            break;
        case Relationship::AFFECTS_STAR:
            isClauseTrue = affectsStarEvaluator.doLinesAffectsStar(leftArg.getLineNum(),
                rightArg.getLineNum());
            break;
        default:
            throw InvalidRelationshipException();
    }

    return ClauseAnswer(isClauseTrue, ResultTable());
}

ClauseAnswer SuchThatEvaluator::evaluateSynRelationship(Relationship relationship,
                                                        QueryArg& leftArg,
                                                        QueryArg& rightArg,
                                                        DECLARATION_TABLE& dt) {
    switch (relationship) {
    case Relationship::FOLLOWS:
        return followsEval.evaluateFollows(leftArg, rightArg, dt);
    case Relationship::FOLLOWS_STAR:
        return followsStarEval.evaluateFollowsStar(leftArg, rightArg, dt);
    case Relationship::PARENT:
        return parentEval.evaluateParent(leftArg, rightArg, dt);
    case Relationship::PARENT_STAR:
        return parentStarEval.evaluateParentStar(leftArg, rightArg, dt);
    case Relationship::USES:
        return usesEval.evaluateUses(leftArg, rightArg, dt);
    case Relationship::MODIFIES:
        return modifiesEval.evaluateModifies(leftArg, rightArg, dt);
    case Relationship::CALLS:
        return callsEval.evaluateCalls(leftArg, rightArg, dt);
    case Relationship::CALLS_STAR:
        return callsStarEval.evaluateCallsStar(leftArg, rightArg, dt);
    case Relationship::NEXT:
        return nextEval.evaluateNext(leftArg, rightArg, dt);
    case Relationship::NEXT_STAR:
        return nextStarEval.evaluateNextStar(leftArg, rightArg, dt);
    case Relationship::NEXT_BIP:
        return nextBipEval.evaluateNextBip(leftArg, rightArg, dt);
    case Relationship::NEXT_BIP_STAR:
        return nextBipStarEval.evaluateNextBipStar(leftArg, rightArg, dt);
    case Relationship::AFFECTS_BIP:
        return affectsBipEval.evaluateAffectsBip(leftArg, rightArg, dt);
    case Relationship::AFFECTS_BIP_STAR:
        return affectsBipStarEval.evaluateAffectsBipStar(leftArg, rightArg, dt);
    case Relationship::AFFECTS:
        return affectsEvaluator.evaluateAffects(leftArg, rightArg, dt);
    case Relationship::AFFECTS_STAR:
        return affectsStarEvaluator.evaluateAffectsStar(leftArg, rightArg, dt);
    default:
        throw InvalidRelationshipException();
    }
}
