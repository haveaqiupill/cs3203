#include "WithOptimiser.h"
#include "ToggleOptimisation.h"
/*
* Current with clause replacement rules:
* <left> = <right>S
1. If both are the same (eg both numbers/string), check that both are the same value
2. If both are declarations, replace the non selection one if theres one selection
    or just replace whichever if both are non selections and have the same attribute
3. If both are declaration and both are selections dont replace
*/

/*
* Method called by QueryOptimiser
*/
Query WithOptimiser::replaceSynonyms(Query& query) {
    if (!WITH_OPTIMISATION_ENABLED) {
        return query;
    }

    CLAUSE_LIST withConditionList = query.getWithConditionList();
    CLAUSE_LIST clauseList = query.getClauseList();

    // return query if size is 0
    if (withConditionList.size() == 0) {
        return query;
    }


    SELECTION_LIST selectionList = query.getSelectionList();
    selectType = query.getSelectionType();
    // initialise synMaps based on clause list
    createSynMaps(clauseList);
    // create set of synonyms in selection list
    getSelectionSyns(selectionList);

    // go through the withConditionList
    for (QueryClause withCl : withConditionList) {
        QueryArg leftArg = withCl.getLeftArg();
        QueryArg rightArg = withCl.getRightArg();
        QueryArgType argTypeL = leftArg.getArgType();
        QueryArgType argTypeR = rightArg.getArgType();

        // check if both types are the same
        if (argTypeL == argTypeR) {
            if (argTypeL == QueryArgType::NUM || argTypeL == QueryArgType::NAME) {
                // discard with clause if trivial comparison
            } else {
                // if both are declarations, check for replacement
                doDeclarationReplacement(withCl, clauseList, selectionList, leftArg, rightArg);
            }
        } else if (argTypeL == QueryArgType::DECLARATION) {
            // check if can replace left arg with right arg
            doValueReplacement(withCl, clauseList, selectionList, leftArg, rightArg);
        } else if (argTypeR == QueryArgType::DECLARATION) {
            // check if can replace right arg with left arg
            doValueReplacement(withCl, clauseList, selectionList, rightArg, leftArg);
        }
    }
    // replace the existing clause list with the new clause list
    query.setClauseList(clauseList);
    query.setWithConditionList(leftOverWithCl);
    return query;
}

/*
* Create the initial synonym mapping.
* Sorts the clauses into synMapNum (for number replacement) and synMapStr (for string replacement).
* Also creates an overall synCount (for all synonyms used in the clauses)
*/
void WithOptimiser::createSynMaps(CLAUSE_LIST& clauseList) {
    std::unordered_set<Relationship> replaceWithNum = rsReplacementLookup.at(QueryArgType::NUM);
    std::unordered_set<Relationship> replaceWithStr = rsReplacementLookup.at(QueryArgType::NAME);

    for (size_t i = 0; i < clauseList.size(); i++) {
        QueryClause cl = clauseList[i];
        if (cl.getClauseType() == ClauseType::PATTERN_CL) {  // pattern cl
            QueryArg leftArg = cl.getLeftArg();
            QueryArgType argTypeL = leftArg.getArgType();
            SYNONYM designEntSyn = cl.getDesignEntitySynonym();
            // for pattern clause, check left arg only
            if (argTypeL == QueryArgType::DECLARATION) {
                Declaration declrL = leftArg.getDeclaration();
                SYNONYM leftSyn = declrL.getSynonym();

                // count synonyms
                if (synCount.find(leftSyn) == synCount.end()) {
                    synCount[leftSyn] = 1;
                } else {
                    synCount[leftSyn]++;
                }

                // add to synMapStr (can only replace variable design entity with name)
                synMapStr[declrL].push_back(std::make_pair(i, 0));
            }

            // also add in the design entity synonym
            // this prevents with clauses that contain this synonym from being discarded
            // count synonyms
                if (synCount.find(designEntSyn) == synCount.end()) {
                    synCount[designEntSyn] = 1;
                } else {
                    synCount[designEntSyn]++;
                }
        } else if (cl.getClauseType() == ClauseType::SUCHTHAT_CL) {  // such that cl
            Relationship rs = cl.getRelationship();
            QueryArg leftArg = cl.getLeftArg();
            QueryArg rightArg = cl.getRightArg();
            QueryArgType argTypeL = leftArg.getArgType();
            QueryArgType argTypeR = rightArg.getArgType();

            // check left arg
            if (argTypeL == QueryArgType::DECLARATION) {
                Declaration declrL = leftArg.getDeclaration();
                SYNONYM leftSyn = declrL.getSynonym();

                // count synonyms
                if (synCount.find(leftSyn) == synCount.end()) {
                    synCount[leftSyn] = 1;
                } else {
                    synCount[leftSyn]++;
                }

                // sort syn based on whether it can be replaced with a num or string
                if (replaceWithNum.find(rs) != replaceWithNum.end()) {
                    synMapNum[declrL].push_back(std::make_pair(i, 0));
                } else {
                    synMapStr[declrL].push_back(std::make_pair(i, 0));
                }
            }

            // check right arg
            if (argTypeR == QueryArgType::DECLARATION) {
                Declaration declrR = rightArg.getDeclaration();
                SYNONYM rightSyn = declrR.getSynonym();

                // count synonyms
                if (synCount.find(rightSyn) == synCount.end()) {
                    synCount[rightSyn] = 1;
                } else {
                    synCount[rightSyn]++;
                }

                // sort syn based on whether it can be replaced with a num or string
                if (replaceWithNum.find(rs) != replaceWithNum.end()) {
                    synMapNum[declrR].push_back(std::make_pair(i, 1));
                } else {
                    synMapStr[declrR].push_back(std::make_pair(i, 1));
                }
            }
        }
    }
}

void WithOptimiser::getSelectionSyns(SELECTION_LIST& selectionList) {
    for (auto& selection : selectionList) {
        selectionSyns.insert(selection.getSynonym());
    }
}

/*
* Method called when both refs in the with clause are declarations
*/
void WithOptimiser::doDeclarationReplacement(QueryClause& withCl, CLAUSE_LIST& clList,
    SELECTION_LIST& selectionList, QueryArg& leftArg, QueryArg& rightArg) {
    Declaration declrL = leftArg.getDeclaration();
    Declaration declrR = rightArg.getDeclaration();

    if (declrL == declrR) {
        // discard with clause if both declarations are the same (trivial comparison)
    } else {
        SYNONYM synL = declrL.getSynonym();
        SYNONYM synR = declrR.getSynonym();
        Attribute attrL = declrL.getAttribute();
        Attribute attrR = declrR.getAttribute();

        if (synCount.find(synL) != synCount.end() && synCount.find(synR) != synCount.end()) {
            // if both synonyms are used by the clauses
            if (synL == synR) {
                // same synonym, diff attribute (keep withcl for evaluator)
            } else if (attrL == attrR) {
                // diff synonym, same attribute: able to replace one with the other
                if (selectionSyns.find(synL) != selectionSyns.end()
                        && selectionSyns.find(synR) != selectionSyns.end()) {
                    // if both synonyms are inside selection, do not replace anything
                    // keep this withcl for the evaluator to evaluate
                } else if (selectionSyns.find(synL) != selectionSyns.end()) {
                    // replace instances of synR with synL
                    replaceDeclarationSynHelper(declrR, declrL, clList, withCl);
                } else if (selectionSyns.find(synR) != selectionSyns.end()) {
                    // replace instances of synL with synR
                    replaceDeclarationSynHelper(declrL, declrR, clList, withCl);
                } else {
                    // both are not in selection, prioritise the one that appears the most
                    int countL = synCount.at(synL);
                    int countR = synCount.at(synR);
                    if (countL >= countR) {  // replace all instances of R with L
                        replaceDeclarationSynHelper(declrR, declrL, clList, withCl);
                    } else {
                        replaceDeclarationSynHelper(declrL, declrR, clList, withCl);
                    }
                }
            }
        }
        leftOverWithCl.push_back(withCl);
    }
}

/*
* Helper method called by doDeclarationReplacement
* Replaces all instances of the specified synonym with the other synonym,
* within all the clauses
*/
void WithOptimiser::replaceDeclarationSynHelper(
    Declaration& toReplace, Declaration& replacement, CLAUSE_LIST& clList, QueryClause& withCl) {
    // if unable to find, it is a pattern design entity synonym
    // not allowed to replace
    if (synMapNum.find(toReplace) == synMapNum.end()
        && synMapStr.find(toReplace) == synMapStr.end()) {
        return;
    }

    std::vector<std::pair<int, int>> clausesToModify =
      synMapNum.find(toReplace) != synMapNum.end() ? synMapNum.at(toReplace)
                                                   : synMapStr.at(toReplace);


    for (auto& clInfo : clausesToModify) {
        int clIndex = std::get<0>(clInfo);  // index in clause list
        int position = std::get<1>(clInfo);  // indicates left/right arg of clause (0/1)

        // check if this clause's arg has already been replaced
        if (argsReplaced.find(clInfo) == argsReplaced.end()) {
            argsReplaced.insert(clInfo);  // track the replacement
            if (position == 0)  {
                // modify left arg of clause
                clList[clIndex].replaceLeftDeclrSynonym(replacement.getSynonym());
            } else {
                // modify right arg of clause
                clList[clIndex].replaceRightDeclrSynonym(replacement.getSynonym());
            }
        } else {
            // already replaced, don't overwrite
        }
    }
}

/*
* Method to perform replacement of synonyms inside the other clauses with values referenced in the with clauses.
* Called when both argument types are not the same
* (i.e. one declaration, one number or one declaration, one string)
*/
void WithOptimiser::doValueReplacement(QueryClause& withCl, CLAUSE_LIST& clList,
    SELECTION_LIST& selectionList, QueryArg& toReplace, QueryArg& replacement) {
    QueryArgType replaceType = replacement.getArgType();
    DECLARATION declr = toReplace.getDeclaration();
    SYNONYM syn = declr.getSynonym();
    ATTRIBUTE attr = declr.getAttribute();

    // if the synonym is inside the selection, keep the with clause
    // also keep if selection is BOOLEAN or no clauses in the list
    if (selectionSyns.find(syn) != selectionSyns.end()
        || selectType == SelectionType::BOOLEAN
        || clList.size() == 0) {
        leftOverWithCl.push_back(withCl);
        return;
    }

    // search for the synonym in the clauses
    // ignore with clause if synonym not used in any clause OR other with clauses
    if (synCount.find(syn) != synCount.end()) {
        // number replacement
        if (replaceType == QueryArgType::NUM && synMapNum.find(declr) != synMapNum.end()) {
            int replaceVal = replacement.getLineNum();
            // get clauses with synonyms that can be replaced with numbers
            std::vector<std::pair<int, int>> clausesToModify = synMapNum.at(declr);
            // replace instances of this syn in the clauses with this number
            for (auto& clInfo : clausesToModify) {
                int clIndex = std::get<0>(clInfo);  // index in clause list
                int position = std::get<1>(clInfo);  // indicates left/right arg of clause (0/1)
                // check if this clause's arg has already been replaced
                if (argsReplaced.find(clInfo) == argsReplaced.end()) {
                    argsReplaced.insert(clInfo);  // track the replacement
                    if (position == 0)  {
                        // modify left arg of clause
                        clList[clIndex].replaceLeftDeclrWithVal(replaceVal);
                    } else {
                        // modify right arg of clause
                        clList[clIndex].replaceRightDeclrWithVal(replaceVal);
                    }
                } else {
                    // already replaced, don't overwrite
                }
            }
        } else if (replaceType == QueryArgType::NAME && synMapStr.find(declr) != synMapStr.end()) {
            // name replacement
            std::string replaceVal = replacement.getName();
            // replace all synonyms in the clauses with this name
            std::vector<std::pair<int, int>> clausesToModify = synMapStr.at(declr);
            for (auto& clInfo : clausesToModify) {
                int clIndex = std::get<0>(clInfo);
                int position = std::get<1>(clInfo);
                // check if the clause has already been replaced
                if (argsReplaced.find(clInfo) == argsReplaced.end()) {
                    argsReplaced.insert(clInfo);  // track the replacement
                    if (position == 0) {
                        // modify left arg of clause
                        clList[clIndex].replaceLeftDeclrWithVal(replaceVal);
                    } else {
                        // modify right arg of clause
                        clList[clIndex].replaceRightDeclrWithVal(replaceVal);
                    }
                } else {
                    // already replaced, don't overwrite
                }
            }
        } else {
            // cannot replace
        }
    }
    leftOverWithCl.push_back(withCl);  // still keep the clause for check
}
