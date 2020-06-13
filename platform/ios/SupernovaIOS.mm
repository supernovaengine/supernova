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

bool SupernovaIOS::getBoolForKey(const char *key, bool defaultValue){
    NSNumber *value = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithUTF8String:key]];
    if (value) {
        return [value boolValue];
    }

    return defaultValue;
}

int SupernovaIOS::getIntegerForKey(const char *key, int defaultValue){
    NSNumber *value = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithUTF8String:key]];
    if (value) {
        return [value intValue];
    }

    return defaultValue;
}

long SupernovaIOS::getLongForKey(const char *key, long defaultValue){
    NSNumber *value = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithUTF8String:key]];
    if (value) {
        return [value longValue];
    }

    return defaultValue;
}

float SupernovaIOS::getFloatForKey(const char *key, float defaultValue){
    NSNumber *value = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithUTF8String:key]];
    if (value) {
        return [value floatValue];
    }

    return defaultValue;
}

double SupernovaIOS::getDoubleForKey(const char *key, double defaultValue){
    NSNumber *value = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithUTF8String:key]];
    if (value) {
        return [value doubleValue];
    }

    return defaultValue;
}

Supernova::Data SupernovaIOS::getDataForKey(const char* key, const Supernova::Data& defaultValue){
    NSData *data = [[NSUserDefaults standardUserDefaults] dataForKey:[NSString stringWithUTF8String:key]];
    if (data){
        return Supernova::Data((unsigned char*)data.bytes, (unsigned int)data.length, true, true);
    }
    
     return defaultValue;
}

std::string SupernovaIOS::getStringForKey(const char *key, std::string defaultValue){
    NSString *str = [[NSUserDefaults standardUserDefaults] stringForKey:[NSString stringWithUTF8String:key]];
    if (str){
        return [str UTF8String];
    }
    
    return defaultValue;
}

void SupernovaIOS::setBoolForKey(const char *key, bool value){
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithBool:value] forKey:[NSString stringWithUTF8String:key]];
}

void SupernovaIOS::setIntegerForKey(const char *key, int value){
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInt:value] forKey:[NSString stringWithUTF8String:key]];
}

void SupernovaIOS::setLongForKey(const char *key, long value){
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithLong:value] forKey:[NSString stringWithUTF8String:key]];
}

void SupernovaIOS::setFloatForKey(const char *key, float value){
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithFloat:value] forKey:[NSString stringWithUTF8String:key]];
}

void SupernovaIOS::setDoubleForKey(const char *key, double value){
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithDouble:value] forKey:[NSString stringWithUTF8String:key]];
}

void SupernovaIOS::setDataForKey(const char* key, Supernova::Data& value){
    [[NSUserDefaults standardUserDefaults] setObject:[NSData dataWithBytes: value.getMemPtr() length: value.length()] forKey:[NSString stringWithUTF8String:key]];
}

void SupernovaIOS::setStringForKey(const char* key, std::string value){
    [[NSUserDefaults standardUserDefaults] setObject:[NSString stringWithUTF8String:value.c_str()] forKey:[NSString stringWithUTF8String:key]];
}

void SupernovaIOS::removeKey(const char *key){
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:[NSString stringWithUTF8String:key]];
}
