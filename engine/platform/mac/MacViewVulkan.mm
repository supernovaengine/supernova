// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import "MacViewVulkan.h"

#include "MacInputRouter.h"
#include "WindowMac.h"

#include <algorithm>

@implementation MacViewVulkan {
    MacInputRouter* _router;
}

- (CALayer*)makeBackingLayer { return [CAMetalLayer layer]; }

- (instancetype)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.wantsLayer = YES;
        // Or a live resize stretches the last frame until the next one lands
        self.layerContentsRedrawPolicy = NSViewLayerContentsRedrawDuringViewResize;
        _router = [[MacInputRouter alloc] initWithView:self];
        [self updateDrawableSize];
    }
    return self;
}

- (CAMetalLayer*)metalLayer { return (CAMetalLayer*)self.layer; }

// The surface extent Vulkan reports is the layer's drawableSize, so it follows
// the view: on a resize, and across displays of different backing scales.
- (void)updateDrawableSize {
    CAMetalLayer* layer = self.metalLayer;
    if (!layer) return;

    const CGFloat scale = self.window.backingScaleFactor;
    if (scale > 0.0) layer.contentsScale = scale;

    const NSSize backing = [self convertSizeToBacking:self.bounds.size];
    layer.drawableSize = CGSizeMake(std::max(backing.width, 1.0), std::max(backing.height, 1.0));

    doriax::WindowMac::refreshDrawableSize();
}

- (void)setFrameSize:(NSSize)size {
    [super setFrameSize:size];
    [self updateDrawableSize];
}

- (void)viewDidChangeBackingProperties {
    [super viewDidChangeBackingProperties];
    [self updateDrawableSize];
}

- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)canBecomeKeyView { return YES; }
- (BOOL)isOpaque { return YES; }
- (BOOL)wantsUpdateLayer { return YES; }

- (void)viewDidMoveToWindow {
    [[self window] setAcceptsMouseMovedEvents:YES];
    [self updateDrawableSize];
}
- (void)updateTrackingAreas { [_router updateTrackingAreas]; [super updateTrackingAreas]; }

- (void)keyDown:(NSEvent*)event { [_router keyDown:event]; }
- (void)keyUp:(NSEvent*)event { [_router keyUp:event]; }
- (void)flagsChanged:(NSEvent*)event { [_router flagsChanged:event]; }

- (void)mouseDown:(NSEvent*)event { [_router mouseDown:event button:D_MOUSE_BUTTON_LEFT]; }
- (void)rightMouseDown:(NSEvent*)event { [_router mouseDown:event button:D_MOUSE_BUTTON_RIGHT]; }
- (void)otherMouseDown:(NSEvent*)event { [_router mouseDown:event button:D_MOUSE_BUTTON_MIDDLE]; }
- (void)mouseUp:(NSEvent*)event { [_router mouseUp:event button:D_MOUSE_BUTTON_LEFT]; }
- (void)rightMouseUp:(NSEvent*)event { [_router mouseUp:event button:D_MOUSE_BUTTON_RIGHT]; }
- (void)otherMouseUp:(NSEvent*)event { [_router mouseUp:event button:D_MOUSE_BUTTON_MIDDLE]; }

- (void)mouseMoved:(NSEvent*)event { [_router mouseMoved:event]; }
- (void)mouseDragged:(NSEvent*)event { [_router mouseMoved:event]; }
- (void)rightMouseDragged:(NSEvent*)event { [_router mouseMoved:event]; }
- (void)otherMouseDragged:(NSEvent*)event { [_router mouseMoved:event]; }

- (void)scrollWheel:(NSEvent*)event { [_router scrollWheel:event]; }
- (void)mouseEntered:(NSEvent*)event { [_router mouseEntered:event]; }
- (void)mouseExited:(NSEvent*)event { [_router mouseExited:event]; }

// NSTextInputClient is a view protocol, so it cannot move into the router
- (void)insertText:(id)string replacementRange:(NSRange)range { (void)range; [_router insertText:string]; }
- (void)doCommandBySelector:(SEL)selector { [_router doCommandBySelector:selector]; }
- (void)setMarkedText:(id)string selectedRange:(NSRange)selected replacementRange:(NSRange)replacement { (void)string; (void)selected; (void)replacement; }
- (void)unmarkText {}
- (NSRange)selectedRange { return NSMakeRange(NSNotFound, 0); }
- (NSRange)markedRange { return NSMakeRange(NSNotFound, 0); }
- (BOOL)hasMarkedText { return NO; }
- (NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range actualRange:(NSRangePointer)actual { (void)range; (void)actual; return nil; }
- (NSArray<NSAttributedStringKey>*)validAttributesForMarkedText { return @[]; }
- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actual { (void)range; (void)actual; return NSZeroRect; }
- (NSUInteger)characterIndexForPoint:(NSPoint)point { (void)point; return 0; }

@end
