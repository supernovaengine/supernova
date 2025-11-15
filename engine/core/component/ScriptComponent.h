//
// (c) 2025 Eduardo Doria.
//

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "ScriptProperty.h"
#include <vector>

namespace Supernova{

    enum class ScriptType {
        SUBCLASS,      // C++ subclass of Shape (or EntityHandle)
        SCRIPT_CLASS,  // C++ ScriptBase
        SCRIPT_LUA,    // Lua behavior script (Script table with properties + methods)
    };

    struct SUPERNOVA_API ScriptEntry {
        ScriptType type;
        std::string path;        // .cpp or .lua (for Lua: script file path)
        std::string headerPath;  // for C++; empty for Lua
        std::string className;   // C++ class or Lua module name (file base name)
        bool enabled = false;
        std::vector<ScriptProperty> properties;

        void* instance = nullptr; // C++ instance OR Lua handle
    };

    struct SUPERNOVA_API ScriptComponent {
        std::vector<ScriptEntry> scripts;
    };

}

#endif //SCRIPT_COMPONENT_H