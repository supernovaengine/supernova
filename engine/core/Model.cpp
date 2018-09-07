#include "Model.h"

#include <istream>
#include <sstream>
#include "Log.h"
#include "render/ObjectRender.h"
#include <algorithm>
#include "tiny_obj_loader.h"

using namespace Supernova;

Model::Model(): Mesh() {
    primitiveType = S_PRIMITIVE_TRIANGLES;
}

Model::Model(const char * path): Model() {
    filename = path;
}

Model::~Model() {
}

bool Model::load(){

    vertices.clear();
    texcoords.clear();
    normals.clear();

    baseDir = File::getBaseDir(filename);

    if (!loadSMODEL(filename))
        loadOBJ(filename);

    return Mesh::load();
}

std::string Model::readFileToString(const char* filename){
    FileData filedata(filename);
    return filedata.readString();
}

bool Model::readModel(std::istream& is, ModelData &modelData){
    char* sig= new char[6];
    int version;

    is.read(sig, sizeof(char) * 6);
    is.read((char*)&version, sizeof(int));

    if (std::string(sig)=="SMODEL" && version==1){
        readString(is, modelData.name);
        readVector3Vector(is, modelData.vertices);
        readVector2VectorVector(is, modelData.texcoords);
        readVector3Vector(is, modelData.normals);
        readVector3Vector(is, modelData.tangents);
        readVector3Vector(is, modelData.bitangents);
        readMeshDataVector(is, modelData.meshes);
        readBoneWeightDataVector(is, modelData.bonesWeights);
        readSkeleton(is, modelData.skeleton);

        return true;
    }

    return false;
}

void Model::readString(std::istream& is, std::string &str){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    str.resize(size);
    is.read((char*)&str[0], size);
}

void Model::readUintVector(std::istream& is, std::vector<unsigned int> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);
    is.read((char*)&vec[0], vec.size() * sizeof(unsigned int));
}

void Model::readVector3Vector(std::istream& is, std::vector<Vector3> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);
    is.read((char*)&vec[0], vec.size() * 3 * sizeof(float));
}

void Model::readVector2Vector(std::istream& is, std::vector<Vector2> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);
    is.read((char*)&vec[0], vec.size() * 2 * sizeof(float));
}

void Model::readVector2VectorVector(std::istream& is, std::vector<std::vector<Vector2>> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);

    for (size_t i = 0; i < size; ++i){
        readVector2Vector(is, vec[i]);
    }
}

void Model::readMeshDataVector(std::istream& is, std::vector<MeshData> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);

    for (size_t i = 0; i < size; ++i){
        readString(is, vec[i].name);
        readUintVector(is, vec[i].indices);
        readMaterialDataVector(is, vec[i].materials);
    }
}

void Model::readMaterialDataVector(std::istream& is, std::vector<MaterialData> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);

    for (size_t i = 0; i < size; ++i){
        is.read((char*)&vec[i].type, sizeof(int));
        readString(is, vec[i].texture);
    }
}

void Model::readBoneWeightDataVector(std::istream& is, std::vector<BoneWeightData> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);

    for (size_t i = 0; i < size; ++i){
        readString(is, vec[i].name);
        readBoneVertexWeightDataVector(is, vec[i].vertexWeights);
    }
}

void Model::readBoneVertexWeightDataVector(std::istream& is, std::vector<BoneVertexWeightData> &vec){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    vec.resize(size);

    for (size_t i = 0; i < size; ++i){
        is.read((char*)&vec[i].vertexId, sizeof(unsigned int));
        is.read((char*)&vec[i].weight, sizeof(float));
    }
}

void Model::readSkeleton(std::istream& is, BoneData* &skeleton){
    size_t size = 0;
    is.read((char*)&size, sizeof(size));

    skeleton = NULL;

    if (size == 1){
        skeleton = new BoneData();
        readBoneData(is, *skeleton);
    }
}

void Model::readBoneData(std::istream& is, BoneData &boneData){
    readString(is, boneData.name);
    is.read((char*)&boneData.bindPosition, 3 * sizeof(float));
    is.read((char*)&boneData.bindRotation, 4 * sizeof(float));
    is.read((char*)&boneData.bindScale, 3 * sizeof(float));
    is.read((char*)&boneData.offsetMatrix, 16 * sizeof(float));

    size_t size = 0;
    is.read((char*)&size, sizeof(size));
    boneData.children.resize(size);

    for (size_t i = 0; i < size; ++i){
        readBoneData(is, boneData.children[i]);
    }
}

bool Model::loadSMODEL(const char* path) {

    std::istringstream matIStream(readFileToString(path));

    ModelData modelData;

    if (!readModel(matIStream, modelData))
        return false;

    vertices.clear();
    for (size_t i = 0; i < modelData.vertices.size(); i++){
        vertices.push_back(modelData.vertices[i]);
    }

    texcoords.clear();
    if (modelData.texcoords.size() > 0) {
        for (size_t i = 0; i < modelData.texcoords[0].size(); i++) {
            texcoords.push_back(modelData.texcoords[0][i]);
        }
    }

    normals.clear();
    for (size_t i = 0; i < modelData.normals.size(); i++){
        normals.push_back(modelData.normals[i]);
    }

    for (size_t i = 0; i < modelData.meshes.size(); i++){
        if (i > (this->submeshes.size() - 1)) {
            this->submeshes.push_back(new SubMesh());
            this->submeshes.back()->createNewMaterial();
        }

        this->submeshes.back()->getIndices()->clear();

        for (size_t j = 0; j < modelData.meshes[i].indices.size(); j++) {
            this->submeshes.back()->addIndex(modelData.meshes[i].indices[j]);
        }

        if (modelData.meshes[i].materials.size() > 0)
            this->submeshes.back()->getMaterial()->setTexturePath(File::simplifyPath(baseDir + modelData.meshes[i].materials[0].texture));
    }

    return true;
}

bool Model::loadOBJ(const char* path){

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    
    tinyobj::FileReader::externalFunc = readFileToString;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path, baseDir.c_str());

    if (!err.empty()) {
        Log::Error("%s (%s)", err.c_str(), path);
    }

    if (ret) {

        for (size_t i = 0; i < materials.size(); i++) {
            if (i > (this->submeshes.size()-1)){
                this->submeshes.push_back(new SubMesh());
                this->submeshes.back()->createNewMaterial();
            }

            this->submeshes.back()->getMaterial()->setTexturePath(File::simplifyPath(baseDir+materials[i].diffuse_texname));
            if (materials[i].dissolve < 1){
                // TODO: Add this check on isTransparent Material method
                transparent = true;
            }
        }

        for (size_t i = 0; i < shapes.size(); i++) {

            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
                size_t fnum = shapes[i].mesh.num_face_vertices[f];

                int material_id = shapes[i].mesh.material_ids[f];
                if (material_id < 0)
                    material_id = 0;

                // For each vertex in the face
                for (size_t v = 0; v < fnum; v++) {
                    tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];

                    this->submeshes[material_id]->addIndex((unsigned int)vertices.size());

                    vertices.push_back(Vector3(attrib.vertices[3*idx.vertex_index+0], attrib.vertices[3*idx.vertex_index+1], attrib.vertices[3*idx.vertex_index+2]));
                    
                    if (attrib.texcoords.size() > 0)
                        texcoords.push_back(Vector2(attrib.texcoords[2*idx.texcoord_index+0], 1.0f - attrib.texcoords[2*idx.texcoord_index+1]));
                    
                    if (attrib.normals.size() > 0)
                        normals.push_back(Vector3(attrib.normals[3*idx.normal_index+0], attrib.normals[3*idx.normal_index+1], attrib.normals[3*idx.normal_index+2]));
                }

                index_offset += fnum;
            }
        }
    }

    return true;
}
