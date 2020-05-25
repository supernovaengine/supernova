#import "SupernovaIOS.h"
#import "GameViewController.h"
#import "EAGLView.h"


int SupernovaIOS::screenWidth;
int SupernovaIOS::screenHeight;

int SupernovaIOS::getScreenWidth(){
    return screenWidth;
}

int SupernovaIOS::getScreenHeight(){
    return screenHeight;
}

void SupernovaIOS::showVirtualKeyboard(){
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    
    GameViewController *rootViewController = (GameViewController *)window.rootViewController;

    [rootViewController showSoftKeyboard];
}

void SupernovaIOS::hideVirtualKeyboard(){
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    
    GameViewController *rootViewController = (GameViewController *)window.rootViewController;
    
    [rootViewController hideSoftKeyboard];
}

std::string SupernovaIOS::getAssetPath(){
    NSMutableString* adjusted_relative_path = [[NSMutableString alloc] initWithString:@"assets"];
    
    return [[[NSBundle mainBundle] pathForResource:adjusted_relative_path ofType:nil] cStringUsingEncoding:NSASCIIStringEncoding];
}

std::string SupernovaIOS::getUserDataPath(){
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    
    return [documentsDirectory UTF8String];
}

FILE* SupernovaIOS::platformFopen(const char* fname, const char* mode){
    return fopen(fname, mode);
}
