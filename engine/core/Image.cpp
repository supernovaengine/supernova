
#include "Image.h"
#include "render/ObjectRender.h"
#include <string>
#include "Log.h"
#include <math.h>

using namespace Supernova;

Image::Image(): Mesh2D() {
    primitiveType = S_PRIMITIVE_TRIANGLES;
    submeshes.push_back(new Submesh());
    texWidth = 0;
    texHeight = 0;
    useTextureRect = false;

    buffers["vertices"] = &buffer;
    buffers["indices"] = &indices;

    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
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

            getMaterial()->setTextureRect(textureRect.getX(),
                                          textureRect.getY(),
                                          textureRect.getWidth(),
                                          textureRect.getHeight());
            
        }else {
            
            if (this->texWidth != 0 && this->texHeight != 0) {
                // 0.1 and 0.2 to work with small and pixel perfect textures
                getMaterial()->setTextureRect((textureRect.getX()+0.1) / (float) texWidth,
                                              (textureRect.getY()+0.1) / (float) texHeight,
                                              (textureRect.getWidth()-0.2) / (float) texWidth,
                                              (textureRect.getHeight()-0.2) / (float) texHeight);
            }
        }
    }
}

void Image::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    if (loaded) {
        createVertices();
        updateBuffers();
    }
}

void Image::setInvertTexture(bool invertTexture){
    Mesh2D::setInvertTexture(invertTexture);
    if (loaded) {
        createVertices();
        updateBuffers();
    }
}

void Image::createVertices(){

    buffer.clear();

    Attribute* atrVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);

    buffer.addVector3(atrVertex, Vector3(0, 0, 0));
    buffer.addVector3(atrVertex, Vector3(width, 0, 0));
    buffer.addVector3(atrVertex, Vector3(width,  height, 0));
    buffer.addVector3(atrVertex, Vector3(0,  height, 0));

    Attribute* atrTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);

    buffer.addVector2(atrTexcoord, Vector2(0.01f, convTex(0.01f)));
    buffer.addVector2(atrTexcoord, Vector2(0.99f, convTex(0.01f)));
    buffer.addVector2(atrTexcoord, Vector2(0.99f, convTex(0.99f)));
    buffer.addVector2(atrTexcoord, Vector2(0.01f, convTex(0.99f)));

    static const unsigned int indices_array[] = {
        0,  1,  2,
        0,  2,  3
    };

    indices.setValues(
            0, indices.getAttribute(S_INDEXATTRIBUTE),
            6, (char*)&indices_array[0], sizeof(unsigned int));

    submeshes[0]->setIndices("indices", 6);

    Attribute* atrNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);

    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
}

bool Image::load(){
    
    if (getMaterial()->getTexture()) {
        getMaterial()->getTexture()->load();
        texWidth = getMaterial()->getTexture()->getWidth();
        texHeight = getMaterial()->getTexture()->getHeight();
        
        normalizeTextureRect();
        
        if (this->width == 0 || this->height == 0) {
            if (getMaterial()->getTextureRect()){
                if (this->width == 0)
                    this->width = ceil(texWidth * getMaterial()->getTextureRect()->getWidth());
                if (this->height == 0)
                    this->height = ceil(texHeight * getMaterial()->getTextureRect()->getHeight());
            }else{
                if (this->width == 0)
                    this->width = texWidth;
                if (this->height == 0)
                    this->height = texHeight;
            }
        }
    }

    createVertices();
    
    return Mesh2D::load();
}
