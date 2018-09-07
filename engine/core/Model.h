#ifndef model_h
#define model_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"
#include "file/FileData.h"

namespace Supernova {

    class Model: public Mesh {
    private:

        struct MaterialData {
            int type;
            std::string texture;
        };

        struct BoneVertexWeightData{
            unsigned int vertexId;
            float weight;
        };

        struct BoneWeightData {
            std::string name;
            std::vector<BoneVertexWeightData> vertexWeights;
        };

        struct MeshData {
            std::string name;
            std::vector<unsigned int> indices;
            std::vector<MaterialData> materials;
        };

        struct BoneData {
            std::string name;
            Vector3 bindPosition;
            Quaternion bindRotation;
            Vector3 bindScale;
            float offsetMatrix[4][4];
            std::vector<BoneData> children;
        };

        struct ModelData {
            std::string name;
            std::vector<Vector3> vertices;
            std::vector<std::vector<Vector2>> texcoords;
            std::vector<Vector3> normals;
            std::vector<Vector3> tangents;
            std::vector<Vector3> bitangents;
            std::vector<MeshData> meshes;
            std::vector<BoneWeightData> bonesWeights;
            BoneData* skeleton;
        };

        const char* filename;
        std::string baseDir;

        bool readModel(std::istream& is, ModelData &modelData);
        void readString(std::istream& is, std::string &str);
        void readUintVector(std::istream& is, std::vector<unsigned int> &vec);
        void readVector3Vector(std::istream& is, std::vector<Vector3> &vec);
        void readVector2Vector(std::istream& is, std::vector<Vector2> &vec);
        void readVector2VectorVector(std::istream& is, std::vector<std::vector<Vector2>> &vec);
        void readMeshDataVector(std::istream& is, std::vector<MeshData> &vec);
        void readMaterialDataVector(std::istream& is, std::vector<MaterialData> &vec);
        void readBoneWeightDataVector(std::istream& is, std::vector<BoneWeightData> &vec);
        void readBoneVertexWeightDataVector(std::istream& is, std::vector<BoneVertexWeightData> &vec);
        void readSkeleton(std::istream& is, BoneData* &skeleton);
        void readBoneData(std::istream& is, BoneData &boneData);

        bool loadOBJ(const char * path);
        bool loadSMODEL(const char* path);
        
        static std::string readFileToString(const char* filename);

    public:
        Model();
        Model(const char * path);
        virtual ~Model();

        virtual bool load();

    };
    
}


#endif /* model_h */
