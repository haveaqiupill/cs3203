#include "OrderWithinGroupOptimiser.h"

#include <algorithm>
#include <utility>

#include "ToggleOptimisation.h"

Query OrderWithinGroupOptimiser::orderWithinGroups(std::vector<int>& noSyns,
    std::vector<std::vector<int>>& groupings, Query& query) {
    if (!ORDER_WITHIN_GRP_OPTIMISATION_ENABLED) {
        return query;
    }

    DECLARATION_TABLE dt = query.getDeclarations();
    CLAUSE_LIST clList = query.getClauseList();
    std::vector<std::vector<int>> optimisedGroupings;
    std::vector<std::pair<double, int>> cumulativeScores;
    for (size_t i = 0; i < groupings.size(); i++) {
        std::vector<std::pair<int, double>> indexToScoreMapping;
        double cumulativeScore = 0.0;
        for (auto const &index : groupings[i]) {
            QueryClause cl = clList[index];
            double score = calculateScore(cl, dt);
            indexToScoreMapping.push_back({ index,  score});
            cumulativeScore = cumulativeScore + score;
        }
        cumulativeScores.push_back({ cumulativeScore, i });

        // Sort the scores in ascending order
        sort(indexToScoreMapping.begin(), indexToScoreMapping.end(),
            [](const std::pair<int, double> a, const std::pair<int, double> b) -> bool {
                return a.second < b.second;
        });

        // Affects(*) clauses are the last in a group
        std::vector<int> affectsQueue;

        // Clauses with at least one synonym has been computed in a previous clause
        std::unordered_set<SYNONYM> computedSyns;
        std::unordered_set<int> visitedClauses;
        std::vector<int> optimisedOrder;

        while (visitedClauses.size() < groupings[i].size()) {
            for (auto p : indexToScoreMapping) {
                checkOverlappingSynonyms(affectsQueue, computedSyns, visitedClauses,
                    optimisedOrder, clList, p);
            }
        }

        for (auto idx : affectsQueue) {
            optimisedOrder.push_back(idx);
        }

        optimisedGroupings.push_back(optimisedOrder);
    }

    // Sort the cumulative scores in ascending order
    sort(cumulativeScores.begin(), cumulativeScores.end(),
        [](const std::pair<double, int> a, const std::pair<double, int> b) -> bool {
            return a.first < b.first;
        });

    // Pass in the scores to obgOptimiser
    OrderByGroupOptimiser obgOptimiser = OrderByGroupOptimiser(pkb);
    Query obgQuery = obgOptimiser.orderGroups(noSyns, optimisedGroupings, cumulativeScores, query);

    return obgQuery;
}

void OrderWithinGroupOptimiser::checkOverlappingSynonyms(std::vector<int>& affectsQueue,
    std::unordered_set<SYNONYM>& computedSyns, std::unordered_set<int>& visitedClauses,
    std::vector<int>& optimisedOrder, CLAUSE_LIST& clList, std::pair<int, double>& p) {
    if (visitedClauses.find(p.first) != visitedClauses.end()) {
        return;
    }

    QueryClause cl = clList[p.first];
    bool hasComputedSyn = false;
    std::vector<SYNONYM> clSyns;

    // Check if it is an Affects clause
    if (cl.getClauseType() == ClauseType::SUCHTHAT_CL) {
        Relationship rs = cl.getRelationship();
        if (rs == Relationship::AFFECTS || rs == Relationship::AFFECTS_STAR) {
            visitedClauses.insert(p.first);
            affectsQueue.push_back(p.first);
            if (cl.getLeftArg().getArgType() == QueryArgType::DECLARATION) {
                SYNONYM leftSyn = cl.getLeftArg().getDeclaration().getSynonym();
                computedSyns.insert(leftSyn);
            }
            if (cl.getRightArg().getArgType() == QueryArgType::DECLARATION) {
                SYNONYM rightSyn = cl.getRightArg().getDeclaration().getSynonym();
                computedSyns.insert(rightSyn);
            }
            return;
        }
    }

    if (cl.getClauseType() == ClauseType::SUCHTHAT_CL ||
        cl.getClauseType() == ClauseType::WITH_CL) {
        if (cl.getLeftArg().getArgType() == QueryArgType::DECLARATION) {
            SYNONYM leftSyn = cl.getLeftArg().getDeclaration().getSynonym();
            if (computedSyns.size() == 0) {
                computedSyns.insert(leftSyn);
                hasComputedSyn = true;
            } else if (computedSyns.find(leftSyn) != computedSyns.end()) {
                hasComputedSyn = true;
            }
            clSyns.push_back(leftSyn);
        }

        if (cl.getRightArg().getArgType() == QueryArgType::DECLARATION) {
            SYNONYM rightSyn = cl.getRightArg().getDeclaration().getSynonym();
            if (computedSyns.size() == 0) {
                computedSyns.insert(rightSyn);
                hasComputedSyn = true;
            } else if (computedSyns.find(rightSyn) != computedSyns.end()) {
                hasComputedSyn = true;
            }
            clSyns.push_back(rightSyn);
        }
    } else if (cl.getClauseType() == ClauseType::PATTERN_CL) {
        // only the left arg in pattern clause can contain a synonym
        if (cl.getLeftArg().getArgType() == QueryArgType::DECLARATION) {
            SYNONYM leftSyn = cl.getLeftArg().getDeclaration().getSynonym();
            if (computedSyns.size() == 0) {
                computedSyns.insert(leftSyn);
                hasComputedSyn = true;
            } else if (computedSyns.find(leftSyn) != computedSyns.end()) {
                hasComputedSyn = true;
            }
            clSyns.push_back(leftSyn);
        }
        // get the design entity synonym
        SYNONYM deSyn = cl.getDesignEntitySynonym();
        if (computedSyns.size() == 0) {
            computedSyns.insert(deSyn);
            hasComputedSyn = true;
        } else if (computedSyns.find(deSyn) != computedSyns.end()) {
            hasComputedSyn = true;
        }
        clSyns.push_back(deSyn);
    }

    if (hasComputedSyn) {
        visitedClauses.insert(p.first);
        optimisedOrder.push_back(p.first);

        for (auto syn : clSyns) {
            computedSyns.insert(syn);
        }
    }
}

double OrderWithinGroupOptimiser::calculateScore(QueryClause& qc, DECLARATION_TABLE& dt) {
    if (!isInitialised) {
        initialiseLookupTable();
        isInitialised = true;
    }

    double score = 0.0;

    // 1. Dynamic scoring according to number of results
    CLAUSE_TYPE ct = qc.getClauseType();
    if (ct == CLAUSE_TYPE::SUCHTHAT_CL) {
        Relationship rs = qc.getRelationship();
        if (rs != Relationship::AFFECTS && rs != Relationship::AFFECTS_STAR &&
            rs != Relationship::NEXT_STAR) {
            score = RELATIONSHIPS_LOOKUP.at(rs) / 2;
        } else {
            score = INT16_MAX / 100;
        }
        score = score + calculateDeScore(qc, dt) / 2;
    } else {
        score = calculateDeScore(qc, dt);
    }

    // 2. Static scoring according to number of synonyms in clause
    score = calculateNumSynonyms(qc, dt) * score;

    // 3. With clauses are more restrictive than such that clauses
    // Pattern clauses are ranked similar to any such that clause
    if (ct == CLAUSE_TYPE::WITH_CL) {
        score = score * 0.7;
    }

    return score;
}

double OrderWithinGroupOptimiser::calculateDeScore(QueryClause& qc, DECLARATION_TABLE& dt) {
    double deScore = 0.0;

    CLAUSE_TYPE ct = qc.getClauseType();
    if (ct == CLAUSE_TYPE::PATTERN_CL) {
        DESIGN_ENTITY de = dt.at(qc.getDesignEntitySynonym());
        deScore = DESIGN_ENTITY_LOOKUP.at(de);

        ARG_TYPE leftArgType = qc.getLeftArg().getArgType();
        if (leftArgType == QueryArgType::DECLARATION) {
            deScore = deScore + DESIGN_ENTITY_LOOKUP.at(DESIGN_ENTITY::VARIABLE);
        }
    } else {
        ARG_TYPE leftArgType = qc.getLeftArg().getArgType();
        ARG_TYPE rightArgType = qc.getRightArg().getArgType();
        if (leftArgType == QueryArgType::DECLARATION) {
            SYNONYM s = qc.getLeftArg().getDeclaration().getSynonym();
            DESIGN_ENTITY de = dt.at(s);
            deScore = deScore + DESIGN_ENTITY_LOOKUP.at(de);
        }

        if (rightArgType == QueryArgType::DECLARATION) {
            SYNONYM s = qc.getRightArg().getDeclaration().getSynonym();
            DESIGN_ENTITY de = dt.at(s);
            deScore = deScore + DESIGN_ENTITY_LOOKUP.at(de);
        }
    }
    return deScore;
}

double OrderWithinGroupOptimiser::calculateNumSynonyms(QueryClause& qc, DECLARATION_TABLE& dt) {
    // Clauses with one constant and one synonym will have a lower score than
    // clauses with two synonyms
    double doubleSyn = 1.0;
    double singleSyn = 0.5;

    CLAUSE_TYPE ct = qc.getClauseType();
    if (ct == CLAUSE_TYPE::SUCHTHAT_CL) {
        ARG_TYPE leftArgType = qc.getLeftArg().getArgType();
        ARG_TYPE rightArgType = qc.getRightArg().getArgType();
        if (leftArgType == QueryArgType::DECLARATION &&
            rightArgType == QueryArgType::DECLARATION) {
            return doubleSyn;
        } else {
            return singleSyn;
        }
    } else if (ct == CLAUSE_TYPE::PATTERN_CL) {
        DESIGN_ENTITY de = dt.at(qc.getDesignEntitySynonym());
        if (de == DESIGN_ENTITY::ASSIGN) {
            // pattern a(leftArg, rightArg)
            ARG_TYPE leftArgType = qc.getLeftArg().getArgType();
            ARG_TYPE rightArgType = qc.getRightArg().getArgType();

            // pattern a(?, _"x"_)
            if (leftArgType == QueryArgType::DECLARATION) {
                // pattern a(v, "x")
                return doubleSyn;
            } else {
                // pattern a(_, "x") || pattern a("y", "x")
                return singleSyn;
            }
        } else {
            // pattern if(leftArg, _, _) || pattern while(leftArg, _)
            ARG_TYPE leftArgType = qc.getLeftArg().getArgType();
            if (leftArgType == QueryArgType::DECLARATION) {
                // pattern if(v, _, _) || pattern while(v, _)
                return doubleSyn;
            } else {
                // pattern if("x", _, _) || pattern while("x", _)
                return singleSyn;
            }
        }
    } else {
        ARG_TYPE leftArgType = qc.getLeftArg().getArgType();
        ARG_TYPE rightArgType = qc.getRightArg().getArgType();
        if ((leftArgType == QueryArgType::DECLARATION ||
            leftArgType == QueryArgType::DECLARATION_WITH_ATTR) &&
            (rightArgType == QueryArgType::DECLARATION ||
            rightArgType == QueryArgType::DECLARATION_WITH_ATTR)) {
            return doubleSyn;
        } else {
            return singleSyn;
        }
    }
}

void OrderWithinGroupOptimiser::initialiseLookupTable() {
    std::vector<Relationship> relationships = {
        Relationship::FOLLOWS,
        Relationship::FOLLOWS_STAR,
        Relationship::PARENT,
        Relationship::PARENT_STAR,
        Relationship::USES,
        Relationship::MODIFIES,
        Relationship::CALLS,
        Relationship::CALLS_STAR,
        Relationship::NEXT,
        Relationship::NEXT_BIP,
        Relationship::NEXT_BIP_STAR,
        Relationship::AFFECTS_BIP
    };

    for (auto rs : relationships) {
        RELATIONSHIPS_LOOKUP[rs] = pkb->getNumOfRelationship(rs);
    }

    std::vector<DesignEntity> designEntities = {
        DesignEntity::STMT,
        DesignEntity::READ,
        DesignEntity::PRINT,
        DesignEntity::CALL,
        DesignEntity::WHILE,
        DesignEntity::IF,
        DesignEntity::ASSIGN,
        DesignEntity::VARIABLE,
        DesignEntity::CONSTANT,
        DesignEntity::PROCEDURE,
        DesignEntity::PROG_LINE
    };

    for (auto de : designEntities) {
        DESIGN_ENTITY_LOOKUP[de] = pkb->getNumOfDesignEntity(de);
    }
}
