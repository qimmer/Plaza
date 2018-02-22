//
// Created by Kim Johannsen on 23/01/2018.
//

#include "FileExplorer.h"
#include <ImGui/ImGuiRenderer.h>
#include <File/Folder.h>
#include <ImGui/imgui/imgui.h>
#include <Core/Hierarchy.h>
#include <File/FileStream.h>
#include <Editor/Project.h>

DefineService(FileExplorer)
EndService()

static bool Visible = true;

bool GetFileExplorerVisible() {
    return Visible;
}

void SetFileExplorerVisible(bool value) {
    Visible = value;
}

static void DrawEntry(Entity entry, int level) {
    if(HasFolder(entry)) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if(level < 1) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if(!IsEntityValid(GetFirstChild(entry))) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        if(ImGui::TreeNodeEx(GetFileName(GetFolderPath(entry)), flags)) {
            for(auto child = GetFirstChild(entry); IsEntityValid(child); child = GetSibling(child)) {
                DrawEntry(child, level + 1);
            }
            ImGui::TreePop();
        }
    }

    if(HasFileStream(entry)) {
        if(ImGui::TreeNodeEx(GetFileName(GetFilePath(entry)), ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {

        }
    }
}

static void Draw(Entity context) {
    if(ImGui::Begin("File Explorer", &Visible)) {
        for(auto project = GetNextEntityThat(0, HasProject); IsEntityValid(project); project = GetNextEntityThat(project, HasProject)) {
            DrawEntry(GetProjectSourceFolder(project), 0);
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

void ImGui::FileStreamContextMenu(Entity stream) {

}
