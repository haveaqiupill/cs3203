#include "PKB.h"
#include "../Commons/Exceptions.h"

void PKB::storeRelationshipNext(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    std::pair <LINE_NUM, LINE_NUM> temp = std::make_pair(lineBefore, lineAfter);
    this->nextSet.insert(temp);

    auto searchLineBefore = this->nextMapKeyLineBefore.find(lineBefore);
    if (searchLineBefore == this->nextMapKeyLineBefore.end()) {
        this->nextMapKeyLineBefore.insert({ lineBefore, std::vector<LINE_NUM>() });
    }
    this->nextMapKeyLineBefore[lineBefore].push_back(lineAfter);

    auto searchLineAfter = this->nextMapKeyLineAfter.find(lineAfter);
    if (searchLineAfter == this->nextMapKeyLineAfter.end()) {
        this->nextMapKeyLineAfter.insert({ lineAfter, std::vector<LINE_NUM>() });
    }
    this->nextMapKeyLineAfter[lineAfter].push_back(lineBefore);
}

std::vector<LINE_NUM> PKB::getNextLines(LINE_NUM lineBefore) {
    if (hasNextLine(lineBefore)) {
        return nextMapKeyLineBefore[lineBefore];
    }
    return std::vector<LINE_NUM>();
}

bool PKB::hasNextRelationship() {
    // Select s such that Next(_, _)
    return !nextSet.empty();
}

bool PKB::hasNextRelationship(LINE_NUM lineBefore, LINE_NUM lineAfter) {
    // Select s such that Next(1, 2)
    std::pair <LINE_NUM, LINE_NUM> temp = std::make_pair(lineBefore, lineAfter);
    return nextSet.find(temp) != nextSet.end();
}

bool PKB::hasNextLine(LINE_NUM lineBefore) {
    // Select s such that Next(1, _)
    return nextMapKeyLineBefore.find(lineBefore) != nextMapKeyLineBefore.end();
}

bool PKB::hasPrevLine(LINE_NUM lineAfter) {
    // Select s such that Next(_, 2)
    return nextMapKeyLineAfter.find(lineAfter) != nextMapKeyLineAfter.end();
}

std::vector<LINE_NUM> PKB::getNextLines(LINE_NUM lineBefore, DESIGN_ENTITY designEntity) {
    // Select s such that Next(1, s)
    std::vector<LINE_NUM> *designEntityList;
    switch (designEntity) {
    case DesignEntity::PROCEDURE:
        throw InvalidDesignEntity();
    default:
        designEntityList = selectDataStructure(designEntity);
    }

    std::vector<LINE_NUM> result;
    if (!hasNextLine(lineBefore)) {
        return result;
    }
    auto toSearch = nextMapKeyLineBefore[lineBefore];
    auto pointer = designEntityList->begin();
    std::vector<LINE_NUM>::iterator temp;
    for (auto& lineNum : toSearch) {
        if ((temp = std::find(pointer, designEntityList->end(), lineNum))
            != designEntityList->end()) {
            pointer = temp;
            result.push_back(lineNum);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getPrevLines(DESIGN_ENTITY designEntity, LINE_NUM lineAfter) {
    // Select s such that Next(s, 2)
    std::vector<LINE_NUM>* designEntityList;
    switch (designEntity) {
    case DesignEntity::PROCEDURE:
        throw InvalidDesignEntity();
    default:
        designEntityList = selectDataStructure(designEntity);
    }

    std::vector<LINE_NUM> result;
    if (!hasPrevLine(lineAfter)) {
        return result;
    }
    auto toSearch = nextMapKeyLineAfter[lineAfter];
    auto pointer = designEntityList->begin();
    std::vector<LINE_NUM>::iterator temp;
    for (auto& lineNum : toSearch) {
        if ((temp = std::find(pointer, designEntityList->end(), lineNum))
            != designEntityList->end()) {
            pointer = temp;
            result.push_back(lineNum);
        }
    }
    return result;
}
std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getNextLinesPair
    (DESIGN_ENTITY left, DESIGN_ENTITY right) {
    // Select s1 such that Next(s1, s2)
    std::vector<LINE_NUM>* leftEntityList;
    std::unordered_set<LINE_NUM>* rightEntitySet;
    leftEntityList = selectDataStructure(left);
    rightEntitySet = selectDataSet(right);

    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    for (auto& lineBefore : *leftEntityList) {
        if (!hasNextLine(lineBefore)) {
            continue;
        }
        auto toSearch = nextMapKeyLineBefore[lineBefore];
        for (auto& lineAfter : toSearch) {
            if (rightEntitySet->find(lineAfter) != rightEntitySet->end()) {
                result.push_back({ lineBefore, lineAfter });
            }
        }
    }
    return result;
}
std::vector<LINE_NUM> PKB::getNextLines(DESIGN_ENTITY designEntity) {
    // Select s such that Next(_, s)
    std::vector<LINE_NUM>* designEntityList;
    switch (designEntity) {
    case DesignEntity::PROCEDURE:
        throw InvalidDesignEntity();
    default:
        designEntityList = selectDataStructure(designEntity);
    }

    std::vector<LINE_NUM> result;
    for (auto& lineNum : *designEntityList) {
        if (hasPrevLine(lineNum)) {
            result.push_back(lineNum);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getPrevLines(DESIGN_ENTITY designEntity) {
    // Select s such that Next(s, _)
    std::vector<LINE_NUM>* designEntityList;
    switch (designEntity) {
    case DesignEntity::PROCEDURE:
        throw InvalidDesignEntity();
    default:
        designEntityList = selectDataStructure(designEntity);
    }

    std::vector<LINE_NUM> result;
    for (auto& lineNum : *designEntityList) {
        if (hasNextLine(lineNum)) {
            result.push_back(lineNum);
        }
    }
    return result;
}

std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* PKB::getNextMapKeyLineBefore() {
    return &nextMapKeyLineBefore;
}

std::unordered_map<LINE_NUM, std::vector<LINE_NUM>>* PKB::getNextMapKeyLineAfter() {
    return &nextMapKeyLineAfter;
}
