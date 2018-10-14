//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxRenderContext.h"
#include "BgfxCommandList.h"
#include "BgfxMesh.h"
#include "BgfxTexture2D.h"
#include "BgfxUniform.h"
#include "BgfxResource.h"
#include "BgfxShaderCache.h"
#include "BgfxOffscreenRenderTarget.h"
#include "BgfxModule.h"

#include <Rendering/ShaderCache.h>
#include <Rendering/Mesh.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Uniform.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture.h>
#include <Rendering/RenderContext.h>

#include <Scene/Transform.h>

#include <Core/Debug.h>
#include <Foundation/AppLoop.h>
#include <Foundation/Invalidation.h>

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

#ifdef __linux__
#include <X11/Xlib.h>
#include <Core/Math.h>

Display glfwGetX11Display(GLFWmonitor *monitor);
Window glfwGetX11Window(GLFWwindow *window);

#endif

#ifdef WIN32
#undef GetHandle
#undef Enum
#include <windows.h>
#include <Core/Algorithms.h>

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

static void ValidateResources() {
    Validate(ComponentOf_Texture());
    Validate(ComponentOf_OffscreenRenderTarget());
    Validate(ComponentOf_BinaryProgram());
    Validate(ComponentOf_Uniform());
    Validate(ComponentOf_VertexDeclaration());
    Validate(ComponentOf_Mesh());
    Validate(ComponentOf_VertexBuffer());
    Validate(ComponentOf_IndexBuffer());
    Validate(ComponentOf_Transform());
}

LocalFunction(OnAppUpdate, void, Entity appLoop) {
    auto owner = GetOwner(appLoop);
    auto moduleData = GetBgfxRenderingData(owner);

    if(moduleData) {
        auto numContexts = 0;
        auto debugFlags = 0;

        for_entity(context, contextData, BgfxRenderContext) {
            numContexts++;

            if(GetRenderContextShowDebug(context)) {
                debugFlags |= BGFX_DEBUG_IFH | BGFX_DEBUG_STATS;
            }
            if(GetRenderContextShowStats(context)) {
                debugFlags |= BGFX_DEBUG_STATS;
            }

            SetInputStateValueByKey(context, MOUSE_SCROLL_DOWN, 0.0f);
            SetInputStateValueByKey(context, MOUSE_SCROLL_UP, 0.0f);
            SetInputStateValueByKey(context, MOUSE_SCROLL_LEFT, 0.0f);
            SetInputStateValueByKey(context, MOUSE_SCROLL_RIGHT, 0.0f);
            SetInputStateValueByKey(context, MOUSE_DOWN, 0.0f);
            SetInputStateValueByKey(context, MOUSE_UP, 0.0f);
            SetInputStateValueByKey(context, MOUSE_LEFT, 0.0f);
            SetInputStateValueByKey(context, MOUSE_RIGHT, 0.0f);

            if(glfwWindowShouldClose(contextData->window)) {
                glfwSetWindowShouldClose(contextData->window, false);
                SetInputStateValueByKey(context, KEY_WINDOW_CLOSE, 1.0f);
            } else {
                SetInputStateValueByKey(context, KEY_WINDOW_CLOSE, 0.0f);
            }

            if(!glfwGetWindowAttrib(contextData->window, GLFW_VISIBLE)) {
                glfwShowWindow(contextData->window);
            }
        }

        if(numContexts) {
            glfwPollEvents();

            auto viewId = 0;

            ValidateResources();

            for_entity(commandList, commandListData, BgfxCommandList) {
                RenderCommandList(commandList, viewId);
                viewId++;
            }

            bgfx::setDebug(debugFlags);
            auto frame = bgfx::frame();
        }
    }
}

static void ResetContext(Entity entity) {
    auto data = GetBgfxRenderContextData(entity);
    if(data) {
        auto size = GetRenderTargetSize(entity);
        auto vsync = GetRenderContextVsync(entity);
        auto fullscreen = GetRenderContextFullscreen(entity);

#ifdef __APPLE__
        id windowHandle = glfwGetCocoaWindow(data->window);
#elif WIN32
        HWND windowHandle = glfwGetWin32Window(data->window);
#elif __linux__
        auto windowHandle = (void*)glfwGetX11Window(data->window);
#endif
        glfwSetWindowSize(data->window, size.x, size.y);
        if(entity == PrimaryContext) {
            bgfx::reset(size.x, size.y, (vsync ? BGFX_RESET_VSYNC : 0) + (fullscreen ? BGFX_RESET_FULLSCREEN : 0));
        } else {
            bgfx::destroy(data->fb);
            data->fb = bgfx::createFrameBuffer(windowHandle, size.x, size.y);
        }
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

    if(data) {
        glfwSetWindowTitle(data->window, after);
    }
}

LocalFunction(OnGlfwWindowResized, void, GLFWwindow *window, int w, int h) {
    SetRenderTargetSize(*(Entity*)glfwGetWindowUserPointer(window), {w, h});
}

LocalFunction(OnCharPressed, void, GLFWwindow *window, unsigned int c) {
    auto entity = *(Entity*)glfwGetWindowUserPointer(window);

    SetInputContextLastCharacter(entity, c);
}

LocalFunction(OnKey, void, GLFWwindow *window, int key, int scanCode, int action, int mods) {
    auto context = *(Entity*)glfwGetWindowUserPointer(window);

    if(action == GLFW_PRESS) {
        SetInputStateValueByKey(context, key, 1.0f);
    }

    if(action == GLFW_RELEASE) {
        SetInputStateValueByKey(context, key, 0.0f);
    }
}

LocalFunction(OnMouseScroll, void, GLFWwindow *window, double x, double y) {
    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_SCROLL_DOWN, fmaxf(-y, 0.0f));
    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_SCROLL_UP, fmaxf(y, 0.0f));
    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_SCROLL_LEFT, fmaxf(-x, 0.0f));
    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_SCROLL_RIGHT, fmaxf(x, 0.0f));
}

LocalFunction(OnMouseMove, void, GLFWwindow *window, double x, double y) {
    auto cp = GetInputContextCursorPosition(*(Entity*)glfwGetWindowUserPointer(window));
    auto dx = x - cp.x;
    auto dy = y - cp.y;

    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_DOWN, fmaxf(dy, 0.0f));
    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_UP, fmaxf(-dy, 0.0f));
    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_LEFT, fmaxf(-dx, 0.0f));
    SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSE_RIGHT, fmaxf(dx, 0.0f));

    SetInputContextCursorPosition(*(Entity*)glfwGetWindowUserPointer(window), {(int)x, (int)y});
}

LocalFunction(OnMouseButton, void, GLFWwindow *window, int button, int action, int mods) {
    if(action == GLFW_PRESS) {
        SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSEBUTTON_0 + button, 1.0f);
    }

    if(action == GLFW_RELEASE) {
        SetInputStateValueByKey(*(Entity*)glfwGetWindowUserPointer(window), MOUSEBUTTON_0 + button, 0.0f);
    }
}

LocalFunction(OnBgfxRenderContextAdded, void, Entity component, Entity entity) {
    auto data = GetBgfxRenderContextData(entity);
    auto size = GetRenderTargetSize(entity);

    if(NumContexts == 1) {
        ProfileStart("glfwInit", 150.0);
        glfwInit();
        ProfileEnd();

        SetAppLoopDisabled(GetBgfxRenderingLoop(ModuleOf_BgfxRendering()), false);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, false);

    auto title = GetRenderContextTitle(entity);
    auto window = glfwCreateWindow(Max(size.x, 32), Max(size.y, 32), title ? title : "", NULL, NULL);
    auto monitor = glfwGetPrimaryMonitor();
    glfwSetWindowSizeCallback(window, OnGlfwWindowResized);
    glfwSetWindowUserPointer(window, (void*)&(((Entity*)data)[-2]));
    glfwSetCharCallback(window, OnCharPressed);
    glfwSetKeyCallback(window, OnKey);
    glfwSetScrollCallback(window, OnMouseScroll);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButton);

    data->window = window;

#ifdef __APPLE__
    void* displayHandle = NULL;
    id windowHandle = glfwGetCocoaWindow(window);
#elif WIN32
    void* displayHandle = NULL;
    HWND windowHandle = glfwGetWin32Window(window);
#elif __linux__
    void* displayHandle = NULL;
    auto windowHandle = (void*)glfwGetX11Window(window);
#endif

    if(NumContexts == 1) {
        data->fb = BGFX_INVALID_HANDLE;

        bgfx::PlatformData pd;
        pd.ndt = displayHandle;
        pd.nwh = windowHandle;
        pd.context = NULL;
        pd.backBuffer = NULL;
        pd.backBufferDS = NULL;
        pd.session = NULL;
        bgfx::setPlatformData(pd);
#ifdef __APPLE__
        bgfx::init(bgfx::RendererType::Metal);
#endif

        ProfileStart("bgfx::init", 150.0);
#ifdef WIN32
        bgfx::init();
#endif

#ifdef __linux__
        bgfx::init();
#endif

        ProfileEnd();

        bgfx::reset(size.x, size.y);

        PrimaryContext = entity;

        for_children(extension, Extensions, ModuleOf_BgfxRendering()) {
            SetExtensionDisabled(extension, false);
        }

    } else {
        data->fb = bgfx::createFrameBuffer(windowHandle, size.x, size.y);
    }
}

LocalFunction(OnBgfxRenderContextRemoved, void, Entity component, Entity entity) {
    auto data = GetBgfxRenderContextData(entity);

    if(bgfx::isValid(data->fb)) {
        bgfx::destroy(data->fb);
    }

    if(NumContexts == 1) {
        for_children(extension, Extensions, ModuleOf_BgfxRendering()) {
            SetExtensionDisabled(extension, true);
        }

        bgfx::shutdown();

        SetAppLoopDisabled(GetBgfxRenderingLoop(ModuleOf_BgfxRendering()), true);
    }

    glfwDestroyWindow(data->window);

    if(NumContexts == 1) {
        glfwTerminate();

        PrimaryContext = 0;
    }
}

LocalFunction(OnContextAdded, void, Entity component, Entity entity) {
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
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnContextAdded, ComponentOf_RenderContext())
    RegisterSubscription(EventOf_EntityComponentAdded(), OnBgfxRenderContextAdded, ComponentOf_BgfxRenderContext())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnContextRemoved, ComponentOf_RenderContext())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnBgfxRenderContextRemoved, ComponentOf_BgfxRenderContext())

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnAppUpdate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderTargetSize()), OnContextResized, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextTitle()), OnContextTitleChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextVsync()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextShowDebug()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextShowStats()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextFullscreen()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InputContextGrabMouse()), OnContextGrabMouseChanged, 0)
EndUnit()
