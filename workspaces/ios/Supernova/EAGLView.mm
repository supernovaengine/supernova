#import "EAGLView.h"
#include "Engine.h"

@implementation EAGLView{
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    
    GLint backingWidth;
    GLint backingHeight;
}

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (void)insertText:(NSString *)text {
    const char *ctext=[text UTF8String];
    Engine::onTextInput(ctext);
}
- (void)deleteBackward {
    Engine::onTextInput("\b");
}
- (BOOL)hasText {
    return YES;
}
- (BOOL)canBecomeFirstResponder {
    return YES;
}

- (void)setupLayer {
    _eaglLayer = (CAEAGLLayer*) self.layer;
    _eaglLayer.opaque = YES;
    
    _eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                     [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}

- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context) {
        NSLog(@"Failed to initialize OpenGLES 2.0 context");
        exit(1);
    }
    
    if (![EAGLContext setCurrentContext:_context]) {
        NSLog(@"Failed to set current OpenGL context");
        exit(1);
    }
}

- (void)deleteBuffers {
    glDeleteRenderbuffers(1, &_depthRenderBuffer);
    glDeleteRenderbuffers(1, &_colorRenderBuffer);
}

- (void)setupRenderBuffer {
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}

- (void)setupDepthBuffer {
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
}

- (void)setupFrameBuffer {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
    
}

- (void)setupDisplayLink {
    CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
}

- (void)render:(CADisplayLink*)displayLink {

    Engine::onDrawFrame();
    
    [_context presentRenderbuffer:GL_RENDERBUFFER];
    
}

- (void) layoutSubviews
{
    CGFloat screenScale = [UIScreen mainScreen].scale;
    backingWidth = self.frame.size.width * screenScale;
    backingHeight = self.frame.size.height * screenScale;
    
    [self deleteBuffers];
    
    [self setupDepthBuffer];
    [self setupRenderBuffer];
    [self setupFrameBuffer];
    
    Engine::onSurfaceChanged(backingWidth, backingHeight);
    
    [self render:nil];
}


- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {

        [self setupLayer];
        [self setupContext];
        
        Engine::onSurfaceCreated();
        
        [self setupDisplayLink];
    }
    return self;
}

- (void)dealloc
{
    
    if ([EAGLContext currentContext] == _context) {
        [EAGLContext setCurrentContext:nil];
    }
}

@end