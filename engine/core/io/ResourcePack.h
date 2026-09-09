// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef RESOURCEPACK_H
#define RESOURCEPACK_H

#include "Export.h"

#include <string>
#include <vector>

namespace doriax {

    // Native exports can pack assets and Lua sources into a single "resources.pak"
    // (Project Settings > Build). Entries are obfuscated, not encrypted. Data::open()
    // reads through it; File keeps to the filesystem and the editor never opens a pack.
    class DORIAX_API ResourcePack {
    public:
        // Paths are the ones Data::open() takes: asset://, lua:// or asset-relative
        static bool contains(const std::string& path);
        static bool read(const std::string& path, std::vector<unsigned char>& outData);
    };

}

#endif
