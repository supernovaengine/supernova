// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "util/Clipboard.h"
#include "util/FileUtils.h"

#include <algorithm>
#include <cctype>
#include <filesystem>

#ifdef DORIAX_HAS_X11_CLIPBOARD
#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include <chrono>
#include <thread>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>

// PNG encoder from the stb implementation TU the engine already links
// (engine/libs/stb/stb_image_write.c); same reuse pattern as PngWriter.cpp.
extern "C" {
typedef void stbi_write_func(void* context, void* data, int size);
int stbi_write_png_to_func(stbi_write_func* func, void* context, int w, int h,
                           int comp, const void* data, int stride_in_bytes);
}
#endif

namespace fs = std::filesystem;

namespace doriax::editor {

#ifdef __APPLE__
// Implemented in ClipboardMac.mm (Objective-C++, NSPasteboard).
bool readClipboardImageMac(ClipboardImage& out);
std::vector<std::string> readClipboardFilePathsMac();
#endif

namespace {

std::string trimClipboardLine(std::string line) {
    while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back()))) {
        line.pop_back();
    }
    size_t first = 0;
    while (first < line.size() &&
           std::isspace(static_cast<unsigned char>(line[first]))) {
        ++first;
    }
    line.erase(0, first);
    if (line.size() >= 2 &&
        ((line.front() == '"' && line.back() == '"') ||
         (line.front() == '\'' && line.back() == '\''))) {
        line = line.substr(1, line.size() - 2);
    }
    return line;
}

int hexDigitValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

std::string decodeFileUri(std::string value) {
    if (value.rfind("file://", 0) == 0) {
        value.erase(0, 7);
        if (value.rfind("localhost/", 0) == 0) value.erase(0, 9);
    }
    std::string decoded;
    decoded.reserve(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2 < value.size()) {
            const int high = hexDigitValue(value[i + 1]);
            const int low = hexDigitValue(value[i + 2]);
            if (high >= 0 && low >= 0) {
                decoded.push_back(static_cast<char>((high << 4) | low));
                i += 2;
                continue;
            }
        }
        decoded.push_back(value[i]);
    }
#ifdef _WIN32
    if (decoded.size() >= 3 && decoded[0] == '/' && decoded[2] == ':') {
        decoded.erase(decoded.begin());
    }
#endif
    return decoded;
}

// Shared by the text-paste heuristic and the native X11 uri-list targets,
// which carry the same format: newline-separated file:// URIs, optionally led
// by a GNOME "copy"/"cut" verb, with '#' lines as uri-list comments. Every
// other line must be a file:// URI resolving to an existing regular file,
// otherwise the content is not a file list and an empty vector is returned —
// in particular, a bare path pasted as text must stay text.
std::vector<std::string> parseFileUriList(const std::string& text) {
    std::vector<std::string> paths;
    bool firstLine = true;
    size_t start = 0;
    while (start <= text.size()) {
        const size_t end = text.find_first_of("\r\n", start);
        std::string line = trimClipboardLine(
            text.substr(start, end == std::string::npos ? std::string::npos : end - start));
        const bool verb = firstLine && (line == "copy" || line == "cut");
        firstLine = false;
        if (!line.empty() && !verb && line.front() != '#') {
            if (line.rfind("file://", 0) != 0) {
                return {};
            }
            fs::path path = FileUtils::pathFromUtf8(decodeFileUri(line));
            std::error_code ec;
            if (!fs::is_regular_file(path, ec) || ec) {
                return {};
            }
            paths.push_back(FileUtils::pathToUtf8(path));
        }
        if (end == std::string::npos) break;
        start = end + 1;
        while (start < text.size() && (text[start] == '\r' || text[start] == '\n')) ++start;
    }
    return paths;
}

} // namespace

std::vector<std::string> clipboardTextToFilePaths(const char* clipboardText) {
    if (!clipboardText) return {};
    return parseFileUriList(clipboardText);
}

#ifdef DORIAX_HAS_X11_CLIPBOARD
namespace {

constexpr auto kClipboardTimeout = std::chrono::milliseconds(500);

bool waitForSelectionNotify(Display* display, Window window, XSelectionEvent& selection) {
    const auto deadline = std::chrono::steady_clock::now() + kClipboardTimeout;
    while (std::chrono::steady_clock::now() < deadline) {
        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);
            if (event.type == SelectionNotify && event.xselection.requestor == window) {
                selection = event.xselection;
                return true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return false;
}

bool waitForProperty(Display* display, Window window, Atom property) {
    const auto deadline = std::chrono::steady_clock::now() + kClipboardTimeout;
    while (std::chrono::steady_clock::now() < deadline) {
        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);
            if (event.type == PropertyNotify && event.xproperty.window == window &&
                event.xproperty.atom == property &&
                event.xproperty.state == PropertyNewValue) {
                return true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return false;
}

bool readProperty(Display* display, Window window, Atom property, bool remove,
                  Atom& actualType, std::vector<unsigned char>& data) {
    int format = 0;
    unsigned long itemCount = 0;
    unsigned long bytesAfter = 0;
    unsigned char* value = nullptr;
    const int status = XGetWindowProperty(
        display, window, property, 0, 0x1fffffffL, remove ? True : False,
        AnyPropertyType, &actualType, &format, &itemCount, &bytesAfter, &value);
    if (status != Success) {
        if (value) XFree(value);
        return false;
    }

    const size_t bytesPerItem = format > 0 ? static_cast<size_t>(format) / 8 : 0;
    if (value && bytesPerItem > 0 && itemCount > 0) {
        const size_t byteCount = static_cast<size_t>(itemCount) * bytesPerItem;
        data.insert(data.end(), value, value + byteCount);
    }
    if (value) XFree(value);
    return bytesAfter == 0;
}

// Asks the selection owner which targets it offers, so absent image targets
// cost one round trip instead of one full conversion timeout per mime type.
bool fetchTargets(Display* display, Window window, Atom clipboard, std::vector<Atom>& out) {
    const Atom targetsAtom = XInternAtom(display, "TARGETS", False);
    const Atom property = XInternAtom(display, "DORIAX_CLIPBOARD_TARGETS", False);
    XDeleteProperty(display, window, property);
    XConvertSelection(display, clipboard, targetsAtom, property, window, CurrentTime);
    XFlush(display);

    XSelectionEvent selection{};
    if (!waitForSelectionNotify(display, window, selection) || selection.property == None) {
        return false;
    }

    Atom actualType = None;
    int format = 0;
    unsigned long itemCount = 0;
    unsigned long bytesAfter = 0;
    unsigned char* value = nullptr;
    const int status = XGetWindowProperty(
        display, window, property, 0, 0x1fffffffL, True, XA_ATOM,
        &actualType, &format, &itemCount, &bytesAfter, &value);
    if (status != Success || !value) {
        if (value) XFree(value);
        return false;
    }
    // Format-32 property data arrives as C longs regardless of pointer size,
    // so it must be read as long[], not as packed 32-bit words.
    if (format == 32) {
        const long* atoms = reinterpret_cast<const long*>(value);
        out.reserve(itemCount);
        for (unsigned long i = 0; i < itemCount; ++i) {
            out.push_back(static_cast<Atom>(atoms[i]));
        }
    }
    XFree(value);
    return !out.empty();
}

bool requestTarget(Display* display, Window window, Atom clipboard, const char* mimeType,
                   std::vector<unsigned char>& out) {
    const Atom target = XInternAtom(display, mimeType, False);
    const Atom property = XInternAtom(display, "DORIAX_CLIPBOARD_IMAGE", False);
    const Atom incr = XInternAtom(display, "INCR", False);
    XDeleteProperty(display, window, property);
    XConvertSelection(display, clipboard, target, property, window, CurrentTime);
    XFlush(display);

    XSelectionEvent selection{};
    if (!waitForSelectionNotify(display, window, selection) || selection.property == None) {
        return false;
    }

    Atom actualType = None;
    std::vector<unsigned char> firstChunk;
    if (!readProperty(display, window, property, true, actualType, firstChunk)) {
        return false;
    }
    if (actualType != incr) {
        out = std::move(firstChunk);
        return !out.empty();
    }

    // INCR transfers deliver chunks through PropertyNotify after the requestor
    // deletes each previous value. The zero-length final property terminates it.
    out.clear();
    XDeleteProperty(display, window, property);
    XFlush(display);
    while (waitForProperty(display, window, property)) {
        std::vector<unsigned char> chunk;
        actualType = None;
        if (!readProperty(display, window, property, true, actualType, chunk)) {
            return false;
        }
        if (chunk.empty()) {
            return !out.empty();
        }
        out.insert(out.end(), chunk.begin(), chunk.end());
        XFlush(display);
    }
    return false;
}

// One clipboard session: a single display connection and TARGETS round trip
// answers both "is it a file list?" and "is it an image?". A file list
// outranks any image target — every Linux file manager publishes copied
// files as text/uri-list (GNOME additionally as x-special/gnome-copied-files).
ClipboardContent readClipboardContentX11() {
    ClipboardContent content;
    Display* display = XOpenDisplay(nullptr);
    if (!display) return content;

    const int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                        0, 0, 1, 1, 0, 0, 0);
    XSelectInput(display, window, PropertyChangeMask);
    const Atom clipboard = XInternAtom(display, "CLIPBOARD", False);

    std::vector<Atom> targets;
    if (fetchTargets(display, window, clipboard, targets)) {
        const auto offered = [&](const char* targetName) {
            const Atom target = XInternAtom(display, targetName, False);
            return std::find(targets.begin(), targets.end(), target) != targets.end();
        };

        for (const char* targetName : {"x-special/gnome-copied-files", "text/uri-list"}) {
            if (!offered(targetName)) continue;
            std::vector<unsigned char> data;
            if (requestTarget(display, window, clipboard, targetName, data)) {
                content.filePaths = parseFileUriList(std::string(data.begin(), data.end()));
                if (!content.filePaths.empty()) break;
            }
        }

        if (content.filePaths.empty()) {
            for (const char* mimeType : {"image/png", "image/jpeg", "image/webp", "image/gif"}) {
                if (!offered(mimeType)) continue;
                std::vector<unsigned char> data;
                if (requestTarget(display, window, clipboard, mimeType, data)) {
                    content.image.mimeType = mimeType;
                    content.image.data = std::move(data);
                    break;
                }
            }
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return content;
}

} // namespace
#endif // DORIAX_HAS_X11_CLIPBOARD

#ifdef _WIN32
namespace {

void appendPngBytes(void* context, void* data, int size) {
    auto* out = static_cast<std::vector<unsigned char>*>(context);
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    out->insert(out->end(), bytes, bytes + size);
}

// Converts the common uncompressed 24/32-bit DIB layouts to PNG. Palette and
// exotic bitfield DIBs are rejected; Windows synthesizes CF_DIB from
// CF_BITMAP/CF_DIBV5, so this covers screenshots and "Copy image" sources.
bool dibToPng(const unsigned char* dib, size_t dataSize, std::vector<unsigned char>& png) {
    if (!dib || dataSize < sizeof(BITMAPINFOHEADER)) return false;
    const BITMAPINFOHEADER* header = reinterpret_cast<const BITMAPINFOHEADER*>(dib);
    if (header->biSize < sizeof(BITMAPINFOHEADER) || header->biSize > dataSize) return false;

    const LONG width = header->biWidth;
    const LONG absHeight = header->biHeight >= 0 ? header->biHeight : -header->biHeight;
    const bool bottomUp = header->biHeight >= 0;
    if (width <= 0 || absHeight <= 0 || width > 16384 || absHeight > 16384) return false;
    if (header->biBitCount != 24 && header->biBitCount != 32) return false;

    size_t pixelOffset = header->biSize + static_cast<size_t>(header->biClrUsed) * 4;
    if (header->biCompression == BI_BITFIELDS) {
        // Accept only the standard BGR(A) masks. They sit right after the
        // core BITMAPINFOHEADER fields in every header version: a plain
        // header appends them (extending the pixel offset), V4/V5 embed them.
        if (header->biSize == sizeof(BITMAPINFOHEADER)) {
            if (dataSize < pixelOffset + 12) return false;
            pixelOffset += 12;
        } else if (header->biSize < sizeof(BITMAPINFOHEADER) + 12) {
            return false;
        }
        const unsigned char* maskBytes = dib + sizeof(BITMAPINFOHEADER);
        DWORD masks[3];
        std::copy(maskBytes, maskBytes + 12, reinterpret_cast<unsigned char*>(masks));
        if (masks[0] != 0x00ff0000u || masks[1] != 0x0000ff00u || masks[2] != 0x000000ffu) {
            return false;
        }
    } else if (header->biCompression != BI_RGB) {
        return false;
    }

    const size_t bytesPerPixel = header->biBitCount / 8;
    const size_t stride = (static_cast<size_t>(width) * bytesPerPixel + 3) & ~static_cast<size_t>(3);
    if (dataSize < pixelOffset + stride * static_cast<size_t>(absHeight)) return false;
    const unsigned char* pixels = dib + pixelOffset;

    std::vector<unsigned char> rgba(static_cast<size_t>(width) * absHeight * 4);
    bool anyAlpha = false;
    for (LONG y = 0; y < absHeight; ++y) {
        const unsigned char* row = pixels + stride * static_cast<size_t>(bottomUp ? absHeight - 1 - y : y);
        unsigned char* out = rgba.data() + static_cast<size_t>(y) * width * 4;
        for (LONG x = 0; x < width; ++x) {
            const unsigned char* px = row + static_cast<size_t>(x) * bytesPerPixel;
            out[0] = px[2];
            out[1] = px[1];
            out[2] = px[0];
            out[3] = bytesPerPixel == 4 ? px[3] : 255;
            anyAlpha = anyAlpha || (bytesPerPixel == 4 && px[3] != 0);
            out += 4;
        }
    }
    // 32-bit DIBs routinely carry a zeroed alpha channel for opaque images;
    // treat an all-zero channel as opaque instead of fully transparent.
    if (bytesPerPixel == 4 && !anyAlpha) {
        for (size_t i = 3; i < rgba.size(); i += 4) rgba[i] = 255;
    }

    png.clear();
    return stbi_write_png_to_func(appendPngBytes, &png, static_cast<int>(width),
                                  static_cast<int>(absHeight), 4, rgba.data(),
                                  static_cast<int>(width) * 4) != 0;
}

// One clipboard session: files (CF_HDROP) outrank image formats.
ClipboardContent readClipboardContentWin32() {
    ClipboardContent content;
    if (!OpenClipboard(nullptr)) return content;

    if (HANDLE data = GetClipboardData(CF_HDROP)) {
        HDROP drop = static_cast<HDROP>(data);
        const UINT count = DragQueryFileW(drop, 0xffffffffu, nullptr, 0);
        for (UINT i = 0; i < count; ++i) {
            const UINT length = DragQueryFileW(drop, i, nullptr, 0);
            std::wstring wide(static_cast<size_t>(length) + 1, L'\0');
            DragQueryFileW(drop, i, wide.data(), length + 1);
            wide.resize(length);
            const int utf8Length = WideCharToMultiByte(
                CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), nullptr, 0,
                nullptr, nullptr);
            std::string path(static_cast<size_t>(utf8Length), '\0');
            WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()),
                                path.data(), utf8Length, nullptr, nullptr);
            content.filePaths.push_back(std::move(path));
        }
    }

    // Lossless path first: browsers and image editors publish a "PNG" format.
    if (content.filePaths.empty()) {
        const UINT pngFormat = RegisterClipboardFormatA("PNG");
        if (HANDLE handle = GetClipboardData(pngFormat)) {
            const SIZE_T size = GlobalSize(handle);
            if (const void* bytes = GlobalLock(handle)) {
                if (size > 0) {
                    content.image.mimeType = "image/png";
                    content.image.data.assign(
                        static_cast<const unsigned char*>(bytes),
                        static_cast<const unsigned char*>(bytes) + size);
                }
                GlobalUnlock(handle);
            }
        }
    }

    if (content.filePaths.empty() && content.image.data.empty()) {
        if (HANDLE handle = GetClipboardData(CF_DIB)) {
            const SIZE_T size = GlobalSize(handle);
            if (const void* bytes = GlobalLock(handle)) {
                std::vector<unsigned char> png;
                if (dibToPng(static_cast<const unsigned char*>(bytes), size, png)) {
                    content.image.mimeType = "image/png";
                    content.image.data = std::move(png);
                }
                GlobalUnlock(handle);
            }
        }
    }

    CloseClipboard();
    return content;
}

} // namespace
#endif // _WIN32

ClipboardContent readClipboardContent() {
#if defined(DORIAX_HAS_X11_CLIPBOARD)
    return readClipboardContentX11();
#elif defined(_WIN32)
    return readClipboardContentWin32();
#elif defined(__APPLE__)
    ClipboardContent content;
    content.filePaths = readClipboardFilePathsMac();
    if (content.filePaths.empty()) {
        readClipboardImageMac(content.image);
    }
    return content;
#else
    // Native Wayland without X11: file managers still surface file:// URIs
    // through the text clipboard, which clipboardTextToFilePaths handles.
    return {};
#endif
}

} // namespace doriax::editor
