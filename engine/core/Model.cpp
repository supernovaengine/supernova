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

Bone* Model::generateSketetalStructure(BoneData boneData, int& numBones){
    Bone* bone = new Bone();

    bone->setIndex(boneData.boneIndex);
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

    bonesNameMapping[bone->getName()] = bone;

    if (bone->getIndex() >= 0){
        numBones++;
    }

    for (size_t i = 0; i < boneData.children.size(); i++){
        bone->addObject(generateSketetalStructure(boneData.children[i], numBones));
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

    skinning = false;
    if (modelData.skeleton){
        skinning = true;
        ((AttributeBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_BONEIDS, 4);
        ((AttributeBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_BONEWEIGHTS, 4);
    }

    AttributeData* attVertex = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    AttributeData* attTexcoord = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    AttributeData* attNormal = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_NORMALS);
    AttributeData* attBoneId = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_BONEIDS);
    AttributeData* attBoneWeight = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_BONEWEIGHTS);

    for (size_t i = 0; i < modelData.vertices.size(); i++){

        if (modelData.vertexMask & VERTEX_ELEMENT_POSITION){
            buffers[0]->addValue(attVertex, modelData.vertices[i].position);
        }
        if (modelData.vertexMask & VERTEX_ELEMENT_UV0){
            buffers[0]->addValue(attTexcoord, modelData.vertices[i].texcoord0);
        }
        if (modelData.vertexMask & VERTEX_ELEMENT_NORMAL){
            buffers[0]->addValue(attNormal, modelData.vertices[i].normal);
        }
        if (modelData.vertexMask & VERTEX_ELEMENT_BONE_INDICES){
            buffers[0]->addValue(attBoneId, modelData.vertices[i].boneIndices);
        }
        if (modelData.vertexMask & VERTEX_ELEMENT_BONE_WEIGHTS){
            buffers[0]->addValue(attBoneWeight, modelData.vertices[i].boneWeights);
        }

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

    int numBones = 0;

    if (modelData.skeleton){
        skeleton = generateSketetalStructure(*modelData.skeleton, numBones);
    }

    bonesMatrix.resize(numBones);

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

        AttributeData* attVertex = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_VERTICES);
        AttributeData* attTexcoord = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
        AttributeData* attNormal = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_NORMALS);

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

                    this->submeshes[material_id]->addIndex(buffers[0]->getCount());

                    buffers[0]->addValue(attVertex,
                                          Vector3(attrib.vertices[3*idx.vertex_index+0],
                                                  attrib.vertices[3*idx.vertex_index+1],
                                                  attrib.vertices[3*idx.vertex_index+2]));

                    if (attrib.texcoords.size() > 0) {
                        buffers[0]->addValue(attTexcoord,
                                              Vector2(attrib.texcoords[2 * idx.texcoord_index + 0],
                                                      1.0f - attrib.texcoords[2 * idx.texcoord_index + 1]));
                    }
                    if (attrib.normals.size() > 0) {
                        buffers[0]->addValue(attNormal,
                                              Vector3(attrib.normals[3 * idx.normal_index + 0],
                                                      attrib.normals[3 * idx.normal_index + 1],
                                                      attrib.normals[3 * idx.normal_index + 2]));
                    }
                }

                index_offset += fnum;
            }
        }
    }

    return true;
}

Bone* Model::findBone(Bone* bone, int boneIndex){
    if (bone->getIndex() == boneIndex){
        return bone;
    }else{
        for (size_t i = 0; i < bone->getObjects().size(); i++){
            Bone* childreturn = findBone((Bone*)bone->getObject(i), boneIndex);
            if (childreturn)
                return childreturn;
        }
    }

    return NULL;
}

Bone* Model::getBone(std::string name){
    if (bonesNameMapping.count(name))
        return bonesNameMapping[name];

    return NULL;
}

void Model::updateBone(int boneIndex, Matrix4 skinning){
    if (boneIndex >= 0 && boneIndex < bonesMatrix.size())
        bonesMatrix[boneIndex] = skinning;
}

void Model::updateMatrix(){
    Mesh::updateMatrix();

    inverseDerivedTransform = (modelMatrix * Matrix4::translateMatrix(center)).inverse();
}

bool Model::load(){

    buffers[0]->clearAll();
    buffers[0]->setName("vertices");
    ((AttributeBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    ((AttributeBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    ((AttributeBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);

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
