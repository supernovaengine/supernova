//
//  Renderer.m
//
//  Created by Eduardo Dória on 24/12/20.
//

#import <simd/simd.h>
#import <ModelIO/ModelIO.h>

#import "Renderer.h"

#include "Engine.h"
#include "SupernovaApple.h"

@implementation Renderer
{
    id <MTLDevice> _device;
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
    Supernova::Engine::systemInit((int)[args count], [self getArray:args]);
    
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
        Supernova::Engine::systemViewLoaded();
    }

    return self;
}

- (void)destroyView
{
    Supernova::Engine::systemShutdown();
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    Supernova::Engine::systemDraw();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    /// Respond to drawable size or orientation changes here
    _screenSize.width = (float) size.width;
    _screenSize.height = (float) size.height;
    Supernova::Engine::systemViewChanged();
}

+ (void)pauseGame {
    Supernova::Engine::systemPause();
}

+ (void)resumeGame {
    Supernova::Engine::systemResume();
}

@end
