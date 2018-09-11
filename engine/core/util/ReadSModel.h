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
        void readVector3Vector(std::vector<Vector3> &vec);
        void readVector2Vector(std::vector<Vector2> &vec);
        void readVector2VectorVector(std::vector<std::vector<Vector2>> &vec);
        void readMeshDataVector(std::vector<MeshData> &vec);
        void readMaterialDataVector(std::vector<MaterialData> &vec);
        void readBoneWeightDataVector(std::vector<BoneWeightData> &vec);
        void readBoneVertexWeightDataVector(std::vector<BoneVertexWeightData> &vec);
        void readSkeleton(BoneData* &skeleton);
        void readBoneData(BoneData &boneData);

    public:

        ReadSModel(std::istream* is);

        bool readModel(SModelData &modelData);

    };

}


#endif //READSMODEL_H
