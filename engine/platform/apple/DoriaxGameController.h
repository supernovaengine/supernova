// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef DoriaxGameController_h
#define DoriaxGameController_h

#import <Foundation/Foundation.h>

// Bridges Apple's GameController framework (GCController) to the engine's
// systemGamepad* events. Shared by the iOS and macOS backends.
@interface DoriaxGameController : NSObject

// Idempotent: starts observing controller connect/disconnect and registers any
// already-connected controllers. Call once after Engine::systemInit.
+ (void)start;

@end

#endif /* DoriaxGameController_h */
