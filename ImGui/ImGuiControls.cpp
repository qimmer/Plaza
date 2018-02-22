#include <ImGui/imgui/imconfig.h>
#include <ImGui/imgui/imgui.h>
#include <ImGui/imgui/imgui_internal.h>
#include <ImGui/ImGuiControls.h>

namespace ImGui {
    bool BeginButtonDropDown(const char* label, ImVec2 buttonSize)
    {
        ImGui::SameLine(0.f, 0.f);

        ImGuiWindow* window = GetCurrentWindow();
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        float x = ImGui::GetCursorPosX();
        float y = ImGui::GetCursorPosY();

        ImVec2 size(20, buttonSize.y);
        bool pressed = ImGui::Button("##", size);

        // Arrow
        ImVec2 center(window->Pos.x + x + 10, window->Pos.y + y + buttonSize.y / 2);
        float r = 8.f;
        center.y -= r * 0.25f;
        ImVec2 a(center.x, center.y + r);
        ImVec2 b(center.x - 0.866f * r, center.y - 0.5f * r);
        ImVec2 c(center.x + 0.866f * r, center.y - 0.5f * r);

        window->DrawList->AddTriangleFilled(a, b, c, GetColorU32(ImGuiCol_Text));

        // Popup

        ImVec2 popupPos;

        popupPos.x = window->Pos.x + x - buttonSize.x;
        popupPos.y = window->Pos.y + y + buttonSize.y;

        ImGui::SetNextWindowPos(popupPos);

        if (pressed)
        {
            ImGui::OpenPopup(label);
        }

        if (ImGui::BeginPopup(label))
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, style.Colors[ImGuiCol_Button]);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, style.Colors[ImGuiCol_Button]);
            ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, style.Colors[ImGuiCol_Button]);
            return true;
        }

        return false;
    }

    void EndButtonDropDown()
    {
        ImGui::PopStyleColor(3);
        ImGui::EndPopup();
    }
}