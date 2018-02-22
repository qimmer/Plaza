//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Rendering/Context.h>
#include <Foundation/AppLoop.h>
#include "BgfxContext.h"
#include "BgfxCommandList.h"
#include "BgfxBinaryShader.h"
#include "BgfxIndexBuffer.h"
#include "BgfxVertexBuffer.h"
#include "BgfxProgram.h"
#include "BgfxTexture2D.h"
#include "BgfxUniform.h"

#include <bgfx/bgfx.h>
#include <Bgfx/bgfx/include/bgfx/platform.h>
#include <Rendering/CommandList.h>
#include <Rendering/BinaryShader.h>
#include <Rendering/VertexBuffer.h>
#include <Rendering/Program.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Uniform.h>
#include <Rendering/IndexBuffer.h>
#include <Foundation/Invalidation.h>

#include <Bgfx/BgfxBinaryShader.h>
#include <Bgfx/BgfxIndexBuffer.h>
#include <Bgfx/BgfxVertexBuffer.h>
#include <Bgfx/BgfxProgram.h>
#include <Bgfx/BgfxTexture2D.h>
#include <Bgfx/BgfxUniform.h>

#include <GLFW/glfw3.h>

#ifdef __APPLE__
extern "C" {

    typedef void* id;

    GLFWAPI id glfwGetCocoaWindow(GLFWwindow* window);
};
#endif

    struct BgfxContext {
        GLFWwindow *window;
        bgfx::FrameBufferHandle fb;
    };

    DefineComponent(BgfxContext)
        ExtensionOf(Context)
    EndComponent()

    DefineService(BgfxContext)
    EndService()

    u32 NumContexts = 0;
    Entity PrimaryContext = 0;

    static void OnAppUpdate(double deltaTime) {
        for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
            if(HasBgfxContext(entity)) {
                SetKeyState(entity, MOUSE_SCROLL_DOWN, 0.0f);
                SetKeyState(entity, MOUSE_SCROLL_UP, 0.0f);
                SetKeyState(entity, MOUSE_SCROLL_LEFT, 0.0f);
                SetKeyState(entity, MOUSE_SCROLL_RIGHT, 0.0f);
                SetKeyState(entity, MOUSE_DOWN, 0.0f);
                SetKeyState(entity, MOUSE_UP, 0.0f);
                SetKeyState(entity, MOUSE_LEFT, 0.0f);
                SetKeyState(entity, MOUSE_RIGHT, 0.0f);
            }
        }

        glfwPollEvents();

        // Validate all resources to eventual model changes
        ValidateAll(UpdateBgfxBinaryShader, HasBgfxBinaryShader);
        ValidateAll(UpdateBgfxProgram, HasBgfxProgram);
        ValidateAll(UpdateBgfxUniform, HasBgfxUniform);
        ValidateAll(UpdateBgfxTexture2D, HasBgfxTexture2D);
        ValidateAll(UpdateBgfxVertexDeclaration, HasBgfxVertexDeclaration);
        ValidateAll(UpdateBgfxVertexBuffer, HasBgfxVertexBuffer);
        ValidateAll(UpdateBgfxIndexBuffer, HasBgfxIndexBuffer);

        // Render all command lists
        for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
            if(HasBgfxCommandList(entity)) {
                RenderCommandList(entity);
            }
        }

        bgfx::frame();

        for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
            if(HasBgfxContext(entity)) {
                if(glfwWindowShouldClose(GetBgfxContext(entity)->window)) {
                    FireEvent(ContextClosing, entity);
                }
            }
        }
    }

    static void OnContextResized(Entity entity, v2i oldSize, v2i newSize) {
        auto data = GetBgfxContext(entity);

#ifdef __APPLE__
        id windowHandle = glfwGetCocoaWindow(data->window);
#endif
        glfwSetWindowSize(data->window, newSize.x, newSize.y);
        if(entity == PrimaryContext) {
            bgfx::reset(newSize.x, newSize.y);
        } else {
            bgfx::destroy(data->fb);
            data->fb = bgfx::createFrameBuffer(windowHandle, newSize.x, newSize.y);
        }
    }

    static void OnContextTitleChanged(Entity entity, StringRef before, StringRef after) {
        auto data = GetBgfxContext(entity);

        glfwSetWindowTitle(data->window, after);
    }

    static void OnGlfwWindowResized(GLFWwindow *window, int w, int h) {
        SetContextSize((Entity)glfwGetWindowUserPointer(window), {w, h});
    }

    static void OnCharPressed(GLFWwindow *window, unsigned int c) {
        FireEvent(CharacterPressed, (Entity)glfwGetWindowUserPointer(window), c);
    }

    static void OnKey(GLFWwindow *window, int key, int scanCode, int action, int mods) {
        if(action == GLFW_PRESS || action == GLFW_REPEAT) {
            SetKeyState((Entity)glfwGetWindowUserPointer(window), key, 1.0f);
        }

        if(action == GLFW_RELEASE) {
            SetKeyState((Entity)glfwGetWindowUserPointer(window), key, 0.0f);
        }
    }

    static void OnMouseScroll(GLFWwindow *window, double x, double y) {
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_DOWN, fmaxf(-y, 0.0f));
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_UP, fmaxf(y, 0.0f));
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_LEFT, fmaxf(-x, 0.0f));
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_RIGHT, fmaxf(x, 0.0f));
    }

    static void OnMouseMove(GLFWwindow *window, double x, double y) {
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_DOWN, fmaxf(y, 0.0f));
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_UP, fmaxf(-y, 0.0f));
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_LEFT, fmaxf(-x, 0.0f));
        SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_RIGHT, fmaxf(x, 0.0f));

        v2d cp;
        glfwGetCursorPos(window, &cp.x, &cp.y);
        SetCursorPosition((Entity)glfwGetWindowUserPointer(window), 0, {(int)cp.x, (int)cp.y});
    }

    static void OnMouseButton(GLFWwindow *window, int button, int action, int mods) {
        if(action == GLFW_PRESS) {
            SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSEBUTTON_0 + button, 1.0f);
        }

        if(action == GLFW_RELEASE) {
            SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSEBUTTON_0 + button, 0.0f);
        }
    }

    static void OnBgfxContextAdded(Entity entity) {
        auto data = GetBgfxContext(entity);
        auto size = GetContextSize(entity);

        if(NumContexts == 1) {
            glfwInit();
            SubscribeAppUpdate(OnAppUpdate);
            SubscribeContextSizeChanged(OnContextResized);
            SubscribeContextTitleChanged(OnContextTitleChanged);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        auto window = glfwCreateWindow(size.x, size.y, GetContextTitle(entity), NULL, NULL);
        glfwSetWindowSizeCallback(window, OnGlfwWindowResized);
        glfwSetWindowUserPointer(window, (void*)entity);
        glfwSetCharCallback(window, OnCharPressed);
        glfwSetKeyCallback(window, OnKey);
        glfwSetScrollCallback(window, OnMouseScroll);
        glfwSetCursorPosCallback(window, OnMouseMove);
        glfwSetMouseButtonCallback(window, OnMouseButton);

        data->window = window;

#ifdef __APPLE__
        id windowHandle = glfwGetCocoaWindow(window);
#endif

        if(NumContexts == 1) {
            data->fb = BGFX_INVALID_HANDLE;

            bgfx::PlatformData pd;
            pd.ndt = NULL;
            pd.nwh = windowHandle;
            pd.context = NULL;
            pd.backBuffer = NULL;
            pd.backBufferDS = NULL;
            pd.session = NULL;
            bgfx::setPlatformData(pd);

            bgfx::init();
            bgfx::reset(size.x, size.y);

            PrimaryContext = entity;

            AddExtension(TypeOf_CommandList(), TypeOf_BgfxCommandList());
            AddExtension(TypeOf_BinaryShader(), TypeOf_BgfxBinaryShader());
            AddExtension(TypeOf_IndexBuffer(), TypeOf_BgfxIndexBuffer());
            AddExtension(TypeOf_VertexBuffer(), TypeOf_BgfxVertexBuffer());
            AddExtension(TypeOf_VertexDeclaration(), TypeOf_BgfxVertexDeclaration());
            AddExtension(TypeOf_Program(), TypeOf_BgfxProgram());
            AddExtension(TypeOf_Texture2D(), TypeOf_BgfxTexture2D());
            AddExtension(TypeOf_Uniform(), TypeOf_BgfxUniform());
        } else {
            data->fb = bgfx::createFrameBuffer(windowHandle, size.x, size.y);
        }
    }

    static void OnBgfxContextRemoved(Entity entity) {
        auto data = GetBgfxContext(entity);

        if(NumContexts > 1) {
            bgfx::destroy(data->fb);
        }

        if(NumContexts == 1) {
            RemoveExtension(TypeOf_CommandList(), TypeOf_BgfxCommandList());
            RemoveExtension(TypeOf_BinaryShader(), TypeOf_BgfxBinaryShader());
            RemoveExtension(TypeOf_IndexBuffer(), TypeOf_BgfxIndexBuffer());
            RemoveExtension(TypeOf_VertexBuffer(), TypeOf_BgfxVertexBuffer());
            RemoveExtension(TypeOf_VertexDeclaration(), TypeOf_BgfxVertexDeclaration());
            RemoveExtension(TypeOf_Program(), TypeOf_BgfxProgram());
            RemoveExtension(TypeOf_Texture2D(), TypeOf_BgfxTexture2D());
            RemoveExtension(TypeOf_Uniform(), TypeOf_BgfxUniform());

            bgfx::shutdown();
        }

        glfwDestroyWindow(data->window);

        if(NumContexts == 1) {
            UnsubscribeContextSizeChanged(OnContextResized);
            UnsubscribeAppUpdate(OnAppUpdate);
            UnsubscribeContextTitleChanged(OnContextTitleChanged);
            glfwTerminate();

            PrimaryContext = 0;
        }
    }

    static void OnContextAdded(Entity entity) {
        NumContexts++;

        if(NumContexts == 1) {
            AddExtension(TypeOf_Context(), TypeOf_BgfxContext());
        }
    }

    static void OnContextRemoved(Entity entity) {
        if(NumContexts == 1) {
            RemoveExtension(TypeOf_Context(), TypeOf_BgfxContext());
        }

        NumContexts--;
    }

    static bool ServiceStart() {
        SubscribeContextAdded(OnContextAdded);
        SubscribeBgfxContextAdded(OnBgfxContextAdded);
        SubscribeContextRemoved(OnContextRemoved);
        SubscribeBgfxContextRemoved(OnBgfxContextRemoved);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeContextAdded(OnContextAdded);
        UnsubscribeBgfxContextAdded(OnBgfxContextAdded);
        UnsubscribeContextRemoved(OnContextRemoved);
        UnsubscribeBgfxContextRemoved(OnBgfxContextRemoved);
        return true;
    }
