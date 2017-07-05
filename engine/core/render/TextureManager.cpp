
#include "TextureManager.h"
#include "image/TextureLoader.h"
#include "Engine.h"
#include "image/ColorType.h"
#include "gles2/GLES2Texture.h"
#include "platform/Log.h"

using namespace Supernova;

std::unordered_map<std::string, Texture> TextureManager::textures;


TextureRender* TextureManager::getTextureRender(){
    if (Engine::getRenderAPI() == S_GLES2){
        return new GLES2Texture();
    }
    
    return NULL;
}

Texture TextureManager::loadTexture(std::string relative_path){
    
    return loadTexture(NULL, relative_path);
}

Texture TextureManager::loadTexture(TextureData* textureData, std::string id){

    //Verify if there is a created texture
    if (textures.count(id) > 0){
        return textures[id];
    }

    TextureLoader image;
    if (textureData == NULL){
        image.loadRawImage(id.c_str());
        textureData = image.getRawImage();
    }

    //If no create a new texture
    TextureRender* texture = getTextureRender();
    textureData->resamplePowerOfTwo();
    texture->loadTexture(textureData);
    std::shared_ptr<TextureRender> texturePtr(texture);

    textures[id] = {texturePtr, textureData->getColorFormat(), textureData->getWidth(), textureData->getHeight()};
    
    Log::Debug(LOG_TAG, "Load texture (texture map size: %lu)", textures.size());

    return textures[id];
}

Texture TextureManager::loadTextureCube(std::vector<std::string> relative_paths, std::string id){
    //Verify if there is a created texture
    if (textures.count(id) > 0){
        return textures[id];
    }
    
    TextureLoader image;
    std::vector<TextureData*> texturesData;
    
    for (int i = 0; i < relative_paths.size(); i++){
        image.loadRawImage(relative_paths[i].c_str());
        texturesData.push_back(new TextureData(*image.getRawImage()));
        texturesData.back()->resamplePowerOfTwo();
    }
    
    //If no create a new texture
    TextureRender* texture = getTextureRender();
    texture->loadTextureCube(texturesData);
    std::shared_ptr<TextureRender> texturePtr(texture);
    
    textures[id] = {texturePtr, -1, -1, -1};
    
    std::vector<TextureData*>::iterator it;
    for (it = texturesData.begin(); it != texturesData.end(); ++it) {
        delete (*it);
    }
    
    Log::Debug(LOG_TAG, "Load texture cube (texture map size: %lu)", textures.size());
    
    return textures[id];
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
        if (iterator->second.getTextureRender().use_count() <= 1){
            if (iterator->second.getTextureRender().get() != NULL)
                iterator->second.getTextureRender().get()->deleteTexture();
            return iterator;
        }
    }

    return textures.end();

}

void TextureManager::clear(){
    textures.clear();
}

