#include <stack>
#include <string>
#include "Utilities.h"

PREFIX_EXPR Utilities::makePrefixArithExpr(CODE_SEGMENT arithExpr) {
    arithExpr = Utilities::reverseArithExpr(arithExpr);
    arithExpr = Utilities::trim(arithExpr);
    arithExpr = "(" + arithExpr + ")";
    std::stack<char> stk;
    PREFIX_EXPR output;

    for (size_t i = 0; i < arithExpr.length(); i++) {
        char nextChar = arithExpr.at(i);
        if (nextChar == ' ') {
            continue;

        } else if (isalnum(nextChar)) {
            if (i > 0 && !isalnum(arithExpr.at(i - 1))) {
                output = Utilities::addSpaceIfNone(output);
            }
            output += nextChar;

        } else if (nextChar == '(') {
            stk.push(nextChar);

        } else if (nextChar == ')') {
            while (stk.top() != '(') {
                output = Utilities::addSpaceIfNone(output);
                output += stk.top();
                stk.pop();
            }
            stk.pop();

        } else {
            while (operatorHasPrecedence(stk.top(), nextChar)) {
                output = Utilities::addSpaceIfNone(output);
                output += stk.top();
                stk.pop();
            }
            stk.push(nextChar);
        }
    }
    output = Utilities::reverseArithExpr(output);
    return output;
}


CODE_SEGMENT Utilities::reverseArithExpr(CODE_SEGMENT arithExpr) {
    CODE_SEGMENT output = "";
    CODE_SEGMENT curTerm = "";

    for (int i = arithExpr.length() - 1; i >= 0; i--) {
        char nextChar = arithExpr.at(i);

        if (nextChar == '(') {
            output += curTerm + ")";
            curTerm = "";
        } else if (nextChar == ')') {
            output += "(";
        } else if (isalnum(nextChar)) {
            curTerm = nextChar + curTerm;
        } else {
            output += curTerm + nextChar;
            curTerm = "";
        }
    }
    output += curTerm;
    return output;
}


CODE_SEGMENT Utilities::trim(CODE_SEGMENT str) {
    if (str == "") {
        return str;
    }

    CODE_SEGMENT before_trim = str;
    do {
        before_trim = str;
        str = Utilities::trimByCharType(str, ' ');
        str = Utilities::trimByCharType(str, '\n');
        str = Utilities::trimByCharType(str, '\t');
    } while (before_trim != str);

    return str;
}

CODE_SEGMENT Utilities::trimByCharType(CODE_SEGMENT str, char c) {
    if (str == "") {
        return str;
    }

    int firstWanted = str.find_first_not_of(c);
    int lastWanted = str.find_last_not_of(c);

    if (std::string::npos == firstWanted) {
        return "";
    }
    return str.substr(firstWanted, (lastWanted - firstWanted + 1));
}

bool Utilities::operatorHasPrecedence(char op1, char op2) {
    return getOperatorPrecedence(op1) > getOperatorPrecedence(op2);
}


int Utilities::getOperatorPrecedence(char op) {
    if ((op == '+') || (op == '-')) {
        return 1;
    } else if ((op == '*') || (op == '/') || (op == '%')) {
        return 2;
    }
    return 0;
}


CODE_SEGMENT Utilities::addSpaceIfNone(CODE_SEGMENT str) {
    if (str.length() > 0 && str.back() != ' ') {
        str += " ";
    }
    return str;
}
