//
// Created by Kim Johannsen on 26/01/2018.
//

#include <Core/Property.h>
#include <Core/Entity.h>
#include <File/Folder.h>
#include <ImGui/ImGuiControls.h>
#include <Core/Hierarchy.h>
#include <ImGui/ImGuiRenderer.h>
#include <Core/Module.h>
#include <Core/Types.h>
#include <climits>
#include <Core/Dictionary.h>
#include <Core/String.h>
#include <Editor/EditorView.h>
#include "PropertyEditor.h"
#include "Editor/Selection.h"

#define DefinePrimitiveImGuiDrawer(TYPE, INTERMEDIATETYPE, DRAWFUNC) \
    static void Draw ## TYPE(Property property) {\
        char id[128];\
        sprintf(id, "##%llu", property);\
        typedef TYPE(*Getter)(Entity entity);\
        typedef void(*Setter)(Entity entity, TYPE value);\
        auto actualValue = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));\
        INTERMEDIATETYPE value = (INTERMEDIATETYPE)actualValue;\
        if(DRAWFUNC) {\
            for(auto i = 0; i < GetNumSelection(); ++i) {\
                ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), (TYPE)value);\
            }\
        }\
    }

#define DefinePrimitiveImGuiDrawerEnum(TYPE, INTERMEDIATETYPE, DRAWFUNC) \
    static void Draw ## TYPE(Property property) {\
        char id[128];\
        sprintf(id, "##%llu", property);\
        typedef TYPE(*Getter)(Entity entity);\
        typedef void(*Setter)(Entity entity, TYPE value);\
        auto actualValue = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));\
        INTERMEDIATETYPE value = (INTERMEDIATETYPE)actualValue;\
        u64 enumValue = (u64)actualValue;\
        auto e = GetPropertyEnum(property);\
        if(IsEnumValid(e)) {\
            if(GetEnumCombinable(e)) {\
                DrawFlag(property, e, &enumValue);\
            } else {\
                DrawEnum(property, e, &enumValue);\
            }\
            if(actualValue != enumValue) {\
                for(auto i = 0; i < GetNumSelection(); ++i) {\
                    ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), (TYPE)enumValue);\
                }\
            }\
        } else {\
            if(DRAWFUNC) {\
                for(auto i = 0; i < GetNumSelection(); ++i) {\
                    ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), (TYPE)value);\
                }\
            }\
        }\
    }

#define HandleDraw(TYPE) \
    if(propertyType == TypeOf_ ## TYPE ()) {\
        Draw ## TYPE (property);\
    } do {} while(false)


struct PropertyEditor {

};


DefineComponent(PropertyEditor)
    Dependency(EditorView)
EndComponent()

static Lookup<Type, bool> SectionOpened;

static bool Visible = true;
static const char *ComponentContextMenuId = "ComponentContextMenu";

bool GetPropertyEditorVisible() {
    return Visible;
}

void SetPropertyEditorVisible(bool value) {
    Visible = value;
}

static void DrawProperty(Property p) {
    typedef Property(*Getter)(Entity entity);\
    typedef void(*Setter)(Entity entity, Property value);\
    auto value = ((Getter)GetPropertyGetter(p))(GetSelectionEntity(0));

    static Vector<StringRef> names;
    static Vector<Property> elements;
    names.clear();
    elements.clear();

    names.push_back("<None>");
    elements.push_back(0);

    char id[128];\
    sprintf(id, "##%llu", p);\

    int currentIndex = 0;
    for(auto cp = GetNextProperty(0); cp; cp = GetNextProperty(cp)) {
        names.push_back(GetPropertyName(cp));
        elements.push_back(cp);

        if(value == cp) {
            currentIndex = elements.size() - 1;
        }
    }

    if(ImGui::Combo(id, &currentIndex, names.data(), names.size())) {
        for(auto i = 0; i < GetNumSelection(); ++i) {
            ((Setter)GetPropertySetter(p))(GetSelectionEntity(i), elements[currentIndex]);
        }
    }
}
static void DrawType(Property p) {
    typedef Type(*Getter)(Entity entity);\
    typedef void(*Setter)(Entity entity, Type value);\
    auto value = ((Getter)GetPropertyGetter(p))(GetSelectionEntity(0));

    static Vector<StringRef> names;
    static Vector<Type> elements;
    names.clear();
    elements.clear();

    names.push_back("<None>");
    elements.push_back(0);

    char id[128];\
    sprintf(id, "##%llu", p);\

    int currentIndex = 0;
    for(auto cp = GetNextType(0); cp; cp = GetNextType(cp)) {
        names.push_back(GetTypeName(cp));
        elements.push_back(cp);

        if(value == cp) {
            currentIndex = elements.size() - 1;
        }
    }

    if(ImGui::Combo(id, &currentIndex, names.data(), names.size())) {
        for(auto i = 0; i < GetNumSelection(); ++i) {
            ((Setter)GetPropertySetter(p))(GetSelectionEntity(i), elements[currentIndex]);
        }
    }
}
static void DrawEnum(Property p, Enum e, u64 *value) {
    Vector<StringRef> elements;

    char id[512];\
    sprintf(id, "##%llu", p);\

    auto numFlags = GetEnumFlags(e);
    int flagIndex = 0;
    for(auto i = 0; i < numFlags; ++i) {
        if(GetEnumFlagValue(e, i) == *value) {
            flagIndex = i;
        }

        elements.push_back(GetEnumFlagName(e, i));
    }

    if(ImGui::Combo(id, &flagIndex, elements.data(), elements.size())) {

        *value = GetEnumFlagValue(e, flagIndex);
    }
}
static void DrawFlag(Property p, Enum e, u64 *value) {
    Vector<StringRef> elements;

    auto numFlags = GetEnumFlags(e);
    int flagIndex = 0;
    for(auto i = 0; i < numFlags; ++i) {
        char id[128];\
        sprintf(id, "%s##%llu%d", GetEnumFlagName(e, i), p, i);\

        auto flagValue = GetEnumFlagValue(e, i);
        auto intValue = *value;

        bool isSet = intValue & flagValue;
        bool pressed = ImGui::Checkbox(id, &isSet);
        if (pressed)
        {
            if (isSet)
                intValue |= flagValue;
            else
                intValue &= ~flagValue;

            *value = intValue;
        }
    }
}
static void DrawEntity(Property property) {
    typedef Entity(*Getter)(Entity entity);
    typedef void(*Setter)(Entity entity, Entity value);
    auto value = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));

    char id[512];

    if(IsEntityValid(value)) {
        if(HasHierarchy(value)) {
            snprintf(id, 512, "%s", GetEntityPath(value));
        } else {
            snprintf(id, 512, "Entity_%u", GetHandleIndex(value));
        }
    } else {
        sprintf(id, "<None>");
    }

    ImGui::PushID(property);
    ImGui::Text("%s", id);
    ImGui::PopID();

    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetTypeName(TypeOf_Entity()))) {
            if(payload->Delivery) {
                for(auto i = 0; i < GetNumSelection(); ++i) {
                    ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), *((Entity*)payload->Data));
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
}
static void Drawm4x4f(Property property) {
    char idx[32], idy[32], idz[32], idw[32];
    sprintf(idx, "##%llu_x", property);
    sprintf(idy, "##%llu_y", property);
    sprintf(idz, "##%llu_z", property);
    sprintf(idw, "##%llu_w", property);

    typedef m4x4f(*Getter)(Entity entity);
    typedef void(*Setter)(Entity entity, m4x4f value);
    auto value = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));
    if(ImGui::InputFloat4(idx, &value.x.x)
       || ImGui::InputFloat4(idy, &value.y.x)
       || ImGui::InputFloat4(idz, &value.z.x)
       || ImGui::InputFloat4(idw, &value.w.x)) {

        for(auto i = 0; i < GetNumSelection(); ++i) {
            ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), value);
        }
    }
}
static void DrawStringRef(Property property) {
    char id[128];
    char buf[PATH_MAX];
    sprintf(id, "##%llu", property);
    typedef StringRef(*Getter)(Entity entity);
    typedef void(*Setter)(Entity entity, StringRef value);
    strcpy(buf, ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0)));
    if(ImGui::InputText(id, buf, PATH_MAX, ImGuiInputTextFlags_EnterReturnsTrue)) {
        for(auto i = 0; i < GetNumSelection(); ++i) {
            ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), buf);
        }
    }
}
static void Drawrgba8(Property property) {
    char id[128];
    sprintf(id, "##%llu", property);
    typedef rgba8(*Getter)(Entity entity);
    typedef void(*Setter)(Entity entity, rgba8 value);
    auto v = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));
    rgba32 rgba = { (float)v.r / 255.0f, (float)v.g / 255.0f, (float)v.b / 255.0f, (float)v.a / 255.0f };
    if(ImGui::ColorEdit4(id, &rgba.r)) {
        for(auto i = 0; i < GetNumSelection(); ++i) {
            ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), {(u8)(rgba.r * 255.0f), (u8)(rgba.g * 255.0f), (u8)(rgba.b * 255.0f), (u8)(rgba.a * 255.0f)});
        }
    }
}
static void Drawrgb8(Property property) {
    char id[128];
    sprintf(id, "##%llu", property);
    typedef rgba8(*Getter)(Entity entity);
    typedef void(*Setter)(Entity entity, rgba8 value);
    auto v = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));
    rgb32 rgba = { (float)v.r / 255.0f, (float)v.g / 255.0f, (float)v.b / 255.0f };

    if(ImGui::ColorEdit3(id, &rgba.r)) {
        for(auto i = 0; i < GetNumSelection(); ++i) {
            ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), {(u8)(rgba.r * 255.0f), (u8)(rgba.g * 255.0f), (u8)(rgba.b * 255.0f) });
        }
    }
}

DefinePrimitiveImGuiDrawer(bool, bool, ImGui::Checkbox(id, &value))
DefinePrimitiveImGuiDrawer(float, float, ImGui::InputFloat(id, &value, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawer(double, float, ImGui::InputFloat(id, &value, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawerEnum(u8, int, ImGui::SliderInt(id, &value, 0, UINT8_MAX))
DefinePrimitiveImGuiDrawerEnum(u16, int, ImGui::SliderInt(id, &value, 0, UINT16_MAX))
DefinePrimitiveImGuiDrawerEnum(u32, int, ImGui::InputInt(id, &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawerEnum(u64, int, ImGui::InputInt(id, &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawer(s8, int, ImGui::SliderInt(id, &value, INT8_MIN, INT8_MAX))
DefinePrimitiveImGuiDrawer(s16, int, ImGui::SliderInt(id, &value, INT16_MIN, INT16_MAX))
DefinePrimitiveImGuiDrawer(s32, int, ImGui::InputInt(id, &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawer(s64, int, ImGui::InputInt(id, &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))

DefinePrimitiveImGuiDrawer(v2f, v2f, ImGui::InputFloat2(id, &value.x, -1, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawer(v3f, v3f, ImGui::InputFloat3(id, &value.x, -1, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawer(v4f, v4f, ImGui::InputFloat4(id, &value.x, -1, ImGuiInputTextFlags_EnterReturnsTrue))

DefinePrimitiveImGuiDrawer(v2i, v2i, ImGui::InputInt2(id, &value.x, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawer(v3i, v3i, ImGui::InputInt3(id, &value.x, ImGuiInputTextFlags_EnterReturnsTrue))
DefinePrimitiveImGuiDrawer(v4i, v4i, ImGui::InputInt4(id, &value.x, ImGuiInputTextFlags_EnterReturnsTrue))

DefinePrimitiveImGuiDrawer(rgba32, rgba32, ImGui::ColorEdit4(id, &value.r))
DefinePrimitiveImGuiDrawer(rgb32, rgb32, ImGui::ColorEdit3(id, &value.r))

static void Draw(Entity view) {
    for(auto type = GetNextType(0); IsTypeValid(type); type = GetNextType(type)) {
        bool showType = false;
        for(auto i = 0; i < GetNumSelection(); ++i) {
            if(HasComponent(GetSelectionEntity(i), type)) {
                showType = true;
            }
        }

        if(!showType) continue;

        if(ImGui::CollapsingHeader(GetTypeName(type), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Columns(2, GetTypeName(type));

            for(auto property = GetNextProperty(0); IsPropertyValid(property); property = GetNextProperty(property)) {
                if(GetPropertyOwner(property) != type) {
                    continue;
                }

                auto propertyType = GetPropertyType(property);

                ImGui::Text("%s", GetPropertyName(property));
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());

                HandleDraw(float);
                HandleDraw(double);
                HandleDraw(bool);
                HandleDraw(StringRef);
                HandleDraw(u8);
                HandleDraw(u16);
                HandleDraw(u32);
                HandleDraw(u64);
                HandleDraw(s8);
                HandleDraw(s16);
                HandleDraw(s32);
                HandleDraw(s64);
                HandleDraw(v2f);
                HandleDraw(v3f);
                HandleDraw(v4f);
                HandleDraw(v2i);
                HandleDraw(v3i);
                HandleDraw(v4i);
                HandleDraw(m4x4f);
                HandleDraw(rgba8);
                HandleDraw(rgba32);
                HandleDraw(rgb8);
                HandleDraw(rgb32);
                HandleDraw(Entity);
                HandleDraw(Type);
                HandleDraw(Property);

                ImGui::PopItemWidth();

                ImGui::NextColumn();
            }

            ImGui::Columns(1);
        }
    }

    if (ImGui::BeginPopup(ComponentContextMenuId)) {
        //ImGui::ComponentContextMenu();
        ImGui::EndPopup();
    }
}

void ImGui::ComponentContextMenu(Type componentType) {
    if(GetNumSelection() > 0) {
        if(ImGui::MenuItem("Remove")) {
            for(auto i = 0; i < GetNumSelection(); ++i) {
                RemoveComponent(GetSelectionEntity(i), componentType);
            }
        }

        if(ImGui::BeginMenu("Add")) {
            for(auto module = GetNextModule(0); IsModuleValid(module); module = GetNextModule(module)) {
                if(ImGui::BeginMenu(GetModuleName(module))) {
                    for(auto i = 0; i < GetModuleTypes(module); ++i) {
                        auto type = GetModuleType(module, i);
                        if(IsComponent(type)) {
                            if(ImGui::MenuItem(GetTypeName(type))) {
                                for(auto i = 0; i < GetNumSelection(); ++i) {
                                    AddComponent(GetSelectionEntity(i), type);
                                }
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
            }

            ImGui::EndMenu();
        }
    }
}

static void OnPropertyEditorAdded(Entity entity) {
    SetEditorViewDrawFunction(entity, Draw);
}

DefineService(PropertyEditor)
        Subscribe(PropertyEditorAdded, OnPropertyEditorAdded)
EndService()