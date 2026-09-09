// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "util/Clipboard.h"

#import <AppKit/AppKit.h>

namespace doriax::editor {

// Declared in Clipboard.cpp; composed into readClipboardContent() on __APPLE__.

bool readClipboardImageMac(ClipboardImage& out) {
    @autoreleasepool {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];

        NSData* png = [pasteboard dataForType:NSPasteboardTypePNG];
        if (png != nil && png.length > 0) {
            const unsigned char* bytes = static_cast<const unsigned char*>(png.bytes);
            out.mimeType = "image/png";
            out.data.assign(bytes, bytes + png.length);
            return true;
        }

        // Screenshots and "Copy image" commonly land as TIFF; re-encode to PNG
        // since providers do not accept TIFF.
        NSData* tiff = [pasteboard dataForType:NSPasteboardTypeTIFF];
        if (tiff != nil && tiff.length > 0) {
            NSBitmapImageRep* rep = [NSBitmapImageRep imageRepWithData:tiff];
            if (rep != nil) {
                NSData* converted = [rep representationUsingType:NSBitmapImageFileTypePNG
                                                      properties:@{}];
                if (converted != nil && converted.length > 0) {
                    const unsigned char* bytes =
                        static_cast<const unsigned char*>(converted.bytes);
                    out.mimeType = "image/png";
                    out.data.assign(bytes, bytes + converted.length);
                    return true;
                }
            }
        }

        return false;
    }
}

std::vector<std::string> readClipboardFilePathsMac() {
    std::vector<std::string> paths;
    @autoreleasepool {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        NSArray<NSURL*>* urls =
            [pasteboard readObjectsForClasses:@[ [NSURL class] ]
                                      options:@{NSPasteboardURLReadingFileURLsOnlyKey : @YES}];
        for (NSURL* url in urls) {
            const char* path = url.path.UTF8String;
            if (path != nullptr && path[0] != '\0') {
                paths.emplace_back(path);
            }
        }
    }
    return paths;
}

} // namespace doriax::editor
