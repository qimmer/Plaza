//
// Created by Kim on 22-07-2018.
//

#ifndef PLAZA_RESTSERVER_H
#define PLAZA_RESTSERVER_H

#include <Core/NativeUtils.h>

struct RestServer {
};

Unit(RestServer)
    Component(RestServer)
        ArrayProperty(RestRouting, RestServerRoutes)

#endif //PLAZA_RESTSERVER_H
