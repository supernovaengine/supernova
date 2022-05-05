//
// (c) 2021 Eduardo Doria.
//

#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Bone.h"
#include "buffer/ExternalBuffer.h"

namespace tinygltf {class Model;}

namespace Supernova{
    class Model: public Mesh{
    private:
        static bool fileExists(const std::string &abs_filename, void *);
        static bool readWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *);
        static std::string readFileToString(const char* filename);

        bool loadGLTFBuffer(int bufferViewIndex, MeshComponent& mesh, int& eBufferIndex, const int stride);
        bool loadGLTFTexture(int textureIndex, Texture& texture, std::string textureName);
        std::string getBufferName(int bufferViewIndex);

        Matrix4 getGLTFNodeMatrix(int nodeIndex);
        Matrix4 getGLTFMeshGlobalMatrix(int nodeIndex, std::map<int, int>& nodesParent);
        Entity generateSketetalStructure(ModelComponent& model, int nodeIndex, int skinIndex);

        tinygltf::Model* gltfModel;
        ExternalBuffer eBuffers[MAX_EXTERNAL_BUFFERS];
        std::vector<float> extraBuffer;

    public:
        Model(Scene* scene); 
        virtual ~Model();   

        bool loadOBJ(const char* filename);
        bool loadGLTF(const char* filename);

        bool loadModel(const char* filename);

        Bone getBone(std::string name);
        Bone getBone(int id);

        float getMorphWeight(std::string name);
        float getMorphWeight(int id);
        void setMorphWeight(std::string name, float value);
        void setMorphWeight(int id, float value);
    };
}

#endif //MODEL_H