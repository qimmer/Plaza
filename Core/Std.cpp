//
// Created by Kim on 02-08-2018.
//

#include <string.h>
#include <malloc.h>
#include <locale>
#include "Std.h"
#include "Types.h"
#include "Property.h"

API_EXPORT char *strnstr(const char *haystack, const char *needle, size_t len)
{
    int i;
    size_t needle_len;

    if (0 == (needle_len = strnlen(needle, len)))
        return (char *)haystack;

    for (i=0; i<=(int)(len-needle_len); i++)
    {
        if ((haystack[0] == needle[0]) &&
            (0 == strncmp(haystack, needle, needle_len)))
            return (char *)haystack;

        haystack++;
    }
    return NULL;
}

API_EXPORT StringRef ToLower(StringRef str) {
    char *buffer = (char*)malloc(strlen(str) + 1);
    strcpy(buffer, str);

    for(int i = 0; buffer[i]; i++){
        buffer[i] = tolower(buffer[i]);
    }

    auto value = Intern(buffer);
    free(buffer);
    return value;
}

API_EXPORT StringRef ToUpper(StringRef str) {
    char *buffer = (char*)malloc(strlen(str) + 1);
    strcpy(buffer, str);

    for(int i = 0; buffer[i]; i++){
        buffer[i] = toupper(buffer[i]);
    }

    auto value = Intern(buffer);
    free(buffer);
    return value;
}
