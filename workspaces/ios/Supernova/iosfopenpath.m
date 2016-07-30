#include "iosfopenpath.h"

#import <Foundation/Foundation.h>

const char* iosfopenpath(const char* fname) {
        NSMutableString* adjusted_relative_path = [[NSMutableString alloc] initWithString:@"/assets/"];
        [adjusted_relative_path appendString:[[NSString alloc] initWithCString:fname encoding:NSASCIIStringEncoding]];
    
        return [[[NSBundle mainBundle] pathForResource:adjusted_relative_path ofType:nil] cStringUsingEncoding:NSASCIIStringEncoding];
}