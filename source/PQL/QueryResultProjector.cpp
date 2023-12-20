#include "QueryResultProjector.h"

#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "Evaluator/EvaluatorUtility.h"
#include "Optimiser/ToggleOptimisation.h"

/**
 * Extracts the selection results from IntermediateAnswer to be formatted and returned
 * to the AutoTester and shown to User.
 *
 * @param intermediateAnswer Contains the ClauseAnswer list and selection synonym.
 * @return RAW_ANSWER of query.
 */
RAW_ANSWER QueryResultProjector::returnResults(IntermediateAnswer intermediateAnswer) {
    SelectionType selType = intermediateAnswer.getSelectionType();
    std::vector<ClauseAnswer> caList = intermediateAnswer.getClauseAnsList();

    if (selType == SelectionType::BOOLEAN) {
        if (OPTIMISATION_ENABLED) {
            return getOptimisedBooleanSelectionResults(caList);
        }
        return getBooleanSelectionResults(caList);
    }

    if (caList.size() == 0) {
        return RAW_ANSWER();
    }

    ResultTable selectionResults = ResultTable();
    std::vector<DECLARATION> dec = intermediateAnswer.getSelection();

    // Selection synonyms
    std::unordered_set<SYNONYM> syns;
    for (size_t i = 0; i < dec.size(); i++) {
        syns.insert(dec.at(i).getSynonym());
    }

    if (QRP_OPTIMISATION_ENABLED) {
        std::vector<ClauseAnswer> evalList;
        for (size_t i = 0; i < caList.size(); i++) {
            ClauseAnswer ca = caList.at(i);
            if (!ca.getIsClauseTrue()) {
                evalList.push_back(ca);
            }

            std::unordered_map<Declaration, int, dec_hash> decs;
            decs = ca.getResultTable().getSynonyms();

            for (auto p : decs) {
                // If selection synonym is in result table of clause group
                // add it to list to be combined
                Declaration dec = p.first;
                if (syns.find(dec.getSynonym()) != syns.end()) {
                    evalList.push_back(caList.at(i));
                    break;
                }
            }
        }

        for (auto ca : evalList) {
            // If any boolean in the clause result is false, return empty result
            if (!ca.getIsClauseTrue()) {
                return RAW_ANSWER();
            } else if (ca.getResultTable().getSynonyms().size() > 0 &&
                ca.getResultTable().getResultTable().size() == 0) {
                return RAW_ANSWER();
            }
            // Else merge the results
            selectionResults = selectionResults.merge(ca.getResultTable());
        }

    } else {
        for (size_t i = 0; i < caList.size(); i++) {
            ClauseAnswer cl = caList.at(i);
            // If any boolean in the clause result is false, return empty result
            if (!cl.getIsClauseTrue()) {
                return RAW_ANSWER();
            } else if (cl.getResultTable().getSynonyms().size() > 0 &&
                cl.getResultTable().getResultTable().size() == 0) {
                return RAW_ANSWER();
            }
            // Else merge the results
            selectionResults = selectionResults.merge(cl.getResultTable());
        }
    }

    if (selectionResults.getSynonyms().size() > 0 &&
        selectionResults.getResultTable().size() == 0) {
        return RAW_ANSWER();
    }

    if (selType == SelectionType::SINGLE) {
        return getSingleSelectionResults(selectionResults, dec.at(0));
    } else {
        return getTupleSelectionResults(selectionResults, dec);
    }
}

/**
 * Evaluates a caList to a BOOLEAN RAW_ANSWER
 *
 * @param caList std::vector<ClauseAnswer> indicating all the ClauseAnswer for the query.
 * @return RAW_ANSWER for Select BOOLEAN query.
 */
RAW_ANSWER QueryResultProjector::getBooleanSelectionResults(std::vector<ClauseAnswer> caList) {
    if (caList.size() == 0) {  // Edge case 0
        return { "TRUE" };
    } else if (caList.size() == 1) {  // Edge case 1
        bool truth = caList[0].getIsClauseTrue() &&
            !(caList[0].getResultTable().getSynonyms().size() > 0 &&
                caList[0].getResultTable().getResultTable().size() == 0);
        if (truth) {
            return { "TRUE" };
        } else {
            return { "FALSE" };
        }
    } else {
        ResultTable selectionResults = caList.at(0).getResultTable();
        for (size_t i = 1; i < caList.size(); i++) {
            ClauseAnswer cl = caList.at(i);
            // Terminate early if any of the clause is false
            if (!cl.getIsClauseTrue()) {
                return { "FALSE" };
            } else if (cl.getResultTable().getSynonyms().size() > 0 &&
                cl.getResultTable().getResultTable().size() == 0) {
                return { "FALSE" };
            }
            // Else merge the results
            selectionResults = selectionResults.merge(cl.getResultTable());
        }

        // Check if the merged result is empty
        if (!selectionResults.getSynonyms().empty() && selectionResults.getResultTable().empty()) {
            return { "FALSE" };
        } else {
            return { "TRUE" };
        }
    }
}

/**
 * Evaluates a caList to a BOOLEAN RAW_ANSWER for optimised queries
 *
 * @param caList std::vector<ClauseAnswer> indicating all the ClauseAnswer for the query.
 * @return RAW_ANSWER for Select BOOLEAN query.
 */
RAW_ANSWER QueryResultProjector::getOptimisedBooleanSelectionResults(
    std::vector<ClauseAnswer>caList) {
    if (caList.size() == 0) {  // Edge case 0
        return { "TRUE" };
    } else {
        for (size_t i = 0; i < caList.size(); i++) {
            ClauseAnswer cl = caList.at(i);
            // Terminate early if any of the clause is false
            if (!cl.getIsClauseTrue()) {
                return { "FALSE" };
            } else if (cl.getResultTable().getSynonyms().size() > 0 &&
                cl.getResultTable().getResultTable().size() == 0) {
                return { "FALSE" };
            }
        }
        return { "TRUE" };
    }
}

/**
 * Converts the column of results that corresponds to the selection synonym
 * to a RAW_ANSWER (std::vector<std::string>>).
 *
 * @param result ResultTable of all synonyms in query.
 * @param selection Synonym to be selected.
 * @return RAW_ANSWER of query.
 */
RAW_ANSWER QueryResultProjector::getSingleSelectionResults(ResultTable result, DECLARATION dec) {
    RAW_ANSWER answer;
    std::unordered_set<std::string> answerSet;

    int position = result.getSynonyms().at(dec);

    for each (ROW row in result.getResultTable()) {
        std::string ans = row.at(position);
        if (answerSet.find(ans) == answerSet.end()) {
            answer.push_back(ans);
            answerSet.insert(ans);
        }
    }

    return answer;
}

RAW_ANSWER QueryResultProjector::getTupleSelectionResults(ResultTable result,
                                                          std::vector<DECLARATION> dec) {
    RAW_ANSWER answer;
    std::unordered_set<std::string> answerSet;

    for each (ROW row in result.getResultTable()) {
        std::string temp = "";
        for (size_t i = 0; i < dec.size(); i++) {
            // Find the column of the declaration
            int position = result.getSynonyms().at(dec.at(i));
            temp.append(row.at(position));

            if (i < dec.size() - 1) {
                temp.append(" ");
            }
        }
        answer.push_back(temp);
    }
    return answer;
}

/**
 * Projects the results to the User and returns formatted result to AutoTester.
 *
 * @param unformattedResults Contains the selection results.
 * @param resultsList Pointer to results list of autotester.
 */
void QueryResultProjector::formatResults(RAW_ANSWER unformattedResults,
                                         RESULT_LIST resultsList) {
    if (unformattedResults.empty()) {
        // std::cout << "none :^) empty unformatted results" << std::endl;
    } else {
        for (unsigned int i = 0; i < unformattedResults.size(); i++) {
            resultsList.push_back(unformattedResults[i]);

            // std::cout << unformattedResults[i];
            if (i != (unformattedResults.size() - 1)) {
                // std::cout << ", ";
            }
        }
    }
}
