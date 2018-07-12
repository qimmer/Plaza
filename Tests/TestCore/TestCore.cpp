//
// Created by Kim on 24/06/2018.
//

#include <Core/Module.h>
#include <Core/CoreModule.h>
#include <Core/NativeUtils.h>
#include <Core/Entity.h>
#include <Core/Component.h>
#include <Test/Test.h>
#include "TestCore.h"

#include <cstring>
#include <Core/Math.h>

struct Person {
    char PersonFirstName[64];
    u8 PersonAge;
};

Test(TestEntity)
{
    Entity entities[] = {
        CreateEntity(),
        CreateEntity(),
        CreateEntity(),
        CreateEntity(),
        0
    };

    for(auto i = 0; i < 4; ++i)
    {
        Verify(IsEntityValid(entities[i]), "Entity should be valid");
    }

    DestroyEntity(entities[0]);
    Verify(!IsEntityValid(entities[0]), "Entity should be invalid");

    for(auto i = 0; entities[i]; ++i)
    {
        DestroyEntity(entities[i]);
    }

    return Success;
}

Test(TestComponent) {
    auto entity = CreateEntity();

    Verify(!HasComponent(ComponentOf_Person(), entity), "Should have no person");
    Verify(AddComponent(ComponentOf_Person(), entity), "Should have added person");
    Verify(HasComponent(ComponentOf_Person(), entity), "Should have person");
    Verify(GetPersonAge(entity) == 0, "Age should be 0 (default)");
    Verify(strcmp(GetPersonFirstName(entity), "") == 0, "FirstName should be empty (default)");

    SetPersonAge(entity, 45);
    SetPersonFirstName(entity, "John");

    Verify(GetPersonAge(entity) == 45, "Age should be 45");
    Verify(strcmp(GetPersonFirstName(entity), "John") == 0, "First Name should be John");

    auto personComponent = ComponentOf_Person();
    auto index = GetComponentIndex(ComponentOf_Component(), personComponent);

    DestroyEntity(entity);

    return Success;
}

static int ageDifference = 0;

LocalFunction(OnAgeChanged, void, Entity person, u8 oldValue, u8 newValue) {
    ageDifference = Abs(newValue - oldValue);
}

Test(TestEvent) {
    auto entity = CreateEntityFromName(0, "Person1");

    auto subscription = CreateEntity();
    SetSubscriptionEvent(subscription, EventOf_PersonAgeChanged());
    SetSubscriptionHandler(subscription, FunctionOf_OnAgeChanged());
    SetSubscriptionSender(subscription, 0);

    SetPersonAge(entity, 0);
    SetPersonAge(entity, 49);

    Verify(ageDifference == 49, "Event is not triggered on general subscription");

    SetSubscriptionSender(subscription, entity);
    SetPersonAge(entity, 50);

    Verify(ageDifference == 1, "Event is not triggered on entity specific subscription");

    SetSubscriptionSender(subscription, entity + 1);
    SetPersonAge(entity, 51);

    Verify(ageDifference == 1, "Event is triggered on wrong entity");

    DestroyEntity(entity);
    DestroyEntity(subscription);

    return Success;
}

BeginModule(TestCore)
    RegisterDependency(Core)
    RegisterUnit(TestCore)
EndModule()

BeginUnit(TestCore)
    BeginComponent(Person)
        RegisterProperty(u8, PersonAge)
        RegisterProperty(StringRef, PersonFirstName)
    EndComponent()

    RegisterTest(TestEntity)
    RegisterTest(TestComponent)
    RegisterTest(TestEvent)
EndUnit()
