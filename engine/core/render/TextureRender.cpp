#include "TextureRender.h"

#include "Engine.h"
#include "gles2/GLES2Texture.h"
#include "Log.h"

using namespace Supernova;

std::unordered_map< std::string, std::shared_ptr<TextureRender> > TextureRender::texturesRender;

TextureRender::TextureRender(){
    this->loaded = false;
    
    this->colorFormat = -1;
    this->width = 0;
    this->height = 0;

    this->nearestScale = false;
}

TextureRender::~TextureRender(){
    
}

std::shared_ptr<TextureRender> TextureRender::sharedInstance(std::string id){

    if (texturesRender.count(id) > 0){
        return texturesRender[id];
    }else{
    
        if (Engine::getRenderAPI() == S_GLES2){
            texturesRender[id] = std::shared_ptr<TextureRender>(new GLES2Texture());
            Log::Debug("Load texture (texture map size: %lu): %s", texturesRender.size(), id.c_str());
            return texturesRender[id];
        }
    }
    
    return NULL;
}

void TextureRender::deleteUnused(){
    
    TextureRender::it_type remove = findToRemove();
    while (remove != texturesRender.end()){
        texturesRender.erase(remove);
        Log::Debug("Delete texture (texture map size: %lu)", texturesRender.size());
        remove = findToRemove();
    }
    
}

TextureRender::it_type TextureRender::findToRemove(){
    
    for(TextureRender::it_type iterator = texturesRender.begin(); iterator != texturesRender.end(); iterator++) {
        if (iterator->second.use_count() <= 1){
            if (iterator->second.get() != NULL)
                iterator->second.get()->deleteTexture();
            return iterator;
        }
    }
    
    return texturesRender.end();
    
}

bool TextureRender::isLoaded(){
    return loaded;
}

int TextureRender::getColorFormat(){
    return colorFormat;
}

int TextureRender::getWidth(){
    return width;
}

int TextureRender::getHeight(){
    return height;
}

void TextureRender::setNearestScale(bool nearestScale){
    this->nearestScale = nearestScale;
}

bool TextureRender::loadTexture(TextureData* texturedata){
    loaded = true;
    
    colorFormat = texturedata->getColorFormat();
    width = texturedata->getWidth();
    height = texturedata->getHeight();
    
    return true;
}

bool TextureRender::loadTextureCube(std::vector<TextureData*> texturesdata){
    loaded = true;
    
    colorFormat = -1;
    width = -1;
    height = -1;
    
    return true;
}

bool TextureRender::loadTextureFrame(int width, int height, bool depthframe){
    loaded = true;
    
    colorFormat = -1;
    this->width = width;
    this->height = height;
    
    return true;
}

bool TextureRender::loadTextureFrameCube(int width, int height){
    loaded = true;
    
    colorFormat = -1;
    this->width = width;
    this->height = height;
    
    return true;
}

void TextureRender::deleteTexture(){
    loaded = false;
}
