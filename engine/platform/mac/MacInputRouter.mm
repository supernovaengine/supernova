// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "MacInputRouter.h"
#include "KeyCodesMac.h"

#include "Engine.h"

using namespace doriax;

@implementation MacInputRouter {
    // The view owns the router, so a strong reference here would be a cycle
    __weak NSView* _view;
    NSTrackingArea* _trackingArea;
    CGPoint _mousePoint;
    BOOL _hasMousePoint;
}

- (instancetype)initWithView:(NSView*)view {
    self = [super init];
    if (self) {
        _view = view;
        _trackingArea = nil;
        _mousePoint = CGPointZero;
        _hasMousePoint = NO;
    }
    return self;
}

- (void)updateTrackingAreas {
    NSView* view = _view;
    if (!view) return;

    if (_trackingArea != nil)
        [view removeTrackingArea:_trackingArea];

    // ActiveAlways so a game keeps tracking the pointer while not frontmost
    const NSTrackingAreaOptions options =
        NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved |
        NSTrackingActiveAlways | NSTrackingInVisibleRect;
    _trackingArea = [[NSTrackingArea alloc] initWithRect:[view bounds]
                                                options:options
                                                  owner:view
                                               userInfo:nil];
    [view addTrackingArea:_trackingArea];
}

// The engine works in backing pixels from the top left; Cocoa reports window
// points from the bottom left.
- (CGPoint)mousePointOf:(NSEvent*)event {
    NSView* view = _view;
    if (!view) return CGPointZero;

    const NSPoint inView = [view convertPoint:[event locationInWindow] fromView:nil];
    const NSPoint inBacking = [view convertPointToBacking:inView];
    const CGFloat height = [view convertSizeToBacking:view.bounds.size].height;
    return CGPointMake(inBacking.x, height - inBacking.y);
}

- (CGPoint)trackedMousePointOf:(NSEvent*)event {
    const bool captured = Engine::getMouseMode() == MouseMode::CAPTURED;
    if (!captured || !_hasMousePoint) {
        _mousePoint = [self mousePointOf:event];
        _hasMousePoint = YES;
        return _mousePoint;
    }

    NSView* view = _view;
    if (!view) return _mousePoint;

    // Captured: the pointer is pinned, so only the deltas are meaningful. A move
    // event's deltaY points down, like the engine coordinates.
    const NSSize delta = [view convertSizeToBacking:NSMakeSize([event deltaX], [event deltaY])];
    _mousePoint.x += delta.width;
    _mousePoint.y += delta.height;
    return _mousePoint;
}

- (void)keyDown:(NSEvent*)event {
    const int key = macKeyFromCode([event keyCode]);
    if (key != D_KEY_UNKNOWN)
        Engine::systemKeyDown(key, [event isARepeat], macKeyModifiers([event modifierFlags]));

    // Feeds the text input manager, which calls back into the view's
    // NSTextInputClient methods for IME and dead keys
    [_view interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent*)event {
    const int key = macKeyFromCode([event keyCode]);
    if (key != D_KEY_UNKNOWN)
        Engine::systemKeyUp(key, [event isARepeat], macKeyModifiers([event modifierFlags]));
}

- (void)flagsChanged:(NSEvent*)event {
    const int key = macKeyFromCode([event keyCode]);
    if (key == D_KEY_UNKNOWN) return;

    const NSUInteger flags = [event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask;
    const int mods = macKeyModifiers([event modifierFlags]);

    // flagsChanged carries no up/down, so the modifier's own bit decides
    if (flags & macModFlagForKey(key)) Engine::systemKeyDown(key, false, mods);
    else Engine::systemKeyUp(key, false, mods);
}

- (void)mouseDown:(NSEvent*)event button:(int)button {
    const CGPoint point = [self trackedMousePointOf:event];
    Engine::systemMouseDown(button, point.x, point.y, macKeyModifiers([event modifierFlags]));
}

- (void)mouseUp:(NSEvent*)event button:(int)button {
    const CGPoint point = [self trackedMousePointOf:event];
    Engine::systemMouseUp(button, point.x, point.y, macKeyModifiers([event modifierFlags]));
}

- (void)mouseMoved:(NSEvent*)event {
    const CGPoint point = [self trackedMousePointOf:event];
    Engine::systemMouseMove(point.x, point.y, macKeyModifiers([event modifierFlags]));
}

- (void)scrollWheel:(NSEvent*)event {
    float dx = (float)[event scrollingDeltaX];
    float dy = (float)[event scrollingDeltaY];
    if ([event hasPreciseScrollingDeltas]) {
        dx *= 0.1f;
        dy *= 0.1f;
    }
    if (dx != 0.0f || dy != 0.0f)
        Engine::systemMouseScroll(dx, dy, macKeyModifiers([event modifierFlags]));
}

- (void)mouseEntered:(NSEvent*)event {
    (void)event;
    Engine::systemMouseEnter();
    // Cocoa resets to the arrow on re-entry, dropping the game's chosen cursor
    if (Engine::getMouseCursor() != CursorType::ARROW)
        Engine::setMouseCursor(Engine::getMouseCursor());
}

- (void)mouseExited:(NSEvent*)event {
    (void)event;
    Engine::systemMouseLeave();
}

- (void)insertText:(id)string {
    NSString* characters = [string isKindOfClass:[NSAttributedString class]]
        ? [(NSAttributedString*)string string] : (NSString*)string;
    const NSUInteger length = [characters length];
    for (NSUInteger i = 0; i < length; i++) {
        const unichar codepoint = [characters characterAtIndex:i];
        if ((codepoint & 0xff00) == 0xf700) continue; // function-key private range
        Engine::systemCharInput(codepoint);
    }
}

// Keys the input manager reports as editing commands instead of characters
- (void)doCommandBySelector:(SEL)selector {
    if (selector == @selector(deleteBackward:)) Engine::systemCharInput('\b');
    else if (selector == @selector(insertNewline:)) Engine::systemCharInput('\r');
    else if (selector == @selector(insertTab:)) Engine::systemCharInput('\t');
    else if (selector == @selector(cancelOperation:)) Engine::systemCharInput(0x1b);
}

@end
