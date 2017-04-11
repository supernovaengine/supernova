#include "RectImage.h"

#include "render/TextureManager.h"


RectImage::RectImage(): Image(){
    frame = 0;
    
    texWidth = 0;
    texHeight = 0;
    
    isSliced = false;
    slicesX = 1;
    slicesY = 1;
}

RectImage::~RectImage(){
    
}

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

bool RectImage::load(){
    
    if ((material.getTextures().size() > 0) && (isSliced)) {
        TextureManager::loadTexture(submeshes[0]->getMaterial()->getTextures()[0]);
        texWidth = TextureManager::getTextureWidth(submeshes[0]->getMaterial()->getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(submeshes[0]->getMaterial()->getTextures()[0]);
        
        if (this->width == 0 && this->height == 0){
            this->width = texWidth / slicesX;
            this->height = texHeight / slicesY;
        }
    }
    updateSlicePosPixels();
    
    renderManager.getRender()->setIsRectImage(isSliced);
    renderManager.getRender()->setTextureSize(texWidth, texHeight);
    renderManager.getRender()->setRectSize(texWidth / slicesX, texHeight / slicesY);
    renderManager.getRender()->setRectPos(&slicePixelsPos);
    
    return Image::load();
}

bool RectImage::draw(){
    return Image::draw();
}
