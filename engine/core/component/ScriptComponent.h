//
// (c) 2025 Eduardo Doria.
//

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "ScriptProperty.h"

namespace Supernova{

    struct SUPERNOVA_API ScriptComponent {
        std::string path;
        std::string headerPath;
        std::string className;
        bool enabled = false;

        std::vector<ScriptProperty> properties;
    };

}

#endif //SCRIPT_COMPONENT_H
