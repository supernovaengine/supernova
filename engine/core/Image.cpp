
#include "Image.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"


Image::Image(): Mesh() {
    primitiveMode = S_TRIANGLES;
    this->width = 0;
    this->height = 0;
}

Image::Image(int width, int height): Mesh() {
    primitiveMode = S_TRIANGLES;
    setSizes(width, height);
}

Image::Image(std::string image_path): Mesh() {
    primitiveMode = S_TRIANGLES;
    loadTexture(image_path.c_str());
}

Image::~Image() {
}

void Image::createVertices(){
    vertices.clear();
    vertices.push_back(Vector3(0, 0, 0));
    vertices.push_back(Vector3(width, 0, 0));
    vertices.push_back(Vector3(width,  height, 0));
    vertices.push_back(Vector3(0,  height, 0));
    
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    
    static const unsigned int indices_array[] = {
        0,  1,  2,
        0,  2,  3
    };
    
    std::vector<unsigned int> indices;
    indices.assign(indices_array, std::end(indices_array));
    submeshes[0].setIndices(indices);

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
}


bool Image::load(){
    if (submeshes[0].getTexture() != NULL && !loaded){
        submeshes[0].getTexture()->getFilePath();
        TextureLoader image(submeshes[0].getTexture()->getFilePath());
        if (this->width == 0)
            this->width = image.getRawImage()->getWidth();
        if (this->height == 0)
            this->height = image.getRawImage()->getHeight();
        TextureManager textureManager;
        textureManager.loadTexture(image.getRawImage(), submeshes[0].getTexture()->getFilePath());
    }
    createVertices();
    return Mesh::load();
}


void Image::setSizes(int width, int height){
    this->width = width;
    this->height = height;
    load();
}

void Image::setWidth(int width){
    setSizes(width, this->height);
}

int Image::getWidth(){
    return width;
}

void Image::setHeight(int height){
    setSizes(this->width, height);
}

int Image::getHeight(){
    return height;
}
