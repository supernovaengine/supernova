#ifndef READSMODEL_H
#define READSMODEL_H

//
// (c) 2018 Eduardo Doria.
//

#include <istream>
#include "SModelData.h"

namespace Supernova {

    class ReadSModel {

    private:

        std::istream* is;

        void readString(std::string &str);
        void readUintVector(std::vector<unsigned int> &vec);
        void readVector4(Vector4 &vec);
        void readVector3(Vector3 &vec);
        void readVector2(Vector2 &vec);
        void readMeshDataVector(std::vector<MeshData> &vec);
        void readMaterialDataVector(std::vector<MaterialData> &vec);
        void readSkeleton(BoneData* &skeleton);
        void readBoneData(BoneData &boneData);
        void readVerticesVector(std::vector<VertexData> &vec, int vertexMask);
        void readVertexData(VertexData &vertexData, int vertexMask);

    public:

        ReadSModel(std::istream* is);

        bool readModel(SModelData &modelData);

    };

}


#endif //READSMODEL_H
