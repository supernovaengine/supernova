// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import "ViewController.h"
#import "Renderer.h"
#import "EngineView.h"
#import "DoriaxGameController.h"

// A hidden window updates the game this often, without drawing
static const NSTimeInterval HIDDEN_UPDATE_PERIOD = 1.0 / 60.0;

@implementation ViewController
{
    EngineView *_view;

    Renderer *_renderer;

    NSTimer *_updateTimer;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    NSArray *args = [[NSProcessInfo processInfo] arguments];

    _view = (EngineView *)self.view;

    _view.device = MTLCreateSystemDefaultDevice();

    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        self.view = [[NSView alloc] initWithFrame:self.view.frame];
        return;
    }

    _renderer = [[Renderer alloc] initWithMetalKitView:_view withArgs:args];

    [_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];

    _view.delegate = _renderer;

    // Not on viewDidDisappear, which minimizing also sends
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(destroyView)
                                                 name:NSApplicationWillTerminateNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(updateFrameRate)
                                                 name:NSWindowDidChangeScreenNotification
                                               object:nil];

    // Also sent on minimize and restore
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(updateVisibility)
                                                 name:NSWindowDidChangeOcclusionStateNotification
                                               object:nil];

    // starts hidden: a window opened behind others gets no occlusion change
    [self updateVisibility];
}

- (void)viewDidAppear
{
    [super viewDidAppear];
    [self updateFrameRate];
}

- (void)destroyView
{
    // nothing may call into the engine once it's torn down
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_updateTimer invalidate];
    _updateTimer = nil;
    _view.delegate = nil;

    [_renderer destroyView];
}

// MTKView's default of 60 runs a 75 Hz display at 37.5
- (void)updateFrameRate
{
    NSScreen *screen = self.view.window.screen ?: NSScreen.mainScreen;
    _view.preferredFramesPerSecond = screen.maximumFramesPerSecond;
}

- (void)updateVisibility
{
    BOOL hidden = (self.view.window.occlusionState & NSWindowOcclusionStateVisible) == 0;
    if (hidden == _view.paused)
        return;

    // a hidden game keeps updating, but the pads are no longer read
    [DoriaxGameController setSuspended:hidden];
    _view.paused = hidden;

    if (hidden) {
        // update-only frames free resources without waiting for the GPU
        [Renderer waitForGPU];
        _updateTimer = [NSTimer timerWithTimeInterval:HIDDEN_UPDATE_PERIOD
                                              repeats:YES
                                                block:^(NSTimer *timer) { [Renderer updateGame]; }];
        [[NSRunLoop currentRunLoop] addTimer:_updateTimer forMode:NSRunLoopCommonModes];
    } else {
        [_updateTimer invalidate];
        _updateTimer = nil;
    }
}

@end
