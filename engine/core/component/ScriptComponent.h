//
// (c) 2025 Eduardo Doria.
//

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

struct ScriptComponent {
    std::string scriptPath;
    std::string scriptClassName;
    bool enabled = false;

    std::map<std::string, std::any> variables;
};

#endif //SCRIPT_COMPONENT_H