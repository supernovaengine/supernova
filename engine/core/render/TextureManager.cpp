
#include "TextureManager.h"
#include "image/TextureLoader.h"
#include "Supernova.h"
#include "image/ColorType.h"


std::unordered_map<std::string, TextureManager::TextureStore> TextureManager::textures;


TextureRender* TextureManager::getTextureRender(){
    if (Supernova::getRenderAPI() == S_GLES2){
        return new GLES2Texture();
    }
    
    return NULL;
}

std::shared_ptr<TextureRender> TextureManager::loadTexture(std::string relative_path){
    
    return loadTexture(NULL, relative_path);
}

std::shared_ptr<TextureRender> TextureManager::loadTexture(TextureFile* textureFile, std::string id){

    //Verify if there is a created texture
    if (textures[id].value){
        return textures[id].value;
    }

    TextureLoader image;
    if (textureFile == NULL){
        image.loadRawImage(id.c_str());
        textureFile = image.getRawImage();
    }

    //If no create a new texture
    TextureRender* texture = getTextureRender();
    textureFile->resamplePowerOfTwo();
    texture->loadTexture(textureFile->getWidth(), textureFile->getHeight(), textureFile->getColorFormat(), textureFile->getData());
    std::shared_ptr<TextureRender> texturePtr(texture);

    bool useAlpha = false;
    if (textureFile->getColorFormat() == S_COLOR_GRAY_ALPHA || textureFile->getColorFormat() == S_COLOR_RGB_ALPHA)
        useAlpha = true;

    textures[id] = {texturePtr, useAlpha, textureFile->getWidth(), textureFile->getHeight()};

    return textures[id].value;
    
}

bool TextureManager::hasAlphaChannel(std::string id){
    return textures[id].hasAlphaChannel;
}

int TextureManager::getTextureWidth(std::string id){
    return textures[id].width;
}

int TextureManager::getTextureHeight(std::string id){
    return textures[id].height;
}

void TextureManager::deleteUnused(){

    TextureManager::it_type remove = findToRemove();
    while (remove != textures.end()){
        textures.erase(remove);
        remove = findToRemove();
    }

}

TextureManager::it_type TextureManager::findToRemove(){

    for(TextureManager::it_type iterator = textures.begin(); iterator != textures.end(); iterator++) {
        if (iterator->second.value.use_count() <= 1){
            iterator->second.value.get()->deleteTexture();
            return iterator;
        }
    }

    return textures.end();

}

void TextureManager::clear(){
    textures.clear();
}

