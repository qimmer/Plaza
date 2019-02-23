//
// Created by Kim on 03-01-2019.
//

#include "Logical.h"

static StringRef FormatArguments(Entity format) {
    // Parse formatted text and replace argument braces with values
    static std::vector<char> formattedText;
    formattedText.clear();

    auto formatString = GetFormatString(format);

    StringRef textStart = formatString;
    auto length = strlen(formatString);

    while(textStart && *textStart) {
        StringRef textEnd = strchr(textStart, '{');
        if(!textEnd) textEnd = formatString + length;

        formattedText.insert(formattedText.end(), textStart, textEnd);

        char* closeBrace = strchr(textEnd, '}');

        if(*textEnd == '{' && closeBrace) {
            char argIndexString[32];
            auto argIndexStringLen = closeBrace - textEnd - 2;
            memcpy(argIndexString, textEnd + 1, argIndexStringLen);
            argIndexString[argIndexStringLen] = '\0';
            auto argIndex = strtoul(argIndexString, NULL, 10);
            u32 argCount = 0;
            auto arguments = GetFormatArguments(format, &argCount);
            if(argIndex < argCount) {
                auto argumentValue = GetFormatArgumentValue(arguments[argIndex]);
                argumentValue = Cast(argumentValue, TypeOf_StringRef);
                formattedText.insert(formattedText.end(), argumentValue.as_StringRef, argumentValue.as_StringRef + strlen(argumentValue.as_StringRef));
            }

            textStart = closeBrace + 1;
        } else {
            textStart = textStart + length;
        }
    }

    formattedText.push_back('\0');

    return Intern(formattedText.data());
}