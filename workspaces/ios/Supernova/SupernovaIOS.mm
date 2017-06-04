#import "SupernovaIOS.h"
#import "GameViewController.h"

void SupernovaIOS::showSoftKeyboard(){
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    
    GameViewController *rootViewController = (GameViewController *)window.rootViewController;

    [rootViewController showSoftKeyboard];
}

void SupernovaIOS::hideSoftKeyboard(){
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    
    GameViewController *rootViewController = (GameViewController *)window.rootViewController;
    
    [rootViewController hideSoftKeyboard];
}

const char* SupernovaIOS::getFullPath(const char* filename) {
    NSMutableString* adjusted_relative_path = [[NSMutableString alloc] initWithString:@"/project/"];
    [adjusted_relative_path appendString:[[NSString alloc] initWithCString:filename encoding:NSASCIIStringEncoding]];
    
    return [[[NSBundle mainBundle] pathForResource:adjusted_relative_path ofType:nil] cStringUsingEncoding:NSASCIIStringEncoding];
}
