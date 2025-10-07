//
// (c) 2025 Eduardo Doria.
//

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "ScriptProperty.h"

struct ScriptComponent {
    std::string path;
    std::string className;
    bool enabled = false;

    std::vector<Supernova::ScriptProperty> properties;
};

#endif //SCRIPT_COMPONENT_H