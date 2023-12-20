#include "stdafx.h"
#include "AbstractParserMock.h"

PARSE_RESULT AbstractParserMock::parse(CODE_SEGMENT code, LINE_NUM firstLine) {
    methodCalls.push_back(PARSE_ARGS{ code, std::to_string(firstLine) });
    return PARSE_RESULT{ firstLine + segmentLen, LINE_NUMS{firstLine} };
}

PARSE_RESULT AbstractParserMock::parse(CODE_SEGMENT code, LINE_NUM firstLine, LINE_NUM parentLine) {
    methodCalls.push_back(
        PARSE_ARGS{code, std::to_string(firstLine) + " " + std::to_string(parentLine)});
    return PARSE_RESULT{ firstLine + segmentLen, LINE_NUMS{firstLine} };
}

std::list<PARSE_ARGS> AbstractParserMock::getAllMethodCalls() {
    return methodCalls;
}
