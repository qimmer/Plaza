//
// Created by Kim Johannsen on 26/01/2018.
//

#include <Core/Entity.h>
#include <File/Folder.h>
#include <ImGui/ImGuiControls.h>
#include <Core/Hierarchy.h>
#include <ImGui/ImGuiRenderer.h>
#include <Core/Module.h>
#include <Core/Types.h>
#include <climits>
#include "PropertyEditor.h"
#include "Selection.h"


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

#define HandleDraw(TYPE) \
    if(propertyType == TypeOf_ ## TYPE ()) {\
        Draw ## TYPE (property);\
    } do {} while(false)


    DefineService(PropertyEditor)
    EndService()

    static bool Visible = true;
    static const char *ComponentContextMenuId = "ComponentContextMenu";

    bool GetPropertyEditorVisible() {
        return Visible;
    }

    void SetPropertyEditorVisible(bool value) {
        Visible = value;
    }

    DefinePrimitiveImGuiDrawer(bool, bool, ImGui::Checkbox(id, &value))
    DefinePrimitiveImGuiDrawer(float, float, ImGui::InputFloat(id, &value, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
    DefinePrimitiveImGuiDrawer(double, float, ImGui::InputFloat(id, &value, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
    DefinePrimitiveImGuiDrawer(u8, int, ImGui::SliderInt(id, &value, 0, UINT8_MAX))
    DefinePrimitiveImGuiDrawer(u16, int, ImGui::SliderInt(id, &value, 0, UINT16_MAX))
    DefinePrimitiveImGuiDrawer(u32, int, ImGui::InputInt(id, &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
    DefinePrimitiveImGuiDrawer(u64, int, ImGui::InputInt(id, &value, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
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

    static void DrawEntity(Property property) {
        typedef Entity(*Getter)(Entity entity);
        typedef void(*Setter)(Entity entity, Entity value);
        auto value = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));

        char id[512];

        if(IsEntityValid(value)) {
            if(HasHierarchy(value)) {
                sprintf(id, "%s##%llu", GetName(value), property);
            } else {
                sprintf(id, "Entity_%d##%llu", GetHandleIndex(value), property);
            }
        } else {
            sprintf(id, "<None>##%llu", property);
        }

        ImGui::Text("%s", id);

        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetTypeName(TypeOf_Entity()))) {
                if(payload->Delivery) {
                    for(auto i = 0; i < GetNumSelection(); ++i) {
                        ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), value);
                    }
                }
            }
            ImGui::EndDragDropTarget();
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

    static void DrawType(Property property) {
        char id[128];
        sprintf(id, "##%llu", property);
        typedef Type(*Getter)(Entity entity);
        typedef void(*Setter)(Entity entity, Type value);
        auto value = ((Getter)GetPropertyGetter(property))(GetSelectionEntity(0));
        if(ImGui::BeginButtonDropDown(id, {50, 20})) {
            for(auto type = GetNextType(0); IsTypeValid(type); type = GetNextType(type)) {
                if(ImGui::MenuItem(GetTypeName(type))) {
                    for(auto i = 0; i < GetNumSelection(); ++i) {
                        ((Setter)GetPropertySetter(property))(GetSelectionEntity(i), type);
                    }
                }
            }
            ImGui::EndButtonDropDown();
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

    static void Draw(Entity context) {
        if(ImGui::Begin("Property Editor", &Visible)) {
            ImGui::Columns(2, "properties", true);

            for(auto type = GetNextType(0); IsTypeValid(type); type = GetNextType(type)) {
                bool showType = false;
                for(auto i = 0; i < GetNumSelection(); ++i) {
                    if(HasComponent(GetSelectionEntity(i), type)) {
                        showType = true;
                    }
                }

                if(!showType) continue;

                if(true) {//ImGui::CollapsingHeader(GetTypeName(type))) {
                    for(auto property = GetNextProperty(0); IsPropertyValid(property); property = GetNextProperty(property)) {
                        if(GetPropertyOwner(property) != type) {
                            continue;
                        }

                        auto propertyType = GetPropertyType(property);

                        ImGui::Text("%s", GetPropertyName(property));
                        ImGui::NextColumn();

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
                        HandleDraw(rgba8);
                        HandleDraw(rgba32);
                        HandleDraw(rgb8);
                        HandleDraw(rgb32);
                        HandleDraw(Entity);
                        HandleDraw(Type);

                        ImGui::NextColumn();
                    }
                }
            }

        }

        if (ImGui::BeginPopup(ComponentContextMenuId)) {
            //ImGui::ComponentContextMenu();
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    static bool ServiceStart() {
        SubscribeImGuiDraw(Draw);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeImGuiDraw(Draw);
        return true;
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
