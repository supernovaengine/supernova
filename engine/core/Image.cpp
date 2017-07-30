
#include "Image.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "platform/Log.h"

using namespace Supernova;

Image::Image(): Mesh2D() {
    primitiveMode = S_TRIANGLES;
}

Image::Image(int width, int height): Mesh2D() {
    primitiveMode = S_TRIANGLES;
    setSize(width, height);
}

Image::Image(std::string image_path): Mesh2D() {
    primitiveMode = S_TRIANGLES;
    setTexture(image_path);
}

Image::~Image() {
}

void Image::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    if (loaded) {
        createVertices();
        updateVertices();
    }
}

void Image::setInvertTexture(bool invertTexture){
    Mesh2D::setInvertTexture(invertTexture);
    if (loaded) {
        createVertices();
        updateVertices();
    }
}

void Image::createVertices(){
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

bool Image::load(){
    if (submeshes[0]->getMaterial()->getTexture() && this->width == 0 && this->height == 0){
        submeshes[0]->getMaterial()->getTexture()->load();
        this->width = submeshes[0]->getMaterial()->getTexture()->getWidth();
        this->height = submeshes[0]->getMaterial()->getTexture()->getHeight();
    }
    
    setInvertTexture(isIn3DScene());
    createVertices();
    
    return Mesh2D::load();
}
