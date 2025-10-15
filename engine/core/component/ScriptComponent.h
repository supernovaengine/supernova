//
// (c) 2025 Eduardo Doria.
//

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "ScriptProperty.h"
#include <vector>

namespace Supernova{

    enum class ScriptType {
        SUBCLASS,    // Inherits from EntityHandle
        PLAIN_CLASS  // Plain C++ class
    };

    struct SUPERNOVA_API ScriptEntry {
        ScriptType type;
        std::string path;
        std::string headerPath;
        std::string className;
        bool enabled = false;
        std::vector<ScriptProperty> properties;

        void* instance = nullptr;
    };

    struct SUPERNOVA_API ScriptComponent {
        std::vector<ScriptEntry> scripts;
    };

}

#endif //SCRIPT_COMPONENT_H