#include "PKB.h"

void PKB::storeRelationshipNextBip(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    // if call line is 0 means no ambiguity
    storeRelationshipNextBip(lineBefore, lineAfter, 0);
}

void PKB::storeRelationshipNextBip(LINE_NUM lineBefore, LINE_NUM lineAfter, LINE_NUM callLine) {
    nextBipSet.insert({ lineBefore, lineAfter });

    if (nextBipMapKeyLineBefore.find(lineBefore) == nextBipMapKeyLineBefore.end()) {
        nextBipMapKeyLineBefore.insert({lineBefore, std::vector<std::pair<LINE_NUM, LINE_NUM>>()});
    }
    nextBipMapKeyLineBefore[lineBefore].push_back({ callLine, lineAfter });

    if (nextBipMapKeyLineAfter.find(lineAfter) == nextBipMapKeyLineAfter.end()) {
        nextBipMapKeyLineAfter.insert({ lineAfter, std::vector<std::pair<LINE_NUM, LINE_NUM>>() });
    }
    nextBipMapKeyLineAfter[lineAfter].push_back({ callLine, lineBefore });
}

void PKB::storeRelationshipNextBipStar(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    nextBipStarSet.insert({ lineBefore, lineAfter });

    if (nextBipStarMapKeyLineBefore.find(lineBefore) == nextBipStarMapKeyLineBefore.end()) {
        nextBipStarMapKeyLineBefore.insert({ lineBefore, std::vector<LINE_NUM>() });
    }
    nextBipStarMapKeyLineBefore[lineBefore].push_back(lineAfter);

    if (nextBipStarMapKeyLineAfter.find(lineAfter) == nextBipStarMapKeyLineAfter.end()) {
        nextBipStarMapKeyLineAfter.insert({ lineAfter, std::vector<LINE_NUM>() });
    }
    nextBipStarMapKeyLineAfter[lineAfter].push_back(lineBefore);
}

bool PKB::hasNextBipRelationship() {
    return !nextBipSet.empty();
}
bool PKB::hasNextBipRelationship(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    return nextBipSet.find({ lineBefore, lineAfter }) != nextBipSet.end();
}
bool PKB::hasNextBipLine(LINE_NUM lineBefore) {
    return nextBipMapKeyLineBefore.find(lineBefore) != nextBipMapKeyLineBefore.end();
}
bool PKB::hasPrevBipLine(LINE_NUM lineAfter) {
    return nextBipMapKeyLineAfter.find(lineAfter) != nextBipMapKeyLineAfter.end();
}

std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getNextBipLines(LINE_NUM lineBefore) {
    if (hasNextBipLine(lineBefore)) {
        return nextBipMapKeyLineBefore[lineBefore];
    }
    return std::vector<std::pair<LINE_NUM, LINE_NUM>>();
}

std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getPrevBipLines(LINE_NUM lineAfter) {
    if (hasPrevBipLine(lineAfter)) {
        return nextBipMapKeyLineAfter[lineAfter];
    }
    return std::vector<std::pair<LINE_NUM, LINE_NUM>>();
}


std::vector<LINE_NUM> PKB::getNextBipLines(LINE_NUM lineBefore, DESIGN_ENTITY designEnt) {
    // Select s such that NextBip(1, s)
    std::vector<LINE_NUM> result;
    if (!hasNextBipLine(lineBefore)) {
        return result;
    }

    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    for (auto pair : nextBipMapKeyLineBefore[lineBefore]) {
        if (designEntSet->find(pair.second) != designEntSet->end()) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getNextBipLines(DESIGN_ENTITY designEnt) {
    // Select s such that NextBip(_, s)
    std::vector<LINE_NUM> result;

    std::vector<LINE_NUM>* designEntList = selectDataStructure(designEnt);
    for (LINE_NUM lineNum : *designEntList) {
        if (hasPrevBipLine(lineNum)) {
            result.push_back(lineNum);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getPrevBipLines(DESIGN_ENTITY designEnt, LINE_NUM lineAfter) {
    // Select s such that NextBip(s, 4)
    std::vector<LINE_NUM> result;
    if (!hasPrevBipLine(lineAfter)) {
        return result;
    }

    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    for (auto pair : nextBipMapKeyLineAfter[lineAfter]) {
        if (designEntSet->find(pair.second) != designEntSet->end()) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getPrevBipLines(DESIGN_ENTITY designEnt) {
    // Select s such that NextBip(s, _)
    std::vector<LINE_NUM> result;

    std::vector<LINE_NUM>* designEntList = selectDataStructure(designEnt);
    for (LINE_NUM lineNum : *designEntList) {
        if (hasNextBipLine(lineNum)) {
            result.push_back(lineNum);
        }
    }
    return result;
}

std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getNextBipLinesPair
    (DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    // Select <s1, s2> such that NextBip(s1, s2)
    std::vector<LINE_NUM>* leftEntityList;
    std::unordered_set<LINE_NUM>* rightEntitySet;
    leftEntityList = selectDataStructure(leftArg);
    rightEntitySet = selectDataSet(rightArg);

    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    for (auto& lineBefore : *leftEntityList) {
        if (!hasNextBipLine(lineBefore)) {
            continue;
        }
        auto toSearch = nextBipMapKeyLineBefore[lineBefore];
        for (auto& pair : toSearch) {
            if (rightEntitySet->find(pair.second) != rightEntitySet->end()) {
                result.push_back({ lineBefore, pair.second });
            }
        }
    }
    return result;
}

bool PKB::hasNextBipStarRelationship() {
    return !nextBipStarSet.empty();
}

bool PKB::hasNextBipStarRelationship(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    return nextBipStarSet.find({ lineBefore, lineAfter }) != nextBipStarSet.end();
}

std::vector<LINE_NUM> PKB::getNextBipStarLines(LINE_NUM lineBefore, DESIGN_ENTITY designEnt) {
    // Select s such that NextBip*(1, s)
    std::vector<LINE_NUM> result;
    if (!hasNextBipLine(lineBefore)) {
        return result;
    }

    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    for (LINE_NUM lineAfter : nextBipStarMapKeyLineBefore[lineBefore]) {
        if (designEntSet->find(lineAfter) != designEntSet->end()) {
            result.push_back(lineAfter);
        }
    }
    return result;
}
std::vector<LINE_NUM> PKB::getNextBipStarLines(DESIGN_ENTITY designEnt) {
    // Select s such that NextBip*(_, s)
    std::vector<LINE_NUM> result;

    std::vector<LINE_NUM>* designEntityVec = selectDataStructure(designEnt);
    for (LINE_NUM lineAfter : *designEntityVec) {
        if (hasPrevBipLine(lineAfter)) {
            result.push_back(lineAfter);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getPrevBipStarLines(DESIGN_ENTITY designEnt, LINE_NUM lineAfter) {
    // Select s such that NextBip*(s, 3)
    std::vector<LINE_NUM> result;
    if (!hasPrevBipLine(lineAfter)) {
        return result;
    }

    std::unordered_set<LINE_NUM>* designEntSet = selectDataSet(designEnt);
    for (LINE_NUM lineBefore : nextBipStarMapKeyLineAfter[lineAfter]) {
        if (designEntSet->find(lineBefore) != designEntSet->end()) {
            result.push_back(lineBefore);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getPrevBipStarLines(DESIGN_ENTITY designEnt) {
    // Select s such that NextBip*(s, _)
    std::vector<LINE_NUM> result;

    std::vector<LINE_NUM>* designEntityVec = selectDataStructure(designEnt);
    for (LINE_NUM lineBefore : *designEntityVec) {
        if (hasNextBipLine(lineBefore)) {
            result.push_back(lineBefore);
        }
    }
    return result;
}

std::vector<std::pair<LINE_NUM, LINE_NUM>>
    PKB::getNextBipStarLinesPair(DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    std::vector<LINE_NUM>* leftEntityList;
    std::unordered_set<LINE_NUM>* rightEntitySet;
    leftEntityList = selectDataStructure(leftArg);
    rightEntitySet = selectDataSet(rightArg);

    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    for (auto& lineBefore : *leftEntityList) {
        if (!hasNextBipLine(lineBefore)) {
            continue;
        }
        auto toSearch = nextBipStarMapKeyLineBefore[lineBefore];
        for (LINE_NUM lineAfter : toSearch) {
            if (rightEntitySet->find(lineAfter) != rightEntitySet->end()) {
                result.push_back({ lineBefore, lineAfter });
            }
        }
    }
    return result;
}
