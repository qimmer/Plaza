//
// Created by Kim on 27-11-2018.
//
//
// Created by Kim on 16-11-2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <Scene/SceneModule.h>

#include "CollisionModule.h"

#include <Scene/Transform.h>

BeginModule(Collision)
    RegisterDependency(Foundation)
    RegisterDependency(Scene)

    RegisterUnit(Collision)
EndModule()

BeginUnit(Collision)
    BeginEnum(CollisionBodyType, false)
        RegisterFlag(CollisionBodyType_None)
        RegisterFlag(CollisionBodyType_Overlap)
        RegisterFlag(CollisionBodyType_Static)
        RegisterFlag(CollisionBodyType_Kinematic)
        RegisterFlag(CollisionBodyType_Dynamic)
    EndEnum()
    BeginComponent(CollisionShape)
        RegisterProperty(float, CollisionShapeDensity)
        RegisterProperty(float, CollisionShapeFriction)
        RegisterProperty(float, CollisionShapeRestitution)
        RegisterProperty(u16, CollisionShapeGroup)
        RegisterProperty(u16, CollisionShapeMask)
        RegisterProperty(v3f, CollisionShapePositionOffset)
        RegisterProperty(v3f, CollisionShapeRotationEulerOffset)
    EndComponent()

    BeginComponent(CollisionBoxShape)
        RegisterProperty(v3f, CollisionShapeExtent)
    EndComponent()

    BeginComponent(CollisionSphereShape)
        RegisterProperty(float, CollisionShapeRadius)
    EndComponent()

    BeginComponent(CollisionBody)
        RegisterBase(Transform)
        RegisterArrayProperty(CollisionShape, CollisionBodyShapes)
        RegisterPropertyEnum(u8, CollisionBodyType, CollisionBodyType)
        RegisterProperty(float, CollisionBodyLinearDamping)
        RegisterProperty(float, CollisionBodyAngularDamping)
        RegisterProperty(float, CollisionBodyGravityScale)
        RegisterProperty(bool, CollisionBodyForbidSleep)
        RegisterProperty(bool, CollisionBodySleeping)
        RegisterProperty(bool, CollisionBodyFixedRotation)
        RegisterProperty(bool, CollisionBodyCCD)
        RegisterProperty(v3f, CollisionBodyAngularVelocity)
        RegisterProperty(v3f, CollisionBodyLinearVelocity)
        RegisterArrayProperty(CollisionContact, CollisionBodyContacts)
    EndComponent()

    BeginComponent(CollisionScene)
        RegisterProperty(v3f, CollisionSceneGravity)
    EndComponent()

    BeginComponent(CollisionForce)
        RegisterProperty(v3f, CollisionForceDirection)
        RegisterProperty(float, CollisionForceFactor)
        RegisterReferenceProperty(CollisionBody, CollisionForceBody)
    EndComponent()

    BeginComponent(CollisionImpulse)
        RegisterProperty(v3f, CollisionImpulseDirection)
        RegisterProperty(float, CollisionImpulseFactor)
        RegisterReferenceProperty(CollisionBody, CollisionImpulseBody)
        RegisterProperty(bool, CollisionImpulseTrigger)
    EndComponent()

    BeginComponent(CollisionContact)
        RegisterReferencePropertyReadOnly(CollisionShape, CollisionContactShape)
        RegisterReferencePropertyReadOnly(CollisionShape, CollisionContactOtherShape)
    EndComponent()
EndUnit()