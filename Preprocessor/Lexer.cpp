//
// Created by Kim on 16-01-2019.
//

#include "Lexer.h"

#include "lexertk/lexertk.hpp"

static bool Lex(Entity lexer, StringRef cppCode) {
    lexertk::generator generator;
    lexertk::helper::bracket_checker bc;

    if (!generator.process(cppCode))
    {
        SetNumLexerTokens(lexer, 0);
        return false;
    }

    if (!bc.process(generator))
    {
        SetNumLexerTokens(lexer, 0);
        return false;
    }

    SetNumLexerTokens(lexer, generator.size());

    auto tokens = GetLexerTokens(lexer, NULL);
    for(auto i = 0; i < generator.size(); ++i) {
        auto& entry = generator[i];

        auto token = tokens[i];
        SetLexerTokenStart(token, (u32)entry.position);
        SetLexerTokenEnd(token, (u32)entry.position + (u32)entry.value.length());
        SetLexerTokenType(token, entry.type);
        SetLexerTokenValue(token, entry.value.c_str());
    }

    return true;
}

BeginUnit(Lexer)
    BeginEnum(LexerTokenType, false)
        RegisterFlag(LexerTokenType_None)
        RegisterFlag(LexerTokenType_Error)
        RegisterFlag(LexerTokenType_ErrSymbol)
        RegisterFlag(LexerTokenType_Err_number)
        RegisterFlag(LexerTokenType_Err_string)
        RegisterFlag(LexerTokenType_ErrFunc)
        RegisterFlag(LexerTokenType_Semicolon)
        RegisterFlag(LexerTokenType_Number)
        RegisterFlag(LexerTokenType_Symbol)
        RegisterFlag(LexerTokenType_String)
        RegisterFlag(LexerTokenType_Assign)
        RegisterFlag(LexerTokenType_ShiftRight)
        RegisterFlag(LexerTokenType_ShiftLeft)
        RegisterFlag(LexerTokenType_LessEq)
        RegisterFlag(LexerTokenType_NotEqual)
        RegisterFlag(LexerTokenType_GreaterEq)
        RegisterFlag(LexerTokenType_Less)
        RegisterFlag(LexerTokenType_Greater)
        RegisterFlag(LexerTokenType_Equal)
        RegisterFlag(LexerTokenType_CloseParan)
        RegisterFlag(LexerTokenType_OpenParan)
        RegisterFlag(LexerTokenType_CloseBracket)
        RegisterFlag(LexerTokenType_OpenBracket)
        RegisterFlag(LexerTokenType_CloseBrace)
        RegisterFlag(LexerTokenType_OpenBrace)
        RegisterFlag(LexerTokenType_Comma)
        RegisterFlag(LexerTokenType_Add)
        RegisterFlag(LexerTokenType_Sub)
        RegisterFlag(LexerTokenType_Div)
        RegisterFlag(LexerTokenType_Mul)
        RegisterFlag(LexerTokenType_Mod)
        RegisterFlag(LexerTokenType_Pow)
        RegisterFlag(LexerTokenType_Colon)
    EndEnum()
    BeginComponent(Lexer)
        RegisterArrayProperty(LexerToken, LexerTokens)
    EndComponent()
    BeginComponent(LexerToken)
        RegisterProperty(u32, LexerTokenStart)
        RegisterProperty(u32, LexerTokenEnd)
        RegisterProperty(StringRef, LexerTokenValue)
        RegisterPropertyEnum(u8, LexerTokenType, LexerTokenType)
    EndComponent()
EndUnit()