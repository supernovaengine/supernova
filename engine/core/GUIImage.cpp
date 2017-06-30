
#include "GUIImage.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"

using namespace Supernova;

GUIImage::GUIImage(): GUIObject(){
    primitiveMode = S_TRIANGLES;
}

GUIImage::~GUIImage(){

}

void GUIImage::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    if (loaded) {
        createVertices();
        render->updateVertices();
        render->updateTexcoords();
        render->updateNormals();
        render->updateIndices();
    }
}

void GUIImage::createVertices(){
    vertices.clear();
    vertices.push_back(Vector3(0, 0, 0));
    vertices.push_back(Vector3(width, 0, 0));
    vertices.push_back(Vector3(width,  height, 0));
    vertices.push_back(Vector3(0,  height, 0));
    
    texcoords.clear();
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    
    if (invertTexture){
        for (int i = 0; i < texcoords.size(); i++){
            texcoords[i].y = 1 - texcoords[i].y;
        }
    }
    
    static const unsigned int indices_array[] = {
        0,  1,  2,
        0,  2,  3
    };
    
    std::vector<unsigned int> indices;
    indices.assign(indices_array, std::end(indices_array));
    submeshes[0]->setIndices(indices);
    
    normals.clear();
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
}

bool GUIImage::load(){
    
    if (submeshes[0]->getMaterial()->getTextures().size() > 0 && this->width == 0 && this->height == 0){
        TextureManager::loadTexture(submeshes[0]->getMaterial()->getTextures()[0]);
        this->width = TextureManager::getTextureWidth(submeshes[0]->getMaterial()->getTextures()[0]);
        this->height = TextureManager::getTextureHeight(submeshes[0]->getMaterial()->getTextures()[0]);
    }
    
    setInvertTexture(isIn3DScene());
    createVertices();

    return GUIObject::load();
}
