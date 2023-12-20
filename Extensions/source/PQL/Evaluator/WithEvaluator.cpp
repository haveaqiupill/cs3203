#include "WithEvaluator.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <utility>

#include "../../Commons/Exceptions.h"
#include "../QueryEvaluator.h"

ClauseAnswer WithEvaluator::evaluateWithClause(QueryArg& leftArg, QueryArg& rightArg,
                                               DECLARATION_TABLE& dt, PKB* pkb) {
    /* Assumptions:
     * Each ref can either be an IDENT, INTEGER, attrRef or synonym
     * Parser already tries its best to fill up all the attrRef to the synonyms
     * Parser has already checked that leftArg and rightArg are of the same type
     */
    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    if (leftArgType == QueryArgType::DECLARATION &&
        rightArgType == QueryArgType::DECLARATION) {
        return evaluateTwoAttributeRef(leftArg, rightArg, dt, pkb);
    } else if (leftArgType == QueryArgType::DECLARATION ||
               rightArgType == QueryArgType::DECLARATION) {
        return evaluateOneAttributeRef(leftArg, rightArg, dt, pkb);
    } else {
        return evaluateZeroAttributeRef(leftArg, rightArg, dt);
    }
}

ClauseAnswer WithEvaluator::evaluateZeroAttributeRef(QueryArg& leftArg, QueryArg& rightArg,
                                                     DECLARATION_TABLE& dt) {
    /*
     * Handles these cases:
     * (IDENT, IDENT)
     * (INTEGER, INTEGER)
     */
    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    bool isClauseTrue;
    ResultTable resultTable = ResultTable();

    if (leftArgType == QueryArgType::NAME) {
        isClauseTrue = leftArg.getName() == rightArg.getName();
    } else if (leftArgType == QueryArgType::NUM) {
        isClauseTrue = leftArg.getLineNum() == rightArg.getLineNum();
    } else {
        throw InvalidArgumentException();
    }

    return ClauseAnswer(isClauseTrue, resultTable);
}

ClauseAnswer WithEvaluator::evaluateOneAttributeRef(QueryArg& leftArg, QueryArg& rightArg,
                                                    DECLARATION_TABLE& dt, PKB *pkb) {
    /*
     * Handles these cases:
     * (attrRef, IDENT) / (IDENT, attrRef)
     * (attrRef, INTEGER) / (INTEGER, attrRef)
     */
    ARG_TYPE leftArgType = leftArg.getArgType();
    ARG_TYPE rightArgType = rightArg.getArgType();

    ResultTable resultTable = ResultTable();
    DECLARATION dec;
    std::string val;

    if (leftArgType == QueryArgType::DECLARATION) {
        dec = leftArg.getDeclaration();
        if (rightArgType == QueryArgType::NAME) {
            val = rightArg.getName();
        } else if (rightArgType == QueryArgType::NUM) {
            val = std::to_string(rightArg.getLineNum());
        } else {
            throw InvalidArgumentException();
        }
    } else {  // rightArgType is DECLARATION
        dec = rightArg.getDeclaration();
        if (leftArgType == QueryArgType::NAME) {
            val = leftArg.getName();
        } else if (leftArgType == QueryArgType::NUM) {
            val = std::to_string(leftArg.getLineNum());
        } else {
            throw InvalidArgumentException();
        }
    }

    // Check if value is actually found in Design Entity
    DesignEntity de = dt.at(dec.getSynonym());
    Attribute atr = dec.getAttribute();

    // Return the corresponding other attribute for two-attribute DesignEntity
    if (isTwoAttrDe(de)) {
        std::vector<std::pair<int, std::string>> res = checkValInPkbWithAttr(de, atr, val, pkb);
        if (!res.empty()) {
            Declaration leftDec = makeLeftDecForTwoAttr(dec, de);
            Declaration rightDec = makeRightDecForTwoAttr(dec, de);
            resultTable = EvaluatorUtility::lineNumStrVecPairToResultTable(leftDec, rightDec, res);
            return ClauseAnswer(true, resultTable);
        } else {
            return ClauseAnswer(false, resultTable);
        }
    } else {
        bool isClauseTrue = checkValInPkb(de, val, pkb);
        if (isClauseTrue) {
            resultTable = EvaluatorUtility::strVecToResultTable(dec, { val });
        }
        return ClauseAnswer(isClauseTrue, resultTable);
    }
}

ClauseAnswer WithEvaluator::evaluateTwoAttributeRef(QueryArg& leftArg, QueryArg& rightArg,
                                                    DECLARATION_TABLE& dt, PKB* pkb) {
    /*
     * Handles this case:
     * (attrRef, attrRef)
     */
    DesignEntity leftDe = dt.at(leftArg.getDeclaration().getSynonym());
    DesignEntity rightDe = dt.at(rightArg.getDeclaration().getSynonym());

    ResultTable resultTable = ResultTable();
    std::vector<std::string> combinedVec;
    Attribute leftAttr = leftArg.getDeclaration().getAttribute();

    // Parser has checked that leftDe and rightDe are both int or str
    if (leftAttr == ATTRIBUTE::PROC_NAME || leftAttr == ATTRIBUTE::VAR_NAME) {
        combinedVec = pkb->findCommonStrItems(leftDe, rightDe);
    } else {
        combinedVec = convertVecIntToVecStr(pkb->findCommonIntItems(leftDe, rightDe));
    }

    if (combinedVec.empty()) {
        return ClauseAnswer(false, resultTable);
    }

    // Populate the result table
    if (isTwoAttrDe(leftDe) && isTwoAttrDe(rightDe)) {
        Declaration dec1 = makeLeftDecForTwoAttr(leftArg.getDeclaration(), leftDe);
        Declaration dec2 = makeRightDecForTwoAttr(leftArg.getDeclaration(), leftDe);
        Declaration dec3 = makeLeftDecForTwoAttr(rightArg.getDeclaration(), rightDe);
        Declaration dec4 = makeRightDecForTwoAttr(rightArg.getDeclaration(), rightDe);

        // Fill up the result table. Each row is {int, string, int, string}
        RESULT_TABLE table;
        if (leftAttr == ATTRIBUTE::PROC_NAME || leftAttr == ATTRIBUTE::VAR_NAME) {
            // Each row is {int, str, int, str}
            for each (std::string ele in combinedVec) {
                std::vector<std::pair<int, std::string>>
                    twoAttrVecLeft = getTwoAttrFromStr(leftDe, ele, pkb);
                std::vector<std::pair<int, std::string>>
                    twoAttrVecRight = getTwoAttrFromStr(rightDe, ele, pkb);
                for each (std::pair<int, std::string> tupLeft in twoAttrVecLeft) {
                    for each (std::pair<int, std::string> tupRight in twoAttrVecRight) {
                        std::vector<std::string> row{ std::to_string(tupLeft.first), ele,
                                                      std::to_string(tupRight.first), ele };
                        table.push_back(row);
                    }
                }
            }
        } else {
            // Each row is {int, str, int, str}
            for each (std::string ele in combinedVec) {
                std::vector<std::pair<int, std::string>>
                    twoAttrVecLeft = getTwoAttrFromInt(leftDe, ele, pkb);
                std::vector<std::pair<int, std::string>>
                    twoAttrVecRight = getTwoAttrFromInt(rightDe, ele, pkb);
                for each (std::pair<int, std::string> tupLeft in twoAttrVecLeft) {
                    for each (std::pair<int, std::string> tupRight in twoAttrVecRight) {
                        std::vector<std::string> row{ ele, tupLeft.second, ele, tupRight.second };
                        table.push_back(row);
                    }
                }
            }
        }

        std::unordered_map<Declaration, int, dec_hash> header{ {dec1, 0}, {dec2, 1},
                                                               {dec3, 2}, {dec4, 3} };
        resultTable = ResultTable(header, table);
    } else if (isTwoAttrDe(leftDe)) {
        Declaration dec1 = makeLeftDecForTwoAttr(leftArg.getDeclaration(), leftDe);
        Declaration dec2 = makeRightDecForTwoAttr(leftArg.getDeclaration(), leftDe);
        Declaration dec3 = rightArg.getDeclaration();

        // Fill up the result table. Each row is either {int, str, int} or {int, str, str}
        RESULT_TABLE table;
        if (leftAttr == ATTRIBUTE::PROC_NAME || leftAttr == ATTRIBUTE::VAR_NAME) {
            // Each row is {int, str, str}
            for each (std::string ele in combinedVec) {
                std::vector<std::pair<int, std::string>>
                    twoAttrVec = getTwoAttrFromStr(leftDe, ele, pkb);
                for each (std::pair<int, std::string> tup in twoAttrVec) {
                    std::vector<std::string> row{ std::to_string(tup.first), ele, ele };
                    table.push_back(row);
                }
            }
        } else {
            // Each row is {int, str, int}
            for each (std::string ele in combinedVec) {
                std::vector<std::pair<int, std::string>>
                    twoAttrVec = getTwoAttrFromInt(leftDe, ele, pkb);
                for each (std::pair<int, std::string> tup in twoAttrVec) {
                    std::vector<std::string> row{ ele, tup.second, ele };
                    table.push_back(row);
                }
            }
        }

        std::unordered_map<Declaration, int, dec_hash> header{ {dec1, 0}, {dec2, 1}, {dec3, 2} };
        resultTable = ResultTable(header, table);
    } else if (isTwoAttrDe(rightDe)) {
        Declaration dec1 = leftArg.getDeclaration();
        Declaration dec2 = makeLeftDecForTwoAttr(rightArg.getDeclaration(), rightDe);
        Declaration dec3 = makeRightDecForTwoAttr(rightArg.getDeclaration(), rightDe);

        // Fill up the result table. Each row is either {int, int, str} or {str, int, str}
        RESULT_TABLE table;
        if (leftAttr == ATTRIBUTE::PROC_NAME || leftAttr == ATTRIBUTE::VAR_NAME) {
            // Each row is {str, int, str}
            for each (std::string ele in combinedVec) {
                std::vector<std::pair<int, std::string>>
                    twoAttrVec = getTwoAttrFromStr(rightDe, ele, pkb);
                for each (std::pair<int, std::string> tup in twoAttrVec) {
                    std::vector<std::string> row{ ele, std::to_string(tup.first), ele };
                    table.push_back(row);
                }
            }
        } else {
            // Each row is {int, int, str}
            for each (std::string ele in combinedVec) {
                std::vector<std::pair<int, std::string>>
                    twoAttrVec = getTwoAttrFromInt(rightDe, ele, pkb);
                for each (std::pair<int, std::string> tup in twoAttrVec) {
                    std::vector<std::string> row{ ele, ele, tup.second };
                    table.push_back(row);
                }
            }
        }

        std::unordered_map<Declaration, int, dec_hash> header{ {dec1, 0}, {dec2, 1}, {dec3, 2} };
        resultTable = ResultTable(header, table);
    } else {
        // LHS and RHS have one attribute
        std::vector<std::pair<std::string, std::string>> vecPair = convertToVecPair(combinedVec);
        resultTable = EvaluatorUtility::strVecPairToResultTable(leftArg.getDeclaration(),
            rightArg.getDeclaration(), vecPair);
    }

    return ClauseAnswer(true, resultTable);
}

// Helper to determine if DesignEntity has two attributes related to it: read, call, print
bool WithEvaluator::isTwoAttrDe(DesignEntity de) {
    if (de == DESIGN_ENTITY::READ || de == DESIGN_ENTITY::CALL || de == DESIGN_ENTITY::PRINT) {
        return true;
    } else {
        return false;
    }
}

// Helper to convert std::vector<LINE_NUM> to std::vector<std::string>
std::vector<std::string> WithEvaluator::convertVecIntToVecStr(std::vector<LINE_NUM> vec) {
    std::vector<std::string> res;
    for each (LINE_NUM lineNum in vec) {
        res.push_back(std::to_string(lineNum));
    }
    return res;
}

// Helper to convert vector of string to vector of pair of string
std::vector<std::pair<std::string, std::string>>
    WithEvaluator::convertToVecPair(std::vector<std::string> vec) {
    std::vector<std::pair<std::string, std::string>> res;
    for each (std::string ele in vec) {
        std::pair<std::string, std::string> toInsert{ ele, ele };
        res.push_back(toInsert);
    }
    return res;
}

// Helper to check if the value for a one-attribute DesignEntity is found in PKB
bool WithEvaluator::checkValInPkb(DesignEntity de, std::string val, PKB* pkb) {
    std::vector<std::string> vec;
    switch (de) {
        case DesignEntity::STMT:
        case DesignEntity::PROG_LINE:
        case DesignEntity::WHILE:
        case DesignEntity::IF:
        case DesignEntity::ASSIGN:
            vec = convertVecIntToVecStr(*pkb->selectDataStructure(de));
            break;
        case DesignEntity::VARIABLE:
            vec = pkb->getAllVariables();
            break;
        case DesignEntity::CONSTANT:
            vec = pkb->getAllConstants();
            break;
        case DesignEntity::PROCEDURE:
            vec = pkb->getAllProcedures();
            break;
        default:
            throw InvalidDesignEntity();
    }

    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

/* Helper to check if the value for a two-attribute DesignEntity is found in PKB and return
 * the corresponding other attribute together
 */
std::vector<std::pair<int, std::string>> WithEvaluator::checkValInPkbWithAttr(DesignEntity de,
                                                                              Attribute atr,
                                                                              std::string val,
                                                                              PKB* pkb) {
    std::vector<std::pair<int, std::string>> result;
    std::vector<std::pair<int, std::string>> allStmt;
    switch (de) {
    case DesignEntity::READ:
        allStmt = pkb->getAllReadStatementsWithAttribute();
        break;
    case DesignEntity::PRINT:
        allStmt = pkb->getAllPrintStatementsWithAttribute();
        break;
    case DesignEntity::CALL:
        allStmt = pkb->getAllCallStatementsWithAttribute();
        break;
    default:
        throw InvalidDesignEntity();
    }

    if (atr == ATTRIBUTE::STMT_NUM) {
        for (auto pair : allStmt) {
            if (std::to_string(pair.first) == val) {
                result.push_back(pair);
            }
        }
    } else if ((atr == ATTRIBUTE::VAR_NAME && de == DesignEntity::READ) ||
        (atr == ATTRIBUTE::VAR_NAME && de == DesignEntity::PRINT)) {
        for (auto pair : allStmt) {
            if (pair.second == val) {
                result.push_back(pair);
            }
        }
    } else if (atr == ATTRIBUTE::PROC_NAME && de == DesignEntity::CALL) {
        for (auto pair : allStmt) {
            if (pair.second == val) {
                result.push_back(pair);
            }
        }
    } else {
        throw InvalidAttrUsageException();
    }

    return result;
}

// Helper to get pair <int, std::string> for two-attribute DesignEntity, given a string
std::vector<std::pair<int, std::string>> WithEvaluator::getTwoAttrFromStr(DesignEntity de,
                                                                          std::string ele,
                                                                          PKB* pkb) {
    std::vector<std::pair<int, std::string>> twoAttrVec;
    switch (de) {
        case DesignEntity::CALL:
            twoAttrVec = pkb->getCallPairWithProcName(ele);
            break;
        case DesignEntity::PRINT:
            twoAttrVec = pkb->getPrintPairWithVarName(ele);
            break;
        case DesignEntity::READ:
            twoAttrVec = pkb->getReadPairWithVarName(ele);
            break;
        default:
            throw InvalidDesignEntity();
    }
    return twoAttrVec;
}

// Helper to get pair <int, std::string> for two-attribute DesignEntity, given an integer
std::vector<std::pair<int, std::string>> WithEvaluator::getTwoAttrFromInt(DesignEntity de,
                                                                          std::string ele,
                                                                          PKB* pkb) {
    std::vector<std::pair<int, std::string>> twoAttrVec;
    switch (de) {
        case DesignEntity::CALL:
            twoAttrVec = pkb->getCallPairWithLineNum(std::stoi(ele));
            break;
        case DesignEntity::PRINT:
            twoAttrVec = pkb->getPrintPairWithLineNum(std::stoi(ele));
            break;
        case DesignEntity::READ:
            twoAttrVec = pkb->getReadPairWithLineNum(std::stoi(ele));
            break;
        default:
            throw InvalidDesignEntity();
    }
    return twoAttrVec;
}

// Helper to create a declaration header for two-attribute DesignEntity
Declaration WithEvaluator::makeLeftDecForTwoAttr(Declaration dec, DesignEntity de) {
    return Declaration(dec.getSynonym(), ATTRIBUTE::STMT_NUM);
}

Declaration WithEvaluator::makeRightDecForTwoAttr(Declaration dec, DesignEntity de) {
    if (de == DesignEntity::CALL) {
        return Declaration(dec.getSynonym(), ATTRIBUTE::PROC_NAME);
    } else if (de == DesignEntity::PRINT || de == DesignEntity::READ) {
        return Declaration(dec.getSynonym(), ATTRIBUTE::VAR_NAME);
    } else {
        throw InvalidDesignEntity();
    }
}
