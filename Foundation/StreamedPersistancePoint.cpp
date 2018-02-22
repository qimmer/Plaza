
#include "StreamedPersistancePoint.h"

struct StreamedPersistancePoint {
    Entity PersistancePointStream;
};

DefineComponent(StreamedPersistancePoint)
EndComponent()

DefineComponentProperty(StreamedPersistancePoint, Entity, PersistancePointStream)
