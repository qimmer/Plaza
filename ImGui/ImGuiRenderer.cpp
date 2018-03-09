//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/MemoryStream.h>
#include <Foundation/AppLoop.h>

#include <Rendering/Context.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/CommandList.h>
#include <Rendering/VertexBuffer.h>
#include <Rendering/IndexBuffer.h>

#include "ImGuiRenderer.h"
#include <Rendering/Context.h>

#include <imgui/imgui.h>
#include <cglm/cglm.h>
#include <Rendering/VertexAttribute.h>
#include <Core/Hierarchy.h>
#include <Rendering/Uniform.h>
#include <Rendering/Batch.h>
#include <File/FileStream.h>
#include <Rendering/Shader.h>
#include <Rendering/Program.h>
#include <Rendering/Mesh.h>
#include <Rendering/Material.h>
#include <Rendering/UniformState.h>
#include <Core/String.h>
#include <Foundation/Visibility.h>
#include <Rendering/VertexDeclaration.h>

using namespace ImGui;

    ImGuiContext *PrimaryImGuiContext = NULL;

    struct ImGuiRenderer {
        ImGuiContext* ImGuiContext;
        Entity CommandList, VertexBuffer, IndexBuffer;
        Vector<Entity> Batches;
        bool ShowMetrics, ShowDemo;
    };

    Entity FontTexture,
            ImGuiVertexDeclaration,
            ImGuiTextureUniform,
            FontTextureUniformState,
            ImGuiVertexShader,
            ImGuiPixelShader,
            ImGuiProgram,
            FontMaterial,
            ImGuiDataRoot;

    DefineComponent(ImGuiRenderer)
    EndComponent()

    DefineService(ImGuiRenderer)
    EndService()

    DefineEvent(ImGuiDraw, EntityHandler)

void *GetDefaultImGuiContext() {
    return PrimaryImGuiContext;
}

void RebuildImGuiFonts() {
    ImGuiIO& io = GetIO();

    // Build atlas
    unsigned char* tex_pixels = NULL;
    v2i textureSize;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &textureSize.x, &textureSize.y);

    // Create texture with previous stream as data
    char texPath[PATH_MAX];
    snprintf(texPath, PATH_MAX, "%s/FontTexture", GetEntityPath(ImGuiDataRoot));

    FontTexture = CreateTexture2D(texPath);
    StreamOpen(FontTexture, StreamMode_Write);
    StreamWrite(FontTexture, textureSize.x * textureSize.y * 4, tex_pixels);
    StreamClose(FontTexture);
    SetTextureSize2D(FontTexture, textureSize);
    SetTextureFormat(FontTexture, TEXTURE_FORMAT_RGBA8);

    snprintf(texPath, PATH_MAX, "%s/FontMaterial", GetEntityPath(ImGuiDataRoot));
    FontMaterial = CreateMaterial(texPath);
    SetMaterialProgram(FontMaterial, ImGuiProgram);
    SetMaterialRenderState(FontMaterial, RenderState_STATE_RGB_WRITE |
                                         RenderState_STATE_ALPHA_WRITE |
                                         RenderState_STATE_MSAA |
                                         RenderState_STATE_BLEND_FUNC(RenderState_STATE_BLEND_SRC_ALPHA, RenderState_STATE_BLEND_INV_SRC_ALPHA));


    snprintf(texPath, PATH_MAX, "%s/FontTextureUniformState", GetEntityPath(FontMaterial));
    FontTextureUniformState = CreateUniformState(texPath);
    SetUniformStateUniform(FontTextureUniformState, ImGuiTextureUniform);
    SetUniformStateTexture(FontTextureUniformState, FontTexture);

    io.Fonts->SetTexID((void*)(size_t)GetHandleIndex(FontMaterial));
    io.Fonts->ClearTexData();

};

    static void InitializeKeyMapping();

    static void OnContextAdded(Entity context) {
        auto data = GetImGuiRenderer(context);

        auto oldContext = ImGui::GetCurrentContext();

        data->ImGuiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(data->ImGuiContext);

        InitializeKeyMapping();

        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/.ImGuiCommandList", GetEntityPath(context));
        data->CommandList = CreateCommandList(path);
        SetCommandListLayer(data->CommandList, 245 + GetImGuiRendererIndex(context));

        snprintf(path, PATH_MAX, "%s/ImGuiVertexBuffer", GetEntityPath(data->CommandList));
        data->VertexBuffer = CreateVertexBuffer(path);

        snprintf(path, PATH_MAX, "%s/ImGuiIndexBuffer", GetEntityPath(data->CommandList));
        data->IndexBuffer = CreateIndexBuffer(path);

        SetVertexBufferDeclaration(data->VertexBuffer, ImGuiVertexDeclaration);

        SetVertexBufferDynamic(data->VertexBuffer, true);
        SetIndexBufferDynamic(data->IndexBuffer, true);

        ImGui::SetCurrentContext(oldContext);
    }

    static void OnContextRemoved(Entity context) {
        auto data = GetImGuiRenderer(context);

        for(auto batch : data->Batches) {
            DestroyEntity(batch);
        }

        DestroyEntity(data->CommandList);
        DestroyContext(data->ImGuiContext);
    }

    static void UpdateBatches(Entity context, Entity commandList) {
        auto data = GetImGuiRenderer(context);
        auto drawData = GetDrawData();
        auto numBatches = 0;
        auto stride = GetVertexStride(ImGuiVertexDeclaration);

        for (auto i = 0; i < drawData->CmdListsCount; i++)
        {
            numBatches += drawData->CmdLists[i]->CmdBuffer.Size;
        }

        for(int i = numBatches; i < data->Batches.size(); ++i) {
            DestroyEntity(data->Batches[i]);
        }

        auto oldSize = data->Batches.size();
        data->Batches.resize(numBatches);

        for(int i = oldSize; i < numBatches; ++i) {
            char path[PATH_MAX];

            snprintf(path, PATH_MAX, "%s/Batch_%i", GetEntityPath(commandList), i);
            data->Batches[i] = CreateBatch(path);

            snprintf(path, PATH_MAX, "%s/Mesh", GetEntityPath(data->Batches[i]));
            auto mesh = CreateMesh(path);
            SetMeshVertexBuffer(mesh, data->VertexBuffer);
            SetMeshIndexBuffer(mesh, data->IndexBuffer);
            SetMeshPrimitiveType(mesh, PrimitiveType_TRIANGLES);

            SetBatchMaterial(data->Batches[i], FontMaterial);
            SetBatchMesh(data->Batches[i], mesh);
        }

        StreamOpen(data->VertexBuffer, StreamMode_Write);
        StreamOpen(data->IndexBuffer, StreamMode_Write);
        StreamSeek(data->VertexBuffer, 0);
        StreamSeek(data->IndexBuffer, 0);

        auto vbOffset = 0;
        auto ibOffset = 0;
        auto subMeshCnt = 0;
        for (auto i = 0; i < drawData->CmdListsCount; i++)
        {
            auto vertexBufferSize = drawData->CmdLists[i]->VtxBuffer.Size * stride;
            auto indexBufferSize = drawData->CmdLists[i]->IdxBuffer.Size * 2;

            StreamWrite(data->VertexBuffer, vertexBufferSize, drawData->CmdLists[i]->VtxBuffer.Data);
            StreamWrite(data->IndexBuffer, indexBufferSize, drawData->CmdLists[i]->IdxBuffer.Data);
            
            auto numVertices = drawData->CmdLists[i]->VtxBuffer.Size;
            auto startIndex = 0;
            for (auto j = 0; j < drawData->CmdLists[i]->CmdBuffer.Size; ++j)
            {
                auto pcmd = drawData->CmdLists[i]->CmdBuffer.Data[j];
                auto batch = data->Batches[subMeshCnt];
                auto mesh = GetBatchMesh(batch);

                SetMeshStartVertex(mesh, vbOffset);
                SetMeshStartIndex(mesh, ibOffset + startIndex);
                SetMeshNumIndices(mesh, pcmd.ElemCount);
                SetMeshNumVertices(mesh, numVertices);

                SetBatchWorldMatrix(batch, m4x4f_Identity);

                auto clipX = std::max((int)pcmd.ClipRect.x, 0);
                auto clipY = std::max((int)pcmd.ClipRect.y, 0);
                SetBatchScissor(batch, {
                        std::max((int)pcmd.ClipRect.x, 0),
                        std::max((int)pcmd.ClipRect.y, 0),
                        std::min((int)pcmd.ClipRect.z, 65535) - clipX,
                        std::min((int)pcmd.ClipRect.w, 65535) - clipY });

                Entity material = GetEntityFromIndex((u32)(u64)pcmd.TextureId);
                SetBatchMaterial(batch, material);

                startIndex += (int) pcmd.ElemCount;
                subMeshCnt++;
            }

            vbOffset += drawData->CmdLists[i]->VtxBuffer.Size;
            ibOffset += drawData->CmdLists[i]->IdxBuffer.Size;
        }

        StreamClose(data->VertexBuffer);
        StreamClose(data->IndexBuffer);
    }

    static void RenderImGui(Entity context, double deltaTime) {
        auto data = GetImGuiRenderer(context);

        SetCurrentContext(GetImGuiRenderer(context)->ImGuiContext);
        ImGuiIO& io = GetIO();

        auto contextSize = GetContextSize(context);
        auto mousePosition = GetCursorPosition(context, 0);

        io.DisplaySize = ImVec2(contextSize.x, contextSize.y);
        io.DisplayFramebufferScale = ImVec2(1, 1);
        io.DeltaTime = deltaTime;

        auto keyStates = GetKeyStates(context);
        for (auto i = 0; i < 512; ++i)
        {
            io.KeysDown[i] = keyStates[i] > 0.5f;
        }

        for (auto i = 0; i < 3; ++i)
        {
            io.MouseDown[i] =  keyStates[i + MOUSEBUTTON_0] > 0.5f;
        }

        io.KeyAlt = keyStates[KEY_LEFT_ALT] > 0.0f
                                              || keyStates[KEY_RIGHT_ALT] > 0.0f;
        io.KeyCtrl = keyStates[KEY_LEFT_CONTROL] > 0.0f
                                               || keyStates[KEY_RIGHT_CONTROL] > 0.0f;
        io.KeyShift = keyStates[KEY_LEFT_SHIFT] > 0.0f
                                                || keyStates[KEY_RIGHT_SHIFT] > 0.0f;

        io.MousePos = ImVec2(mousePosition.x, mousePosition.y);
        io.MouseWheel = (keyStates[MOUSE_SCROLL_UP]  - keyStates[MOUSE_SCROLL_DOWN]) / 20.0f;

        NewFrame();

        static bool drawWindows = true;
        if(ImGui::IsKeyPressed(KEY_F8)) {
            drawWindows = !drawWindows;
        }

        if(drawWindows) {
            FireEvent(ImGuiDraw, context);
        }

        if(ImGui::IsKeyPressed(KEY_F9)) {
            data->ShowMetrics = !data->ShowMetrics;
        }

        if(data->ShowMetrics) ShowMetricsWindow(&data->ShowMetrics);

        Render();

        m4x4f projectionMatrix, viewMatrix;
        glm_ortho(0, io.DisplaySize.x, io.DisplaySize.y, 0, 0, 1000, (vec4*)&projectionMatrix.x);
        glm_mat4_identity((vec4*)&viewMatrix.x);

        SetCommandListProjectionMatrix(data->CommandList, projectionMatrix);
        SetCommandListViewMatrix(data->CommandList, viewMatrix);
        SetCommandListClearTargets(data->CommandList, ClearTarget_Depth);
        SetCommandListClearDepth(data->CommandList, 0.0f);
        SetCommandListForceOrder(data->CommandList, true);
        SetCommandListRenderTarget(data->CommandList, context);
        SetCommandListViewport(data->CommandList, {0, 0, contextSize.x, contextSize.y});

        UpdateBatches(context, data->CommandList);
    }

    static void OnAppUpdate(double deltaTime) {
        for_entity(context, ImGuiRenderer) {
            RenderImGui(context, deltaTime);
        }
    }

    static void OnCharPressed(Entity context, char c) {
        auto data = GetImGuiRenderer(context);

        SetCurrentContext(data->ImGuiContext);

        GetIO().AddInputCharacter(c);
    }


    static void InitializeProgram() {
        char path[PATH_MAX];

        snprintf(path, PATH_MAX, "%s/Program", GetEntityPath(ImGuiDataRoot));
        ImGuiProgram = CreateProgram(path);

        snprintf(path, PATH_MAX, "%s/VertexShader", GetEntityPath(ImGuiProgram));
        ImGuiVertexShader = CreateShader(path);

        snprintf(path, PATH_MAX, "%s/PixelShader", GetEntityPath(ImGuiProgram));
        ImGuiPixelShader = CreateShader(path);

        snprintf(path, PATH_MAX, "%s/TextureUniform", GetEntityPath(ImGuiDataRoot));
        ImGuiTextureUniform = CreateUniform(path);

        SetStreamPath(ImGuiVertexShader, "res://imgui/shaders/imgui.vs");
        SetStreamPath(ImGuiPixelShader, "res://imgui/shaders/imgui.ps");
        SetStreamPath(ImGuiProgram, "res://imgui/shaders/imgui.var");

        SetShaderType(ImGuiVertexShader, ShaderType_Vertex);
        SetShaderType(ImGuiPixelShader, ShaderType_Pixel);

        SetUniformName(ImGuiTextureUniform, "s_tex");
        SetUniformType(ImGuiTextureUniform, TypeOf_Entity());
        SetUniformArrayCount(ImGuiTextureUniform, 1);
    }

    static void InitializeVertexDeclaration() {
        char path[PATH_MAX];

        snprintf(path, PATH_MAX, "%s/VertexDeclaration", GetEntityPath(ImGuiDataRoot));
        ImGuiVertexDeclaration = CreateVertexDeclaration(path);

        Entity attribute;

        snprintf(path, PATH_MAX, "%s/Position", GetEntityPath(ImGuiVertexDeclaration));
        attribute = CreateVertexAttribute(path);
        SetVertexAttributeType(attribute, TypeOf_v2f());
        SetVertexAttributeUsage(attribute, VertexAttributeUsage_Position);

        snprintf(path, PATH_MAX, "%s/TexCoord", GetEntityPath(ImGuiVertexDeclaration));
        attribute = CreateVertexAttribute(path);
        SetVertexAttributeType(attribute, TypeOf_v2f());
        SetVertexAttributeUsage(attribute, VertexAttributeUsage_TexCoord0);

        snprintf(path, PATH_MAX, "%s/Color", GetEntityPath(ImGuiVertexDeclaration));
        attribute = CreateVertexAttribute(path);
        SetVertexAttributeType(attribute, TypeOf_rgba8());
        SetVertexAttributeUsage(attribute, VertexAttributeUsage_Color0);
        SetVertexAttributeNormalize(attribute, true);
    }

    static void InitializeFontTexture() {
        ImGuiIO& io = ImGui::GetIO();

        io.Fonts->AddFontDefault();
        RebuildImGuiFonts();
    }

    static void InitializeKeyMapping() {
        ImGuiIO& io = ImGui::GetIO();

        io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = KEY_HOME;
        io.KeyMap[ImGuiKey_End] = KEY_END;
        io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = KEY_A;
        io.KeyMap[ImGuiKey_C] = KEY_C;
        io.KeyMap[ImGuiKey_V] = KEY_V;
        io.KeyMap[ImGuiKey_X] = KEY_X;
        io.KeyMap[ImGuiKey_Y] = KEY_Y;
        io.KeyMap[ImGuiKey_Z] = KEY_Z;
    }

    static bool ServiceStart() {
        ImGui::GetIO();
        PrimaryImGuiContext = ImGui::GetCurrentContext();

        ImGuiDataRoot = CreateHierarchy("/.ImGui");

        InitializeProgram();
        InitializeFontTexture();
        InitializeKeyMapping();
        InitializeVertexDeclaration();

        SubscribeAppUpdate(OnAppUpdate);
        SubscribeImGuiRendererAdded(OnContextAdded);
        SubscribeImGuiRendererRemoved(OnContextRemoved);
        SubscribeCharacterPressed(OnCharPressed);

        AddExtension(TypeOf_Context(), TypeOf_ImGuiRenderer());

        return true;
    }

    static bool ServiceStop() {
        RemoveExtension(TypeOf_Context(), TypeOf_ImGuiRenderer());

        DestroyEntity(FontTexture);
        DestroyEntity(ImGuiVertexDeclaration);
        DestroyEntity(ImGuiProgram);

        ImGui::SetCurrentContext(PrimaryImGuiContext);
        PrimaryImGuiContext = NULL;

        UnsubscribeAppUpdate(OnAppUpdate);
        UnsubscribeImGuiRendererAdded(OnContextAdded);
        UnsubscribeImGuiRendererRemoved(OnContextRemoved);
        UnsubscribeCharacterPressed(OnCharPressed);

        ImGui::Shutdown();

        DestroyEntity(ImGuiDataRoot);

        return true;
    }
