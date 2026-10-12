// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import <MetalKit/MetalKit.h>

@interface Renderer : NSObject <MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view withArgs:(nullable NSArray*)args;

- (void)destroyView;

+ (void)pauseGame;
+ (void)resumeGame;
// A frame without drawing, for a window that can't be seen
+ (void)updateGame;
// Returns once the GPU has finished every frame sent to it
+ (void)waitForGPU;

@property (class, nonatomic, assign, readonly, nullable) MTKView* view;
@property (class, nonatomic, assign, readonly) CGSize screenSize;

@end

