
#include "Image.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"



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
    if (submeshes[0].getTexture() != "" && !loaded && this->width == 0 && this->height == 0){
        TextureLoader image(submeshes[0].getTexture());
        if (this->width == 0)
            this->width = image.getRawImage()->getWidth();
        if (this->height == 0)
            this->height = image.getRawImage()->getHeight();
        TextureManager::loadTexture(image.getRawImage(), submeshes[0].getTexture());
    }

    createVertices();
    return Mesh2D::load();
}
