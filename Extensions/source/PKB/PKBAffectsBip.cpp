#include "PKB.h"

void PKB::storeRelationshipAffectsBip(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    affectsBipSet.insert({ lineBefore, lineAfter });

    if (affectsBipMapKeyLineBefore.find(lineBefore) == affectsBipMapKeyLineBefore.end()) {
        affectsBipMapKeyLineBefore.insert({ lineBefore, std::vector<LINE_NUM>() });
    }
    affectsBipMapKeyLineBefore[lineBefore].push_back(lineAfter);

    if (affectsBipMapKeyLineAfter.find(lineAfter) == affectsBipMapKeyLineAfter.end()) {
        affectsBipMapKeyLineAfter.insert({ lineAfter, std::vector<LINE_NUM>() });
    }
    affectsBipMapKeyLineAfter[lineAfter].push_back(lineBefore);

    storeRelationshipAffectsBipStar(lineBefore, lineAfter);
}

void PKB::storeRelationshipAffectsBipStar(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    affectsBipStarSet.insert({ lineBefore, lineAfter });

    if (affectsBipStarMapKeyLineBefore.find(lineBefore) == affectsBipStarMapKeyLineBefore.end()) {
        affectsBipStarMapKeyLineBefore.insert({ lineBefore, std::vector<LINE_NUM>() });
    }
    affectsBipStarMapKeyLineBefore[lineBefore].push_back(lineAfter);

    if (affectsBipStarMapKeyLineAfter.find(lineAfter) == affectsBipStarMapKeyLineAfter.end()) {
        affectsBipStarMapKeyLineAfter.insert({ lineAfter, std::vector<LINE_NUM>() });
    }
    affectsBipStarMapKeyLineAfter[lineAfter].push_back(lineBefore);
}

bool PKB::hasAffectsBipRelationship() {
    return !affectsBipSet.empty();
}

bool PKB::hasAffectsBipRelationship(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    return affectsBipSet.find({ lineBefore, lineAfter }) != affectsBipSet.end();
}

bool PKB::hasAffectsBipLine(LINE_NUM lineBefore) {
    return affectsBipMapKeyLineBefore.find(lineBefore) != affectsBipMapKeyLineBefore.end();
}

bool PKB::hasAffectingBipLine(LINE_NUM lineAfter) {
    return affectsBipMapKeyLineAfter.find(lineAfter) != affectsBipMapKeyLineAfter.end();
}

std::vector<LINE_NUM> PKB::getAffectsBipLines(LINE_NUM lineBefore, DESIGN_ENTITY designEnt) {
    // Select s such that AffectsBip(1, s)
    std::vector<LINE_NUM> result;
    if (!hasAffectsBipLine(lineBefore)) {
        return result;
    }
    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    std::vector<LINE_NUM> toSearch = affectsBipMapKeyLineBefore[lineBefore];
    for (LINE_NUM lineAfter : toSearch) {
        if (designEntSet->find(lineAfter) != designEntSet->end()) {
            result.push_back(lineAfter);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getAffectsBipLines(DESIGN_ENTITY designEnt) {
    // Select s such that AffectsBip(_, s)
    std::vector<LINE_NUM> result;

    std::vector<LINE_NUM>* designEntList = selectDataStructure(designEnt);
    for (LINE_NUM lineNum : *designEntList) {
        if (hasAffectingBipLine(lineNum)) {
            result.push_back(lineNum);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getAffectingBipLines(DESIGN_ENTITY designEnt, LINE_NUM lineAfter) {
    // Select s such that AffectsBip(s, 4)
    std::vector<LINE_NUM> result;
    if (!hasAffectingBipLine(lineAfter)) {
        return result;
    }
    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    std::vector<LINE_NUM> toSearch = affectsBipMapKeyLineAfter[lineAfter];
    for (LINE_NUM lineBefore : toSearch) {
        if (designEntSet->find(lineBefore) != designEntSet->end()) {
            result.push_back(lineBefore);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getAffectingBipLines(DESIGN_ENTITY designEnt) {
    // Select s such that AffectsBip(s, _)
    std::vector<LINE_NUM> result;

    std::vector<LINE_NUM>* designEntList = selectDataStructure(designEnt);
    for (LINE_NUM lineNum : *designEntList) {
        if (hasAffectsBipLine(lineNum)) {
            result.push_back(lineNum);
        }
    }
    return result;
}

std::vector<std::pair<LINE_NUM, LINE_NUM>>
    PKB::getAffectsBipLinesPair(DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    std::vector<LINE_NUM>* leftEntityList;
    std::unordered_set<LINE_NUM>* rightEntitySet;
    leftEntityList = selectDataStructure(leftArg);
    rightEntitySet = selectDataSet(rightArg);

    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    for (auto& lineBefore : *leftEntityList) {
        if (!hasAffectsBipLine(lineBefore)) {
            continue;
        }
        auto toSearch = affectsBipMapKeyLineBefore[lineBefore];
        for (LINE_NUM lineAfter : toSearch) {
            if (rightEntitySet->find(lineAfter) != rightEntitySet->end()) {
                result.push_back({ lineBefore, lineAfter });
            }
        }
    }
    return result;
}

bool PKB::hasAffectsBipStarRelationship() {
    return !affectsBipStarSet.empty();
}

bool PKB::hasAffectsBipStarRelationship(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    return affectsBipStarSet.find({ lineBefore, lineAfter }) != affectsBipStarSet.end();
}
std::vector<LINE_NUM> PKB::getAffectsBipStarLines(LINE_NUM lineBefore, DESIGN_ENTITY designEnt) {
    std::vector<LINE_NUM> result;
    if (!hasAffectsBipLine(lineBefore)) {
        return result;
    }
    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    std::vector<LINE_NUM> toSearch = affectsBipStarMapKeyLineBefore[lineBefore];
    for (LINE_NUM lineAfter : toSearch) {
        if (designEntSet->find(lineAfter) != designEntSet->end()) {
            result.push_back(lineAfter);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getAffectsBipStarLines(DESIGN_ENTITY designEnt) {
    return getAffectsBipLines(designEnt);
}

std::vector<LINE_NUM> PKB::getAffectingBipStarLines(DESIGN_ENTITY designEnt, LINE_NUM lineAfter) {
    std::vector<LINE_NUM> result;
    if (!hasAffectingBipLine(lineAfter)) {
        return result;
    }
    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    std::vector<LINE_NUM> toSearch = affectsBipStarMapKeyLineAfter[lineAfter];
    for (LINE_NUM lineBefore : toSearch) {
        if (designEntSet->find(lineBefore) != designEntSet->end()) {
            result.push_back(lineBefore);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getAffectingBipStarLines(DESIGN_ENTITY designEnt) {
    return getAffectingBipLines(designEnt);
}

std::vector<std::pair<LINE_NUM, LINE_NUM>>
    PKB::getAffectsBipStarLinesPair(DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    std::vector<LINE_NUM>* leftEntityList;
    std::unordered_set<LINE_NUM>* rightEntitySet;
    leftEntityList = selectDataStructure(leftArg);
    rightEntitySet = selectDataSet(rightArg);

    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    for (auto& lineBefore : *leftEntityList) {
        if (!hasAffectsBipLine(lineBefore)) {
            continue;
        }
        auto toSearch = affectsBipStarMapKeyLineBefore[lineBefore];
        for (LINE_NUM lineAfter : toSearch) {
            if (rightEntitySet->find(lineAfter) != rightEntitySet->end()) {
                result.push_back({ lineBefore, lineAfter });
            }
        }
    }
    return result;
}
