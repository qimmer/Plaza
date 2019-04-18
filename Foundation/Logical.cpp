//
// Created by Kim on 03-01-2019.
//

#include "Logical.h"

static StringRef FormatArguments(Entity format) {
    // Parse formatted text and replace argument braces with values
    static eastl::fixed_vector<char, 1024> formattedText;
    formattedText.clear();

    auto formatData = GetFormat(format);

    StringRef textStart = formatData.FormatString;
    auto length = strlen(formatData.FormatString);

    while(textStart && *textStart) {
        StringRef textEnd = strchr(textStart, '{');
        if(!textEnd) textEnd = formatData.FormatString + length;

        formattedText.insert(formattedText.end(), textStart, textEnd);

        StringRef closeBrace = strchr(textEnd, '}');

        if(*textEnd == '{' && closeBrace) {
            char argIndexString[32];
            auto argIndexStringLen = closeBrace - textEnd - 2;
            memcpy(argIndexString, textEnd + 1, argIndexStringLen);
            argIndexString[argIndexStringLen] = '\0';
            auto argIndex = strtoul(argIndexString, NULL, 10);

            if(argIndex < formatData.FormatArguments.GetSize()) {
                auto argumentValue = GetFormatArgument(formatData.FormatArguments[argIndex]).FormatArgumentValue;
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