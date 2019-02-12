//
// Created by Kim on 18-01-2019.
//

#ifndef PLAZA_CPPPARSER_H
#define PLAZA_CPPPARSER_H

#include <Core/NativeUtils.h>

struct CppScopeSegment {
    Entity CppScopeSegmentToken;
};

struct CppScope {
};

struct CppStatement {
};

struct CppExpression {
};

struct CppFunctionDeclaration {
};

struct CppStructDeclaration {
};

struct CppParser {

};

Unit(CppParser)
    Component(CppScope)
        ArrayProperty(CppScope, CppScopeSubScopes)

    Component(CppScopeSegment)
        ReferenceProperty(LexerToken, CppScopeSegmentToken)

    Component(CppStatement)
    Component(CppExpression)
    Component(CppFunctionDeclaration)



#endif //PLAZA_CPPPARSER_H
