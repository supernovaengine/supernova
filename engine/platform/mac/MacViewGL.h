// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef MacViewGL_h
#define MacViewGL_h

#import <Cocoa/Cocoa.h>

// The OpenGL drawable, for GRAPHIC_BACKEND=glcore. MTKView has no GL surface,
// which is the one thing the two paths cannot share; input is not part of it
// and goes through the same MacInputRouter.
@interface MacViewGL : NSOpenGLView <NSTextInputClient>

// 0 disables vsync, 1 syncs to the vertical retrace
- (void)setSwapInterval:(int)interval;

- (void)makeContextCurrent;
- (void)presentFrame;

@end

#endif /* MacViewGL_h */
