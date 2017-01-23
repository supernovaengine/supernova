#include "Model.h"

#include "platform/Log.h"
#include "PrimitiveMode.h"
#include <stdio.h>
#include <algorithm>
#include "tiny_obj_loader.h"
#include "Supernova.h"

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

bool Model::loadOBJ(const char * path){

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    
    std::string baseDir = getBaseDir(path);

    bool ret = tinyobj::LoadObj(shapes, materials, err, path, (baseDir+"/").c_str());

    if (!err.empty()) {
        Log::Error(LOG_TAG, "%s (%s)", err.c_str(), path);
    }

    if (ret) {

        for (size_t i = 0; i < materials.size(); i++) {
            if (i > (this->submeshes.size()-1)){
                this->submeshes.push_back(new Submesh());
            }
            //char str[80];
            //strcpy(str, "jeep");
            //strcat(str, materials[i].diffuse_texname.c_str());
            //printf("material[%ld].file = %s\n", i, materials[i].diffuse_texname.c_str());
            if (materials[i].dissolve < 1){
                this->submeshes[this->submeshes.size()-1]->transparent = true;
            }
            
            this->setTexture(baseDir+"/"+materials[i].diffuse_texname, (int)i);
        }

        for (size_t i = 0; i < shapes.size(); i++) {

            assert((shapes[i].mesh.indices.size() % 3) == 0);
            for (size_t f = 0; f < shapes[i].mesh.material_ids.size(); f++) {
                //printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
                
                int material_id = shapes[i].mesh.material_ids[f];
                if (material_id < 0)
                    material_id = 0;

                this->submeshes[material_id]->addIndex(shapes[i].mesh.indices[3*f+0]);
                this->submeshes[material_id]->addIndex(shapes[i].mesh.indices[3*f+1]);
                this->submeshes[material_id]->addIndex(shapes[i].mesh.indices[3*f+2]);

            }

            //printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
            assert((shapes[i].mesh.positions.size() % 3) == 0);
            for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
                //printf("  v[%ld] = (%f, %f, %f)\n", v,shapes[i].mesh.positions[3*v+0],shapes[i].mesh.positions[3*v+1],shapes[i].mesh.positions[3*v+2]);
                vertices.push_back(Vector3(shapes[i].mesh.positions[3*v+0],shapes[i].mesh.positions[3*v+1],shapes[i].mesh.positions[3*v+2]));
            }

            //printf("shape[%ld].texcoords: %ld\n", i, shapes[i].mesh.texcoords.size());
            assert((shapes[i].mesh.texcoords.size() % 2) == 0);
            for (size_t t = 0; t < shapes[i].mesh.texcoords.size() / 2; t++) {
                //printf("  tx[%ld] = (%f, %f)\n", t,shapes[i].mesh.texcoords[2*t+0],shapes[i].mesh.texcoords[2*t+1]);
                //Invert V because OpenGL defaults
                texcoords.push_back(Vector2(shapes[i].mesh.texcoords[2*t+0], 1 - shapes[i].mesh.texcoords[2*t+1]));
            }

            assert((shapes[i].mesh.normals.size() % 3) == 0);
            for (size_t n = 0; n < shapes[i].mesh.normals.size() / 3; n++) {
                //printf("  tx[%ld] = (%f, %f, %f)\n", n,shapes[i].mesh.normals[3*n+0], shapes[i].mesh.normals[3*n+1], shapes[i].mesh.normals[3*n+2]);
                //Invert V because OpenGL defaults
                normals.push_back(Vector3(shapes[i].mesh.normals[3*n+0], shapes[i].mesh.normals[3*n+1], shapes[i].mesh.normals[3*n+2]));
            }
            
        }

    }


    return true;
}
