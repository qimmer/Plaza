//
// Created by Kim Johannsen on 19-03-2018.
//

#include <ImGui/ImGuiRenderer.h>
#include "CodeEditor.h"
#include "Editor/EditorStandardCommands.h"
#include "Editor/EditorStyle.h"
#include <imgui/imgui.h>
#include <Foundation/Stream.h>
#include <Core/String.h>
#include <imguicodeeditor/imguicodeeditor.h>

struct CodeEditor {
    Entity CodeEditorStream;
    //ImString CodeBuffer;
    bool Invalidated, Active;
};


static void SaveCode(Entity codeEditor) {
    auto data = GetCodeEditor(codeEditor);
    auto stream = GetCodeEditorStream(codeEditor);

    if(!StreamOpen(data->CodeEditorStream, StreamMode_Write))
    {
        Log(LogChannel_Core, LogSeverity_Error, "Code editor save failed. Cannot open '%s' for writing.", GetStreamPath(data->CodeEditorStream));
        return;
    }

    //StreamWrite(data->CodeEditorStream, data->CodeBuffer.size(), data->CodeBuffer.c_str());
    StreamClose(data->CodeEditorStream);
}

LocalFunction(OnCommandReleased, void, Entity command) {
    if(command == GetSaveCommand()) {
        for_entity(codeEditor, data, CodeEditor) {
            auto data = GetCodeEditor(codeEditor);

            if(!data->Invalidated || !data->Active) continue;

            SaveCode(codeEditor);
        }
    }
}

void EditCode(Entity stream) {
    char editorPath[PathMax];
    snprintf(editorPath, PathMax, "/.editor/CodeEditor/%llu", stream);

    auto editor = CreateEntityFromPath(editorPath);
    SetCodeEditorStream(editor, stream);
}

static void Draw(Entity context) {
    static String buffer;

    for_entity(codeEditor, data, CodeEditor) {
        auto data = GetCodeEditor(codeEditor);
        auto stream = GetCodeEditorStream(codeEditor);

        bool show = true;
        char title[PathMax];
        snprintf(title, PathMax, "%s##%llu", GetStreamPath(stream), codeEditor);

        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

        if(ImGui::Begin(title, &show)) {
            auto size = ImGui::GetContentRegionAvail();
            size.y -= 5;

            ImGuiID id = 0;

            ImGui::PushFont(GetMonospaceFont());
            /*if(ImGui::InputTextWithSyntaxHighlighting(id, data->CodeBuffer, ImGuiCe::LANG_GLSL, size)) {
                data->Invalidated = true;
            }*/
            ImGui::PopFont();

        }
        data->Active = ImGui::IsWindowFocused();

        ImGui::End();
    }
}

LocalFunction(OnStreamChanged, void, Entity stream) {
    for_entity(codeEditor, data, CodeEditor) {
        if(GetCodeEditorStream(codeEditor) == stream) {
            FireEvent(EventOf_CodeEditorChanged(), codeEditor);
        }
    }
}

LocalFunction(OnRemoved, void, Entity codeEditor) {
    if(GetCodeEditor(codeEditor)->Invalidated) {

    }
}

LocalFunction(OnCodeEditorChanged, void, Entity sceneEditor) {
    auto data = GetCodeEditor(sceneEditor);

    if(IsEntityValid(data->CodeEditorStream) && HasStream(data->CodeEditorStream)) {
        if(!StreamOpen(data->CodeEditorStream, StreamMode_Read))
        {
            Log(LogChannel_Core, LogSeverity_Error, "Code editor load failed. Cannot open '%s' for reading.", GetStreamPath(data->CodeEditorStream));
            return;
        }
        StreamSeek(data->CodeEditorStream, StreamSeek_End);
        auto size = StreamTell(data->CodeEditorStream);
        StreamSeek(data->CodeEditorStream, 0);

        static Vector<char> buffer;
        buffer.resize(size);
        StreamRead(data->CodeEditorStream, size, buffer.data());

        if(memcmp(buffer.data(), "\xEF\xBB\xBF", 3) == 0) {
            buffer.erase(buffer.begin());
            buffer.erase(buffer.begin());
            buffer.erase(buffer.begin());
        }

        for(auto it = buffer.begin(); it != buffer.end(); ++it) {
            if(*it == '\r') buffer.erase(it);
        }

        buffer.push_back('\0');

//        data->CodeBuffer = buffer.data();

        StreamClose(data->CodeEditorStream);

        data->Invalidated = false;
    }
}

LocalFunction(OnServiceStart, void, Service service) {
    ImGuiCe::CodeEditor::ImFonts[0] =
            ImGuiCe::CodeEditor::ImFonts[1] =
            ImGuiCe::CodeEditor::ImFonts[2] =
            ImGuiCe::CodeEditor::ImFonts[3] =
            GetMonospaceFont();
}

BeginUnit(CodeEditor)
    BeginComponent(CodeEditor)
        RegisterProperty(Entity, CodeEditorStream)
        DefineExtensionMethod(Stream, void, EditCode, Entity entity)
EndComponent()
EndUnit()
(Entity, CodeEditorStream)

DefineService(CodeEditor)
    ServiceDependency(EditorStyle)
    RegisterSubscription(CommandReleased, OnCommandReleased, 0)
    RegisterSubscription(ImGuiDraw, Draw, 0)
    RegisterSubscription(CodeEditorRemoved, OnRemoved, 0)
        RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CodeEditor()), OnCodeEditorChanged, 0)
        RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Stream()), OnStreamChanged, 0)
        RegisterSubscription(EventOf_StreamContentChanged(), OnStreamChanged, 0)
    RegisterSubscription(CodeEditorStarted, OnServiceStart, 0)
EndService()
