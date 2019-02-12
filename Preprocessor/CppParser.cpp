//
// Created by Kim on 18-01-2019.
//

#include "CppParser.h"
#include "Lexer.h"

static void ParseScope(const Entity *tokens, u32 numTokens);
static u32 ParseScope(Entity scope, const Entity *tokens, u32 numTokens, u32 scopeStart);

static const u8 openTokens[] = {
    LexerTokenType_OpenBrace,
    LexerTokenType_OpenBracket,
    LexerTokenType_OpenParan,
    0
};

static const u8 closeTokens[] = {
    LexerTokenType_CloseBrace,
    LexerTokenType_CloseBracket,
    LexerTokenType_CloseParan,
    0
};

static const u8 terminatingTokens[] = {
        LexerTokenType_Comma,
        LexerTokenType_Semicolon,
        0
};

static bool IsTokenOpen(Entity token) {
    auto type = GetLexerTokenType(token);
    return type == LexerTokenType_OpenBrace || type == LexerTokenType_OpenBracket || type == LexerTokenType_OpenParan;
}

static bool IsTokenClose(Entity token) {
    auto type = GetLexerTokenType(token);
    return type == LexerTokenType_CloseBrace || type == LexerTokenType_CloseBracket || type == LexerTokenType_CloseParan;
}

static u32 ParseScope(Entity scope, const Entity *tokens, u32 numTokens, u32 scopeStart) {
    auto index = scopeStart;
    while(index < numTokens) {
        auto token = tokens[index];

        if(IsTokenClose(token)) {
            return index;
        } else if(IsTokenOpen(token)) {
            auto subScope = AddCppScopeSubScopes(scope);
            index += ParseScope(subScope, tokens, numTokens, index + 1);
        } else {

        }

        index++;
    }
}

static void Parse(Entity lexer) {
    u32 numTokens = 0;
    auto tokens = GetLexerTokens(lexer, &numTokens);

    //ParseScope(tokens, numTokens, 0);
}