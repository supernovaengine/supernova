#ifndef model_h
#define model_h

#include <vector>
#include <map>
#include "Mesh.h"
#include "Bone.h"
#include "math/Vector3.h"
#include "file/FileData.h"
#include "util/SModelData.h"

namespace Supernova {

    class Model: public Mesh {
    private:

        const char* filename;
        std::string baseDir;

        struct BoneInfo{
            Bone* object;
            int index;
        };

        std::map<std::string, Bone*> bonesNameMapping;

        Bone* generateSketetalStructure(BoneData boneData, int& numBones);
        Bone* findBone(Bone* bone, int boneIndex);

        bool loadOBJ(const char * path);
        bool loadSMODEL(const char* path);
        
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
