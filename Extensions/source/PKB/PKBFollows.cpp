#include "PKB.h"

// Setter method to interact with front-end (Parser & DesignExtractor)
void PKB::storeRelationshipFollows(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    std::pair <LINE_NUM, LINE_NUM> temp = std::make_pair(lineBefore, lineAfter);
    this->followsSet.insert(temp);

    this->followsMapKeyLineBefore.insert({ lineBefore, std::vector<LINE_NUM>() });
    this->followsMapKeyLineBefore[lineBefore].push_back(lineAfter);

    this->followsMapKeyLineAfter.insert({ lineAfter, std::vector<LINE_NUM>() });
    this->followsMapKeyLineAfter[lineAfter].push_back(lineBefore);

    this->storeRelationshipFollowsStar(lineBefore, lineAfter);
}

void PKB::storeRelationshipFollowsStar(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    std::pair <LINE_NUM, LINE_NUM> temp = std::make_pair(lineBefore, lineAfter);
    this->followsStarSet.insert(temp);

    auto searchLineBefore = this->followsStarMapKeyLineBefore.find(lineBefore);
    if (searchLineBefore == this->followsStarMapKeyLineBefore.end()) {
        this->followsStarMapKeyLineBefore.insert({ lineBefore, std::vector<LINE_NUM>() });
    }
    this->followsStarMapKeyLineBefore[lineBefore].push_back(lineAfter);

    auto searchLineAfter = this->followsStarMapKeyLineAfter.find(lineAfter);
    if (searchLineAfter == this->followsStarMapKeyLineAfter.end()) {
        this->followsStarMapKeyLineAfter.insert({ lineAfter, std::vector<LINE_NUM>() });
    }
    this->followsStarMapKeyLineAfter[lineAfter].push_back(lineBefore);
}

// Getter method to interact with PKB (DE and PKB)
bool PKB::doesFollows(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    // Sample query: Select s such that Follows(1, 2)
    std::pair<LINE_NUM, LINE_NUM> followsPair(lineBefore, lineAfter);
    return followsSet.find(followsPair) != followsSet.end();
}

bool PKB::doesFollowsStar(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    // Sample query: Select s such that Follows*(1, 2)
    std::pair<LINE_NUM, LINE_NUM> followsStarPair(lineBefore, lineAfter);
    return followsStarSet.find(followsStarPair) != followsStarSet.end();
}

std::vector<LINE_NUM> PKB::getLineAfterFromFollows(LINE_NUM lineBefore) {
    // Sample query: Select s such that Follows(1, s)
    if (hasFollower(lineBefore)) {
        return followsMapKeyLineBefore[lineBefore];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<LINE_NUM> PKB::getLineAfterFromFollowsStar(LINE_NUM lineBefore) {
    // Sample query: Select s such that Follows*(1, s)
    if (hasFollower(lineBefore)) {
        return followsStarMapKeyLineBefore[lineBefore];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<LINE_NUM> PKB::getLineBeforeFromFollows(LINE_NUM lineAfter) {
    // Sample query: Select s such that Follows(s, 2)
    if (hasFollowee(lineAfter)) {
        return followsMapKeyLineAfter[lineAfter];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<LINE_NUM> PKB::getLineBeforeFromFollowsStar(LINE_NUM lineAfter) {
    // Sample query: Select s such that Follows*(s, 2)
    if (hasFollowee(lineAfter)) {
        return followsStarMapKeyLineAfter[lineAfter];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<LINE_NUM> PKB::getFollowsAfter(LINE_NUM lineBefore, DESIGN_ENTITY designEntity) {
    // Select s such that Follows(3, s)
    std::vector<LINE_NUM> result;
    if (!hasFollower(lineBefore)) {
        return result;
    }
    LINE_NUM lineAfter = followsMapKeyLineBefore[lineBefore][0];
    std::unordered_set<LINE_NUM>* designEntitySet = selectDataSet(designEntity);
    if (designEntitySet->find(lineAfter) != designEntitySet->end()) {
        result.push_back(lineAfter);
    }
    return result;
}
std::vector<LINE_NUM> PKB::getFollowsAfter(DESIGN_ENTITY designEntity) {
    // Select s such that Follows(_, s)
    std::vector<LINE_NUM>* designEntityVec = selectDataStructure(designEntity);
    std::vector<LINE_NUM> result;
    for (LINE_NUM lineAfter : *designEntityVec) {
        if (hasFollowee(lineAfter)) {
            result.push_back(lineAfter);
        }
    }
    return result;
}
std::vector<LINE_NUM> PKB::getFollowsBefore(DESIGN_ENTITY designEnt, LINE_NUM lineAfter) {
    // Select s such that Follows(s, 3)
    std::vector<LINE_NUM> result;
    if (!hasFollowee(lineAfter)) {
        return result;
    }
    LINE_NUM lineBefore = followsMapKeyLineAfter[lineAfter][0];
    std::unordered_set<LINE_NUM>* designEntitySet = selectDataSet(designEnt);
    if (designEntitySet->find(lineBefore) != designEntitySet->end()) {
        result.push_back(lineBefore);
    }
    return result;
}
std::vector<LINE_NUM> PKB::getFollowsBefore(DESIGN_ENTITY designEnt) {
    // Select s such that Follows(s, _)
    std::vector<LINE_NUM>* designEntityVec = selectDataStructure(designEnt);
    std::vector<LINE_NUM> result;
    for (LINE_NUM lineBefore : *designEntityVec) {
        if (hasFollower(lineBefore)) {
            result.push_back(lineBefore);
        }
    }
    return result;
}

std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getFollowsPair
    (DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    // Select <s. s1> such that Follows(s, s1)
    auto leftArgVec = selectDataStructure(leftArg);
    auto rightArgVec = selectDataStructure(rightArg);

    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    for (LINE_NUM lineBefore : *leftArgVec) {
        if (!hasFollower(lineBefore)) {
            continue;
        }
        std::vector<LINE_NUM>* searchList = &followsMapKeyLineBefore[lineBefore];
        auto start = searchList->begin();
        auto end = searchList->end();
        std::vector<LINE_NUM>::iterator temp;
        for (LINE_NUM lineAfter : *rightArgVec) {
            if ((temp = std::find(start, end, lineAfter)) != end) {
                result.push_back({ lineBefore, lineAfter });
                start = temp;
            }
        }
    }
    return result;
}

bool PKB::hasFollowsRelationship() {
    // Select BOOLEAN such that Follows(_, _)
    return !followsSet.empty();
}
bool PKB::hasFollowee(LINE_NUM lineNum) {
    // Select BOOLEAN such that Follows(_, 3)
    return followsMapKeyLineAfter.find(lineNum) != followsMapKeyLineAfter.end();
}
bool PKB::hasFollower(LINE_NUM lineNum) {
    // Select BOOLEAN such that Follows(3, _)
    return followsMapKeyLineBefore.find(lineNum) != followsMapKeyLineBefore.end();
}

std::vector<LINE_NUM> PKB::getFollowsStarAfter(LINE_NUM lineBefore, DESIGN_ENTITY designEntity) {
    // Select s such that Follows*(3, s)
    std::vector<LINE_NUM> result;
    if (!hasFollower(lineBefore)) {
        return result;
    }
    std::vector<LINE_NUM> linesAfter = followsStarMapKeyLineBefore[lineBefore];
    std::vector<LINE_NUM>* designEntityVec = selectDataStructure(designEntity);
    auto start = designEntityVec->begin();
    std::vector<LINE_NUM>::iterator temp;
    for (LINE_NUM lineAfter : linesAfter) {
        if ((temp = std::find(start, designEntityVec->end(), lineAfter))
            != designEntityVec->end()) {
            result.push_back(lineAfter);
            start = temp;
        }
    }
    return result;
}
std::vector<LINE_NUM> PKB::getFollowsStarAfter(DESIGN_ENTITY designEnt) {
    // Select s such that Follows*(_, s) == Follows(_, s)
    return getFollowsAfter(designEnt);
}

std::vector<LINE_NUM> PKB::getFollowsStarBefore(DESIGN_ENTITY designEnt, LINE_NUM lineAfter) {
    // Select s such that Follows*(s, 3)
    std::vector<LINE_NUM> result;
    if (!hasFollowee(lineAfter)) {
        return result;
    }
    std::vector<LINE_NUM> linesBefore = followsStarMapKeyLineAfter[lineAfter];
    std::vector<LINE_NUM>* designEntityVec = selectDataStructure(designEnt);
    auto start = designEntityVec->begin();
    std::vector<LINE_NUM>::iterator temp;
    for (LINE_NUM lineBefore : linesBefore) {
        if ((temp = std::find(start, designEntityVec->end(), lineBefore))
            != designEntityVec->end()) {
            result.push_back(lineBefore);
            start = temp;
        }
    }
    return result;
}
std::vector<LINE_NUM> PKB::getFollowsStarBefore(DESIGN_ENTITY designEnt) {
    // Select s such that Follows*(s, _) == Follows(s, _)
    return getFollowsBefore(designEnt);
}
std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getFollowsStarPair
(DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    auto leftArgVec = selectDataStructure(leftArg);
    auto rightArgVec = selectDataStructure(rightArg);

    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    for (LINE_NUM lineBefore : *leftArgVec) {
        if (!hasFollower(lineBefore)) {
            continue;
        }
        std::vector<LINE_NUM>* searchList = &followsStarMapKeyLineBefore[lineBefore];
        auto start = searchList->begin();
        auto end = searchList->end();
        std::vector<LINE_NUM>::iterator temp;
        for (LINE_NUM lineAfter : *rightArgVec) {
            if ((temp = std::find(start, end, lineAfter)) != end) {
                result.push_back({ lineBefore, lineAfter });
                start = temp;
            }
        }
    }
    return result;
}
