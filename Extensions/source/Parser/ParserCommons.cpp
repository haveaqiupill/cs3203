#include <algorithm>
#include <stack>
#include "ParserCommons.h"
#include "../Commons/Utilities.h"
#include "../Commons/InvalidSimpleException.h"

const char NAME_REGEX_STR[] = "[A-Za-z][A-Za-z\\d]*(?![A-Za-z0-9])";
const std::regex INTEGER_REGEX("(0|([1-9]\\d*))(?![A-Za-z0-9])");


bool ParserCommons::isValidName(CODE_SEGMENT name) {
    return std::regex_match(name, std::regex(NAME_REGEX_STR)) && name != "none";
}


VAR_NAMES ParserCommons::getVarsInExpr(CODE_SEGMENT expr) {
    return findAllMatches(expr, std::regex(NAME_REGEX_STR));
}


INTEGERS ParserCommons::getIntsInExpr(CODE_SEGMENT expr) {
    std::list<CODE_SEGMENT> codeSegmentMatches = findAllMatches(expr, INTEGER_REGEX);
    INTEGERS intMatches;

    for (const CODE_SEGMENT &segment : codeSegmentMatches) {
        // workaround since cpp regex engine does not support lookbehind.
        size_t nextPos = expr.find(segment, 0);
        while (nextPos != std::string::npos) {
            if (nextPos == 0 || !std::isalnum(expr.at(nextPos - 1))) {
                intMatches.push_back(segment);
                break;
            }
            nextPos = expr.find(segment, nextPos + 1);
        }
    }
    return intMatches;
}



bool ParserCommons::isValidCondExpr(CODE_SEGMENT condExpr) {
    condExpr = Utilities::trim(condExpr);
    std::vector<int> nestedLevels = getNestedLevelByChar(condExpr);

    // shortest possible cond_expr has length of 3.
    if (condExpr.length() < 3) {
        return false;
    }

    // verify bracket count
    if ((condExpr.back() == ')' && nestedLevels.back() > 1)
    || condExpr.back() != ')' && nestedLevels.back() > 0) {
        return false;
    }

    // verify bracket order
    for (int nestedLevel : nestedLevels) {
        if (nestedLevel < 0) {
            return false;
        }
    }

    // if it's of the form !(cond_expr)
    if (condExpr.at(0) == '!') {
        return isValidBracketedCondExpr(
            condExpr.substr(1, condExpr.length() - 1));
    }

    // if it's of the form (cond_expr)||(cond_expr) or (cond_expr)&&(cond_expr)
    for (size_t i = 0; i < condExpr.length() - 1; i++) {
        if ((condExpr.substr(i, 2) == "&&" || condExpr.substr(i, 2) == "||")
        && nestedLevels.at(i) == 0 && nestedLevels.at(i + 1) == 0) {
            CODE_SEGMENT subExpr1 = Utilities::trim(
                condExpr.substr(0, i));
            CODE_SEGMENT subExpr2 = Utilities::trim(
                condExpr.substr(i + 2, condExpr.length() - i - 2));

            return isValidBracketedCondExpr(subExpr1) && isValidBracketedCondExpr(subExpr2);
        }
    }

    // base case - can't be decomposed into smaller condExpr.
    return isValidRelExpr(condExpr);
}


// cond_expr in simple spec does not allow arbitrary bracketing
bool ParserCommons::isValidBracketedCondExpr(CODE_SEGMENT bracketedCondExpr) {
    bracketedCondExpr = Utilities::trim(bracketedCondExpr);
    if (!codeSegmentBeginsAndEndsWith(bracketedCondExpr, "(", ")")) {
        return false;
    }
    return isValidCondExpr(
        Utilities::trim(bracketedCondExpr.substr(1, bracketedCondExpr.length() - 2)));
}


CODE_SEGMENT ParserCommons::extractFromBrackets(CODE_SEGMENT bracketedCode) {
    bracketedCode = Utilities::trim(bracketedCode);
    CODE_SEGMENT code = bracketedCode.substr(1, bracketedCode.length() - 2);
    return Utilities::trim(code);
}


bool ParserCommons::isValidRelExpr(CODE_SEGMENT relExpr) {
    relExpr = Utilities::trim(relExpr);
    std::vector<int> nestedLevels = getNestedLevelByChar(relExpr);

    // shortest possible rel_expr has length of 3.
    if (relExpr.length() < 3) {
        return false;
    }

    for (size_t i = 0; i < relExpr.length() - 1; i++) {
        char nextChar = relExpr.at(i);

        if ((nextChar == '>' || nextChar == '<' || nextChar == '!' || nextChar == '=')
        && nestedLevels.at(i) == 0) {
            CODE_SEGMENT relFactor1 = relExpr.substr(0, i);
            CODE_SEGMENT relFactor2;

            if (relExpr.length() > i + 1 && relExpr.at(i + 1) == '=') {
                relFactor2 = relExpr.substr(i + 2, relExpr.length() - i - 2);
            } else if (nextChar == '!' || nextChar == '=') {
                continue;
            } else {
                relFactor2 = relExpr.substr(i + 1, relExpr.length() - i - 1);
            }

            return isValidArithExpr(Utilities::trim(relFactor1))
                && isValidArithExpr(Utilities::trim(relFactor2));
        }
    }
    return false;
}


bool ParserCommons::isValidArithExpr(CODE_SEGMENT arithExpr) {
    arithExpr = Utilities::trim(arithExpr);
    arithExpr = std::regex_replace(arithExpr, std::regex("[1-9]\\d*"), "1");
    arithExpr = std::regex_replace(arithExpr, std::regex("0"), "1");
    arithExpr = std::regex_replace(arithExpr, std::regex("[A-Za-z][A-Za-z\\d]*"), "1");
    arithExpr = std::regex_replace(arithExpr, std::regex("[*/\\-%]"), "+");
    arithExpr = std::regex_replace(arithExpr, std::regex("\\s"), "");

    while (arithExpr.find("1+1") != std::string::npos
    || arithExpr.find("(1)") != std::string::npos) {
        arithExpr = std::regex_replace(arithExpr, std::regex("1\\+1"), "1");
        arithExpr = std::regex_replace(arithExpr, std::regex("\\(1\\)"), "1");
    }
    return arithExpr == "1";
}


StmtType ParserCommons::inferStmtTypeFromStart(CODE_SEGMENT code) {
    code = Utilities::trim(code);

    if (startOfCodeSegmentMatchesRegexStr(
    code, "read\\s+[A-Za-z][A-Za-z\\d]*\\s*;")) {
        return StmtType::READ;
    }

    if (startOfCodeSegmentMatchesRegexStr(
        code, "print\\s+[A-Za-z][A-Za-z\\d]*\\s*;")) {
        return StmtType::PRINT;
    }

    if (startOfCodeSegmentMatchesRegexStr(
        code, "call\\s+[A-Za-z][A-Za-z\\d]*\\s*;")) {
        return StmtType::CALL;
    }

    if (startOfCodeSegmentMatchesRegexStr(
        code, "[A-Za-z][A-Za-z\\d]*\\s*=")) {
        return StmtType::ASSIGNMENT;
    }

    if (startOfCodeSegmentMatchesRegexStr(
        code, "if\\s*\\(")) {
        return StmtType::IF_ELSE;
    }

    if (startOfCodeSegmentMatchesRegexStr(
        code, "while\\s*\\(")) {
        return StmtType::WHILE;
    }

    throw new InvalidSimpleException(
        InvalidSimpleException::INVALID_STMT_MSG);
}


bool ParserCommons::codeSegmentBeginsWith(CODE_SEGMENT code, CODE_SEGMENT beginning) {
    if (code.length() < beginning.length()) {
        return false;
    }

    return code.substr(0, beginning.length()) == beginning;
}


bool ParserCommons::codeSegmentEndsWith(CODE_SEGMENT code, CODE_SEGMENT ending) {
    if (code.length() < ending.length()) {
        return false;
    }

    return code.substr(code.length() - ending.length()) == ending;
}

bool ParserCommons::codeSegmentBeginsAndEndsWith(
CODE_SEGMENT code, CODE_SEGMENT beginning, CODE_SEGMENT ending) {
    return codeSegmentBeginsWith(code, beginning) && codeSegmentEndsWith(code, ending);
}

// private methods below =========================================================


bool ParserCommons::startOfCodeSegmentMatchesRegexStr(
CODE_SEGMENT code, std::string reg_str) {
    std::smatch match;
    std::regex_search(code, match, std::regex(reg_str));
    if (!match.empty() && match.position() == 0) {
        return true;
    }

    return false;
}


std::vector<int> ParserCommons::getNestedLevelByChar(CODE_SEGMENT code) {
    int nestedLevel = 0;
    std::vector<int> result(code.length());

    for (size_t i = 0; i < code.length(); i++) {
        char nextChar = code.at(i);
        result.at(i) = nestedLevel;
        if (nextChar == '(') {
            nestedLevel++;
        } else if (nextChar == ')') {
            nestedLevel--;
        }
    }

    return result;
}


std::list<CODE_SEGMENT> ParserCommons::findAllMatches(CODE_SEGMENT code, std::regex reg) {
    std::list<std::string> matches;
    std::smatch match;
    std::sregex_iterator next(code.begin(), code.end(), reg);
    std::sregex_iterator end;
    while (next != end) {
        match = *next;
        matches.push_back(match.str());
        next++;
    }
    matches.sort();
    matches.unique();
    return matches;
}
