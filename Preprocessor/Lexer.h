//
// Created by Kim on 16-01-2019.
//

#ifndef PLAZA_LEXER_H
#define PLAZA_LEXER_H

#include <Core/NativeUtils.h>

enum {
    LexerTokenType_None         =   0,
    LexerTokenType_Error       =   1,
    LexerTokenType_ErrSymbol   =   2,
    LexerTokenType_Err_number   =   3,
    LexerTokenType_Err_string  =   4,
    LexerTokenType_ErrFunc     =   5,
    LexerTokenType_Semicolon    =   6,
    LexerTokenType_Number      =   7,
    LexerTokenType_Symbol      =   8,
    LexerTokenType_String       =   9,
    LexerTokenType_Assign      =  10,
    LexerTokenType_ShiftRight  =  11,
    LexerTokenType_ShiftLeft    =  12,
    LexerTokenType_LessEq      =  13,
    LexerTokenType_NotEqual    =  14,
    LexerTokenType_GreaterEq    =  15,
    LexerTokenType_Less        = '<',
    LexerTokenType_Greater     = '>',
    LexerTokenType_Equal        = '=',
    LexerTokenType_CloseParan  = ')',
    LexerTokenType_OpenParan   = '(',
    LexerTokenType_CloseBracket = ']',
    LexerTokenType_OpenBracket = '[',
    LexerTokenType_CloseBrace  = '}',
    LexerTokenType_OpenBrace    = '{',
    LexerTokenType_Comma       = ',',
    LexerTokenType_Add         = '+',
    LexerTokenType_Sub          = '-',
    LexerTokenType_Div         = '/',
    LexerTokenType_Mul         = '*',
    LexerTokenType_Mod          = '%',
    LexerTokenType_Pow         = '^',
    LexerTokenType_Colon       = ':'
};

struct Lexer {
    ChildArray LexerTokens;
};

struct LexerToken {
    u32 LexerTokenStart, LexerTokenEnd;
    u8 LexerTokenType;
    StringRef LexerTokenValue;
};

Unit(Lexer)
    Enum(LexerTokenType)

    Component(Lexer)
        ArrayProperty(LexerToken, LexerTokens)

    Component(LexerToken)
        Property(u32, LexerTokenStart)
        Property(u32, LexerTokenEnd)
        Property(u8, LexerTokenType)
        Property(StringRef, LexerTokenValue)

#endif //PLAZA_LEXER_H
