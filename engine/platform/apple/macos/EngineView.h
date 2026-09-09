// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import "MacViewMetal.h"

// The macOS Metal drawable. Input handling is MacViewMetal's; this subclass
// exists so the storyboard and Renderer keep referring to EngineView.
@interface EngineView : MacViewMetal

@end
