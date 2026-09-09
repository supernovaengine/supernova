// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "SystemMac.h"
#include "WindowMac.h"

#import <Foundation/Foundation.h>

#if defined(SOKOL_METAL)
#import <MetalKit/MetalKit.h>
#elif defined(SOKOL_VULKAN)
#include "VulkanContext.h"
#endif

using namespace doriax;

namespace {
    // Finder gives a bare executable the home folder as working directory, so
    // relative paths never resolve there. The main bundle points at the folder
    // holding the executable, or at the resources of a real .app.
    bool bundleResourcePath(NSString* name, std::string& out) {
        NSString* path = [[NSBundle mainBundle] pathForResource:name ofType:nil];
        if (!path) return false;

        out = [path UTF8String];
        return true;
    }
}

int SystemMac::getScreenWidth() {
    return WindowMac::getDrawableWidth();
}

int SystemMac::getScreenHeight() {
    return WindowMac::getDrawableHeight();
}

int SystemMac::getSampleCount() {
    return 1;
}

#if defined(SOKOL_METAL)
sg_environment SystemMac::getSokolEnvironment() {
    MTKView* view = (__bridge MTKView*)WindowMac::contentView();
    sg_environment env = {};

    env.defaults.sample_count = (int)view.sampleCount;
    env.defaults.color_format = SG_PIXELFORMAT_BGRA8;
    env.defaults.depth_format = SG_PIXELFORMAT_DEPTH_STENCIL;
    env.metal.device = (__bridge const void*)view.device;

    return env;
}

// Only valid inside the view's draw callback, where currentDrawable is set
sg_swapchain SystemMac::getSokolSwapchain() {
    MTKView* view = (__bridge MTKView*)WindowMac::contentView();
    sg_swapchain swapchain = {};

    swapchain.width = (int)view.drawableSize.width;
    swapchain.height = (int)view.drawableSize.height;
    swapchain.sample_count = (int)view.sampleCount;
    swapchain.color_format = SG_PIXELFORMAT_BGRA8;
    swapchain.depth_format = SG_PIXELFORMAT_DEPTH_STENCIL;
    swapchain.metal.current_drawable = (__bridge const void*)view.currentDrawable;
    swapchain.metal.depth_stencil_texture = (__bridge const void*)view.depthStencilTexture;
    swapchain.metal.msaa_color_texture = (__bridge const void*)view.multisampleColorTexture;

    return swapchain;
}
#elif defined(SOKOL_VULKAN)
sg_environment SystemMac::getSokolEnvironment() {
    return VulkanContext::environment();
}

sg_swapchain SystemMac::getSokolSwapchain() {
    return VulkanContext::swapchain();
}
#endif

bool SystemMac::isFullscreen() {
    return WindowMac::isFullscreen();
}

void SystemMac::requestFullscreen() {
    WindowMac::requestFullscreen();
}

void SystemMac::exitFullscreen() {
    WindowMac::exitFullscreen();
}

bool SystemMac::isWindowMaximized() {
    return WindowMac::isMaximized();
}

void SystemMac::maximizeWindow() {
    WindowMac::maximize();
}

void SystemMac::restoreWindow() {
    WindowMac::restore();
}

void SystemMac::setWindowSize(int width, int height) {
    WindowMac::setSize(width, height);
}

bool SystemMac::isWindowResizable() {
    return WindowMac::isResizable();
}

void SystemMac::setWindowResizable(bool resizable) {
    WindowMac::setResizable(resizable);
}

void SystemMac::setWindowTitle(const std::string& title) {
    WindowMac::setTitle(title);
}

void SystemMac::quit() {
    WindowMac::quit();
}

void SystemMac::setMouseCursor(CursorType type) {
    WindowMac::setMouseCursor(type);
}

void SystemMac::setMouseMode(MouseMode mode) {
    WindowMac::setMouseMode(mode);
}

void SystemMac::setMousePosition(float x, float y) {
    WindowMac::setMousePosition(x, y);
}

// The bundle lookup covers an exported game, which ships its assets next to the
// executable. A project run from outside the editor keeps them in the project
// directory, so the editor bakes that path in (Generator::getPlatformCMakeConfig).
std::string SystemMac::getAssetPath() {
    std::string bundlePath;
    if (bundleResourcePath(@"assets", bundlePath)) return bundlePath;

#ifdef DORIAX_ASSET_PATH
    return DORIAX_ASSET_PATH;
#else
    return "assets";
#endif
}

std::string SystemMac::getUserDataPath() {
    return ".";
}

std::string SystemMac::getLuaPath() {
    std::string bundlePath;
    if (bundleResourcePath(@"lua", bundlePath)) return bundlePath;

#ifdef DORIAX_LUA_PATH
    return DORIAX_LUA_PATH;
#else
    return "lua";
#endif
}

std::string SystemMac::getShaderPath() {
    std::string bundlePath;
    if (bundleResourcePath(@"shaders", bundlePath)) return bundlePath;

#ifdef DORIAX_SHADER_PATH
    return DORIAX_SHADER_PATH;
#else
    return System::getShaderPath();
#endif
}
