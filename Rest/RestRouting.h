//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_RESTROUTING_H
#define PLAZA_RESTROUTING_H

#include <Core/NativeUtils.h>

Unit(RestRouting)
    Component(RestRouting)
        Property(StringRef, RestRoutingRoute)

    Event(RestRequest, Entity requestStream, Entity responseStream)



#endif //PLAZA_RESTROUTING_H
