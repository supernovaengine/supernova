#include "RectImage.h"

#include "render/TextureManager.h"


RectImage::RectImage(): Image(){
    texWidth = 0;
    texHeight = 0;
    
    isSliced = false;
}

RectImage::~RectImage(){
    
}

void RectImage::setRect(float x, float y, float width, float height){
    setRect(TextureRect(x, y, width, height));
}

void RectImage::setRect(TextureRect textureRect){
    this->textureRect = textureRect;
    if (!isSliced) {
        isSliced = true;
        if (loaded)
            reload();
    }
    generateNormalizateRect();
}

void RectImage::generateNormalizateRect(){

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

/*
void RectImage::updateSlicePosPixels(){
    if (texWidth > 0 && texHeight > 0){
        int slicesPosY;
        int slicesPosX;
        
        if (frame < slicesX * slicesY){
            slicesPosY = floor(frame / slicesX);
            slicesPosX = frame - slicesX * slicesPosY;
        }else{
            slicesPosY = 0;
            slicesPosX = 0;
        }
        
        slicePixelsPos = std::make_pair(slicesPosX * texWidth / slicesX, slicesPosY * texHeight / slicesY);
    }
}

void RectImage::setSlices(int slicesX, int slicesY){
    if (slicesX >= 1 && slicesY >= 1){
        this->slicesX = slicesX;
        this->slicesY = slicesY;
        
        if ((slicesX > 1 || slicesY > 1) && (material.getTextures().size() > 0)){
            isSliced = true;
            updateSlicePosPixels();
        }else{
            isSliced = false;
        }
    }
}

void RectImage::setFrame(int frame){
    this->frame = frame;
    updateSlicePosPixels();
}
*/
bool RectImage::load(){
    
    if (material.getTextures().size() > 0) {
        TextureManager::loadTexture(submeshes[0]->getMaterial()->getTextures()[0]);
        texWidth = TextureManager::getTextureWidth(submeshes[0]->getMaterial()->getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(submeshes[0]->getMaterial()->getTextures()[0]);

        if (isSliced) {
            generateNormalizateRect();

            if (this->width == 0 && this->height == 0) {
                this->width = texWidth * submeshes[0]->getMaterial()->getTextureRect().getWidth();
                this->height = texHeight * submeshes[0]->getMaterial()->getTextureRect().getHeight();
            }
        }
    }
    /*
    renderManager.getRender()->setIsRectImage(isSliced);
    renderManager.getRender()->setTextureRect(&normalizedRect);
    */
    return Image::load();
}

bool RectImage::draw(){
    return Image::draw();
}
