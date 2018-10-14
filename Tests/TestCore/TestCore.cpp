//
// Created by Kim on 24/06/2018.
//

#include <Core/Module.h>
#include <Core/CoreModule.h>
#include <Core/NativeUtils.h>
#include <Core/Entity.h>
#include <Core/Component.h>
#include <Core/Instance.h>
#include <Core/Identification.h>
#include <Test/Test.h>
#include "TestCore.h"

#include <cstring>
#include <Core/Algorithms.h>
#include <Core/Vector.h>
#include <Test/TestModule.h>

struct TestCore {
    Entity FamilyWithNoChildren, FamilyWithTwoChildren, FamilyInstance;
};

struct Family {
};

struct Person {
    u8 PersonAge;
    Entity PersonPartner, PersonMother, PersonFather;
};

Test(TestInstance) {

    auto familyWithNoChildren = GetFamilyWithNoChildren(ModuleOf_TestCore());

    auto man = AddFamilyMembers(familyWithNoChildren); SetPersonAge(man, 25); SetName(man, "Finn");
    auto woman = AddFamilyMembers(familyWithNoChildren); SetPersonAge(woman, 22); SetName(woman, "Gerda");

    SetPersonPartner(man, woman);
    SetPersonPartner(woman, man);

    auto familyWithTwoChildren = GetFamilyWithTwoChildren(ModuleOf_TestCore());

    SetInstanceTemplate(familyWithTwoChildren, familyWithNoChildren);

    Verify(GetNumFamilyMembers(familyWithTwoChildren) == 2, "Wrong number of family members after instantiation.");

    auto father = GetFamilyMembers(familyWithTwoChildren)[0];
    auto mother = GetFamilyMembers(familyWithTwoChildren)[1];

    Verify(GetName(mother) == Intern("Gerda"), "Mother has wrong name.");
    Verify(GetName(father) == Intern("Finn"), "Father has wrong name.");

    SetName(man, "George");
    auto fatherName = GetName(father);
    Verify(fatherName == Intern("George"), "Father has wrong name after template name changed.");

    auto child1 = AddFamilyMembers(familyWithTwoChildren); SetPersonAge(child1, 5); SetName(child1, "Anders"); SetPersonMother(child1, mother);

    Verify(GetInstanceTemplate(familyWithTwoChildren) == familyWithNoChildren, "Wrong template set on familyWithTwoChildren");

    auto familyInstance = GetFamilyInstance(ModuleOf_TestCore());
    SetInstanceTemplate(familyInstance, familyWithTwoChildren);

    Verify(GetInstanceTemplate(familyInstance) == familyWithTwoChildren, "Wrong template set on familyInstance");
    Verify(GetNumFamilyMembers(familyWithTwoChildren) == 3, "Wrong number of family members. Should be 3 (father, mother, child1)");

    auto child2 = AddFamilyMembers(familyWithTwoChildren); SetPersonAge(child2, 7); SetName(child2, "Bonnie"); SetPersonMother(child2, mother);

    Verify(GetNumFamilyMembers(familyInstance) == 4, "Wrong number of family members on instance after adding 2nd child to template. Should be 4 (father, mother, child1, child2)");

    return Success;
}

static int ageDifference = 0;

LocalFunction(OnAgeChanged, void, Entity person, u8 oldValue, u8 newValue) {
    ageDifference = Abs(newValue - oldValue);
}

Test(TestEvent) {
    auto entity = GetFamilyWithNoChildren(ModuleOf_TestCore());

    auto event = GetPropertyChangedEvent(PropertyOf_PersonAge());

    auto subscription = AddSubscriptions(ModuleOf_TestCore());
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

    RemoveSubscriptions(ModuleOf_TestCore(), 0);

    return Success;
}

BeginModule(TestCore)
    RegisterDependency(Core)
    RegisterDependency(Test)
    RegisterUnit(TestCore)
EndModule()

BeginUnit(TestCore)
    BeginComponent(Person)
        RegisterProperty(u8, PersonAge)
        RegisterReferenceProperty(Person, PersonPartner)
        RegisterReferenceProperty(Person, PersonMother)
        RegisterReferenceProperty(Person, PersonFather)
    EndComponent()
    BeginComponent(Family)
        RegisterArrayProperty(Person, FamilyMembers)
    EndComponent()
    BeginComponent(TestCore)
        RegisterChildProperty(Family, FamilyWithNoChildren)
        RegisterChildProperty(Family, FamilyWithTwoChildren)
        RegisterChildProperty(Family, FamilyInstance)
    EndComponent()

    AddComponent(module, ComponentOf_TestCore());

    RegisterTest(TestInstance)
    RegisterTest(TestEvent)
EndUnit()
