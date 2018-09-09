//
// Created by Kim on 09-09-2018.
//

#include "Date.h"

#include <time.h>
#include <stdio.h>

API_EXPORT Date GetDateNow() {
    return (Date)time(NULL);
}

API_EXPORT StringRef FormatDate(Date date, bool utc) {
    time_t t = (time_t)date;
    if(utc) {
        return Intern(asctime(gmtime(&t)));
    } else {
        return Intern(asctime(localtime(&t)));
    }
}

BeginUnit(Date)
    RegisterFunction(GetDateNow)
EndUnit()