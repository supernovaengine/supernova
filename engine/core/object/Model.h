//
// (c) 2021 Eduardo Doria.
//

#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Bone.h"
#include "action/Animation.h"

namespace tinygltf {class Model;}

namespace Supernova{
    class Model: public Mesh{
    public:
        Model(Scene* scene);
        virtual ~Model();

        bool loadModel(std::string filename);

        bool loadOBJ(std::string filename);
        bool loadGLTF(std::string filename);

        Animation getAnimation(int index);
        Animation findAnimation(std::string name);

        Bone getBone(std::string name);
        Bone getBone(int id);

        float getMorphWeight(std::string name);
        float getMorphWeight(int id);
        void setMorphWeight(std::string name, float value);
        void setMorphWeight(int id, float value);
    };
}

#endif //MODEL_H