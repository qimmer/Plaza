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

#include "Box2DModule.h"

#include <Scene/Scene.h>
#include <Scene/Transform.h>
#include <Collision/CollisionModule.h>

#include <Foundation/Timer.h>
#include <Foundation/StopWatch.h>

#include <Box2D/Box2D.h>

#include <eastl/unordered_map.h>

static float timeStep = 1.0f / 60.0f;
static bool isUpdatingTransforms = false;

struct Box2DCollisionBody {
    b2Body *Body;
};

struct Box2DCollisionScene {
    b2World *World;
};


class ContactListener : public b2ContactListener {
    eastl::unordered_map<b2Contact*, eastl::pair<Entity, Entity>> contactEntityMapping;
public:
    virtual void BeginContact(b2Contact *contact) {
        b2ContactListener::BeginContact(contact);

        auto shape1 = GetEntityByIndex((u32)(size_t)contact->GetFixtureA()->GetUserData());
        auto shape2 = GetEntityByIndex((u32)(size_t)contact->GetFixtureB()->GetUserData());

        auto body1 = GetOwner(shape1);
        auto body2 = GetOwner(shape2);

        auto contactEntity1 = AddCollisionBodyContacts(body1);
        SetCollisionContactShape(contactEntity1, shape1);
        SetCollisionContactOtherShape(contactEntity1, shape2);

        auto contactEntity2 = AddCollisionBodyContacts(body2);
        SetCollisionContactShape(contactEntity2, shape2);
        SetCollisionContactOtherShape(contactEntity2, shape1);

        contactEntityMapping[contact] = eastl::make_pair(contactEntity1, contactEntity2);
    }

    virtual void EndContact(b2Contact *contact) {
        b2ContactListener::EndContact(contact);

        auto& mapping = contactEntityMapping[contact];
        RemoveCollisionBodyContactsByValue(GetOwner(mapping.first), mapping.first);
        RemoveCollisionBodyContactsByValue(GetOwner(mapping.second), mapping.second);
        contactEntityMapping.erase(contact);
    }
};

static void UpdateBody(Entity body) {
    auto data = GetBox2DCollisionBodyData(body);
    auto bodyData = GetCollisionBodyData(body);
    auto transformData = GetTransformData(body);

    if(!data || !data->Body) return;

    switch(bodyData->CollisionBodyType) {
        case CollisionBodyType_Kinematic:
            data->Body->SetType(b2_kinematicBody);
            break;
        case CollisionBodyType_Dynamic:
            data->Body->SetType(b2_dynamicBody);
            break;
        default:
            data->Body->SetType(b2_staticBody);
            break;
    }
    data->Body->SetAwake(!bodyData->CollisionBodySleeping);
    data->Body->SetSleepingAllowed(!bodyData->CollisionBodyForbidSleep);
    data->Body->SetTransform({transformData->Position2D.x, transformData->Position2D.y}, transformData->RotationEuler3D.z);
    data->Body->SetBullet(bodyData->CollisionBodyCCD);
    data->Body->SetFixedRotation(bodyData->CollisionBodyFixedRotation);
    data->Body->SetGravityScale(bodyData->CollisionBodyGravityScale);
    data->Body->SetAngularDamping(bodyData->CollisionBodyAngularDamping);
    data->Body->SetLinearDamping(bodyData->CollisionBodyLinearDamping);
}

static void AddFixture(b2Body *body, Entity shape, bool isSensor) {
    auto shapeData = GetCollisionShapeData(shape);

    b2FixtureDef fixtureDef;
    fixtureDef.density = shapeData->CollisionShapeDensity;
    fixtureDef.friction = shapeData->CollisionShapeFriction;
    fixtureDef.restitution = shapeData->CollisionShapeRestitution;
    fixtureDef.filter.categoryBits = shapeData->CollisionShapeGroup;
    fixtureDef.filter.maskBits = shapeData->CollisionShapeMask;
    fixtureDef.isSensor = isSensor;

    auto boxData = GetCollisionBoxShapeData(shape);
    auto sphereData = GetCollisionSphereShapeData(shape);

    b2Fixture *fixture = NULL;
    if(boxData) {
        b2PolygonShape shape;
        shape.SetAsBox(boxData->CollisionShapeExtent.x, boxData->CollisionShapeExtent.y, {shapeData->CollisionShapePositionOffset.x, shapeData->CollisionShapePositionOffset.y}, shapeData->CollisionShapeRotationEulerOffset.z);
        fixtureDef.shape = &shape;
        fixture = body->CreateFixture(&fixtureDef);
    }
    else if(sphereData) {
        b2CircleShape shape;
        shape.m_p = {shapeData->CollisionShapePositionOffset.x, shapeData->CollisionShapePositionOffset.y};
        shape.m_radius = sphereData->CollisionShapeRadius;

        fixtureDef.shape = &shape;
        fixture = body->CreateFixture(&fixtureDef);
    }

    if(fixture) {
        fixture->SetUserData((void*)(size_t)GetEntityIndex(shape));
    }
}

static void BuildBody(Entity body) {
    auto scene = GetSceneNodeScene(body);
    auto sceneData = GetBox2DCollisionSceneData(scene);

    if(!sceneData) return;

    auto data = GetBox2DCollisionBodyData(body);

    if(data->Body) {
        sceneData->World->DestroyBody(data->Body);
        data->Body = NULL;
    }

    b2BodyDef bodyDef;
    data->Body = sceneData->World->CreateBody(&bodyDef);
    data->Body->SetUserData((void*)(size_t)GetEntityIndex(body));

    UpdateBody(body);

    auto isSensor = GetCollisionBodyType(body) == CollisionBodyType_Overlap;

    for_children(shape, CollisionBodyShapes, body) {
        AddFixture(data->Body, shape, isSensor);
    }
}

LocalFunction(OnSceneAdded, void, Entity component, Entity entity) {
    auto data = GetBox2DCollisionSceneData(entity);
    auto gravity = GetCollisionSceneGravity(entity);
    data->World = new b2World({gravity.x, gravity.y});
    data->World->SetContactListener(new ContactListener());
}

LocalFunction(OnSceneChanged, void, Entity entity) {
    auto data = GetBox2DCollisionSceneData(entity);

    if(!data) return;

    auto gravity = GetCollisionSceneGravity(entity);
    data->World->SetGravity({gravity.x, gravity.y});
}

LocalFunction(OnSceneRemoved, void, Entity component, Entity entity) {
    auto data = GetBox2DCollisionSceneData(entity);
    delete data->World;
}

LocalFunction(OnBodyAdded, void, Entity component, Entity entity) {
    BuildBody(entity);
}

LocalFunction(OnBodyRemoved, void, Entity component, Entity entity) {
    auto data = GetBox2DCollisionBodyData(entity);

    if(!data->Body) return;

    data->Body->GetWorld()->DestroyBody(data->Body);
}

LocalFunction(OnGravityChanged, void, Entity entity, v3f oldValue, v3f newValue) {
    auto data = GetBox2DCollisionSceneData(entity);
    data->World->SetGravity({newValue.x, newValue.y});
}

LocalFunction(OnShapeChanged, void, Entity entity) {
    auto body = GetOwner(entity);
    BuildBody(body);
}

LocalFunction(OnBodyChanged, void, Entity entity) {
    UpdateBody(entity);
}

LocalFunction(OnAngularVelocityChanged, void, Entity entity) {
    auto data = GetBox2DCollisionBodyData(entity);

    if(!isUpdatingTransforms && data && data->Body) {
        data->Body->SetAngularVelocity(GetCollisionBodyAngularVelocity(entity).z);
    }
}

LocalFunction(OnLinearVelocityChanged, void, Entity entity) {
    auto data = GetBox2DCollisionBodyData(entity);

    if(!isUpdatingTransforms && data && data->Body) {
        auto vel = GetCollisionBodyLinearVelocity(entity);
        data->Body->SetLinearVelocity({vel.x, vel.y});
    }
}

LocalFunction(OnSceneNodeSceneChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_CollisionBody())) {
        BuildBody(entity);
    }
}

LocalFunction(OnBodyInvalidated, void, Entity entity) {
    BuildBody(entity);
}

LocalFunction(OnImpulse, void, Entity entity, bool oldVal, bool newVal) {
    if(!newVal) return;

    auto impulseData = GetCollisionImpulseData(entity);
    if(!impulseData) return;

    auto bodyData = GetBox2DCollisionBodyData(impulseData->CollisionImpulseBody);
    if(!bodyData || !bodyData->Body) return;

    bodyData->Body->ApplyLinearImpulseToCenter({impulseData->CollisionImpulseDirection.x * impulseData->CollisionImpulseFactor, impulseData->CollisionImpulseDirection.y * impulseData->CollisionImpulseFactor}, true);
}

static void UpdateBody(Entity body, Box2DCollisionBody *bodyData) {
    auto updatedLinearVelocity = bodyData->Body->GetLinearVelocity();
    auto updatedAngularVelocity = bodyData->Body->GetAngularVelocity();

    SetCollisionBodyAngularVelocity(body, {0.0f, 0.0f, updatedAngularVelocity});
    SetCollisionBodyLinearVelocity(body, {updatedLinearVelocity.x, updatedLinearVelocity.y, 0.0f});

    auto transform = bodyData->Body->GetTransform();
    SetPosition2D(body, {transform.p.x, transform.p.y});
    SetRotation2D(body, transform.q.GetAngle());
}

LocalFunction(OnUpdate, void) {
    auto deltaTime = GetStopWatchElapsedSeconds(StopWatchOf_Box2D());
    SetStopWatchElapsedSeconds(StopWatchOf_Box2D(), 0.0);

    for_entity(force, forceData, CollisionForce) {
        auto bodyData = GetBox2DCollisionBodyData(forceData->CollisionForceBody);
        if(!bodyData || !bodyData->Body) continue;

        auto factor = forceData->CollisionForceFactor * (float)deltaTime;
        bodyData->Body->ApplyLinearImpulseToCenter({forceData->CollisionForceDirection.x * factor, forceData->CollisionForceDirection.y * factor}, true);
    }

    for_entity(scene, sceneData, Box2DCollisionScene) {
        auto velocityIterations = 6;
        auto positionIterations = 2;

        sceneData->World->Step(deltaTime, velocityIterations, positionIterations);
    }

    isUpdatingTransforms = true;
    for_entity(body, bodyData, Box2DCollisionBody) {
        if(!bodyData->Body) continue;

        UpdateBody(body, bodyData);
    }

    isUpdatingTransforms = false;
}

BeginModule(Box2DCollision)
    RegisterDependency(Collision)

    RegisterUnit(Box2DCollision)
EndModule()


BeginUnit(Box2DCollision)
    BeginComponent(Box2DCollisionBody)
        RegisterBase(CollisionBody)
        RegisterPropertyReadOnly(NativePtr, Body)
    EndComponent()

    BeginComponent(Box2DCollisionScene)
        RegisterBase(CollisionScene)
        RegisterPropertyReadOnly(NativePtr, World)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyShapes()), OnBodyInvalidated, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyForbidSleep()), OnBodyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyAngularDamping()), OnBodyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodySleeping()), OnBodyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyCCD()), OnBodyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyFixedRotation()), OnBodyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyGravityScale()), OnBodyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyLinearDamping()), OnBodyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyType()), OnBodyChanged, 0)
    //RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TransformLocalMatrix()), OnBodyChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyLinearVelocity()), OnLinearVelocityChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionBodyAngularVelocity()), OnAngularVelocityChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeDensity()), OnShapeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeRestitution()), OnShapeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeFriction()), OnShapeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeGroup()), OnShapeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeMask()), OnShapeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapePositionOffset()), OnShapeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeRotationEulerOffset()), OnShapeChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeExtent()), OnShapeChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionShapeRadius()), OnShapeChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionSceneGravity()), OnSceneChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CollisionImpulseTrigger()), OnImpulse, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_SceneNodeScene()), OnSceneNodeSceneChanged, 0)

    RegisterSubscription(EventOf_EntityComponentAdded(), OnBodyAdded, ComponentOf_Box2DCollisionBody())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnBodyRemoved, ComponentOf_Box2DCollisionBody())
    RegisterSubscription(EventOf_EntityComponentAdded(), OnSceneAdded, ComponentOf_Box2DCollisionScene())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnSceneRemoved, ComponentOf_Box2DCollisionScene())

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnUpdate, AppLoopOf_Box2D())

    SetAppLoopOrder(AppLoopOf_Box2D(), AppLoopOrder_Update * 1.05f);
    SetStopWatchRunning(StopWatchOf_Box2D(), true);

    auto ext = AddExtensions(module);
    SetExtensionComponent(ext, ComponentOf_CollisionScene());
    SetExtensionExtenderComponent(ext, ComponentOf_Box2DCollisionScene());

    ext = AddExtensions(module);
    SetExtensionComponent(ext, ComponentOf_CollisionBody());
    SetExtensionExtenderComponent(ext, ComponentOf_Box2DCollisionBody());
EndUnit()