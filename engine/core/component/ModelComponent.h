//
// (c) 2024 Eduardo Doria.
//

#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include "buffer/ExternalBuffer.h"
#include <vector>
#include <map>
#include <string>

namespace tinygltf {class Model;}

namespace Supernova{

    struct SUPERNOVA_API ModelComponent{
        tinygltf::Model* gltfModel = NULL;

        Matrix4 inverseDerivedTransform;
        
        Entity skeleton;

        std::map<std::string, Entity> bonesNameMapping;
        std::map<int, Entity> bonesIdMapping;

        std::map<std::string, int> morphNameMapping;

        std::vector<Entity> animations;

        std::string filename;

        bool needUpdateModel = true;
    };

}

#endif //MODEL_COMPONENT_H