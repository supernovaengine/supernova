#import "GameViewController.h"
#import "EAGLView.h"
#include "Engine.h"


@interface GameViewController ()

@property (strong, nonatomic) EAGLView *glView;

@end

@implementation GameViewController

- (void)viewDidLoad
{
    
    [super viewDidLoad];
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    int width=rect.size.width;
    int height=rect.size.height;
    
    CGFloat screenScale = [UIScreen mainScreen].scale;
    
    Supernova::Engine::onStart(width*screenScale, height*screenScale);
    
    self.glView = [[EAGLView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
    if([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
        self.glView.contentScaleFactor = [[UIScreen mainScreen] scale];
    }
    self.glView.contentMode = UIViewContentModeScaleToFill;
    self.view = self.glView;
    
    self.view.userInteractionEnabled = YES;
}

static CGPoint getNormalizedPoint(UIView* view, CGPoint locationInView)
{
    const float normalizedX = (locationInView.x / view.bounds.size.width) * 2.f - 1.f;
    const float normalizedY = -((locationInView.y / view.bounds.size.height) * 2.f - 1.f);
    return CGPointMake(normalizedX, normalizedY);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesBegan:touches withEvent:event];
    UITouch* touchEvent = [touches anyObject];
    CGPoint locationInView = [touchEvent locationInView:self.view];
    CGPoint normalizedPoint = getNormalizedPoint(self.view, locationInView);
    Supernova::Engine::onTouchPress(normalizedPoint.x, normalizedPoint.y);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesMoved:touches withEvent:event];
    UITouch* touchEvent = [touches anyObject];
    CGPoint locationInView = [touchEvent locationInView:self.view];
    CGPoint normalizedPoint = getNormalizedPoint(self.view, locationInView);
    Supernova::Engine::onTouchDrag(normalizedPoint.x, normalizedPoint.y);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesEnded:touches withEvent:event];
    UITouch* touchEvent = [touches anyObject];
    CGPoint locationInView = [touchEvent locationInView:self.view];
    CGPoint normalizedPoint = getNormalizedPoint(self.view, locationInView);
    Supernova::Engine::onTouchUp(normalizedPoint.x, normalizedPoint.y);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesCancelled:touches withEvent:event];
}

- (void) showSoftKeyboard{
    [self.glView becomeFirstResponder];
}

- (void) hideSoftKeyboard{
    [self.glView resignFirstResponder];
}

@end
