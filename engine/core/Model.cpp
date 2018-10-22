#include "Model.h"

#include <istream>
#include <sstream>
#include "Log.h"
#include "render/ObjectRender.h"
#include <algorithm>
#include "tiny_obj_loader.h"
#include "util/ReadSModel.h"

using namespace Supernova;

Model::Model(): Mesh() {
    primitiveType = S_PRIMITIVE_TRIANGLES;
    skeleton = NULL;
}

Model::Model(const char * path): Model() {
    filename = path;
}

Model::~Model() {
}

std::string Model::readFileToString(const char* filename){
    FileData filedata(filename);
    return filedata.readString();
}

Bone* Model::generateSketetalStructure(BoneData boneData){
    Bone* bone = new Bone();

    bone->setId(boneData.boneId);
    bone->setName(boneData.name);
    bone->setBindPosition(boneData.bindPosition);
    bone->setBindRotation(boneData.bindRotation);
    bone->setBindScale(boneData.bindScale);
    bone->moveToBind();

    Matrix4 offsetMatrix(boneData.offsetMatrix[0][0], boneData.offsetMatrix[1][0], boneData.offsetMatrix[2][0], boneData.offsetMatrix[3][0],
                         boneData.offsetMatrix[0][1], boneData.offsetMatrix[1][1], boneData.offsetMatrix[2][1], boneData.offsetMatrix[3][1],
                         boneData.offsetMatrix[0][2], boneData.offsetMatrix[1][2], boneData.offsetMatrix[2][2], boneData.offsetMatrix[3][2],
                         boneData.offsetMatrix[0][3], boneData.offsetMatrix[1][3], boneData.offsetMatrix[2][3], boneData.offsetMatrix[3][3]);


    bone->setOffsetMatrix(offsetMatrix);

    for (size_t i = 0; i < boneData.children.size(); i++){
        bone->addObject(generateSketetalStructure(boneData.children[i]));
    }

    bone->model = this;

    return bone;
}

bool Model::loadSMODEL(const char* path) {

    std::istringstream matIStream(readFileToString(path));

    SModelData modelData;

    ReadSModel readSModel(&matIStream);

    if (!readSModel.readModel(modelData))
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

    if (modelData.skeleton){
        skeleton = generateSketetalStructure(*modelData.skeleton);
    }

    boneWeights.resize(vertices.size());
    boneIds.resize(vertices.size());

    skinning = false;
    if (modelData.skeleton){
        skinning = true;
    }

    for (size_t i = 0; i < modelData.boneWeights.size(); i++){

        BoneInfo boneInfo;
        boneInfo.index = i;
        boneInfo.object = findBone(skeleton, modelData.boneWeights[i].boneId);

        bonesMapping[modelData.boneWeights[i].boneId] = boneInfo;

        for (size_t j = 0; j < modelData.boneWeights[i].vertexWeights.size(); j++){
            unsigned int vertexId = modelData.boneWeights[i].vertexWeights[j].vertexId;
            float weight = modelData.boneWeights[i].vertexWeights[j].weight;

            if (boneWeights[vertexId][0] == 0) {
                boneWeights[vertexId][0] = weight;
                boneIds[vertexId][0] = i;
            }else if (boneWeights[vertexId][1] == 0) {
                boneWeights[vertexId][1] = weight;
                boneIds[vertexId][1] = i;
            }else if (boneWeights[vertexId][2] == 0) {
                boneWeights[vertexId][2] = weight;
                boneIds[vertexId][2] = i;
            }else if (boneWeights[vertexId][3] == 0) {
                boneWeights[vertexId][3] = weight;
                boneIds[vertexId][3] = i;
            }
        }
    }

    bonesMatrix.resize(modelData.boneWeights.size());

    if (modelData.skeleton){
        addObject(skeleton);
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

Bone* Model::findBone(Bone* bone, unsigned int boneId){
    if (bone->getId() == boneId){
        return bone;
    }else{
        for (size_t i = 0; i < bone->getObjects().size(); i++){
            Bone* childreturn = findBone((Bone*)bone->getObject(i), boneId);
            if (childreturn)
                return childreturn;
        }
    }

    return NULL;
}

Bone* Model::getBone(unsigned int boneId){
    if (bonesMapping.count(boneId))
        return bonesMapping[boneId].object;

    return NULL;
}

void Model::updateBone(unsigned int boneId, Matrix4 skinning){
    if (bonesMapping.count(boneId))
        bonesMatrix[bonesMapping[boneId].index] = skinning;
}

void Model::updateMatrix(){
    Mesh::updateMatrix();

    inverseDerivedTransform = (modelMatrix * Matrix4::translateMatrix(center)).inverse();
}

bool Model::load(){

    vertices.clear();
    texcoords.clear();
    normals.clear();

    baseDir = File::getBaseDir(filename);

    if (!loadSMODEL(filename))
        loadOBJ(filename);

    if (skeleton)
        skinning = true;

    return Mesh::load();
}

Matrix4 Model::getInverseDerivedTransform(){
    return inverseDerivedTransform;
}
