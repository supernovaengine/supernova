// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import "MacViewMetal.h"

#include "MacInputRouter.h"

@implementation MacViewMetal {
    MacInputRouter* _router;
}

// MTKView's initWithFrame:device: does not route through initWithFrame:, and a
// storyboard uses initWithCoder:, so all three have to end up with a router.
// MTKView may chain them, hence the guard.
- (void)setupInput {
    if (_router) return;
    _router = [[MacInputRouter alloc] initWithView:self];
}

- (instancetype)initWithFrame:(NSRect)frame device:(id<MTLDevice>)device {
    self = [super initWithFrame:frame device:device];
    if (self) [self setupInput];
    return self;
}

- (instancetype)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) [self setupInput];
    return self;
}

- (instancetype)initWithCoder:(NSCoder*)coder {
    self = [super initWithCoder:coder];
    if (self) [self setupInput];
    return self;
}

- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)canBecomeKeyView { return YES; }
- (BOOL)wantsUpdateLayer { return YES; }

- (void)viewDidMoveToWindow { [[self window] setAcceptsMouseMovedEvents:YES]; }
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
