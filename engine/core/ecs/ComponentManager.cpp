// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ComponentManager.h"

#include <mutex>
#include <string>
#include <unordered_map>

using namespace doriax;

// One registry inside the library, so every module caches the same index for a type.
// Keyed by mangled name, not type_index: identity is then plain string equality.
size_t ComponentTypeRegistry::indexOf(const char* mangledName){
    static std::mutex mutex;
    static std::unordered_map<std::string, size_t> indices;
    static size_t next = 0;

    std::lock_guard<std::mutex> lock(mutex);

    auto it = indices.find(mangledName);
    if (it != indices.end()){
        return it->second;
    }

    indices.emplace(mangledName, next);
    return next++;
}
