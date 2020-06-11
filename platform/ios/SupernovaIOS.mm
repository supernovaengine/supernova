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
    
    NSString* path = [[NSBundle mainBundle] pathForResource:adjusted_relative_path ofType:nil];
    
    if (!path)
        return "";
    
    return [path cStringUsingEncoding:NSASCIIStringEncoding];
}

std::string SupernovaIOS::getUserDataPath(){
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    
    return [documentsDirectory UTF8String];
}

std::string SupernovaIOS::getLuaPath(){
    NSMutableString* adjusted_relative_path = [[NSMutableString alloc] initWithString:@"lua"];
    
    NSString* path = [[NSBundle mainBundle] pathForResource:adjusted_relative_path ofType:nil];
    
    if (!path)
        return "";
    
    return [path cStringUsingEncoding:NSASCIIStringEncoding];
}
