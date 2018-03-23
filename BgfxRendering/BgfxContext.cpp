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
#include "BgfxVertexDeclaration.h"
#include "BgfxOffscreenRenderTarget.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <Rendering/CommandList.h>
#include <Rendering/BinaryShader.h>
#include <Rendering/VertexBuffer.h>
#include <Rendering/Program.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Uniform.h>
#include <Rendering/IndexBuffer.h>

#include <BgfxRendering/BgfxBinaryShader.h>
#include <BgfxRendering/BgfxIndexBuffer.h>
#include <BgfxRendering/BgfxVertexBuffer.h>
#include <BgfxRendering/BgfxProgram.h>
#include <BgfxRendering/BgfxTexture2D.h>
#include <BgfxRendering/BgfxUniform.h>

#include <GLFW/glfw3.h>
#include <cmath>
#include <cfloat>
#include <Rendering/RenderTarget.h>
#include <Rendering/VertexDeclaration.h>
#include <Rendering/OffscreenRenderTarget.h>

#ifdef __APPLE__
extern "C" {

    typedef void* id;

    GLFWAPI id glfwGetCocoaWindow(GLFWwindow* window);
};
#endif

#ifdef WIN32
#undef GetHandle
#include <windows.h>
#include <Rendering/VertexDeclaration.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/OffscreenRenderTarget.h>

#undef CreateService
extern "C" {
    GLFWAPI HWND glfwGetWin32Window(GLFWwindow *window);
}
#endif

struct BgfxContext {
    GLFWwindow *window;
    bgfx::FrameBufferHandle fb;
    int debugFlags;
};

DefineComponent(BgfxContext)
    ExtensionOf(Context)
EndComponent()

DefineService(BgfxContext)
EndService()

u16 GetBgfxContextHandle(Entity entity) {
    return GetBgfxContext(entity)->fb.idx;
}

static u32 NumContexts = 0;
static Entity PrimaryContext = 0;
static double timeSinceLastPoll = FLT_MAX;

static void OnAppUpdate(double deltaTime) {
    timeSinceLastPoll += deltaTime;

    auto numContexts = GetNumBgfxContext();

    for(auto i = 0; i < numContexts; ++i) {
        auto entity = GetBgfxContextEntity(i);
        SetKeyState(entity, MOUSE_SCROLL_DOWN, 0.0f);
        SetKeyState(entity, MOUSE_SCROLL_UP, 0.0f);
        SetKeyState(entity, MOUSE_SCROLL_LEFT, 0.0f);
        SetKeyState(entity, MOUSE_SCROLL_RIGHT, 0.0f);
        SetKeyState(entity, MOUSE_DOWN, 0.0f);
        SetKeyState(entity, MOUSE_UP, 0.0f);
        SetKeyState(entity, MOUSE_LEFT, 0.0f);
        SetKeyState(entity, MOUSE_RIGHT, 0.0f);
    }

    // Cap polls to 60fps to prevent overloading OS
    if(timeSinceLastPoll > (1.0f / 200.0f)) {
        timeSinceLastPoll = 0.0f;

        glfwPollEvents();
    }

    // Render all command lists
    Entity viewIdTaken[256];
    memset(viewIdTaken, 0, sizeof(Entity) * 256);

    auto numCommandLists = GetNumBgfxCommandList();
    for(auto i = 0; i < numCommandLists; ++i) {
        auto entity = GetBgfxCommandListEntity(i);
        auto viewId = GetCommandListLayer(entity);
        if(viewIdTaken[viewId]) {
            Log(LogChannel_Core, LogSeverity_Error, "Command list '%s' and '%s' are using same layer. Last command list skipped.", GetEntityPath(viewIdTaken[viewId]), GetEntityPath(entity));
            continue;
        }

        RenderCommandList(entity, viewId);
    }

    bgfx::frame();

    for(auto i = 0; i < numContexts; ++i) {
        auto entity = GetBgfxContextEntity(i);
        if(glfwWindowShouldClose(GetBgfxContextByIndex(i)->window)) {
            FireEvent(ContextClosing, entity);
        }
    }
}

static void ResetContext(Entity entity) {
    auto data = GetBgfxContext(entity);
    auto size = GetRenderTargetSize(entity);
    auto vsync = GetContextVsync(entity);
    auto fullscreen = GetContextFullscreen(entity);

#ifdef __APPLE__
    id windowHandle = glfwGetCocoaWindow(data->window);
#elif WIN32
    HWND windowHandle = glfwGetWin32Window(data->window);
#endif
    glfwSetWindowSize(data->window, size.x, size.y);
    if(entity == PrimaryContext) {
        bgfx::reset(size.x, size.y, (vsync ? BGFX_RESET_VSYNC : 0) + (fullscreen ? BGFX_RESET_FULLSCREEN : 0));
    } else {
        bgfx::destroy(data->fb);
        data->fb = bgfx::createFrameBuffer(windowHandle, size.x, size.y);
    }
}

static void OnContextResized(Entity entity, v2i oldSize, v2i newSize) {
    if(HasBgfxContext(entity)) ResetContext(entity);
}

static void OnContextFlagChanged(Entity entity, bool oldValue, bool newValue) {
    ResetContext(entity);
}

static void OnContextTitleChanged(Entity entity, StringRef before, StringRef after) {
    auto data = GetBgfxContext(entity);

    glfwSetWindowTitle(data->window, after);
}

static void OnGlfwWindowResized(GLFWwindow *window, int w, int h) {
    SetRenderTargetSize((Entity)glfwGetWindowUserPointer(window), {w, h});
}

static void OnCharPressed(GLFWwindow *window, unsigned int c) {
    FireEvent(CharacterPressed, (Entity)glfwGetWindowUserPointer(window), c);
}

static void OnKey(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    auto context = (Entity)glfwGetWindowUserPointer(window);

    if(action == GLFW_PRESS) {
        SetKeyState(context, key, 1.0f);

        if(key == KEY_F9) {
            auto data = GetBgfxContext(context);
            data->debugFlags = (data->debugFlags + 1) % 2;
            bgfx::setDebug(data->debugFlags * 4);
        }

        if(key == KEY_F11) {
            SetContextFullscreen(context, !GetContextFullscreen(context));
        }

        if(key == KEY_F10) {
            SetContextVsync(context, !GetContextVsync(context));
        }
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
    auto cp = GetCursorPosition((Entity)glfwGetWindowUserPointer(window), 0);
    auto dx = x - cp.x;
    auto dy = y - cp.y;

    SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_DOWN, fmaxf(dy, 0.0f));
    SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_UP, fmaxf(-dy, 0.0f));
    SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_LEFT, fmaxf(-dx, 0.0f));
    SetKeyState((Entity)glfwGetWindowUserPointer(window), MOUSE_RIGHT, fmaxf(dx, 0.0f));

    SetCursorPosition((Entity)glfwGetWindowUserPointer(window), 0, {(int)x, (int)y});
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
    auto size = GetRenderTargetSize(entity);

    if(NumContexts == 1) {
        glfwInit();
        SubscribeAppUpdate(OnAppUpdate);
        SubscribeRenderTargetSizeChanged(OnContextResized);
        SubscribeContextTitleChanged(OnContextTitleChanged);
        SubscribeContextVsyncChanged(OnContextFlagChanged);
        SubscribeContextFullscreenChanged(OnContextFlagChanged);
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
#elif WIN32
    HWND windowHandle = glfwGetWin32Window(window);
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
#ifdef __APPLE__
        bgfx::init(bgfx::RendererType::OpenGL);
#endif

#ifdef WIN32
        bgfx::init(bgfx::RendererType::Direct3D11);
#endif
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
        AddExtension(TypeOf_OffscreenRenderTarget(), TypeOf_BgfxOffscreenRenderTarget());
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
        RemoveExtension(TypeOf_OffscreenRenderTarget(), TypeOf_BgfxOffscreenRenderTarget());

        bgfx::shutdown();
    }

    glfwDestroyWindow(data->window);

    if(NumContexts == 1) {
        UnsubscribeRenderTargetSizeChanged(OnContextResized);
        UnsubscribeAppUpdate(OnAppUpdate);
        UnsubscribeContextTitleChanged(OnContextTitleChanged);
        UnsubscribeContextVsyncChanged(OnContextFlagChanged);
        UnsubscribeContextFullscreenChanged(OnContextFlagChanged);
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
