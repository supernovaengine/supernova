// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "DoriaxMac.h"

#import <Cocoa/Cocoa.h>

#if defined(SOKOL_METAL)
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import "MacViewMetal.h"
#elif defined(SOKOL_VULKAN)
#import "MacViewVulkan.h"
#include "VulkanContext.h"
#else
#import "MacViewGL.h"
#endif

#include "SystemMac.h"
#include "WindowMac.h"

#include "Engine.h"

#import "DoriaxGameController.h"

#ifndef DORIAX_VSYNC_ENABLED
#define DORIAX_VSYNC_ENABLED 1
#endif

// 0 = windowed, 1 = maximized (zoomed), 2 = fullscreen
#ifndef DORIAX_WINDOW_MODE
#define DORIAX_WINDOW_MODE 0
#endif

#ifndef DORIAX_WINDOW_RESIZABLE
#define DORIAX_WINDOW_RESIZABLE 1
#endif

#ifndef DORIAX_WINDOW_TITLE
#define DORIAX_WINDOW_TITLE "Doriax"
#endif

using namespace doriax;

namespace {
#if defined(SOKOL_METAL)
    MacViewMetal* gameView = nil;
#elif defined(SOKOL_VULKAN)
    MacViewVulkan* gameView = nil;

    VkResult createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        VkMetalSurfaceCreateInfoEXT surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceInfo.pLayer = gameView.metalLayer;
        return vkCreateMetalSurfaceEXT(instance, &surfaceInfo, nullptr, surface);
    }

    bool createContext() {
        VulkanContextConfig config;
        config.surfaceExtension = VK_EXT_METAL_SURFACE_EXTENSION_NAME;
        config.createSurface = createWindowSurface;
        config.vsync = DORIAX_VSYNC_ENABLED != 0;
        return VulkanContext::create(config);
    }
#else
    MacViewGL* gameView = nil;
#endif
}

#if defined(SOKOL_METAL)
@interface DoriaxMacDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate, MTKViewDelegate>
#else
@interface DoriaxMacDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
#endif
@property (nonatomic, strong) NSTimer* frameTimer;
@end

@implementation DoriaxMacDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    (void)notification;

    WindowMacConfig config;
    config.title = DORIAX_WINDOW_TITLE;
    config.width = DEFAULT_WINDOW_WIDTH;
    config.height = DEFAULT_WINDOW_HEIGHT;
    config.resizable = DORIAX_WINDOW_RESIZABLE;

    WindowMac::create(config);
    WindowMac::setWindowDelegate((__bridge void*)self);
    WindowMac::setDrawableResizeCallback([](int, int) {
        Engine::systemViewChanged();
    });

    const NSRect viewFrame = NSMakeRect(0, 0, config.width, config.height);

#if defined(SOKOL_METAL)
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device) {
        NSLog(@"Error: Metal is not supported on this Mac");
        [NSApp terminate:nil];
        return;
    }

    gameView = [[MacViewMetal alloc] initWithFrame:viewFrame device:device];
    gameView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    gameView.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    // Paused: the frame timer below drives the view, like the OpenGL loop
    gameView.paused = YES;
    gameView.enableSetNeedsDisplay = NO;
    gameView.delegate = self;

    WindowMac::setContentView((__bridge void*)gameView);
    static_cast<CAMetalLayer*>(gameView.layer).displaySyncEnabled = DORIAX_VSYNC_ENABLED ? YES : NO;
#elif defined(SOKOL_VULKAN)
    gameView = [[MacViewVulkan alloc] initWithFrame:viewFrame];
    WindowMac::setContentView((__bridge void*)gameView);

    // The surface comes from the view's layer, so both go up before
    // systemViewLoaded, where sokol reads the environment
    if (!createContext()) {
        VulkanContext::destroy();
        NSLog(@"Error: Vulkan is not available on this Mac");
        [NSApp terminate:nil];
        return;
    }
#else
    gameView = [[MacViewGL alloc] initWithFrame:viewFrame];
    if (!gameView || ![gameView pixelFormat]) {
        NSLog(@"Error: no OpenGL 4.1 core pixel format available");
        [NSApp terminate:nil];
        return;
    }

    WindowMac::setContentView((__bridge void*)gameView);
    [gameView makeContextCurrent];
    [gameView setSwapInterval:DORIAX_VSYNC_ENABLED ? 1 : 0];
#endif

    Engine::systemViewLoaded();
    Engine::systemViewChanged();

    [DoriaxGameController start];

    WindowMac::show(true);
    WindowMac::applyInitialWindowMode(DORIAX_WINDOW_MODE == 1, DORIAX_WINDOW_MODE == 2);

    // A timer-driven loop keeps the run loop live, so Cocoa still delivers
    // input and window events between frames. VSync paces the actual rate.
    self.frameTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 / 240.0
                                                      repeats:YES
                                                        block:^(NSTimer*){ [self renderFrame]; }];
    [[NSRunLoop currentRunLoop] addTimer:self.frameTimer forMode:NSRunLoopCommonModes];
}

- (void)renderFrame {
    if (!gameView) return;
#if defined(SOKOL_METAL)
    // Runs drawInMTKView below, the only place currentDrawable is valid
    [gameView draw];
#elif defined(SOKOL_VULKAN)
    VulkanContext::beginFrame();
    Engine::systemDraw();
    VulkanContext::endFrame();
#else
    [gameView makeContextCurrent];
    Engine::systemDraw();
    [gameView presentFrame];
#endif
}

#if defined(SOKOL_METAL)
- (void)drawInMTKView:(MTKView*)view {
    (void)view;
    Engine::systemDraw();
}

// The Metal counterpart of MacViewGL's reshape
- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size {
    (void)view;
    (void)size;
    WindowMac::refreshDrawableSize();
}
#endif

- (void)windowDidResize:(NSNotification*)notification {
    (void)notification;
    WindowMac::refreshDrawableSize();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    (void)sender;
    return YES;
}

// The pointer is disassociated globally, so a capture left on while another app
// is in front would freeze the cursor system wide.
- (void)applicationDidResignActive:(NSNotification*)notification {
    (void)notification;
    WindowMac::setCursorCaptured(false, false);
}

- (void)applicationDidBecomeActive:(NSNotification*)notification {
    (void)notification;
    if (Engine::getMouseMode() == MouseMode::CAPTURED)
        WindowMac::setMouseMode(MouseMode::CAPTURED);
}

- (void)applicationWillTerminate:(NSNotification*)notification {
    (void)notification;
    [self.frameTimer invalidate];
    self.frameTimer = nil;
    Engine::systemViewDestroyed();
    Engine::systemShutdown();
#if defined(SOKOL_VULKAN)
    // After sokol has released its own objects, which live on this device
    VulkanContext::destroy();
#endif
    gameView = nil;
    WindowMac::destroy();
}

@end

int DoriaxMac::init(int argc, char **argv) {
    @autoreleasepool {
        Engine::systemInit(argc, argv, new SystemMac());

        WindowMac::setupApplication();
        WindowMac::installMinimalMenuBar(DORIAX_WINDOW_TITLE);

        DoriaxMacDelegate* delegate = [[DoriaxMacDelegate alloc] init];
        [NSApp setDelegate:delegate];
        [NSApp run];
    }
    return 0;
}
