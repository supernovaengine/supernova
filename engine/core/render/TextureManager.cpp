
#include "TextureManager.h"
#include "image/TextureLoader.h"
#include "Supernova.h"


std::unordered_map<std::string, std::shared_ptr<TextureRender>> TextureManager::textures;


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
    if (textures[id]){
        return textures[id];
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

    textures[id] = texturePtr;

    return textures[id];
    
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
        if (iterator->second.use_count() <= 1){
            iterator->second.get()->deleteTexture();
            return iterator;
        }
    }

    return textures.end();

}

void TextureManager::clear(){
    textures.clear();
}

