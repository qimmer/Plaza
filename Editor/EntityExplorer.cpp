//
// Created by Kim Johannsen on 26/01/2018.
//

#include <Core/Entity.h>
#include <File/Folder.h>
#include <ImGui/imgui/imgui.h>
#include <Core/Hierarchy.h>
#include <ImGui/ImGuiRenderer.h>
#include <Core/Module.h>
#include "EntityExplorer.h"
#include "Selection.h"


    DefineService(EntityExplorer)
    EndService()

    static bool Visible = true;
    static const char *EntityContextMenuId = "EntityExplorerContextMenu";

    bool GetEntityExplorerVisible() {
        return Visible;
    }

    void SetEntityExplorerVisible(bool value) {
        Visible = value;
    }

    static void DrawEntry(Entity entry, int level) {
        if(HasHierarchy(entry)) {
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

            if(ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload(GetTypeName(TypeOf_Entity()), &entry, sizeof(Entity));
                ImGui::EndDragDropSource();

                isDragging = true;
                isClicked = false;
            }

            if(isOpen) {
                for(auto child = GetFirstChild(entry); IsEntityValid(child); child = GetSibling(child)) {
                    DrawEntry(child, level + 1);
                }
                ImGui::TreePop();
            }

            if(!isDragging) {
                ImGui::OpenPopupOnItemClick(EntityContextMenuId, 1);
            }


            if(isClicked && !ImGui::IsPopupOpen(EntityContextMenuId)) {
                if(!ImGui::GetIO().KeyCtrl) {
                    DeselectAll();
                }

                AddSelection(entry);
            }
        }
    }

    static void Draw(Entity context) {
        if(ImGui::Begin("Entity Explorer", &Visible)) {
            for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
                if(!HasHierarchy(entity) || IsEntityValid(GetParent(entity))) continue;
                DrawEntry(entity, 0);
            }

            if (ImGui::BeginPopup(EntityContextMenuId)) {
                ImGui::EntityContextMenu();
                ImGui::EndPopup();
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


void ImGui::EntityContextMenu() {
    if(GetNumSelection() > 0 && ImGui::MenuItem("Destroy Entity")) {
        while(GetNumSelection()) {
            DestroyEntity(GetSelectionEntity(0));
        }
    }

    if(GetNumSelection() <= 1 && ImGui::BeginMenu("Create")) {

        for(auto module = GetNextModule(0); IsModuleValid(module); module = GetNextModule(module)) {
            if(ImGui::BeginMenu(GetModuleName(module))) {
                for(auto i = 0; i < GetModuleTypes(module); ++i) {
                    auto type = GetModuleType(module, i);
                    if(IsComponent(type)) {
                        if(ImGui::MenuItem(GetTypeName(type))) {
                            auto newEntity = CreateEntity();
                            AddHierarchy(newEntity);
                            AddComponent(newEntity, type);

                            if(GetNumSelection() > 0) {
                                SetParent(newEntity, GetSelectionEntity(0));
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
