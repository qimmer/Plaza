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
    struct b2Body *Body;
};

struct Box2DCollisionScene {
    struct b2World *World;
};


class ContactListener : public b2ContactListener {
    eastl::unordered_map<b2Contact*, eastl::pair<Entity, Entity>> contactEntityMapping;
public:
    virtual void BeginContact(b2Contact *contact) {
        b2ContactListener::BeginContact(contact);

        auto shape1 = GetEntityByIndex((u32)(size_t)contact->GetFixtureA()->GetUserData());
        auto shape2 = GetEntityByIndex((u32)(size_t)contact->GetFixtureB()->GetUserData());

        auto body1 = GetOwnership(shape1).Owner;
        auto body2 = GetOwnership(shape2).Owner;
        auto body1Data = GetCollisionBody(body1);
        auto body2Data = GetCollisionBody(body2);

        auto contactEntity1 = CreateEntity();
        auto contactEntity2 = CreateEntity();

        body1Data.CollisionBodyContacts.Add(contactEntity1);
        body2Data.CollisionBodyContacts.Add(contactEntity2);

        SetCollisionBody(body1, body1Data);
        SetCollisionBody(body2, body2Data);

        SetCollisionContact(contactEntity1, {shape1, shape2});
        SetCollisionContact(contactEntity2, {shape2, shape1});

        contactEntityMapping[contact] = eastl::make_pair(contactEntity1, contactEntity2);
    }

    virtual void EndContact(b2Contact *contact) {
        b2ContactListener::EndContact(contact);

        auto& mapping = contactEntityMapping[contact];

        auto body1 = GetOwnership(mapping.first).Owner;
        auto body2 = GetOwnership(mapping.second).Owner;
        auto body1Data = GetCollisionBody(body1);
        auto body2Data = GetCollisionBody(body2);

        body1Data.CollisionBodyContacts.Remove(body1Data.CollisionBodyContacts.GetIndex(mapping.first));
        body2Data.CollisionBodyContacts.Remove(body2Data.CollisionBodyContacts.GetIndex(mapping.second));

        SetCollisionBody(body1, body1Data);
        SetCollisionBody(body2, body2Data);

        contactEntityMapping.erase(contact);
    }
};

static void UpdateBody(Entity body) {
    auto data = GetBox2DCollisionBody(body);
    auto bodyData = GetCollisionBody(body);
    auto transformData = GetTransform(body);

    if(!data.Body) return;

    switch(bodyData.CollisionBodyType) {
        case CollisionBodyType_Kinematic:
            data.Body->SetType(b2_kinematicBody);
            break;
        case CollisionBodyType_Dynamic:
            data.Body->SetType(b2_dynamicBody);
            break;
        default:
            data.Body->SetType(b2_staticBody);
            break;
    }
    data.Body->SetAwake(!bodyData.CollisionBodySleeping);
    data.Body->SetSleepingAllowed(!bodyData.CollisionBodyForbidSleep);
    data.Body->SetTransform({transformData.Position2D.x, transformData.Position2D.y}, transformData.RotationEuler3D.z);
    data.Body->SetBullet(bodyData.CollisionBodyCCD);
    data.Body->SetFixedRotation(bodyData.CollisionBodyFixedRotation);
    data.Body->SetGravityScale(bodyData.CollisionBodyGravityScale);
    data.Body->SetAngularDamping(bodyData.CollisionBodyAngularDamping);
    data.Body->SetLinearDamping(bodyData.CollisionBodyLinearDamping);
    data.Body->SetAngularVelocity(bodyData.CollisionBodyAngularVelocity.z);
    data.Body->SetLinearVelocity({bodyData.CollisionBodyLinearVelocity.x, bodyData.CollisionBodyLinearVelocity.y});
}

static void AddFixture(b2Body *body, Entity shape, bool isSensor) {
    auto shapeData = GetCollisionShape(shape);

    b2FixtureDef fixtureDef;
    fixtureDef.density = shapeData.CollisionShapeDensity;
    fixtureDef.friction = shapeData.CollisionShapeFriction;
    fixtureDef.restitution = shapeData.CollisionShapeRestitution;
    fixtureDef.filter.categoryBits = shapeData.CollisionShapeGroup;
    fixtureDef.filter.maskBits = shapeData.CollisionShapeMask;
    fixtureDef.isSensor = isSensor;

    auto boxData = GetCollisionBoxShape(shape);
    auto sphereData = GetCollisionSphereShape(shape);

    b2Fixture *fixture = NULL;
    if(HasComponent(shape, ComponentOf_CollisionBoxShape())) {
        b2PolygonShape shape;
        shape.SetAsBox(boxData.CollisionShapeExtent.x, boxData.CollisionShapeExtent.y, {shapeData.CollisionShapePositionOffset.x, shapeData.CollisionShapePositionOffset.y}, shapeData.CollisionShapeRotationEulerOffset.z);
        fixtureDef.shape = &shape;
        fixture = body->CreateFixture(&fixtureDef);
    }
    else if(HasComponent(shape, ComponentOf_CollisionSphereShape())) {
        b2CircleShape shape;
        shape.m_p = {shapeData.CollisionShapePositionOffset.x, shapeData.CollisionShapePositionOffset.y};
        shape.m_radius = sphereData.CollisionShapeRadius;

        fixtureDef.shape = &shape;
        fixture = body->CreateFixture(&fixtureDef);
    }

    if(fixture) {
        fixture->SetUserData((void*)(size_t)GetEntityIndex(shape));
    }
}

static void BuildBody(Entity body) {
    auto scene = GetSceneNode(body).SceneNodeScene;
    auto sceneData = GetBox2DCollisionScene(scene);
    auto data = GetBox2DCollisionBody(body);

    if(data.Body) {
        sceneData.World->DestroyBody(data.Body);
        data.Body = NULL;
    }

    b2BodyDef bodyDef;
    data.Body = sceneData.World->CreateBody(&bodyDef);
    data.Body->SetUserData((void*)(size_t)GetEntityIndex(body));
    SetBox2DCollisionBody(body, data);

    UpdateBody(body);

    auto isSensor = GetCollisionBody(body).CollisionBodyType == CollisionBodyType_Overlap;

    auto bodyData = GetCollisionBody(body);
    for(auto shape : bodyData.CollisionBodyShapes) {
        AddFixture(data.Body, shape, isSensor);
    }
}

static void OnBox2DCollisionSceneChanged(Entity entity, const Box2DCollisionScene& oldData, const Box2DCollisionScene& newData) {
    if(oldData.World && !newData.World) {
        delete oldData.World;
    }

    if(!oldData.World && !newData.World) {
        auto data = newData;
        auto gravity = GetCollisionScene(entity).CollisionSceneGravity;
        data.World = new b2World({gravity.x, gravity.y});
        data.World->SetContactListener(new ContactListener());
        SetBox2DCollisionScene(entity, data);
    }
}

static void OnCollisionSceneChanged(Entity entity, const CollisionScene& oldData, const CollisionScene& newData) {
    if(HasComponent(entity, ComponentOf_Box2DCollisionScene())) {
        auto data = GetBox2DCollisionScene(entity);
        auto gravity = newData.CollisionSceneGravity;
        data.World->SetGravity({gravity.x, gravity.y});
        SetBox2DCollisionScene(entity, data);
    }
}

static void OnCollisionBodyChanged(Entity entity, const CollisionBody& oldData, const CollisionBody& newData) {
    if(isUpdatingTransforms) return;

    if(oldData.CollisionBodyType != newData.CollisionBodyType) {
        BuildBody(entity);
    } else {
        UpdateBody(entity);
    }

}

static void OnBox2DCollisionBodyChanged(Entity entity, const Box2DCollisionBody& oldData, const Box2DCollisionBody& newData) {
    if(oldData.Body && !newData.Body) {
        oldData.Body->GetWorld()->DestroyBody(oldData.Body);
    }
}

static void OnCollisionShapeChanged(Entity entity) {
    auto body = GetOwnership(entity).Owner;
    BuildBody(body);
}

static void OnSceneNodeChanged(Entity entity, const SceneNode& oldData, const SceneNode& newData) {
    if(oldData.SceneNodeScene != newData.SceneNodeScene && HasComponent(entity, ComponentOf_CollisionBody())) {
        BuildBody(entity);
    }
}

static void OnCollisionImpulseChanged(Entity entity, const CollisionImpulse& oldData, const CollisionImpulse& newData) {
    if(!oldData.CollisionImpulseTrigger && newData.CollisionImpulseTrigger) {
        auto bodyData = GetBox2DCollisionBody(newData.CollisionImpulseBody);
        if(!bodyData.Body) return;

        bodyData.Body->ApplyLinearImpulseToCenter({newData.CollisionImpulseDirection.x * newData.CollisionImpulseFactor, newData.CollisionImpulseDirection.y * newData.CollisionImpulseFactor}, true);
    }
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    CollisionForce forceData;
    for_entity_data(force, ComponentOf_CollisionForce(), &forceData) {
        auto bodyData = GetBox2DCollisionBody(forceData.CollisionForceBody);
        if(!bodyData.Body) continue;

        auto factor = forceData.CollisionForceFactor * (float)newData.AppLoopDeltaTime;
        bodyData.Body->ApplyLinearImpulseToCenter({forceData.CollisionForceDirection.x * factor, forceData.CollisionForceDirection.y * factor}, true);
    }

    Box2DCollisionScene sceneData;
    for_entity_data(scene, ComponentOf_Box2DCollisionScene(), &sceneData) {
        auto velocityIterations = 6;
        auto positionIterations = 2;

        sceneData.World->Step(newData.AppLoopDeltaTime, velocityIterations, positionIterations);
    }

    isUpdatingTransforms = true;
    Box2DCollisionBody bodyData;
    for_entity_data(body, ComponentOf_Box2DCollisionBody(), &bodyData) {
        if(!bodyData.Body) continue;

        auto updatedLinearVelocity = bodyData.Body->GetLinearVelocity();
        auto updatedAngularVelocity = bodyData.Body->GetAngularVelocity();

        auto collisionBodyData = GetCollisionBody(body);
        collisionBodyData.CollisionBodyAngularVelocity = {0.0f, 0.0f, updatedAngularVelocity};
        collisionBodyData.CollisionBodyLinearVelocity = {updatedLinearVelocity.x, updatedLinearVelocity.y, 0.0f};
        SetCollisionBody(body, collisionBodyData);

        auto transform = bodyData.Body->GetTransform();
        auto transformData = GetTransform(body);
        transformData.Position2D = {transform.p.x, transform.p.y};
        transformData.Rotation2D = transform.q.GetAngle();
        SetTransform(body, transformData);
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

    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Update * 1.05f)
    RegisterSystem(OnCollisionImpulseChanged, ComponentOf_CollisionImpulse())
    RegisterSystem(OnSceneNodeChanged, ComponentOf_SceneNode())
    RegisterSystem(OnCollisionShapeChanged, ComponentOf_CollisionShape())
    RegisterSystem(OnBox2DCollisionBodyChanged, ComponentOf_Box2DCollisionBody())
    RegisterSystem(OnCollisionBodyChanged, ComponentOf_CollisionBody())
    RegisterSystem(OnCollisionSceneChanged, ComponentOf_CollisionScene())
    RegisterSystem(OnBox2DCollisionSceneChanged, ComponentOf_Box2DCollisionScene())

    auto ext = CreateEntity();
    SetExtension(ext, {ComponentOf_CollisionScene(), ComponentOf_Box2DCollisionScene(), true});
    moduleData.Extensions.Add(ext);

    ext = CreateEntity();
    SetExtension(ext, {ComponentOf_CollisionBody(), ComponentOf_Box2DCollisionBody(), true});
    moduleData.Extensions.Add(ext);
EndUnit()