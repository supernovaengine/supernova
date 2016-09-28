
#include "TextureManager.h"
#include "Supernova.h"


std::vector<TextureManager::TextureStore> TextureManager::textures;


TextureRender* TextureManager::getTextureRender(){
    if (Supernova::getRenderAPI() == S_GLES2){
        return new GLES2Texture();
    }
    
    return NULL;
}

TextureRender* TextureManager::loadTexture(std::string relative_path){
    
    //Verify if there is a created texture
    for (unsigned i=0; i<textures.size(); i++){
        std::string teste =textures.at(i).key;
        if (teste == relative_path){
            textures.at(i).reference = textures.at(i).reference + 1;
            return textures.at(i).value;
        }
    }
    
    TextureRender* texture = getTextureRender();
    texture->loadTexture(relative_path.c_str());
    textures.push_back((TextureStore){texture, relative_path, 1});
    return texture;
}


void TextureManager::deleteTexture(std::string relative_path){
    int remove = -1;
    
    for (unsigned i=0; i<textures.size(); i++){
        if (textures.at(i).key == relative_path){
            textures.at(i).reference = textures.at(i).reference - 1;
            if (textures.at(i).reference == 0){
                remove = i;
                textures.at(i).value->deleteTexture();
            }
        }
    }
    
    if (remove > 0)
        textures.erase(textures.begin() + remove);
    
}

void TextureManager::clear(){
    textures.clear();
}

