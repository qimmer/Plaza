
#include "SQLPersistancePoint.h"


struct SQLPersistancePoint {
};

DefineComponent(SQLPersistancePoint)

EndComponent()

DefineService(SQLPersistancePoint)

EndService()

static bool ServiceStart() {
    return true;
}

static bool ServiceStop() {
    return true;
}
