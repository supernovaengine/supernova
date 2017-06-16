#include "RectImage.h"

#include "render/TextureManager.h"


using namespace Supernova;

RectImage::RectImage(): Image(){
    texWidth = 0;
    texHeight = 0;
    
    useTextureRect = false;
}

RectImage::~RectImage(){
    
}

void RectImage::setRect(float x, float y, float width, float height){
    setRect(Rect(x, y, width, height));
}

void RectImage::setRect(Rect textureRect){
    this->textureRect = textureRect;
    
    if (loaded && !useTextureRect){
        useTextureRect = true;
        reload();
    }else{
        useTextureRect = true;
        normalizeTextureRect();
    }
}

void RectImage::normalizeTextureRect(){

    if (useTextureRect){
        if (textureRect.isNormalized()){
            
            submeshes[0]->getMaterial()->setTextureRect(textureRect.getX(),
                                                        textureRect.getY(),
                                                        textureRect.getWidth(),
                                                        textureRect.getHeight());

        }else {

            if (this->texWidth != 0 && this->texHeight != 0) {
                submeshes[0]->getMaterial()->setTextureRect(textureRect.getX() / (float) texWidth,
                                                            textureRect.getY() / (float) texHeight,
                                                            textureRect.getWidth() / (float) texWidth,
                                                            textureRect.getHeight() / (float) texHeight);
            }
        }
    }
}

bool RectImage::load(){
    
    if (material.getTextures().size() > 0) {
        TextureManager::loadTexture(submeshes[0]->getMaterial()->getTextures()[0]);
        texWidth = TextureManager::getTextureWidth(submeshes[0]->getMaterial()->getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(submeshes[0]->getMaterial()->getTextures()[0]);

        normalizeTextureRect();

        if (this->width == 0 && this->height == 0) {
            if (submeshes[0]->getMaterial()->getTextureRect()){
                this->width = texWidth * submeshes[0]->getMaterial()->getTextureRect()->getWidth();
                this->height = texHeight * submeshes[0]->getMaterial()->getTextureRect()->getHeight();
            }
        }
    }

    return Image::load();
}

bool RectImage::draw(){

    return Image::draw();
}
