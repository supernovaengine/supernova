// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "WindowMac.h"

#import <Cocoa/Cocoa.h>

#include <algorithm>

using namespace doriax;

namespace {

    NSWindow* gWindow = nil;
    NSView* gContentView = nil;

    std::function<void(int, int)> gResizeCallback;
    int gDrawableWidth = 0;
    int gDrawableHeight = 0;

    bool gCursorHidden = false;
    bool gCursorCaptured = false;
    bool gHasSavedCursorPosition = false;
    CGPoint gSavedCursorPosition{};

    NSString* stringFromUtf8(const std::string& text) {
        NSString* result = [[NSString alloc] initWithBytes:text.data()
                                                    length:text.size()
                                                  encoding:NSUTF8StringEncoding];
        return result ?: @"";
    }

    // AppKit cancels [NSCursor hide] whenever it resets cursor rects; a 1x1
    // transparent cursor survives those resets.
    NSCursor* invisibleCursor() {
        static NSCursor* cursor = nil;
        if (!cursor) {
            NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(1.0, 1.0)];
            [image lockFocus];
            [NSColor.clearColor set];
            NSRectFill(NSMakeRect(0.0, 0.0, 1.0, 1.0));
            [image unlockFocus];
            cursor = [[NSCursor alloc] initWithImage:image hotSpot:NSZeroPoint];
        }
        return cursor;
    }

    // Cocoa screen points have their origin at the bottom left of the primary
    // screen; the Quartz warping calls want the top left.
    CGPoint cocoaPointToQuartz(NSPoint point) {
        NSScreen* primary = NSScreen.screens.firstObject;
        return CGPointMake(point.x, NSMaxY(primary.frame) - point.y);
    }

}

// undocumented methods for creating cursors (see GLFW 3.4 and imgui_impl_osx.mm)
@interface NSCursor()
+ (id)_windowResizeNorthWestSouthEastCursor;
+ (id)_windowResizeNorthEastSouthWestCursor;
+ (id)_windowResizeNorthSouthCursor;
+ (id)_windowResizeEastWestCursor;
@end

void WindowMac::setupApplication() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
}

void WindowMac::installMinimalMenuBar(const std::string& applicationName) {
    NSMenu* menuBar = [[NSMenu alloc] init];
    NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
    [menuBar addItem:appMenuItem];

    NSMenu* appMenu = [[NSMenu alloc] init];
    NSString* quitTitle = [@"Quit " stringByAppendingString:stringFromUtf8(applicationName)];
    [appMenu addItemWithTitle:quitTitle
                       action:@selector(terminate:)
                keyEquivalent:@"q"];
    [appMenuItem setSubmenu:appMenu];

    [NSApp setMainMenu:menuBar];
}

void WindowMac::create(const WindowMacConfig& config) {
    NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                              NSWindowStyleMaskMiniaturizable;
    if (config.resizable) style |= NSWindowStyleMaskResizable;

    const NSRect contentRect = NSMakeRect(0, 0, std::max(config.width, 1),
                                          std::max(config.height, 1));
    gWindow = [[NSWindow alloc] initWithContentRect:contentRect
                                          styleMask:style
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    gWindow.title = stringFromUtf8(config.title);
    // Created programmatically, so Cocoa would otherwise free it on close and
    // leave every delegate holding a dangling pointer.
    gWindow.releasedWhenClosed = NO;
    gWindow.tabbingMode = NSWindowTabbingModeDisallowed;
    gWindow.collectionBehavior |= NSWindowCollectionBehaviorFullScreenPrimary;
    gWindow.acceptsMouseMovedEvents = YES;
    [gWindow center];
}

void WindowMac::destroy() {
    if (!gWindow) return;
    setCursorCaptured(false, true);
    setCursorHidden(false);
    [gWindow orderOut:nil];
    gWindow.delegate = nil;
    gContentView = nil;
    gWindow = nil;
    gResizeCallback = nullptr;
    gDrawableWidth = gDrawableHeight = 0;
}

void* WindowMac::nativeWindow() {
    return (__bridge void*)gWindow;
}

void* WindowMac::contentView() {
    return (__bridge void*)gContentView;
}

void WindowMac::setContentView(void* view) {
    gContentView = (__bridge NSView*)view;
    if (gWindow) gWindow.contentView = gContentView;
    refreshDrawableSize();
}

void WindowMac::setWindowDelegate(void* delegate) {
    if (gWindow) gWindow.delegate = (__bridge id<NSWindowDelegate>)delegate;
}

void WindowMac::show(bool focusContentView) {
    if (!gWindow) return;
    [NSApp activateIgnoringOtherApps:YES];
    [gWindow makeKeyAndOrderFront:nil];
    // The content view implements NSTextInputClient and accepts first responder;
    // it has to hold it or keyDown never reaches the engine.
    if (focusContentView && gContentView)
        [gWindow makeFirstResponder:gContentView];
}

void WindowMac::applyInitialWindowMode(bool maximized, bool fullscreen) {
    if (fullscreen) requestFullscreen();
    else if (maximized) maximize();
}

int WindowMac::getDrawableWidth() {
    return gDrawableWidth;
}

int WindowMac::getDrawableHeight() {
    return gDrawableHeight;
}

void WindowMac::refreshDrawableSize() {
    if (!gContentView) return;
    const NSSize backing = [gContentView convertSizeToBacking:gContentView.bounds.size];
    const int width = std::max((int)backing.width, 1);
    const int height = std::max((int)backing.height, 1);
    if (width == gDrawableWidth && height == gDrawableHeight) return;

    gDrawableWidth = width;
    gDrawableHeight = height;
    if (gResizeCallback) gResizeCallback(width, height);
}

void WindowMac::setDrawableResizeCallback(std::function<void(int, int)> callback) {
    gResizeCallback = std::move(callback);
}

bool WindowMac::isFullscreen() {
    return gWindow && (gWindow.styleMask & NSWindowStyleMaskFullScreen) != 0;
}

void WindowMac::requestFullscreen() {
    if (gWindow && !isFullscreen()) [gWindow toggleFullScreen:nil];
}

void WindowMac::exitFullscreen() {
    if (gWindow && isFullscreen()) [gWindow toggleFullScreen:nil];
}

bool WindowMac::isMaximized() {
    return gWindow && gWindow.zoomed;
}

void WindowMac::maximize() {
    if (gWindow && !isFullscreen() && !gWindow.zoomed) [gWindow zoom:nil];
}

void WindowMac::restore() {
    if (gWindow && !isFullscreen() && gWindow.zoomed) [gWindow zoom:nil];
}

void WindowMac::setSize(int width, int height) {
    if (!gWindow || width < 1 || height < 1 || isFullscreen()) return;
    NSRect content = [gWindow contentRectForFrameRect:gWindow.frame];
    // Cocoa positions from the bottom left, so keep the top edge fixed
    const CGFloat top = content.origin.y + content.size.height;
    content.size = NSMakeSize(width, height);
    content.origin.y = top - height;
    [gWindow setFrame:[gWindow frameRectForContentRect:content] display:YES];
}

bool WindowMac::isResizable() {
    return gWindow && (gWindow.styleMask & NSWindowStyleMaskResizable) != 0;
}

void WindowMac::setResizable(bool resizable) {
    if (!gWindow) return;
    if (resizable) gWindow.styleMask |= NSWindowStyleMaskResizable;
    else gWindow.styleMask &= ~NSWindowStyleMaskResizable;
}

void WindowMac::setTitle(const std::string& title) {
    if (gWindow) gWindow.title = stringFromUtf8(title);
}

void WindowMac::quit() {
    [NSApp terminate:nil];
}

void WindowMac::setMouseCursor(CursorType type) {
    // A hidden cursor would be made visible again by setting a shape
    if (gCursorHidden) return;

    NSCursor* cursor = [NSCursor arrowCursor];
    switch (type) {
        case CursorType::ARROW:
            break;
        case CursorType::IBEAM:
            cursor = [NSCursor IBeamCursor];
            break;
        case CursorType::CROSSHAIR:
            cursor = [NSCursor crosshairCursor];
            break;
        case CursorType::POINTING_HAND:
            cursor = [NSCursor pointingHandCursor];
            break;
        case CursorType::RESIZE_EW:
            cursor = [NSCursor respondsToSelector:@selector(_windowResizeEastWestCursor)]
                ? [NSCursor _windowResizeEastWestCursor] : [NSCursor resizeLeftRightCursor];
            break;
        case CursorType::RESIZE_NS:
            cursor = [NSCursor respondsToSelector:@selector(_windowResizeNorthSouthCursor)]
                ? [NSCursor _windowResizeNorthSouthCursor] : [NSCursor resizeUpDownCursor];
            break;
        case CursorType::RESIZE_NWSE:
            cursor = [NSCursor respondsToSelector:@selector(_windowResizeNorthWestSouthEastCursor)]
                ? [NSCursor _windowResizeNorthWestSouthEastCursor] : [NSCursor closedHandCursor];
            break;
        case CursorType::RESIZE_NESW:
            cursor = [NSCursor respondsToSelector:@selector(_windowResizeNorthEastSouthWestCursor)]
                ? [NSCursor _windowResizeNorthEastSouthWestCursor] : [NSCursor closedHandCursor];
            break;
        case CursorType::RESIZE_ALL:
            cursor = [NSCursor closedHandCursor];
            break;
        case CursorType::NOT_ALLOWED:
            cursor = [NSCursor operationNotAllowedCursor];
            break;
    }

    [cursor set];
}

void WindowMac::setMouseMode(MouseMode mode) {
    switch (mode) {
        case MouseMode::NORMAL:
        case MouseMode::CONFINED:
            // macOS has no cursor confinement, so CONFINED behaves as NORMAL
            setCursorCaptured(false, false);
            setCursorHidden(false);
            break;
        case MouseMode::HIDDEN:
            setCursorCaptured(false, false);
            setCursorHidden(true);
            break;
        case MouseMode::CAPTURED:
            setCursorHidden(true);
            setCursorCaptured(true, false);
            break;
    }
}

void WindowMac::setMousePosition(float x, float y) {
    if (!gWindow || !gContentView) return;

    // Engine coordinates are backing pixels from the top left of the view;
    // CGWarpMouseCursorPosition wants global points from the top left.
    const CGFloat scale = gWindow.backingScaleFactor > 0 ? gWindow.backingScaleFactor : 1.0;
    const NSPoint inView = NSMakePoint(x / scale, gContentView.bounds.size.height - (y / scale));
    const NSPoint inWindow = [gContentView convertPoint:inView toView:nil];
    const NSRect inScreen = [gWindow convertRectToScreen:NSMakeRect(inWindow.x, inWindow.y, 0, 0)];

    CGWarpMouseCursorPosition(cocoaPointToQuartz(inScreen.origin));
}

void WindowMac::setCursorHidden(bool hidden) {
    // [NSCursor hide]/[unhide] are reference counted and not idempotent, so only
    // toggle on an actual transition.
    if (hidden == gCursorHidden) return;
    if (hidden) [NSCursor hide];
    else [NSCursor unhide];
    gCursorHidden = hidden;

    // Reapply the shape and ask AppKit to reset cursor rects, so a view that
    // repaints its cursor does not undo the hide mid-drag.
    if (hidden) [invisibleCursor() set];
    else [[NSCursor arrowCursor] set];
    if (gWindow && gContentView)
        [gWindow invalidateCursorRectsForView:gContentView];
}

bool WindowMac::applyHiddenCursorShape() {
    if (!gCursorHidden) return false;
    [invisibleCursor() set];
    return true;
}

void WindowMac::setCursorCaptured(bool captured, bool restorePosition) {
    if (captured == gCursorCaptured) return;

    if (captured) {
        CGEventRef event = CGEventCreate(nullptr);
        if (event) {
            gSavedCursorPosition = CGEventGetLocation(event);
            gHasSavedCursorPosition = true;
            CFRelease(event);
        }
        CGAssociateMouseAndMouseCursorPosition(false);
        gCursorCaptured = true;
        return;
    }

    CGAssociateMouseAndMouseCursorPosition(true);
    gCursorCaptured = false;
    if (restorePosition && gHasSavedCursorPosition)
        CGWarpMouseCursorPosition(gSavedCursorPosition);
    gHasSavedCursorPosition = false;
}

bool WindowMac::isCursorCaptured() {
    return gCursorCaptured;
}

void WindowMac::confinePointerToWindow() {
    if (!gWindow || !gContentView) return;
    const NSRect screenRect = [gWindow convertRectToScreen:gContentView.bounds];
    const NSPoint point = NSEvent.mouseLocation;
    const NSPoint clamped = NSMakePoint(
        std::clamp(point.x, NSMinX(screenRect), NSMaxX(screenRect) - 1.0),
        std::clamp(point.y, NSMinY(screenRect), NSMaxY(screenRect) - 1.0));
    if (!NSEqualPoints(point, clamped))
        CGWarpMouseCursorPosition(cocoaPointToQuartz(clamped));
}
