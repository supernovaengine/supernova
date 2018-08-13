#ifndef model_h
#define model_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"
#include "file/FileData.h"

namespace Supernova {

    class Model: public Mesh {
    private:

        struct MeshVertexData {
            Vector3 vertex;
            Vector2 texcoord;
            Vector3 normal;
            Vector3 tangent;
            Vector3 bitangent;
        };

        struct MeshMaterialData {
            int type;
            std::string texture;
        };

        struct SubMeshData {
            std::string name;
            std::vector<MeshVertexData> meshVertices;
            std::vector<unsigned int> indices;
            std::vector<MeshMaterialData> materials;
        };

        struct MeshData {
            std::vector<SubMeshData> subMeshesData;
        };

        const char* filename;
        std::string baseDir;

        void readMeshVerticesVector(FileData& file, std::vector<MeshVertexData> &vec);
        void readIndicesVector(FileData& file, std::vector<unsigned int> &vec);
        void readString(FileData& file, std::string &str);
        void readMeshMaterialsVector(FileData& file, std::vector<MeshMaterialData> &vec);
        void readSubMeshesVector(FileData& file, std::vector<SubMeshData> &vec);

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
