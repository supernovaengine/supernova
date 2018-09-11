#ifndef SMODELDATA_H
#define SMODELDATA_H

//
// (c) 2018 Eduardo Doria.
//

#include <vector>
#include <string>
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "math/Quaternion.h"

using namespace Supernova;

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

struct SModelData {
    std::string name;
    std::vector<Vector3> vertices;
    std::vector<std::vector<Vector2>> texcoords;
    std::vector<Vector3> normals;
    std::vector<Vector3> tangents;
    std::vector<Vector3> bitangents;
    std::vector<MeshData> meshes;
    std::vector<BoneWeightData> boneWeights;
    BoneData* skeleton;
};

#endif //SMODELDATA_H
