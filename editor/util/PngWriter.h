// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <filesystem>

namespace doriax::editor {

// Encodes and writes a 16-bit single-channel (grayscale) PNG. `pixels` holds
// width*height 16-bit samples in native little-endian order (low byte first);
// the encoder swaps to PNG's big-endian, applies adaptive per-row filtering and
// DEFLATEs via stb_image_write's zlib compressor (stb cannot emit 16-bit PNGs).
// Returns false on invalid arguments or a write/compress failure.
//
// This lives in the editor utility layer (not in any feature/UI/AI module) so the
// terrain editor and the AI terrain capability share one encoder.
bool writeGray16Png(const std::filesystem::path& path, int width, int height,
                    const unsigned char* pixels, std::size_t pixelByteCount);

} // namespace doriax::editor
