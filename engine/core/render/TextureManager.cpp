
#include "TextureManager.h"
#include "image/TextureLoader.h"
#include "Engine.h"
#include "image/ColorType.h"
#include "gles2/GLES2Texture.h"
#include "platform/Log.h"

using namespace Supernova;

std::unordered_map<std::string, TextureManager::TextureStore> TextureManager::textures;


TextureRender* TextureManager::getTextureRender(){
    if (Engine::getRenderAPI() == S_GLES2){
        return new GLES2Texture();
    }
    
    return NULL;
}

std::shared_ptr<TextureRender> TextureManager::loadTexture(std::string relative_path){
    
    return loadTexture(NULL, relative_path);
}

std::shared_ptr<TextureRender> TextureManager::loadTexture(TextureFile* textureFile, std::string id){

    //Verify if there is a created texture
    if (textures.count(id) > 0){
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
    texture->loadTexture(textureFile);
    std::shared_ptr<TextureRender> texturePtr(texture);

    bool useAlpha = false;
    if (textureFile->getColorFormat() == S_COLOR_GRAY_ALPHA ||
        textureFile->getColorFormat() == S_COLOR_RGB_ALPHA ||
        textureFile->getColorFormat() == S_COLOR_ALPHA)
        useAlpha = true;

    textures[id] = {texturePtr, useAlpha, textureFile->getWidth(), textureFile->getHeight()};
    
    Log::Debug(LOG_TAG, "Load texture (texture map size: %lu)", textures.size());

    return textures[id].value;
}

std::shared_ptr<TextureRender> TextureManager::loadTextureCube(std::vector<std::string> relative_paths, std::string id){
    //Verify if there is a created texture
    if (textures.count(id) > 0){
        return textures[id].value;
    }
    
    TextureLoader image;
    std::vector<TextureFile*> textureFiles;
    bool useAlpha = false;
    
    for (int i = 0; i < relative_paths.size(); i++){
        image.loadRawImage(relative_paths[i].c_str());
        textureFiles.push_back(new TextureFile(*image.getRawImage()));
        textureFiles.back()->resamplePowerOfTwo();
        if (textureFiles.back()->getColorFormat() == S_COLOR_GRAY_ALPHA || textureFiles.back()->getColorFormat() == S_COLOR_RGB_ALPHA)
            useAlpha = true;
    }
    
    //If no create a new texture
    TextureRender* texture = getTextureRender();
    texture->loadTextureCube(textureFiles);
    std::shared_ptr<TextureRender> texturePtr(texture);
    
    textures[id] = {texturePtr, useAlpha, -1, -1};
    
    std::vector<TextureFile*>::iterator it;
    for (it = textureFiles.begin(); it != textureFiles.end(); ++it) {
        delete (*it);
    }
    
    Log::Debug(LOG_TAG, "Load texture cube (texture map size: %lu)", textures.size());
    
    return textures[id].value;
}

bool TextureManager::hasAlphaChannel(std::string id){
    if (textures.count(id) > 0){
        return textures[id].hasAlphaChannel;
    }
    return false;
}

int TextureManager::getTextureWidth(std::string id){
    if (textures.count(id) > 0){
        return textures[id].width;
    }
    return false;
}

int TextureManager::getTextureHeight(std::string id){
    if (textures.count(id) > 0){
        return textures[id].height;
    }
    return false;
}

void TextureManager::deleteUnused(){

    TextureManager::it_type remove = findToRemove();
    while (remove != textures.end()){
        textures.erase(remove);
        remove = findToRemove();
    }
    
    Log::Debug(LOG_TAG, "Delete texture (texture map size: %lu)", textures.size());
}

TextureManager::it_type TextureManager::findToRemove(){

    for(TextureManager::it_type iterator = textures.begin(); iterator != textures.end(); iterator++) {
        if (iterator->second.value.use_count() <= 1){
            if (iterator->second.value.get() != NULL)
                iterator->second.value.get()->deleteTexture();
            return iterator;
        }
    }

    return textures.end();

}

void TextureManager::clear(){
    textures.clear();
}

