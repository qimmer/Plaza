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
#include "BgfxTextureReadBack.h"

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
#include <Rendering/TextureReadBack.h>
#include <Foundation/MemoryStream.h>

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

static u32 NumContexts = 0, Frame = 0;
static Entity PrimaryContext = 0;

bgfx::UniformHandle u_uvOffsetSizePerSampler;

static void UpdateTextureReadBack(Entity readBack, TextureReadBack *data) {
    auto bgfxData = GetBgfxTextureReadBackData(readBack);

    if(Frame >= bgfxData->ReadyFrame) {
        auto sourceHandle = GetBgfxResourceHandle(data->TextureReadBackSourceTexture);
        auto blitHandle = GetBgfxResourceHandle(data->TextureReadBackBlitTexture);
        auto bufferData = GetMemoryStreamBuffer(data->TextureReadBackBuffer);

        if(HasComponent(GetOwner(data->TextureReadBackSourceTexture), ComponentOf_OffscreenRenderTarget())) {
            SetTextureSize2D(data->TextureReadBackBlitTexture, GetTextureSize2D(data->TextureReadBackSourceTexture));
            SetTextureFormat(data->TextureReadBackBlitTexture, GetTextureFormat(data->TextureReadBackSourceTexture));
            SetTextureFlag(data->TextureReadBackBlitTexture, TextureFlag_BLIT_DST | TextureFlag_READ_BACK);

            bgfx::blit(255, bgfx::TextureHandle {blitHandle}, 0, 0, bgfx::TextureHandle {sourceHandle});
            bgfxData->ReadyFrame = bgfx::readTexture(bgfx::TextureHandle {blitHandle}, bufferData);
        } else {
            bgfxData->ReadyFrame = bgfx::readTexture(bgfx::TextureHandle {sourceHandle}, bufferData);
        }
    }
}

LocalFunction(OnResourceSubmission, void, Entity appLoop) {
    Validate(ComponentOf_Texture());
    Validate(ComponentOf_OffscreenRenderTarget());
    Validate(ComponentOf_BinaryProgram());
    Validate(ComponentOf_Uniform());
    Validate(ComponentOf_VertexDeclaration());
    Validate(ComponentOf_Mesh());
    Validate(ComponentOf_VertexBuffer());
    Validate(ComponentOf_IndexBuffer());
}

LocalFunction(OnTextureReadBack, void, Entity appLoop) {
    for_entity(readBack, data, TextureReadBack) {
        UpdateTextureReadBack(readBack, data);
    }
}

LocalFunction(OnInputPoll, void, Entity appLoop) {
    auto numContexts = 0;

    for_entity(context, contextData, BgfxRenderContext) {
        numContexts++;

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

        auto deadZone = GetInputContextDeadZone(context);

        for(auto i = 0; i < GLFW_JOYSTICK_LAST; ++i) {
            if(!glfwJoystickPresent(i)) {
                SetInputStateValueByKey(context, GAMEPAD_CONNECTED + i * GAMEPAD_MULTIPLIER, 0.0f);
                continue;
            }

            SetInputStateValueByKey(context, GAMEPAD_CONNECTED + i * GAMEPAD_MULTIPLIER, 1.0f);

            s32 numAxises;
            s32 numButtons;
            s32 numHats;
            const float* axisValues = glfwGetJoystickAxes(i, &numAxises);
            auto *filteredAxisValues = (float*)alloca(numAxises * sizeof(float));
            memcpy(filteredAxisValues, axisValues, numAxises * sizeof(float));
            const unsigned char* buttonStates = glfwGetJoystickButtons(i, &numButtons);
            const unsigned char* hatStates = glfwGetJoystickHats(i, &numHats);

            for(auto j = 0; j < numAxises; ++j) {
                if(fabsf(filteredAxisValues[j]) < deadZone) {
                    filteredAxisValues[j] = 0.0f;
                }
            }

            for(auto j = 0; j < numButtons; ++j) {
                SetInputStateValueByKey(context, GAMEPAD_A + j + (GAMEPAD_MULTIPLIER * i), buttonStates[j] ? 1.0f : 0.0f);
            }

            for(auto j = 0; j < Min(numAxises, 4); ++j) {
                SetInputStateValueByKey(context, GAMEPAD_LS_LEFT + (j*2+0) + (GAMEPAD_MULTIPLIER * i), Max(-filteredAxisValues[j], 0.0f));
                SetInputStateValueByKey(context, GAMEPAD_LS_LEFT + (j*2+1) + (GAMEPAD_MULTIPLIER * i), Max(filteredAxisValues[j], 0.0f));
            }

            if(numAxises > 5) {
                SetInputStateValueByKey(context, GAMEPAD_LT + (GAMEPAD_MULTIPLIER * i), filteredAxisValues[4]);
                SetInputStateValueByKey(context, GAMEPAD_RT + (GAMEPAD_MULTIPLIER * i), filteredAxisValues[5]);
            }

            if(numHats) {
                SetInputStateValueByKey(context, GAMEPAD_DPAD_LEFT + (GAMEPAD_MULTIPLIER * i), (hatStates[0] & GLFW_HAT_LEFT) ? 1.0f : 0.0f);
                SetInputStateValueByKey(context, GAMEPAD_DPAD_RIGHT + (GAMEPAD_MULTIPLIER * i), (hatStates[0] & GLFW_HAT_RIGHT) ? 1.0f : 0.0f);
                SetInputStateValueByKey(context, GAMEPAD_DPAD_UP+ (GAMEPAD_MULTIPLIER * i), (hatStates[0] & GLFW_HAT_UP) ? 1.0f : 0.0f);
                SetInputStateValueByKey(context, GAMEPAD_DPAD_DOWN + (GAMEPAD_MULTIPLIER * i), (hatStates[0] & GLFW_HAT_DOWN) ? 1.0f : 0.0f);
            }
        }
    }

    if(numContexts) {
        glfwPollEvents();
    }
}

LocalFunction(OnPresent, void, Entity appLoop) {
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
    }

    if(numContexts) {
        auto viewId = 0;

        bgfx::setDebug(debugFlags);
        Frame = bgfx::frame();
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
            if(bgfx::isValid(data->fb)) {
                bgfx::destroy(data->fb);
            }

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
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));
    SetRenderTargetSize(entity, {w, h});
}

LocalFunction(OnCharPressed, void, GLFWwindow *window, unsigned int c) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    SetInputContextLastCharacter(entity, c);
}

LocalFunction(OnKey, void, GLFWwindow *window, int key, int scanCode, int action, int mods) {
    auto context = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    if(action == GLFW_PRESS) {
        SetInputStateValueByKey(context, key, 1.0f);
    }

    if(action == GLFW_RELEASE) {
        SetInputStateValueByKey(context, key, 0.0f);
    }
}

LocalFunction(OnMouseScroll, void, GLFWwindow *window, double x, double y) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));
    
    SetInputStateValueByKey(entity, MOUSE_SCROLL_DOWN, fmaxf(-y, 0.0f));
    SetInputStateValueByKey(entity, MOUSE_SCROLL_UP, fmaxf(y, 0.0f));
    SetInputStateValueByKey(entity, MOUSE_SCROLL_LEFT, fmaxf(-x, 0.0f));
    SetInputStateValueByKey(entity, MOUSE_SCROLL_RIGHT, fmaxf(x, 0.0f));
}

LocalFunction(OnMouseMove, void, GLFWwindow *window, double x, double y) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    auto cp = GetInputContextCursorPosition(entity);
    auto dx = x - cp.x;
    auto dy = y - cp.y;

    SetInputStateValueByKey(entity, MOUSE_DOWN, fmaxf(dy, 0.0f));
    SetInputStateValueByKey(entity, MOUSE_UP, fmaxf(-dy, 0.0f));
    SetInputStateValueByKey(entity, MOUSE_LEFT, fmaxf(-dx, 0.0f));
    SetInputStateValueByKey(entity, MOUSE_RIGHT, fmaxf(dx, 0.0f));

    SetInputContextCursorPosition(entity, {(int)x, (int)y});
}

LocalFunction(OnMouseButton, void, GLFWwindow *window, int button, int action, int mods) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    if(action == GLFW_PRESS) {
        SetInputStateValueByKey(entity, MOUSEBUTTON_0 + button, 1.0f);
    }

    if(action == GLFW_RELEASE) {
        SetInputStateValueByKey(entity, MOUSEBUTTON_0 + button, 0.0f);
    }
}

LocalFunction(OnBgfxRenderContextAdded, void, Entity component, Entity entity) {
    auto data = GetBgfxRenderContextData(entity);
    auto size = GetRenderTargetSize(entity);

    if(NumContexts == 1) {
        glfwInit();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, false);

    auto title = GetRenderContextTitle(entity);
    auto window = glfwCreateWindow(Max(size.x, 32), Max(size.y, 32), title ? title : "", NULL, NULL);
    auto monitor = glfwGetPrimaryMonitor();
    glfwSetWindowSizeCallback(window, OnGlfwWindowResized);
    glfwSetWindowUserPointer(window, (void*)(size_t)GetEntityIndex(entity));
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
        bgfx::setPlatformData(pd);

        bgfx::Init init;

#ifdef __APPLE__
        bgfx::init(init);
#endif

#ifdef WIN32
        bgfx::init(init);
#endif

#ifdef __linux__
        bgfx::init();
#endif

        bgfx::reset(size.x, size.y);

        u_uvOffsetSizePerSampler = bgfx::createUniform("u_uvOffsetSizePerSampler", bgfx::UniformType::Vec4, 8);

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

        bgfx::destroy(u_uvOffsetSizePerSampler);

        bgfx::shutdown();
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

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnInputPoll, AppLoopOf_InputPoll())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnResourceSubmission, AppLoopOf_ResourceSubmission())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnTextureReadBack, AppLoopOf_ResourceDownload())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnPresent, AppLoopOf_Present())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderTargetSize()), OnContextResized, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextTitle()), OnContextTitleChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextVsync()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextShowDebug()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextShowStats()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderContextFullscreen()), OnContextFlagChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InputContextGrabMouse()), OnContextGrabMouseChanged, 0)
EndUnit()
