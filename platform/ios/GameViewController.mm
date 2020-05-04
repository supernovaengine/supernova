#import "GameViewController.h"
#import "EAGLView.h"
#include "Engine.h"

#define MAX_TOUCHES 10

@interface GameViewController ()

@property (strong, nonatomic) EAGLView *glView;

@end

@implementation GameViewController

static UITouch* touches[MAX_TOUCHES];

- (void)viewDidLoad
{
    
    [super viewDidLoad];
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    int width=rect.size.width;
    int height=rect.size.height;
    
    self.glView = [[EAGLView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
    if([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
        self.glView.contentScaleFactor = [[UIScreen mainScreen] scale];
    }
    self.glView.contentMode = UIViewContentModeScaleToFill;
    self.view = self.glView;
    
    self.view.userInteractionEnabled = YES;
    self.view.multipleTouchEnabled = YES;
}

static CGPoint getNormalizedPoint(UIView* view, CGPoint locationInView)
{
    const float normalizedX = (locationInView.x / view.bounds.size.width) * 2.f - 1.f;
    const float normalizedY = (locationInView.y / view.bounds.size.height) * 2.f - 1.f;
    return CGPointMake(normalizedX, normalizedY);
}

static int getTouchId(UITouch *touch, bool remove = false)
{
    int next = -1;
    for (int i = 0; i < MAX_TOUCHES; i++) {
        if (touches[i] == touch){
            if (remove)
                touches[i] = NULL;
            return i;
        }
        if (next == -1 && touches[i] == NULL) {
            next = i;
        }
    }
    
    if (next != -1) {
        touches[next] = touch;
        return next;
    }
    
    return -1;
}

static void clearTouches()
{
    for (int i = 0; i < MAX_TOUCHES; i++) {
        touches[i] = NULL;
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesBegan:touches withEvent:event];
    for (UITouch *touch in touches) {
        //NSValue *touchValue = [NSValue valueWithPointer:touch];
        CGPoint locationInView = [touch locationInView:self.view];
        CGPoint normalizedPoint = getNormalizedPoint(self.view, locationInView);
        Supernova::Engine::systemTouchStart(getTouchId(touch), normalizedPoint.x, normalizedPoint.y);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesMoved:touches withEvent:event];
    for (UITouch *touch in touches) {
        CGPoint locationInView = [touch locationInView:self.view];
        CGPoint normalizedPoint = getNormalizedPoint(self.view, locationInView);
        Supernova::Engine::systemTouchDrag(getTouchId(touch), normalizedPoint.x, normalizedPoint.y);
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesEnded:touches withEvent:event];
    for (UITouch *touch in touches) {
        CGPoint locationInView = [touch locationInView:self.view];
        CGPoint normalizedPoint = getNormalizedPoint(self.view, locationInView);
        Supernova::Engine::systemTouchEnd(getTouchId(touch, true), normalizedPoint.x, normalizedPoint.y);
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesCancelled:touches withEvent:event];
    clearTouches();
}

- (void) showSoftKeyboard{
    [self.glView becomeFirstResponder];
}

- (void) hideSoftKeyboard{
    [self.glView resignFirstResponder];
}

@end
