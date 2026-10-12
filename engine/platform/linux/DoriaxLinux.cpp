// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "DoriaxLinux.h"

#include "GamepadLinux.h"
#include "LinuxInputRouter.h"
#include "SystemLinux.h"
#include "WindowLinux.h"

#include "Engine.h"

#if defined(SOKOL_VULKAN)
// After WindowLinux.h: the Xlib surface entry points come from <X11/Xlib.h>
#include "VulkanContext.h"
#else
#include <GL/glx.h>
#endif
#include <X11/Xutil.h>

#include <poll.h>
#include <time.h>

#include <cmath>
#include <cstdio>
#include <cstring>

#ifndef DORIAX_VSYNC_ENABLED
#define DORIAX_VSYNC_ENABLED 1
#endif

// 0 = windowed, 1 = maximized, 2 = fullscreen
#ifndef DORIAX_WINDOW_MODE
#define DORIAX_WINDOW_MODE 0
#endif

#ifndef DORIAX_WINDOW_RESIZABLE
#define DORIAX_WINDOW_RESIZABLE 1
#endif

#ifndef DORIAX_WINDOW_TITLE
#define DORIAX_WINDOW_TITLE "Doriax"
#endif

#ifndef DORIAX_APP_ID
#define DORIAX_APP_ID "Doriax"
#endif

using namespace doriax;

namespace {

// A hidden window updates the game this often, without drawing
constexpr double HIDDEN_UPDATE_PERIOD = 1.0 / 60.0;

#if !defined(SOKOL_VULKAN)
using CreateContextAttribsProc =
    GLXContext (*)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
using SwapIntervalProc = void (*)(Display*, GLXDrawable, int);

GLXFBConfig framebufferConfig = nullptr;
GLXContext glContext = nullptr;
SwapIntervalProc swapIntervalEXT = nullptr;
bool glxContextError = false;
#endif

GamepadLinux gamepads;
LinuxInputRouter inputRouter;

bool shouldClose = false;
bool mapped = true;
bool minimized = false;

double monotonicSeconds() {
    timespec now{};
    clock_gettime(CLOCK_MONOTONIC, &now);
    return double(now.tv_sec) + double(now.tv_nsec) / 1.0e9;
}

void waitForEvents(double timeout) {
    Display* display = WindowLinux::display();
    if (timeout <= 0.0 || XPending(display)) return;

    pollfd connection{ConnectionNumber(display), POLLIN, 0};
    poll(&connection, 1, static_cast<int>(std::ceil(timeout * 1000.0)));
}

#if !defined(SOKOL_VULKAN)
template <typename T>
T glxProc(const char* name) {
    return reinterpret_cast<T>(
        glXGetProcAddressARB(reinterpret_cast<const GLubyte*>(name)));
}

bool hasGlxExtension(Display* display, int screen, const char* name) {
    const char* extensions = glXQueryExtensionsString(display, screen);
    if (!extensions || !name || !*name || std::strchr(name, ' ')) return false;

    const size_t length = std::strlen(name);
    for (const char* match = extensions; (match = std::strstr(match, name));
         match += length) {
        const bool startsToken = match == extensions || match[-1] == ' ';
        const bool endsToken = match[length] == '\0' || match[length] == ' ';
        if (startsToken && endsToken) return true;
    }
    return false;
}

int recordGlxContextError(Display*, XErrorEvent*) {
    glxContextError = true;
    return 0;
}

// One configuration for the window, matching the editor backend's choice
bool selectVisual(XVisualInfo*& info) {
    static const int attributes[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        None
    };
    Display* display = WindowLinux::display();
    int count = 0;
    GLXFBConfig* configs = glXChooseFBConfig(
        display, WindowLinux::screen(), attributes, &count);
    if (!configs || count == 0) {
        if (configs) XFree(configs);
        std::fprintf(stderr, "Error: No GLX framebuffer configuration available.\n");
        return false;
    }
    framebufferConfig = configs[0];
    XFree(configs);

    info = glXGetVisualFromFBConfig(display, framebufferConfig);
    if (!info) {
        std::fprintf(stderr, "Error: No X visual for the GLX configuration.\n");
        return false;
    }
    return true;
}

bool createContext() {
    Display* display = WindowLinux::display();
    const int screen = WindowLinux::screen();

    if (!hasGlxExtension(display, screen, "GLX_ARB_create_context")) {
        std::fprintf(stderr, "Error: GLX_ARB_create_context is required.\n");
        return false;
    }
    auto createContextAttribs =
        glxProc<CreateContextAttribsProc>("glXCreateContextAttribsARB");
    if (!createContextAttribs) {
        std::fprintf(stderr, "Error: GLX_ARB_create_context is required.\n");
        return false;
    }

    const int attributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    // Some drivers report an unsupported profile with an X error instead of
    // returning null
    XSync(display, False);
    glxContextError = false;
    XErrorHandler previousHandler = XSetErrorHandler(recordGlxContextError);
    glContext = createContextAttribs(
        display, framebufferConfig, nullptr, True, attributes);
    XSync(display, False);
    XSetErrorHandler(previousHandler);

    if (glxContextError || !glContext) {
        if (glContext) glXDestroyContext(display, glContext);
        glContext = nullptr;
        std::fprintf(stderr, "Error: Could not create an OpenGL 4.1 core context.\n");
        return false;
    }
    if (!glXMakeCurrent(display, WindowLinux::handle(), glContext)) {
        std::fprintf(stderr, "Error: Could not activate the OpenGL context.\n");
        glXDestroyContext(display, glContext);
        glContext = nullptr;
        return false;
    }

    if (hasGlxExtension(display, screen, "GLX_EXT_swap_control")) {
        swapIntervalEXT = glxProc<SwapIntervalProc>("glXSwapIntervalEXT");
        if (swapIntervalEXT)
            swapIntervalEXT(display, WindowLinux::handle(),
                            DORIAX_VSYNC_ENABLED ? 1 : 0);
    }
    return true;
}

void destroyContext() {
    Display* display = WindowLinux::display();
    if (glContext && display) {
        glXMakeCurrent(display, None, nullptr);
        glXDestroyContext(display, glContext);
    }
    glContext = nullptr;
    swapIntervalEXT = nullptr;
    framebufferConfig = nullptr;
}

void beginFrame() {
}

void endFrame() {
    glXSwapBuffers(WindowLinux::display(), WindowLinux::handle());
}

#else

VkResult createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    VkXlibSurfaceCreateInfoKHR surfaceInfo{};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.dpy = WindowLinux::display();
    surfaceInfo.window = WindowLinux::handle();
    return vkCreateXlibSurfaceKHR(instance, &surfaceInfo, nullptr, surface);
}

bool createContext() {
    VulkanContextConfig config;
    config.surfaceExtension = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
    config.createSurface = createWindowSurface;
    config.vsync = DORIAX_VSYNC_ENABLED != 0;
    return VulkanContext::create(config);
}

void destroyContext() {
    VulkanContext::destroy();
}

void beginFrame() {
    VulkanContext::beginFrame();
}

void endFrame() {
    VulkanContext::endFrame();
}

#endif

void processEvents() {
    Display* display = WindowLinux::display();
    while (XPending(display)) {
        XEvent event{};
        XNextEvent(display, &event);

        if (inputRouter.handleEvent(event))
            continue;

        if (WindowLinux::isCloseEvent(event)) {
            shouldClose = true;
            continue;
        }

        switch (event.type) {
            case ConfigureNotify:
                WindowLinux::updateSize(event.xconfigure.width,
                                        event.xconfigure.height);
                break;
            case FocusOut:
                // Match the other backends: a captured pointer is released when
                // the window loses focus so it can never be trapped.
                WindowLinux::releasePointerGrab();
                break;
            case FocusIn:
                if (WindowLinux::isRelativeMouse())
                    WindowLinux::setMouseMode(MouseMode::CAPTURED);
                break;
            case MapNotify:
                mapped = true;
                break;
            case UnmapNotify:
                mapped = false;
                break;
            case PropertyNotify:
                if (WindowLinux::isStateEvent(event))
                    minimized = WindowLinux::isMinimized();
                break;
            default:
                break;
        }
    }
}

}

int DoriaxLinux::init(int argc, char **argv) {
    Engine::systemInit(argc, argv, new SystemLinux(&inputRouter));

    if (!WindowLinux::openDisplay())
        return -1;

    WindowLinuxConfig config;
    config.title = DORIAX_WINDOW_TITLE;
    config.appId = DORIAX_APP_ID;
    config.width = DEFAULT_WINDOW_WIDTH;
    config.height = DEFAULT_WINDOW_HEIGHT;
    config.resizable = DORIAX_WINDOW_RESIZABLE;

#if defined(SOKOL_VULKAN)
    // A Vulkan surface presents to any drawable, so the window keeps the
    // screen's own visual instead of one matched to a GLX configuration.
    config.visual = DefaultVisual(WindowLinux::display(), WindowLinux::screen());
    config.depth = DefaultDepth(WindowLinux::display(), WindowLinux::screen());

    const bool created = WindowLinux::create(config);
#else
    XVisualInfo* info = nullptr;
    if (!selectVisual(info)) {
        WindowLinux::destroy();
        return -1;
    }
    config.visual = info->visual;
    config.depth = info->depth;

    const bool created = WindowLinux::create(config);
    XFree(info);
#endif
    if (!created) {
        WindowLinux::destroy();
        return -1;
    }
    WindowLinux::show();

    if (!createContext()) {
        destroyContext();
        WindowLinux::destroy();
        return -1;
    }

    XWindowAttributes attributes{};
    XGetWindowAttributes(WindowLinux::display(), WindowLinux::handle(), &attributes);
    WindowLinux::updateSize(attributes.width, attributes.height);

    // Installed after the initial size, so the engine is not told the view
    // changed before it has been told the view exists.
    WindowLinux::setResizeCallback([](int, int) {
        Engine::systemViewChanged();
    });

    Engine::systemViewLoaded();
    Engine::systemViewChanged();

    if (DORIAX_WINDOW_MODE == 1)
        WindowLinux::maximize();
    else if (DORIAX_WINDOW_MODE == 2)
        WindowLinux::requestFullscreen();

    bool hidden = false;
    double nextUpdate = 0.0;

    while (!shouldClose) {
        // Presents stall while hidden, so the game updates without drawing
        if (hidden)
            waitForEvents(nextUpdate - monotonicSeconds());

        processEvents();
        if (shouldClose)
            break;

        const bool wasHidden = hidden;
        hidden = !mapped || minimized;
        // a hidden game keeps updating, but the pads are no longer polled
        if (hidden && !wasHidden)
            gamepads.release();

        if (!hidden) {
            gamepads.poll(monotonicSeconds());
            beginFrame();
            Engine::systemDraw();
            endFrame();
            continue;
        }

        const double now = monotonicSeconds();
        if (now >= nextUpdate) {
            Engine::systemUpdate();

            // A late update is made up on the next one
            nextUpdate += HIDDEN_UPDATE_PERIOD;
            if (std::abs(nextUpdate - now) > HIDDEN_UPDATE_PERIOD)
                nextUpdate = now + HIDDEN_UPDATE_PERIOD;
        }
    }

    Engine::systemViewDestroyed();
    Engine::systemShutdown();

    gamepads.shutdown();
    destroyContext();
    WindowLinux::destroy();
    return 0;
}
