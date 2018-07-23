#ifndef model_h
#define model_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"
#include "file/FileData.h"

namespace Supernova {

    class Model: public Mesh {
    private:

        struct MeshVertex {
            Vector3 vertex;
            Vector2 texcoord;
            Vector3 normal;
            Vector3 tangent;
            Vector3 bitangent;
        };

        struct MeshMaterial {
            int type;
            std::string texture;
        };

        struct MeshNode {
            std::vector<MeshVertex> meshVertices;
            std::vector<unsigned int> indices;
            std::vector<MeshMaterial> materials;
        };

        struct MeshData {
            std::vector<MeshNode> meshNodes;
        };

        const char* filename;
        std::string baseDir;

        void readMeshVerticesVector(FileData& file, std::vector<MeshVertex> &vec);
        void readIndicesVector(FileData& file, std::vector<unsigned int> &vec);
        void readString(FileData& file, std::string &str);
        void readMeshMaterialsVector(FileData& file, std::vector<MeshMaterial> &vec);
        void readMeshNodesVector(FileData& file, std::vector<MeshNode> &vec);

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
