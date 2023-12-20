#pragma once
#include <list>
#include <string>
#include <regex>
#include <utility>
#include <vector>
#include "../Commons/Types.h"
#include "StmtType.h"

typedef std::list<VAR_NAME> VAR_NAMES;
typedef std::list<LINE_NUM> LINE_NUMS;
typedef std::string INTEGER;
typedef std::list<INTEGER> INTEGERS;
typedef std::pair<LINE_NUM, LINE_NUMS> PARSE_RESULT;

class ParserCommons {
 public:
     static VAR_NAMES getVarsInExpr(CODE_SEGMENT expr);
     static INTEGERS getIntsInExpr(CODE_SEGMENT expr);
     static bool isValidCondExpr(CODE_SEGMENT condExpr);
     static bool isValidBracketedCondExpr(CODE_SEGMENT condExpr);
     static CODE_SEGMENT extractFromBrackets(CODE_SEGMENT bracketedCode);
     static bool isValidArithExpr(CODE_SEGMENT arithExpr);
     static bool isValidName(CODE_SEGMENT name);
     static bool isValidRelExpr(CODE_SEGMENT relExpr);
     static StmtType inferStmtTypeFromStart(CODE_SEGMENT code);
     static bool codeSegmentBeginsWith(CODE_SEGMENT main, CODE_SEGMENT beginning);
     static bool codeSegmentEndsWith(CODE_SEGMENT main, CODE_SEGMENT ending);
     static bool codeSegmentBeginsAndEndsWith(CODE_SEGMENT main,
         CODE_SEGMENT beginning, CODE_SEGMENT ending);
 private:
     static bool startOfCodeSegmentMatchesRegexStr(CODE_SEGMENT code, std::string reg_str);
     static std::list<CODE_SEGMENT> findAllMatches(CODE_SEGMENT code, std::regex reg);
     static std::vector<int> getNestedLevelByChar(CODE_SEGMENT code);
};
