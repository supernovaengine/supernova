//
// (c) 2024 Eduardo Doria.
//

#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Bone.h"
#include "action/Animation.h"

namespace tinygltf {class Model;}

namespace Supernova{
    class SUPERNOVA_API Model: public Mesh{
    public:
        Model(Scene* scene);
        virtual ~Model();

        bool loadModel(const std::string& filename);

        bool loadOBJ(const std::string& filename);
        bool loadGLTF(const std::string& filename);

        Animation getAnimation(int index);
        Animation findAnimation(const std::string& name);

        Bone getBone(const std::string& name);
        Bone getBone(int id);

        float getMorphWeight(const std::string& name);
        float getMorphWeight(int id);
        void setMorphWeight(const std::string& name, float value);
        void setMorphWeight(int id, float value);
    };
}

#endif //MODEL_H