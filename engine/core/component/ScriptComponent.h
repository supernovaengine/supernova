//
// (c) 2025 Eduardo Doria.
//

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

struct ScriptComponent {
    std::string path;
    std::string className;
    bool enabled = false;

    std::map<std::string, std::any> variables;
};

#endif //SCRIPT_COMPONENT_H