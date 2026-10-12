// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import <simd/simd.h>
#import <ModelIO/ModelIO.h>

#import "Renderer.h"

#include "Engine.h"
#include "DoriaxApple.h"
#import "DoriaxGameController.h"

#if TARGET_OS_OSX
// A hidden window updates the game this often, without drawing
static const NSTimeInterval HIDDEN_UPDATE_PERIOD = 1.0 / 60.0;
#endif

@implementation Renderer
{
    id <MTLDevice> _device;
#if TARGET_OS_OSX
    NSTimer* _hiddenTimer;
#endif
}

static MTKView* _view;
static CGSize _screenSize;

+ (MTKView*)view {
    return _view;
}

+ (CGSize)screenSize {
    return _screenSize;
}

-(char**)getArray:(NSArray*)a_array
{
    unsigned long count = [a_array count];
    char **array = (char **)malloc((count + 1) * sizeof(char*));

    for (unsigned i = 0; i < count; i++)
    {
         array[i] = strdup([[a_array objectAtIndex:i] UTF8String]);
    }
    array[count] = NULL;
    return array;
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view withArgs:(NSArray*)args;
{
    doriax::Engine::systemInit((int)[args count], [self getArray:args], new DoriaxApple());

    [DoriaxGameController start];

    self = [super init];
    if(self)
    {
        _view = view;
        _device = view.device;
        
        //[view setPreferredFramesPerSecond:60];
        //[view setSampleCount:4]; //causing issues in real devices when using SG_LOADACTION_LOAD
        [view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [view setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];
        
        _screenSize.width = (float) view.drawableSize.width;
        _screenSize.height = (float) view.drawableSize.height;
        doriax::Engine::systemViewLoaded();

#if TARGET_OS_OSX
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowDidChangeOcclusionState:)
                                                     name:NSWindowDidChangeOcclusionStateNotification
                                                   object:nil];
#endif
    }

    return self;
}

- (void)destroyView
{
    doriax::Engine::systemViewDestroyed();
    doriax::Engine::systemShutdown();
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    doriax::Engine::systemDraw();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    /// Respond to drawable size or orientation changes here
    _screenSize.width = (float) size.width;
    _screenSize.height = (float) size.height;
    doriax::Engine::systemViewChanged();
}

#if TARGET_OS_OSX
// Also sent on minimize and restore. Same test as the editor: a window fully
// covered, on another Space or of a hidden app is hidden too.
- (void)windowDidChangeOcclusionState:(NSNotification *)notification
{
    NSWindow* window = notification.object;
    if (window != _view.window)
        return;

    const bool hidden = window.miniaturized ||
        (window.occlusionState & NSWindowOcclusionStateVisible) == 0;
    if (hidden == (_hiddenTimer != nil))
        return;

    // a hidden game keeps updating, but the pads are no longer read
    [DoriaxGameController setSuspended:hidden];
    // Nothing drawn can be seen, so the game updates at a steady rate instead
    _view.paused = hidden;
    if (hidden) {
        _hiddenTimer = [NSTimer timerWithTimeInterval:HIDDEN_UPDATE_PERIOD
                                              repeats:YES
                                                block:^(NSTimer*){ doriax::Engine::systemUpdate(); }];
        [[NSRunLoop currentRunLoop] addTimer:_hiddenTimer forMode:NSRunLoopCommonModes];
    } else {
        [_hiddenTimer invalidate];
        _hiddenTimer = nil;
    }
}
#endif

+ (void)pauseGame {
    doriax::Engine::systemPause();
}

+ (void)resumeGame {
    doriax::Engine::systemResume();
}

@end
