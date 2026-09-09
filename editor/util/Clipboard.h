// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>

namespace doriax::editor {

struct ClipboardImage {
    std::string mimeType;
    std::vector<unsigned char> data;
};

struct ClipboardContent {
    // Copied files from the native file-list format (Windows CF_HDROP, macOS
    // pasteboard file URLs, X11 text/uri-list / x-special/gnome-copied-files).
    std::vector<std::string> filePaths;
    // Encoded image bytes from a native image target (X11 image/*, Windows
    // "PNG"/CF_DIB, macOS pasteboard PNG/TIFF). Only read when no file list
    // is present.
    ClipboardImage image;
};

// One native clipboard probe answering "files, image, or neither" in a single
// session (one X11 display connection and TARGETS round trip; one Win32
// OpenClipboard). Returns empty content when the platform exposes neither;
// clipboardTextToFilePaths() is the text fallback for file lists.
ClipboardContent readClipboardContent();

// Interprets clipboard text as a list of copied files: every non-empty line
// (ignoring a leading GNOME "copy"/"cut" verb and '#' uri-list comments)
// must be a file:// URI resolving to an existing regular file, otherwise the
// text is not a file list and an empty vector is returned. Bare paths are
// deliberately NOT accepted: pasting a path as text must insert the text,
// never hijack it into an attachment — real file copies arrive through
// readClipboardContent() or as file:// URIs.
std::vector<std::string> clipboardTextToFilePaths(const char* clipboardText);

} // namespace doriax::editor
