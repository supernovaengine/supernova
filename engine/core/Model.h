#ifndef model_h
#define model_h

#include <vector>
#include <map>
#include "Mesh.h"
#include "Bone.h"
#include "math/Vector3.h"
#include "file/FileData.h"
#include "util/SModelData.h"

namespace tinygltf {class Model;}

namespace Supernova {

    class Model: public Mesh {

    private:
        InterleavedBuffer buffer;
        IndexBuffer indices;

        const char* filename;
        std::string baseDir;

        struct BoneInfo{
            Bone* object;
            int index;
        };

        tinygltf::Model* gltfModel;

        std::map<std::string, Bone*> bonesNameMapping;

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

    public:
        Model();
        Model(const char * path);
        virtual ~Model();

        Bone* getBone(std::string name);
        void updateBone(int boneIndex, Matrix4 skinning);

        Matrix4 getInverseDerivedTransform();

        virtual void updateMatrix();

        virtual bool load();

    };
    
}


#endif /* model_h */
