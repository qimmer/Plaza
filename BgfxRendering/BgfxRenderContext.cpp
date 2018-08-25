//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxRenderContext.h"
#include "BgfxCommandList.h"
#include "BgfxMesh.h"
#include "BgfxTexture2D.h"
#include "BgfxUniform.h"
#include "BgfxShaderCache.h"
#include "BgfxOffscreenRenderTarget.h"
#include "BgfxModule.h"

#include <Rendering/CommandList.h>
#include <Rendering/ShaderCache.h>
#include <Rendering/Mesh.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Uniform.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture.h>
#include <Rendering/RenderContext.h>
#include <Rendering/Renderer.h>

#include <Core/Debug.h>
#include <Foundation/AppLoop.h>

#include <Input/InputContext.h>
#include <Input/Key.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cfloat>

#ifdef __APPLE__
extern "C" {

    typedef void* id;

    GLFWAPI id glfwGetCocoaWindow(GLFWwindow* window);
};
#endif

#ifdef WIN32
#undef GetHandle
#undef Enum
#include <windows.h>
#include <Core/Math.h>

#undef CreateService
#undef CreateEvent

extern "C" {
    GLFWAPI HWND glfwGetWin32Window(GLFWwindow *window);
}
#endif

struct BgfxRenderContext {
    GLFWwindow *window;
    bgfx::FrameBufferHandle fb;
    int debugFlags;
};

static u32 NumContexts = 0;
static Entity PrimaryContext = 0;

LocalFunction(OnAppUpdate, void, Entity appLoop) {
    auto owner = GetOwner(appLoop);
    auto contextData = GetBgfxRenderContextData(owner);
    auto moduleData = GetBgfxRenderingData(owner);

    if(moduleData) {
        for_entity(context, contextData, BgfxRenderContext) {
            SetInputStateValueByKey(context, MOUSE_SCROLL_DOWN, 0.0f);
            SetInputStateValueByKey(context, MOUSE_SCROLL_UP, 0.0f);
            SetInputStateValueByKey(context, MOUSE_SCROLL_LEFT, 0.0f);
            SetInputStateValueByKey(context, MOUSE_SCROLL_RIGHT, 0.0f);
            SetInputStateValueByKey(context, MOUSE_DOWN, 0.0f);
            SetInputStateValueByKey(context, MOUSE_UP, 0.0f);
            SetInputStateValueByKey(context, MOUSE_LEFT, 0.0f);
            SetInputStateValueByKey(context, MOUSE_RIGHT, 0.0f);

            if(glfwWindowShouldClose(contextData->window)) {
                SetInputStateValueByKey(context, KEY_WINDOW_CLOSE, 1.0f);
                SetInputStateValueByKey(context, KEY_WINDOW_CLOSE, 0.0f);
            }
        }

        glfwPollEvents();

        auto viewId = 0;

        for_entity(commandList, commandListData, BgfxCommandList) {
            RenderCommandList(commandList, viewId);
            viewId++;
        }

        auto frame = bgfx::frame();
    }
}

static void ResetContext(Entity entity) {
    auto data = GetBgfxRenderContextData(entity);
    auto size = GetRenderTargetSize(entity);
    auto vsync = GetRenderContextVsync(entity);
    auto fullscreen = GetRenderContextFullscreen(entity);

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

LocalFunction(OnContextResized, void, Entity entity, v2i oldSize, v2i newSize) {
    if(HasComponent(entity, ComponentOf_BgfxRenderContext())) ResetContext(entity);
}

LocalFunction(OnContextFlagChanged, void, Entity entity, bool oldValue, bool newValue) {
    ResetContext(entity);
}

LocalFunction(OnContextTitleChanged, void, Entity entity, StringRef before, StringRef after) {
    auto data = GetBgfxRenderContextData(entity);

    glfwSetWindowTitle(data->window, after);
}

LocalFunction(OnGlfwWindowResized, void, GLFWwindow *window, int w, int h) {
    SetRenderTargetSize((Entity)glfwGetWindowUserPointer(window), {w, h});
}

LocalFunction(OnCharPressed, void, GLFWwindow *window, unsigned int c) {
    auto entity = (Entity)glfwGetWindowUserPointer(window);

    SetInputContextLastCharacter(entity, c);
}

LocalFunction(OnKey, void, GLFWwindow *window, int key, int scanCode, int action, int mods) {
    auto context = (Entity)glfwGetWindowUserPointer(window);

    if(action == GLFW_PRESS) {
        SetInputStateValueByKey(context, key, 1.0f);
    }

    if(action == GLFW_RELEASE) {
        SetInputStateValueByKey(context, key, 0.0f);
    }
}

LocalFunction(OnMouseScroll, void, GLFWwindow *window, double x, double y) {
    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_DOWN, fmaxf(-y, 0.0f));
    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_UP, fmaxf(y, 0.0f));
    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_LEFT, fmaxf(-x, 0.0f));
    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_SCROLL_RIGHT, fmaxf(x, 0.0f));
}

LocalFunction(OnMouseMove, void, GLFWwindow *window, double x, double y) {
    auto cp = GetInputContextCursorPosition((Entity)glfwGetWindowUserPointer(window));
    auto dx = x - cp.x;
    auto dy = y - cp.y;

    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_DOWN, fmaxf(dy, 0.0f));
    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_UP, fmaxf(-dy, 0.0f));
    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_LEFT, fmaxf(-dx, 0.0f));
    SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSE_RIGHT, fmaxf(dx, 0.0f));

    SetInputContextCursorPosition((Entity)glfwGetWindowUserPointer(window), {(int)x, (int)y});
}

LocalFunction(OnMouseButton, void, GLFWwindow *window, int button, int action, int mods) {
    if(action == GLFW_PRESS) {
        SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSEBUTTON_0 + button, 1.0f);
    }

    if(action == GLFW_RELEASE) {
        SetInputStateValueByKey((Entity)glfwGetWindowUserPointer(window), MOUSEBUTTON_0 + button, 0.0f);
    }
}

LocalFunction(OnBgfxRenderContextAdded, void, Entity entity) {
    auto data = GetBgfxRenderContextData(entity);
    auto size = GetRenderTargetSize(entity);

    if(NumContexts == 1) {
        glfwInit();

        SetAppLoopDisabled(GetBgfxRenderingLoop(ModuleOf_BgfxRendering()), false);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto title = GetRenderContextTitle(entity);
    auto window = glfwCreateWindow(Max(size.x, 32), Max(size.y, 32), title ? title : "", NULL, NULL);
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
        bgfx::init(bgfx::RendererType::Metal);
#endif

#ifdef WIN32
        bgfx::init();
#endif
        bgfx::reset(size.x, size.y);

        PrimaryContext = entity;

        for_children(extension, Extensions, ModuleOf_BgfxRendering()) {
            SetExtensionDisabled(extension, false);
        }

    } else {
        data->fb = bgfx::createFrameBuffer(windowHandle, size.x, size.y);
    }
}

LocalFunction(OnBgfxRenderContextRemoved, void, Entity entity) {
    auto data = GetBgfxRenderContextData(entity);

    if(NumContexts > 1) {
        bgfx::destroy(data->fb);
        SetAppLoopDisabled(GetBgfxRenderingLoop(ModuleOf_BgfxRendering()), true);
    }

    if(NumContexts == 1) {
        for_children(extension, Extensions, ModuleOf_BgfxRendering()) {
            SetExtensionDisabled(extension, true);
        }

        bgfx::shutdown();
    }

    glfwDestroyWindow(data->window);

    if(NumContexts == 1) {
        glfwTerminate();

        PrimaryContext = 0;
    }
}

LocalFunction(OnContextAdded, void, Entity entity) {
    NumContexts++;
}

LocalFunction(OnContextRemoved, void, Entity entity) {
    NumContexts--;
}

LocalFunction(OnContextGrabMouseChanged, void, Entity entity, bool oldValue, bool newValue) {
    auto data = GetBgfxRenderContextData(entity);
    if(data) {
        glfwSetInputMode(data->window, GLFW_CURSOR, newValue ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

BeginUnit(BgfxRenderContext)
    BeginComponent(BgfxRenderContext)
        RegisterBase(RenderContext)
        RegisterBase(InputContext)
    EndComponent()

    RegisterSubscription(EntityComponentAdded, OnContextAdded, ComponentOf_RenderContext())
    RegisterSubscription(EntityComponentAdded, OnBgfxRenderContextAdded, ComponentOf_BgfxRenderContext())
    RegisterSubscription(EntityComponentRemoved, OnContextRemoved, ComponentOf_RenderContext())
    RegisterSubscription(EntityComponentRemoved, OnBgfxRenderContextRemoved, ComponentOf_BgfxRenderContext())

    RegisterSubscription(AppLoopFrameChanged, OnAppUpdate, 0)
    RegisterSubscription(RenderTargetSizeChanged, OnContextResized, 0)
    RegisterSubscription(RenderContextTitleChanged, OnContextTitleChanged, 0)
    RegisterSubscription(RenderContextVsyncChanged, OnContextFlagChanged, 0)
    RegisterSubscription(RenderContextFullscreenChanged, OnContextFlagChanged, 0)
    RegisterSubscription(InputContextGrabMouseChanged, OnContextGrabMouseChanged, 0)
EndUnit()
