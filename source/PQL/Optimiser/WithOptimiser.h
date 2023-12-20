#pragma once

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <vector>
#include <string>
#include "../Query/Query.h"
#include "QueryOptimiserLookup.h"

typedef std::unordered_map<Declaration, std::vector<std::pair<int, int>>, dec_hash> DECLR_MAPPING;
typedef std::set<std::pair<int, int>> CLAUSE_INFO;
typedef std::unordered_map<SYNONYM, int> SYNONYM_COUNT;

class WithOptimiser {
 public:
    WithOptimiser() {}
    Query replaceSynonyms(Query& queryObject);
    void doValueReplacement(
        QueryClause& withCl, CLAUSE_LIST& clList, SELECTION_LIST& selectionList,
        QueryArg& toReplace, QueryArg& replacement);
    void doDeclarationReplacement(
        QueryClause& withCl, CLAUSE_LIST& clList, SELECTION_LIST& selectionList,
        QueryArg& leftArg, QueryArg& rightArg);
    void replaceDeclarationSynHelper(
        Declaration& toReplace, Declaration& replacement, CLAUSE_LIST& clList, QueryClause& withCl);
    void createSynMaps(CLAUSE_LIST& clauseList);
    void getSelectionSyns(SELECTION_LIST& selectionList);

 private:
    std::unordered_set<SYNONYM> selectionSyns;
    SYNONYM_COUNT synCount;  // count of number of appearances of each synonym
    SYNONYM_COUNT withClSyns;  // count of number of appearances of syn in with cl
    DECLR_MAPPING synMapNum;  // syn map for number replacement
    DECLR_MAPPING synMapStr;  // syn map for str replacement
    CLAUSE_INFO argsReplaced;  // set of tuples indicating index + arg of clause
    CLAUSE_LIST leftOverWithCl;  // with clauses not used for replacement
    SelectionType selectType;
};
