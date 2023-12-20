#include "PKB.h"
#include "../Commons/Exceptions.h"

void PKB::storeRelationshipParent(LINE_NUM parent, LINE_NUM child) {
    std::pair<LINE_NUM, LINE_NUM> temp = std::make_pair(parent, child);
    this->parentSet.insert(temp);

    // each child can only have one parent
    this->parentMapKeyChildLine.insert({ child, std::vector<LINE_NUM>() });
    this->parentMapKeyChildLine[child].push_back(parent);

    auto searchParent = this->parentMapKeyParentLine.find(parent);
    if (searchParent == this->parentMapKeyParentLine.end()) {  // element not in map yet
        this->parentMapKeyParentLine.insert({ parent, std::vector<LINE_NUM>() });
    }
    this->parentMapKeyParentLine[parent].push_back(child);

    this->storeRelationshipParentStar(parent, child);
}

void PKB::storeRelationshipParentStar(LINE_NUM parent, LINE_NUM child) {
    std::pair<LINE_NUM, LINE_NUM> temp = std::make_pair(parent, child);
    this->parentStarSet.insert(temp);

    auto searchParent = this->parentStarMapKeyParentLine.find(parent);
    if (searchParent == this->parentStarMapKeyParentLine.end()) {
        this->parentStarMapKeyParentLine.insert({ parent, std::vector<LINE_NUM>() });
    }
    this->parentStarMapKeyParentLine[parent].push_back(child);

    auto searchChild = this->parentStarMapKeyChildLine.find(child);
    if (searchChild == this->parentStarMapKeyChildLine.end()) {
        this->parentStarMapKeyChildLine.insert({ child, std::vector<LINE_NUM>() });
    }
    this->parentStarMapKeyChildLine[child].push_back(parent);
}

bool PKB::doesParent(LINE_NUM parentLine, LINE_NUM childLine) {
    // Select s such that Parent(1, 2)
    std::pair<LINE_NUM, LINE_NUM> parentPair(parentLine, childLine);
    return parentSet.find(parentPair) != parentSet.end();
}

bool PKB::doesParentStar(LINE_NUM parentLine, LINE_NUM childLine) {
    // Select s such that Parent*(1, 2)
    std::pair<LINE_NUM, LINE_NUM> parentStarPair(parentLine, childLine);
    return parentStarSet.find(parentStarPair) != parentStarSet.end();
}

std::vector<LINE_NUM> PKB::getChildLineFromParent(LINE_NUM parentLine) {
    // Select s such that Parent(1, s)
    if (isParent(parentLine)) {
        return parentMapKeyParentLine[parentLine];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<LINE_NUM> PKB::getChildLineFromParentStar(LINE_NUM parentLine) {
    // Select s such that Parent*(1, s)
    if (isParent(parentLine)) {
        return parentStarMapKeyParentLine[parentLine];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<LINE_NUM> PKB::getParentLineFromParent(LINE_NUM childLine) {
    // Select s such that Parent(s, 2)
    if (isChild(childLine)) {
        return parentMapKeyChildLine[childLine];
    } else {
        return std::vector<LINE_NUM>();
    }
}

std::vector<LINE_NUM> PKB::getParentLineFromParentStar(LINE_NUM childLine) {
    // Select s such that Parent*(s, 2)
    if (isChild(childLine)) {
        return parentStarMapKeyChildLine[childLine];
    } else {
        return std::vector<LINE_NUM>();
    }
}

bool PKB::isParent(LINE_NUM lineNum) {
    // Select BOOLEAN such that Parent(1, _)
    return parentMapKeyParentLine.find(lineNum) != parentMapKeyParentLine.end();
}

bool PKB::isParent(DESIGN_ENTITY entity) {
    // Select BOOLEAN such that Parent(s, _)
    switch (entity) {
    case DesignEntity::IF:
        if (statementIfSet.size() != 0) {
            return true;
        } else {
            return false;
        }
    case DesignEntity::WHILE:
        if (statementWhileSet.size() != 0) {
            return true;
        } else {
            return false;
        }
    case DesignEntity::STMT:
    case DesignEntity::PROG_LINE:
        return (isParent(DesignEntity::IF) || isParent(DesignEntity::WHILE));
    default:
        return false;
    }
}

bool PKB::isChild(LINE_NUM lineNum) {
    // Select BOOLEAN such that Parent(_, 4)
    return parentMapKeyChildLine.find(lineNum) != parentMapKeyChildLine.end();
}

std::vector<LINE_NUM> PKB::getParentAfter(LINE_NUM lineNum, DESIGN_ENTITY designEntity) {
    // Select s such that Parent(3, s)
    if (!isParent(lineNum)) {
        return std::vector<LINE_NUM>();
    }
    std::unordered_set<LINE_NUM>* designEntitySet = selectDataSet(designEntity);

    std::vector<LINE_NUM> result;
    for (LINE_NUM childLine : parentMapKeyParentLine[lineNum]) {
        if (designEntitySet->find(childLine) != designEntitySet->end()) {
            result.push_back(childLine);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getParentAfter(DESIGN_ENTITY designEntity) {
    // Select s such that Parent(_, s)
    std::vector<LINE_NUM>* designEntityVec = selectDataStructure(designEntity);
    std::vector<LINE_NUM> result;
    for (LINE_NUM lineNum : *designEntityVec) {
        if (isChild(lineNum)) {
            result.push_back(lineNum);
        }
    }
    return result;
}

std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getParentPair
    (DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    // Select s, s1 such that Parent(s, s1)
    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    if (!isParent(leftArg)) {
        return result;
    }
    std::vector<LINE_NUM>* leftArgVec;
    switch (leftArg) {
    case DesignEntity::STMT:
    case DesignEntity::PROG_LINE:
        leftArgVec = &sortedParentsList;
        break;
    case DesignEntity::IF:
        leftArgVec = &sortedIfList;
        break;
    case DesignEntity::WHILE:
        leftArgVec = &sortedWhileList;
        break;
    default:
        throw InvalidDesignEntity();
    }
    std::unordered_set<LINE_NUM>* rightArgSet = selectDataSet(rightArg);

    for (LINE_NUM parentLine : *leftArgVec) {
        if (!isParent(parentLine)) {
            continue;
        }
        std::vector<LINE_NUM> childList = parentMapKeyParentLine[parentLine];
        for (LINE_NUM childLine : childList) {
            if (rightArgSet->find(childLine) != rightArgSet->end()) {
                std::pair<LINE_NUM, LINE_NUM> resPair{ parentLine, childLine };
                result.push_back(resPair);
            }
        }
    }

    return result;
}
std::vector<LINE_NUM> PKB::getParentBefore(DESIGN_ENTITY designEntity, LINE_NUM lineNum) {
    // Select s such that Parent(s, 1)
    if (!isChild(lineNum) || !isParent(designEntity)) {
        return std::vector<LINE_NUM>();
    }
    // each child only has one parent line
    LINE_NUM parentLine = parentMapKeyChildLine[lineNum][0];
    std::unordered_set<LINE_NUM>* designEntitySet;
    switch (designEntity) {
    case DesignEntity::STMT:
    case DesignEntity::PROG_LINE:
        designEntitySet = &statementSet;
        break;
    case DesignEntity::IF:
        designEntitySet = &statementIfSet;
        break;
    case DesignEntity::WHILE:
        designEntitySet = &statementWhileSet;
        break;
    default:
        throw InvalidDesignEntity();
    }
    std::vector<LINE_NUM> result;
    if (designEntitySet->find(parentLine) != designEntitySet->end()) {
        result.push_back(parentLine);
    }
    return result;
}
std::vector<LINE_NUM> PKB::getParentBefore(DESIGN_ENTITY designEntity) {
    // Select s such that Parent(s, _)
    std::vector<LINE_NUM> result;
    if (!isParent(designEntity)) {
        return result;
    }

    switch (designEntity) {
    case DesignEntity::STMT:
    case DesignEntity::PROG_LINE:
        result = sortedParentsList;
        break;
    case DesignEntity::IF:
        result = sortedIfList;
        break;
    case DesignEntity::WHILE:
        result = sortedWhileList;
        break;
    default:
        throw InvalidDesignEntity();
    }
    return result;
}

std::vector<LINE_NUM> PKB::getParentStarAfter(LINE_NUM lineNum, DESIGN_ENTITY designEntity) {
    // Select s such that Parent*(1, s)
    std::vector<LINE_NUM> result;
    if (!isParent(lineNum)) {
        return result;
    }

    std::unordered_set<LINE_NUM>* designEntitySet = selectDataSet(designEntity);

    for (LINE_NUM childLine : parentStarMapKeyParentLine[lineNum]) {
        if (designEntitySet->find(childLine) != designEntitySet->end()) {
            result.push_back(childLine);
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getParentStarAfter(DESIGN_ENTITY designEntity) {
    // Select s such that Parent*(_, s)
    return getParentAfter(designEntity);
}

std::vector<LINE_NUM> PKB::getParentStarBefore(DESIGN_ENTITY designEntity, LINE_NUM lineNum) {
    // Select s such that Parent*(s, 1)
    std::vector<LINE_NUM> result;
    if (!isParent(designEntity)) {
        return result;
    }
    std::vector<LINE_NUM>* designEntityVec;
    switch (designEntity) {
    case DesignEntity::STMT:
    case DesignEntity::PROG_LINE:
        designEntityVec = &sortedParentsList;
        break;
    case DesignEntity::IF:
        designEntityVec = &sortedIfList;
        break;
    case DesignEntity::WHILE:
        designEntityVec = &sortedWhileList;
        break;
    default:
        throw InvalidDesignEntity();
    }

    auto start = designEntityVec->begin();
    std::vector<LINE_NUM>::iterator temp;
    for (LINE_NUM parentLine : parentStarMapKeyChildLine[lineNum]) {
        if ((temp = std::find(start, designEntityVec->end(), parentLine))
            != designEntityVec->end()) {
            result.push_back(parentLine);
            start = temp;  // O(n) solution since both vectors are sorted
        }
    }
    return result;
}

std::vector<LINE_NUM> PKB::getParentStarBefore(DESIGN_ENTITY designEntity) {
    // Select s such that Parent*(s, _)
    return getParentBefore(designEntity);
}

std::vector<std::pair<LINE_NUM, LINE_NUM>> PKB::getParentStarPair
(DESIGN_ENTITY leftArg, DESIGN_ENTITY rightArg) {
    // Select s such that Parent*(s, a)
    std::vector<std::pair<LINE_NUM, LINE_NUM>> result;
    if (!isParent(leftArg)) {
        return result;
    }
    std::vector<LINE_NUM>* leftArgVec;
    switch (leftArg) {
    case DesignEntity::STMT:
    case DesignEntity::PROG_LINE:
        leftArgVec = &sortedParentsList;
        break;
    case DesignEntity::IF:
        leftArgVec = &sortedIfList;
        break;
    case DesignEntity::WHILE:
        leftArgVec = &sortedWhileList;
        break;
    default:
        throw InvalidDesignEntity();
    }

    std::unordered_set<LINE_NUM>* rightArgSet = selectDataSet(rightArg);

    for (LINE_NUM parentLine : *leftArgVec) {
        if (!isParent(parentLine)) {
            continue;
        }
        std::vector<LINE_NUM> childList = parentStarMapKeyParentLine[parentLine];
        for (LINE_NUM childLine : childList) {
            if (rightArgSet->find(childLine) != rightArgSet->end()) {
                std::pair<LINE_NUM, LINE_NUM> resPair{ parentLine, childLine };
                result.push_back(resPair);
            }
        }
    }

    return result;
}

bool PKB::hasParentRelationship() {
    // Select BOOLEAN such that Parent(_, _)
    return !parentMapKeyParentLine.empty();
}
