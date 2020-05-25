#ifndef model_h
#define model_h

//
// (c) 2019 Eduardo Doria.
//

#include <vector>
#include <map>
#include "Mesh.h"
#include "Bone.h"
#include "math/Vector3.h"
#include "file/Data.h"
#include "util/SModelData.h"
#include "action/Animation.h"

namespace tinygltf {class Model;}

namespace Supernova {

    class Model: public Mesh {

    private:
        InterleavedBuffer buffer;
        IndexBuffer indices;
        std::vector<Animation*> animations;

        const char* filename;
        std::string baseDir;

        struct BoneInfo{
            Bone* object;
            int index;
        };

        tinygltf::Model* gltfModel;

        std::map<std::string, Bone*> bonesNameMapping;
        std::map<int, Bone*> bonesIdMapping;

        std::map<std::string, int> morphNameMapping;

        Bone* generateSketetalStructure(int nodeIndex, int skinIndex);
        Bone* findBone(Bone* bone, int boneIndex);

        bool loadOBJ(const char * filename);
        bool loadGLTF(const char * filename);

        bool loadGLTFBuffer(int bufferViewIndex);
        std::string getBufferName(int bufferViewIndex);

        static bool fileExists(const std::string &abs_filename, void *);
        static bool readWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *);
        static std::string readFileToString(const char* filename);

    protected:
        Bone* skeleton;
        Matrix4 inverseDerivedTransform;

        std::vector<Matrix4> bonesMatrix;
        std::vector<float> morphWeights;

        bool skinning;
        bool morphTargets;

    public:
        Model();
        Model(const char * path);
        virtual ~Model();

        Bone* getBone(std::string name);
        void updateBone(int boneIndex, Matrix4 skinning);

        Animation* getAnimation(int index);
        Animation* findAnimation(std::string name);

        float getMorphWeight(int index);
        float getMorphWeight(std::string name);

        void setMorphWeight(int index, float value);
        void setMorphWeight(std::string name, float value);

        void clearAnimations();

        Matrix4 getInverseDerivedTransform();

        virtual void updateModelMatrix();

        virtual bool renderLoad(bool shadow);

        virtual bool load();

    };
    
}


#endif /* model_h */
