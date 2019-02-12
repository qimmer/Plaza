//
// Created by Kim on 27-11-2018.
//

#ifndef PLAZA_COLLISIONMODULE_H
#define PLAZA_COLLISIONMODULE_H

#include <Core/Module.h>

struct CollisionBody {
    v3f CollisionBodyAngularVelocity, CollisionBodyLinearVelocity;
    float CollisionBodyLinearDamping, CollisionBodyAngularDamping, CollisionBodyGravityScale;
    u8 CollisionBodyType;
    bool CollisionBodyForbidSleep, CollisionBodySleeping, CollisionBodyFixedRotation, CollisionBodyCCD;
};

struct CollisionShape {
    float CollisionShapeDensity, CollisionShapeFriction, CollisionShapeRestitution;
    u16 CollisionShapeGroup, CollisionShapeMask;
    v3f CollisionShapePositionOffset, CollisionShapeRotationEulerOffset;
};

struct CollisionScene {
    v3f CollisionSceneGravity;
};

struct CollisionBoxShape {
    v3f CollisionShapeExtent;
};

struct CollisionSphereShape {
    float CollisionShapeRadius;
};

struct CollisionForce {
    v3f CollisionForceDirection;
    float CollisionForceFactor;
    Entity CollisionForceBody;
};

struct CollisionImpulse {
    v3f CollisionImpulseDirection;
    float CollisionImpulseFactor;
    bool CollisionImpulseTrigger;
    Entity CollisionImpulseBody;
};

struct CollisionContact {
    Entity CollisionContactShape, CollisionContactOtherShape;
};

enum {
    CollisionBodyType_None,
    CollisionBodyType_Overlap,
    CollisionBodyType_Static,
    CollisionBodyType_Kinematic,
    CollisionBodyType_Dynamic
};

enum {
    CollisionShapeGroup_0 = 0,
    CollisionShapeGroup_1 = 1,
    CollisionShapeGroup_2 = 2,
    CollisionShapeGroup_3 = 4,
    CollisionShapeGroup_4 = 8,
    CollisionShapeGroup_5 = 16,
    CollisionShapeGroup_6 = 32,
    CollisionShapeGroup_7 = 64,
    CollisionShapeGroup_8 = 128,
    CollisionShapeGroup_9 = 256,
    CollisionShapeGroup_10 = 512,
    CollisionShapeGroup_11 = 1024,
    CollisionShapeGroup_12 = 2048,
    CollisionShapeGroup_13 = 4096,
    CollisionShapeGroup_14 = 8192,
    CollisionShapeGroup_15 = 16384
};

Module(Collision)

Unit(Collision)
    Enum(CollisionBodyType)
    Component(CollisionShape)
        Property(float, CollisionShapeDensity)
        Property(float, CollisionShapeFriction)
        Property(float, CollisionShapeRestitution)
        Property(u16, CollisionShapeGroup)
        Property(u16, CollisionShapeMask)
        Property(v3f, CollisionShapePositionOffset)
        Property(v3f, CollisionShapeRotationEulerOffset)

    Component(CollisionBoxShape)
        Property(v3f, CollisionShapeExtent)

    Component(CollisionSphereShape)
        Property(float, CollisionShapeRadius)

    Component(CollisionBody)
        ArrayProperty(CollisionShape, CollisionBodyShapes)
        Property(u8, CollisionBodyType)
        Property(float, CollisionBodyLinearDamping)
        Property(float, CollisionBodyAngularDamping)
        Property(float, CollisionBodyGravityScale)
        Property(bool, CollisionBodyForbidSleep)
        Property(bool, CollisionBodySleeping)
        Property(bool, CollisionBodyFixedRotation)
        Property(bool, CollisionBodyCCD)
        Property(v3f, CollisionBodyAngularVelocity)
        Property(v3f, CollisionBodyLinearVelocity)
        ArrayProperty(CollisionContact, CollisionBodyContacts)

    Component(CollisionScene)
        Property(v3f, CollisionSceneGravity)

    Component(CollisionForce)
        Property(v3f, CollisionForceDirection)
        Property(float, CollisionForceFactor)
        ReferenceProperty(CollisionBody, CollisionForceBody)

    Component(CollisionImpulse)
        Property(v3f, CollisionImpulseDirection)
        Property(float, CollisionImpulseFactor)
        ReferenceProperty(CollisionBody, CollisionImpulseBody)
        Property(bool, CollisionImpulseTrigger)

    Component(CollisionContact)
        ReferenceProperty(CollisionShape, CollisionContactShape)
        ReferenceProperty(CollisionShape, CollisionContactOtherShape)


#endif //PLAZA_COLLISIONMODULE_H
