
#include "TextureManager.h"
#include "image/TextureLoader.h"
#include "Supernova.h"


std::vector<TextureManager::TextureStore> TextureManager::textures;


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
    for (unsigned i=0; i<textures.size(); i++){
        std::string teste = textures.at(i).key;
        if (teste == id){
            return textures.at(i).value;
        }
    }
    
    TextureLoader image;
    if (textureFile == NULL){
        image.loadRawImage(id.c_str());
        textureFile = image.getRawImage();
    }
    
    TextureRender* texture = getTextureRender();
    textureFile->resamplePowerOfTwo();
    texture->loadTexture(textureFile->getWidth(), textureFile->getHeight(), textureFile->getColorFormat(), textureFile->getData());
    std::shared_ptr<TextureRender> texturePtr(texture);
    textures.push_back((TextureStore){texturePtr, id});
    
    return textures.at(textures.size()-1).value;
    
}

void TextureManager::deleteUnused(){
    
    int remove = findToRemove();
    while (remove >= 0){
        textures.erase(textures.begin() + remove);
        remove = findToRemove();
    }
    
}

int TextureManager::findToRemove(){
    for (unsigned i=0; i<textures.size(); i++){
        if (textures.at(i).value.use_count() <= 1){
            textures.at(i).value.get()->deleteTexture();
            return i;
        }
    }
    return -1;
}

void TextureManager::clear(){
    textures.clear();
}

