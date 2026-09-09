// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef MacViewVulkan_h
#define MacViewVulkan_h

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

// The Vulkan drawable, for GRAPHIC_BACKEND=vulkan. A Vulkan surface on macOS is
// always a CAMetalLayer, so this is a view backed by one; input is not part of it
// and goes through the same MacInputRouter.
@interface MacViewVulkan : NSView <NSTextInputClient>

// The layer the surface presents into
@property (nonatomic, readonly) CAMetalLayer* metalLayer;

@end

#endif /* MacViewVulkan_h */
