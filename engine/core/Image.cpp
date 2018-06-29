
#include "Image.h"
#include "render/ObjectRender.h"
#include <string>
#include "image/TextureLoader.h"
#include "Log.h"
#include <math.h>

using namespace Supernova;

Image::Image(): Mesh2D() {
    primitiveType = S_PRIMITIVE_TRIANGLES;
    texWidth = 0;
    texHeight = 0;
    useTextureRect = false;
}

Image::Image(int width, int height): Image() {
    setSize(width, height);
}

Image::Image(std::string image_path): Image() {
    setTexture(image_path);
}

Image::~Image() {
}

void Image::setTextureRect(float x, float y, float width, float height){
    setTextureRect(Rect(x, y, width, height));
}

void Image::setTextureRect(Rect textureRect){
    if ( this->textureRect != textureRect) {
        this->textureRect = textureRect;

        if (loaded && !useTextureRect) {
            useTextureRect = true;
            reload();
        } else {
            useTextureRect = true;
            normalizeTextureRect();
        }
    }
}

Rect Image::getTextureRect(){
    return this->textureRect;
}

void Image::normalizeTextureRect(){
    
    if (useTextureRect){
        if (textureRect.isNormalized()){
            
            submeshes[0]->getMaterial()->setTextureRect(textureRect.getX(),
                                                        textureRect.getY(),
                                                        textureRect.getWidth(),
                                                        textureRect.getHeight());
            
        }else {
            
            if (this->texWidth != 0 && this->texHeight != 0) {
                // 0.1 and 0.2 to work with small and pixel perfect textures
                submeshes[0]->getMaterial()->setTextureRect((textureRect.getX()+0.01) / (float) texWidth,
                                                            (textureRect.getY()+0.01) / (float) texHeight,
                                                            (textureRect.getWidth()-0.02) / (float) texWidth,
                                                            (textureRect.getHeight()-0.02) / (float) texHeight);
            }
        }
    }
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
    texcoords.push_back(Vector2(0.01f, 0.01f));
    texcoords.push_back(Vector2(0.99f, 0.01f));
    texcoords.push_back(Vector2(0.99f, 0.99f));
    texcoords.push_back(Vector2(0.01f, 0.99f));

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
    
    if (submeshes[0]->getMaterial()->getTexture()) {
        submeshes[0]->getMaterial()->getTexture()->load();
        texWidth = submeshes[0]->getMaterial()->getTexture()->getWidth();
        texHeight = submeshes[0]->getMaterial()->getTexture()->getHeight();
        
        normalizeTextureRect();
        
        if (this->width == 0 || this->height == 0) {
            if (submeshes[0]->getMaterial()->getTextureRect()){
                if (this->width == 0)
                    this->width = ceil(texWidth * submeshes[0]->getMaterial()->getTextureRect()->getWidth());
                if (this->height == 0)
                    this->height = ceil(texHeight * submeshes[0]->getMaterial()->getTextureRect()->getHeight());
            }else{
                if (this->width == 0)
                    this->width = texWidth;
                if (this->height == 0)
                    this->height = texHeight;
            }
        }
    }
    
    //setInvertTexture(isIn3DScene());
    createVertices();
    
    return Mesh2D::load();
}
