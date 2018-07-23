#include "Model.h"

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

void Model::readMeshVerticesVector(FileData& file, std::vector<MeshVertex> &vec){
    typename std::vector<MeshVertex>::size_type size = 0;
    file.read((unsigned char*)&size, sizeof(size));
    vec.resize(size);
    file.read((unsigned char*)&vec[0], vec.size() * sizeof(MeshVertex));
}

void Model::readIndicesVector(FileData& file, std::vector<unsigned int> &vec){
    typename std::vector<unsigned int>::size_type size = 0;
    file.read((unsigned char*)&size, sizeof(size));
    vec.resize(size);
    file.read((unsigned char*)&vec[0], vec.size() * sizeof(unsigned int));
}

void Model::readString(FileData& file, std::string &str){
    typename std::string::size_type size = 0;
    file.read((unsigned char*)&size, sizeof(size));
    str.resize(size);
    file.read((unsigned char*)&str[0], size);
}

void Model::readMeshMaterialsVector(FileData& file, std::vector<MeshMaterial> &vec){
    typename std::vector<MeshMaterial>::size_type size = 0;
    file.read((unsigned char*)&size, sizeof(size));
    vec.resize(size);

    for (typename std::vector<MeshNode>::size_type i = 0; i < size; ++i){
        file.read((unsigned char*)&vec[i].type, sizeof(int));
        readString(file, vec[i].texture);
    }
}

void Model::readMeshNodesVector(FileData& file, std::vector<MeshNode> &vec){
    typename std::vector<MeshVertex>::size_type size = 0;
    file.read((unsigned char*)&size, sizeof(size));
    vec.resize(size);

    for (typename std::vector<MeshNode>::size_type i = 0; i < size; ++i){
        readMeshVerticesVector(file, vec[i].meshVertices);
        readIndicesVector(file, vec[i].indices);
        readMeshMaterialsVector(file, vec[i].materials);
    }
}

bool Model::loadSMODEL(const char* path) {
    char* sig= new char[6];
    int readversion;
    MeshData meshData;

    FileData file(path);

    file.read((unsigned char*)sig, sizeof(char) * 6);
    file.read((unsigned char*)&readversion, sizeof(int));

    if (std::string(sig)!="SMODEL")
        return false;

    readMeshNodesVector(file, meshData.meshNodes);

    int indexOffset = 0;

    for (size_t i = 0; i < meshData.meshNodes.size(); i++){
        if (i > (this->submeshes.size()-1)){
            this->submeshes.push_back(new Submesh());
            this->submeshes.back()->createNewMaterial();
        }

        for (size_t v = 0; v < meshData.meshNodes[i].meshVertices.size(); v++){
            vertices.push_back(meshData.meshNodes[i].meshVertices[v].vertex);
            texcoords.push_back(meshData.meshNodes[i].meshVertices[v].texcoord);
            normals.push_back(meshData.meshNodes[i].meshVertices[v].normal);
        }

        this->submeshes[i]->getIndices()->clear();

        for (size_t j = 0; j < meshData.meshNodes[i].indices.size(); j++) {
            this->submeshes[i]->addIndex(meshData.meshNodes[i].indices[j] + indexOffset);
        }

        indexOffset += meshData.meshNodes[i].meshVertices.size();

        this->submeshes[i]->getMaterial()->setTexturePath(File::simplifyPath(baseDir+meshData.meshNodes[i].materials[0].texture));

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
                this->submeshes.push_back(new Submesh());
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
