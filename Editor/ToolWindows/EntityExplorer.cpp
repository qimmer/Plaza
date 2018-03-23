//
// Created by Kim Johannsen on 26/01/2018.
//

#include <Core/Entity.h>
#include <File/Folder.h>
#include <ImGui/imgui/imgui.h>
#include <Core/Hierarchy.h>
#include <ImGui/ImGuiRenderer.h>
#include <Core/Module.h>
#include <Core/String.h>
#include <Foundation/PersistancePoint.h>
#include <Foundation/Persistance.h>
#include "EntityExplorer.h"
#include "Editor/Selection.h"

DefineService(EntityExplorer)
EndService()

static bool Visible = true, DrawHidden = false, DrawFromRoot = false;
static Entity Root = 0;
static const char *EntityContextMenuId = "EntityExplorerContextMenu";

bool GetEntityExplorerVisible() {
    return Visible;
}

void SetEntityExplorerVisible(bool value) {
    Visible = value;
}

static void DrawEntry(Entity entry, int level) {
    if(HasHierarchy(entry) && (GetName(entry)[0] != '.' || DrawHidden)) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if(!IsEntityValid(GetFirstChild(entry))) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        if(HasSelection(entry)) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool isOpen = ImGui::TreeNodeEx(GetName(entry), flags);
        bool isClicked = ImGui::IsItemHoveredRect() && (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1));
        bool isDragging = false;

        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetTypeName(TypeOf_Entity()))) {
                if(payload->Delivery && *((Entity*)payload->Data) != entry) {
                    SetParent(*((Entity*)payload->Data), entry);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if(ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(GetTypeName(TypeOf_Entity()), &entry, sizeof(Entity));
            ImGui::EndDragDropSource();

            isDragging = true;
            isClicked = false;
        }

        if(isOpen) {
            for_children(child, entry) {
                DrawEntry(child, level + 1);
            }
            ImGui::TreePop();
        }

        if(isClicked && !ImGui::IsPopupOpen(EntityContextMenuId) && !isDragging) {
            if(!ImGui::GetIO().KeyCtrl) {
                DeselectAll();
            }

            AddSelection(entry);
        }
    }
}

static void Draw(Entity context) {
    if(ImGui::Begin("Entity Explorer", &Visible)) {
        ImGui::Checkbox("Draw Hidden Entities", &DrawHidden);

        if(ImGui::IsWindowHovered()) {
            if(ImGui::IsMouseReleased(1)) {
                ImGui::OpenPopup(EntityContextMenuId);
            }

            if(ImGui::IsMouseReleased(0) && !ImGui::GetIO().KeyCtrl && !ImGui::IsMouseDragging(0)) {
                DeselectAll();
            }
        }

        if (ImGui::BeginPopup(EntityContextMenuId)) {
            ImGui::EntityContextMenu();
            ImGui::EndPopup();
        }

        for_children(entity, 0) {
            DrawEntry(entity, 0);
        }

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

static Type SelectComponentMenu() {
    for(auto module = GetNextModule(0); IsModuleValid(module); module = GetNextModule(module)) {
        if(ImGui::BeginMenu(GetModuleName(module))) {
            for(auto i = 0; i < GetModuleTypes(module); ++i) {
                auto type = GetModuleType(module, i);

                if(!IsComponentAbstract(type) && ImGui::MenuItem(GetTypeName(type))) {
                    ImGui::EndMenu();
                    return type;
                }
            }
            ImGui::EndMenu();
        }
    }

    return 0;
}


void ImGui::EntityContextMenu() {
    if(GetNumSelection() <= 1 && ImGui::BeginMenu("Create")) {
        auto type = SelectComponentMenu();
        if(IsTypeValid(type)) {
            auto newEntity = CreateEntity();
            AddHierarchy(newEntity);
            AddComponent(newEntity, type);

            char name[PATH_MAX];
            sprintf(name, "%s_%iu", GetTypeName(type), GetHandleIndex(newEntity));

            SetName(newEntity, name);

            if(GetNumSelection() > 0) {
                SetParent(newEntity, GetSelectionEntity(0));

                if(HasPersistance(GetSelectionEntity(0))) {
                    SetEntityPersistancePoint(newEntity, GetEntityPersistancePoint(GetSelectionEntity(0)));
                }
                if(HasPersistancePoint(GetSelectionEntity(0))) {
                    SetEntityPersistancePoint(newEntity, GetSelectionEntity(0));
                }
            }
        }

        ImGui::EndMenu();
    }

    if(GetNumSelection() > 0 && ImGui::BeginMenu("Add")) {
        auto type = SelectComponentMenu();
        if(IsTypeValid(type)) {
            for(auto i = 0; i < GetNumSelection(); ++i) {
                auto entity = GetSelectionEntity(i);
                AddComponent(entity, type);
            }
        }

        ImGui::EndMenu();
    }

    if(GetNumSelection() > 0 && ImGui::BeginMenu("Remove")) {
        for(auto type = GetNextType(0); IsTypeValid(type); type = GetNextType(type)) {
            if(IsComponent(type)) {
                auto showType = false;
                for(auto i = 0; i < GetNumSelection(); ++i) {
                    if(HasComponent(GetSelectionEntity(i), type)) {
                        showType = true;
                        break;
                    }
                }

                if(showType && ImGui::MenuItem(GetTypeName(type))) {
                    for(auto i = 0; i < GetNumSelection(); ++i) {
                        if(HasComponent(GetSelectionEntity(i), type)) {
                            RemoveComponent(GetSelectionEntity(i), type);
                        }
                    }
                }
            }
        }

        ImGui::EndMenu();
    }

    if(GetNumSelection() > 0) {
        ImGui::Separator();

        for(auto f = GetNextFunction(0); IsFunctionValid(f); f = GetNextFunction(f)) {
            if(GetFunctionContextType(f) != TypeOf_Entity() || GetFunctionArguments(f) != 1 || GetFunctionArgumentType(f, 0) != TypeOf_Entity()) continue;

            char returnData[128];
            for(auto i = 0; i < GetNumSelection(); ++i) {
                if(ImGui::MenuItem(GetFunctionName(f))) {
                    for(auto j = 0; j < GetNumSelection(); ++j) {
                        auto entity = GetSelectionEntity(j);
                        CallFunction(f, &entity, &returnData);
                    }
					break;
                }
            }
        }

        ImGui::Separator();

        for(auto f = GetNextFunction(0); IsFunctionValid(f); f = GetNextFunction(f)) {
            auto contextType = GetFunctionContextType(f);
            if(!IsTypeValid(contextType) || !IsComponent(contextType) || GetFunctionArguments(f) != 1 || GetFunctionArgumentType(f, 0) != TypeOf_Entity()) continue;

            char returnData[128];
            for(auto i = 0; i < GetNumSelection(); ++i) {
                if(HasComponent(GetSelectionEntity(i), contextType)) {
                    if(ImGui::MenuItem(GetFunctionName(f))) {
                        for(auto j = 0; j < GetNumSelection(); ++j) {
                            auto entity = GetSelectionEntity(j);
                            CallFunction(f, &entity, &returnData);
                        }
                    }
                    break;
                }
            }
        }
    }
}
