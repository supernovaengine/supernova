// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "util/PngWriter.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace fs = std::filesystem;

namespace doriax::editor {

// Public DEFLATE (zlib) compressor from the stb_image_write implementation TU
// (engine/libs/stb/stb_image_write.c), reused so we don't hand-roll a compressor.
extern "C" unsigned char* stbi_zlib_compress(unsigned char* data, int data_len, int* out_len, int quality);

namespace {

uint32_t pngCrc32(const unsigned char* data, size_t len, uint32_t crc) {
    crc = ~crc;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            crc = (crc >> 1) ^ (0xEDB88320u & (~(crc & 1) + 1));
        }
    }
    return ~crc;
}

void pngPutU32(std::vector<unsigned char>& out, uint32_t v) {
    out.push_back(static_cast<unsigned char>((v >> 24) & 0xFF));
    out.push_back(static_cast<unsigned char>((v >> 16) & 0xFF));
    out.push_back(static_cast<unsigned char>((v >> 8) & 0xFF));
    out.push_back(static_cast<unsigned char>(v & 0xFF));
}

void pngWriteChunk(std::vector<unsigned char>& out, const char tag[4], const unsigned char* data, size_t len) {
    pngPutU32(out, static_cast<uint32_t>(len));
    const size_t typeStart = out.size();
    out.insert(out.end(), tag, tag + 4);
    if (len) out.insert(out.end(), data, data + len);
    pngPutU32(out, pngCrc32(out.data() + typeStart, len + 4, 0));
}

unsigned char pngPaeth(int a, int b, int c) {
    const int p = a + b - c;
    const int pa = std::abs(p - a);
    const int pb = std::abs(p - b);
    const int pc = std::abs(p - c);
    if (pa <= pb && pa <= pc) return static_cast<unsigned char>(a);
    return pb <= pc ? static_cast<unsigned char>(b) : static_cast<unsigned char>(c);
}

} // namespace

bool writeGray16Png(const fs::path& path, int width, int height,
                    const unsigned char* pixels, std::size_t pixelByteCount) {
    const int bpp = 2; // bytes per 16-bit sample
    const size_t rowBytes = static_cast<size_t>(width) * bpp;
    if (width <= 0 || height <= 0 || !pixels ||
        pixelByteCount < rowBytes * static_cast<size_t>(height)) {
        return false;
    }

    // PNG stores 16-bit samples big-endian; the source is native little-endian.
    std::vector<unsigned char> rows(static_cast<size_t>(height) * rowBytes);
    for (int y = 0; y < height; ++y) {
        const unsigned char* src = pixels + static_cast<size_t>(y) * rowBytes;
        unsigned char* dst = rows.data() + static_cast<size_t>(y) * rowBytes;
        for (int x = 0; x < width; ++x) {
            dst[x * 2] = src[x * 2 + 1]; // high byte first
            dst[x * 2 + 1] = src[x * 2];
        }
    }

    // Adaptive per-row filtering: pick the filter minimizing the sum of absolute
    // signed bytes (the stb/libpng heuristic) so smooth gradients compress well.
    std::vector<unsigned char> filtered;
    filtered.reserve(static_cast<size_t>(height) * (1 + rowBytes));
    std::vector<unsigned char> candidate(rowBytes);
    std::vector<unsigned char> best(rowBytes);
    for (int y = 0; y < height; ++y) {
        const unsigned char* cur = rows.data() + static_cast<size_t>(y) * rowBytes;
        const unsigned char* prior = y > 0 ? rows.data() + static_cast<size_t>(y - 1) * rowBytes : nullptr;

        int bestType = 0;
        long bestCost = -1;
        for (int ft = 0; ft <= 4; ++ft) {
            long cost = 0;
            for (size_t i = 0; i < rowBytes; ++i) {
                const int raw = cur[i];
                const int a = i >= static_cast<size_t>(bpp) ? cur[i - bpp] : 0;        // left
                const int b = prior ? prior[i] : 0;                                   // up
                const int c = (prior && i >= static_cast<size_t>(bpp)) ? prior[i - bpp] : 0; // up-left
                int f;
                switch (ft) {
                    case 1: f = raw - a; break;                  // Sub
                    case 2: f = raw - b; break;                  // Up
                    case 3: f = raw - ((a + b) >> 1); break;     // Average
                    case 4: f = raw - pngPaeth(a, b, c); break;  // Paeth
                    default: f = raw; break;                     // None
                }
                const unsigned char fb = static_cast<unsigned char>(f & 0xFF);
                candidate[i] = fb;
                cost += std::abs(static_cast<int>(static_cast<signed char>(fb)));
            }
            if (bestCost < 0 || cost < bestCost) {
                bestCost = cost;
                bestType = ft;
                best.swap(candidate);
            }
        }
        filtered.push_back(static_cast<unsigned char>(bestType));
        filtered.insert(filtered.end(), best.begin(), best.end());
    }

    int zlen = 0;
    unsigned char* zdata = stbi_zlib_compress(filtered.data(), static_cast<int>(filtered.size()), &zlen, 8);
    if (!zdata) return false;

    std::vector<unsigned char> out;
    const unsigned char sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    out.insert(out.end(), sig, sig + 8);

    std::vector<unsigned char> ihdr;
    pngPutU32(ihdr, static_cast<uint32_t>(width));
    pngPutU32(ihdr, static_cast<uint32_t>(height));
    ihdr.push_back(16); // bit depth
    ihdr.push_back(0);  // color type: grayscale
    ihdr.push_back(0);  // compression
    ihdr.push_back(0);  // filter
    ihdr.push_back(0);  // interlace
    pngWriteChunk(out, "IHDR", ihdr.data(), ihdr.size());
    pngWriteChunk(out, "IDAT", zdata, static_cast<size_t>(zlen));
    pngWriteChunk(out, "IEND", nullptr, 0);

    // stbi_zlib_compress allocates with stb's default allocator (malloc).
    std::free(zdata);

    FILE* f = std::fopen(path.string().c_str(), "wb");
    if (!f) return false;
    const size_t wrote = std::fwrite(out.data(), 1, out.size(), f);
    std::fclose(f);
    return wrote == out.size();
}

} // namespace doriax::editor
