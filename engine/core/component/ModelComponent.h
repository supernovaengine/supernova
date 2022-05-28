//
// (c) 2022 Eduardo Doria.
//

#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include <vector>
#include <map>

namespace Supernova{

    struct ModelComponent{
        Matrix4 inverseDerivedTransform;
        
        Entity skeleton;

        std::map<std::string, Entity> bonesNameMapping;
        std::map<int, Entity> bonesIdMapping;

        std::map<std::string, int> morphNameMapping;

        std::vector<Entity> animations;
    };

}

#endif //MODEL_COMPONENT_H