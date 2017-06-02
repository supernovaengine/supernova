#include "Model.h"

#include "platform/Log.h"
#include "PrimitiveMode.h"
#include <algorithm>
#include "tiny_obj_loader.h"
#include "Supernova.h"
#include "FileData.h"

Model::Model(): Mesh() {
    primitiveMode = S_TRIANGLES;
}

Model::Model(const char * path): Model() {
    filename = path;
}

Model::~Model() {
}

bool Model::load(){

    loadOBJ(filename);

    return Mesh::load();
}

std::string Model::getBaseDir (const std::string str){
    size_t found;

    found=str.find_last_of("/\\");
    
    std::string result = str.substr(0,found);
    
    if (str == result)
        result= "";
    
    return result;
}

std::string Model::readDataFile(const char* filename){
    FileData filedata(filename);
    return filedata.readString();
}

bool Model::loadOBJ(const char* path){

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    
    std::string baseDir = getBaseDir(path);
    
    tinyobj::FileReader::externalFunc = readDataFile;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path, (baseDir+"/").c_str());

    if (!err.empty()) {
        Log::Error(LOG_TAG, "%s (%s)", err.c_str(), path);
    }

    if (ret) {

        for (size_t i = 0; i < materials.size(); i++) {
            if (i > (this->submeshes.size()-1)){
                this->submeshes.push_back(new Submesh());
                this->submeshes.back()->createNewMaterial();
            }

            this->submeshes.back()->getMaterial()->setTexture(baseDir+"/"+materials[i].diffuse_texname);
            if (materials[i].dissolve < 1){
                this->submeshes.back()->getMaterial()->transparent = true;
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

                    this->submeshes[material_id]->addIndex(vertices.size());

                    vertices.push_back(Vector3(attrib.vertices[3*idx.vertex_index+0],attrib.vertices[3*idx.vertex_index+1],attrib.vertices[3*idx.vertex_index+2]));
                    texcoords.push_back(Vector2(attrib.texcoords[2*idx.texcoord_index+0], attrib.texcoords[2*idx.texcoord_index+1]));
                    normals.push_back(Vector3(attrib.normals[3*idx.normal_index+0], attrib.normals[3*idx.normal_index+1], attrib.normals[3*idx.normal_index+2]));
                }

                index_offset += fnum;
            }
        }
    }


    return true;
}
