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
#include <Rendering/RenderingModule.h>

#include <Scene/Transform.h>

#include <Core/Debug.h>
#include <Foundation/AppLoop.h>

#include <Input/InputContext.h>
#include <Input/Key.h>
#include <Input/InputModule.h>

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
    float keyStates[1024];
    GLFWwindow *window;
    bgfx::FrameBufferHandle fb;
    int debugFlags;
};

static u32 NumContexts = 0, Frame = 0;
static Entity PrimaryContext = 0;
static bool isInsidePoll = false;

bgfx::UniformHandle u_uvOffsetSizePerSampler;

static void InitContext(Entity entity);

static void UpdateTextureReadBack(Entity readBack, const TextureReadBack& data) {
    auto bgfxData = GetBgfxTextureReadBack(readBack);

    if(Frame >= bgfxData.ReadyFrame) {
        auto sourceHandle = GetBgfxResource(data.TextureReadBackSourceTexture).BgfxResourceHandle;
        auto blitHandle = GetBgfxResource(data.TextureReadBackBlitTexture).BgfxResourceHandle;
        auto bufferData = GetMemoryStreamBuffer(data.TextureReadBackBuffer);

        if(HasComponent(GetOwnership(data.TextureReadBackSourceTexture).Owner, ComponentOf_OffscreenRenderTarget())) {
            
            SetTexture2D(data.TextureReadBackBlitTexture, {GetTexture2D(data.TextureReadBackSourceTexture).TextureSize2D});
            auto textureData = GetTexture(data.TextureReadBackSourceTexture);
            textureData.TextureFlag = TextureFlag_BLIT_DST | TextureFlag_READ_BACK;
            textureData.TextureMipLevels = 0;
            textureData.TextureDynamic = false;
            SetTexture(data.TextureReadBackBlitTexture, textureData);

            bgfx::blit(255, bgfx::TextureHandle {blitHandle}, 0, 0, bgfx::TextureHandle {sourceHandle});
            bgfxData.ReadyFrame = bgfx::readTexture(bgfx::TextureHandle {blitHandle}, bufferData);
        } else {
            bgfxData.ReadyFrame = bgfx::readTexture(bgfx::TextureHandle {sourceHandle}, bufferData);
        }

        SetBgfxTextureReadBack(readBack, bgfxData);
    }
}

static void ResetContext(Entity entity) {
    auto data = GetBgfxRenderContext(entity);
    if(!data.window) {
        InitContext(entity);
        data = GetBgfxRenderContext(entity);
    }

    auto size = GetRenderTarget(entity).RenderTargetSize;
    auto vsync = GetRenderContext(entity).RenderContextVsync;
    auto fullscreen = GetRenderContext(entity).RenderContextFullscreen;

#ifdef __APPLE__
    id windowHandle = glfwGetCocoaWindow(data.window);
#elif WIN32
    HWND windowHandle = glfwGetWin32Window(data.window);
#elif __linux__
    auto windowHandle = (void*)glfwGetX11Window(data.window);
#endif
    glfwSetWindowSize(data.window, size.x, size.y);
    if(entity == PrimaryContext) {
        bgfx::reset(size.x, size.y, (vsync ? BGFX_RESET_VSYNC : 0) + (fullscreen ? BGFX_RESET_FULLSCREEN : 0));
    } else {
        if(bgfx::isValid(data.fb)) {
            bgfx::destroy(data.fb);
        }

        data.fb = bgfx::createFrameBuffer(windowHandle, size.x, size.y);
    }

    glfwSetWindowTitle(data.window, GetRenderContext(entity).RenderContextTitle);
}

static void DestroyContext(Entity entity) {
    auto data = GetBgfxRenderContext(entity);

    if(bgfx::isValid(data.fb)) {
        bgfx::destroy(data.fb);
    }

    if(NumContexts == 1) {
        auto moduleData = GetModule(ModuleOf_BgfxRendering());
        for(auto extension : moduleData.Extensions) {
            auto extensionData = GetExtension(extension);
            extensionData.ExtensionDisabled = true;
            SetExtension(extension, extensionData);
        }

        bgfx::destroy(u_uvOffsetSizePerSampler);

        bgfx::shutdown();
    }

    glfwDestroyWindow(data.window);

    if(NumContexts == 1) {
        glfwTerminate();

        PrimaryContext = 0;
    }

    NumContexts--;
}

static void OnInputContextChanged(Entity entity, const InputContext& oldData, const InputContext& newData) {
    if(oldData.InputContextGrabMouse != newData.InputContextGrabMouse) {
        auto data = GetBgfxRenderContext(entity);
        glfwSetInputMode(data.window, GLFW_CURSOR, newData.InputContextGrabMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

static void OnTextureReadBack(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    TextureReadBack data;
    for_entity_data(readBack, ComponentOf_TextureReadBack(), &data) {
        UpdateTextureReadBack(readBack, data);
    }
}

static void OnInputPoll(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    if(isInsidePoll) return;

    auto numContexts = 0;
    isInsidePoll = true;

    BgfxRenderContext contextData;
    for_entity_data(context, ComponentOf_BgfxRenderContext(), &contextData) {
        if(!contextData.window) {
            InitContext(context);
            contextData = GetBgfxRenderContext(context);
        }
        numContexts++;

        contextData.keyStates[MOUSE_SCROLL_DOWN] = 0.0f;
        contextData.keyStates[MOUSE_SCROLL_UP] = 0.0f;
        contextData.keyStates[MOUSE_SCROLL_LEFT] = 0.0f;
        contextData.keyStates[MOUSE_SCROLL_RIGHT] = 0.0f;
        contextData.keyStates[MOUSE_DOWN] = 0.0f;
        contextData.keyStates[MOUSE_UP] = 0.0f;
        contextData.keyStates[MOUSE_LEFT] = 0.0f;
        contextData.keyStates[MOUSE_RIGHT] = 0.0f;

        auto inputContextData = GetInputContext(context);

        if(glfwWindowShouldClose(contextData.window)) {
            glfwSetWindowShouldClose(contextData.window, false);
            contextData.keyStates[KEY_WINDOW_CLOSE] = 1.0f;
        } else {
            contextData.keyStates[KEY_WINDOW_CLOSE] = 0.0f;
        }

        if(!glfwGetWindowAttrib(contextData.window, GLFW_VISIBLE)) {
            glfwShowWindow(contextData.window);
        }

        auto deadZone = inputContextData.InputContextDeadZone;

        for(auto i = 0; i < GLFW_JOYSTICK_LAST; ++i) {
            if(!glfwJoystickPresent(i)) {
                contextData.keyStates[GAMEPAD_CONNECTED + i * GAMEPAD_MULTIPLIER] = 0.0f;
                continue;
            }

            contextData.keyStates[GAMEPAD_CONNECTED + i * GAMEPAD_MULTIPLIER] = 1.0f;

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
                contextData.keyStates[GAMEPAD_A + j + (GAMEPAD_MULTIPLIER * i)] = buttonStates[j] ? 1.0f : 0.0f;
            }

            for(auto j = 0; j < Min(numAxises, 4); ++j) {
                contextData.keyStates[GAMEPAD_LS_LEFT + (j*2+0) + (GAMEPAD_MULTIPLIER * i)] = Max(-filteredAxisValues[j], 0.0f);
                contextData.keyStates[GAMEPAD_LS_LEFT + (j*2+1) + (GAMEPAD_MULTIPLIER * i)] = Max(filteredAxisValues[j], 0.0f);
            }

            if(numAxises > 5) {
                contextData.keyStates[GAMEPAD_LT + (GAMEPAD_MULTIPLIER * i)] = filteredAxisValues[4];
                contextData.keyStates[GAMEPAD_RT + (GAMEPAD_MULTIPLIER * i)] = filteredAxisValues[5];
            }

            if(numHats) {
                contextData.keyStates[GAMEPAD_DPAD_LEFT + (GAMEPAD_MULTIPLIER * i)] = (hatStates[0] & GLFW_HAT_LEFT) ? 1.0f : 0.0f;
                contextData.keyStates[GAMEPAD_DPAD_RIGHT + (GAMEPAD_MULTIPLIER * i)] = (hatStates[0] & GLFW_HAT_RIGHT) ? 1.0f : 0.0f;
                contextData.keyStates[GAMEPAD_DPAD_UP+ (GAMEPAD_MULTIPLIER * i)] = (hatStates[0] & GLFW_HAT_UP) ? 1.0f : 0.0f;
                contextData.keyStates[GAMEPAD_DPAD_DOWN + (GAMEPAD_MULTIPLIER * i)] = (hatStates[0] & GLFW_HAT_DOWN) ? 1.0f : 0.0f;
            }
        }

        SetBgfxRenderContext(context, contextData);
    }

    if(numContexts) {
        glfwPollEvents();
    }

    for_entity_data(context, ComponentOf_BgfxRenderContext(), &contextData) {
        auto inputContextData = GetInputContext(context);

        for(auto inputState : inputContextData.InputContextStates) {
            auto inputStateData = GetInputState(inputState);
            auto actualState = contextData.keyStates[inputStateData.InputStateKey];
            if(actualState != inputStateData.InputStateValue) {
                inputStateData.InputStateValue = actualState;
                SetInputState(inputState, inputStateData);
            }
        }
    }


    isInsidePoll = false;
}

static void OnPresent(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    auto numContexts = 0;
    auto debugFlags = 0;

    BgfxRenderContext bgfxContextData;
    for_entity_data(context, ComponentOf_BgfxRenderContext(), &bgfxContextData) {
        if(!bgfxContextData.window) {
            InitContext(context);
            bgfxContextData = GetBgfxRenderContext(context);
        }

        numContexts++;

        auto contextData = GetRenderContext(context);

        if(contextData.RenderContextShowDebug) {
            debugFlags |= BGFX_DEBUG_IFH | BGFX_DEBUG_STATS;
        }
        if(contextData.RenderContextShowStats) {
            debugFlags |= BGFX_DEBUG_STATS;
        }
    }

    if(numContexts) {
        auto viewId = 0;

        bgfx::setDebug(debugFlags);
        Frame = bgfx::frame();
    }
}

static void OnRenderTargetChanged(Entity entity, const RenderTarget& oldData, const RenderTarget& newData) {
    if(HasComponent(entity, ComponentOf_BgfxRenderContext())) ResetContext(entity);
}

static void OnRenderContextChanged(Entity entity, const RenderContext& oldData, const RenderContext& newData) {
    if(HasComponent(entity, ComponentOf_BgfxRenderContext())) ResetContext(entity);
}

static void OnGlfwWindowResized(GLFWwindow *window, int w, int h) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));
    SetRenderTarget(entity, {{w, h}});
}

static void OnCharPressed(GLFWwindow *window, unsigned int c) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    auto data = GetInputContext(entity);
    data.InputContextLastCharacter = c;
    SetInputContext(entity, data);
}

static void OnGlfwFramebufferResized(GLFWwindow *window, int w, int h) {

}

static void OnGlfwWindowRefresh(GLFWwindow *window) {

}

static void OnKey(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    auto context = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    auto contextData = GetBgfxRenderContext(context);
    if(action == GLFW_PRESS) {
        contextData.keyStates[key] = 1.0f;
        SetBgfxRenderContext(context, contextData);
    }

    if(action == GLFW_RELEASE) {
        contextData.keyStates[key] = 0.0f;
        SetBgfxRenderContext(context, contextData);
    }
}

static void OnMouseScroll(GLFWwindow *window, double x, double y) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    auto contextData = GetBgfxRenderContext(entity);
    contextData.keyStates[MOUSE_SCROLL_DOWN] = fmaxf(-y, 0.0f);
    contextData.keyStates[MOUSE_SCROLL_UP] = fmaxf(y, 0.0f);
    contextData.keyStates[MOUSE_SCROLL_LEFT] = fmaxf(-x, 0.0f);
    contextData.keyStates[MOUSE_SCROLL_RIGHT] = fmaxf(x, 0.0f);
    SetBgfxRenderContext(entity, contextData);
}

static void OnMouseMove(GLFWwindow *window, double x, double y) {
    auto entity = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    auto data = GetInputContext(entity);
    auto dx = x - data.InputContextCursorPosition.x;
    auto dy = y - data.InputContextCursorPosition.y;

    auto contextData = GetBgfxRenderContext(entity);
    contextData.keyStates[MOUSE_DOWN] = fmaxf(dy, 0.0f);
    contextData.keyStates[MOUSE_UP] = fmaxf(-dy, 0.0f);
    contextData.keyStates[MOUSE_LEFT] = fmaxf(-dx, 0.0f);
    contextData.keyStates[MOUSE_RIGHT] = fmaxf(dx, 0.0f);
    SetBgfxRenderContext(entity, contextData);

    data.InputContextCursorPosition = {(int)x, (int)y};
    SetInputContext(entity, data);
}

static void OnMouseButton(GLFWwindow *window, int button, int action, int mods) {
    auto context = GetEntityByIndex((u32)(size_t)glfwGetWindowUserPointer(window));

    auto contextData = GetBgfxRenderContext(context);
    if(action == GLFW_PRESS) {
        contextData.keyStates[MOUSEBUTTON_0 + button] = 1.0f;
        SetBgfxRenderContext(context, contextData);
    }

    if(action == GLFW_RELEASE) {
        contextData.keyStates[MOUSEBUTTON_0 + button] = 0.0f;
        SetBgfxRenderContext(context, contextData);
    }
}

static void InitContext(Entity entity) {
    NumContexts++;

    auto data = GetBgfxRenderContext(entity);
    auto size = GetRenderTarget(entity).RenderTargetSize;

    if(NumContexts == 1) {
        glfwInit();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, false);

    auto title = GetRenderContext(entity).RenderContextTitle;
    auto window = glfwCreateWindow(Max(size.x, 32), Max(size.y, 32), title ? title : "", NULL, NULL);
    auto monitor = glfwGetPrimaryMonitor();
    glfwSetWindowSizeCallback(window, OnGlfwWindowResized);
    glfwSetWindowUserPointer(window, (void*)(size_t)GetEntityIndex(entity));
    glfwSetCharCallback(window, OnCharPressed);
    glfwSetKeyCallback(window, OnKey);
    glfwSetScrollCallback(window, OnMouseScroll);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetFramebufferSizeCallback(window, OnGlfwFramebufferResized);
    glfwSetWindowRefreshCallback(window, OnGlfwWindowRefresh);

    data.window = window;

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
        data.fb = BGFX_INVALID_HANDLE;

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

        auto moduleData = GetModule(ModuleOf_BgfxRendering());
        for(auto extension : moduleData.Extensions) {
            auto extensionData = GetExtension(extension);
            extensionData.ExtensionDisabled = false;
            SetExtension(extension, extensionData);
        }

    } else {
        data.fb = bgfx::createFrameBuffer(windowHandle, size.x, size.y);
    }
}

BeginUnit(BgfxRenderContext)
    BeginComponent(BgfxRenderContext)
        RegisterBase(RenderContext)
        RegisterBase(InputContext)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterDeferredSystem(OnRenderContextChanged, ComponentOf_RenderContext(), AppLoopOrder_Present)
    RegisterDeferredSystem(OnRenderTargetChanged, ComponentOf_RenderContext(), AppLoopOrder_Present)
    RegisterDeferredSystem(OnPresent, ComponentOf_RenderContext(), AppLoopOrder_Present)
    RegisterDeferredSystem(OnInputPoll, ComponentOf_RenderContext(), AppLoopOrder_Input)
    RegisterDeferredSystem(OnInputContextChanged, ComponentOf_InputContext(), AppLoopOrder_Input)
    RegisterDeferredSystem(OnTextureReadBack, ComponentOf_RenderContext(), AppLoopOrder_ResourceDownload)
EndUnit()
