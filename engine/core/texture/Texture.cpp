#include "Texture.h"

#include "pool/TexturePool.h"
#include "Engine.h"
#include "Log.h"

using namespace Supernova;

Texture::Texture(){
    this->render = NULL;
    this->framebuffer = NULL;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = true;
    this->needLoad = false;
}

Texture::Texture(std::string path){
    this->render = NULL;
    this->framebuffer = NULL;
    this->paths[0] = path;
    this->id = path;
    this->type = TextureType::TEXTURE_2D;
    this->loadFromPath = true;
    this->releaseDataAfterLoad = true;
    this->needLoad = true;
}

Texture::Texture(TextureData data, std::string id){
    this->render = NULL;
    this->framebuffer = NULL;
    this->data[0] = data;
    this->id = id;
    this->type = TextureType::TEXTURE_2D;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = true;
}

Texture::Texture(const Texture& rhs){
    render = rhs.render;
    framebuffer = rhs.framebuffer;
    type = rhs.type;
    id = rhs.id;
    for (int i = 0; i < 6; i++){
        paths[i] = rhs.paths[i];
        data[i] = rhs.data[i];
    }
    loadFromPath = rhs.loadFromPath;
    releaseDataAfterLoad = rhs.releaseDataAfterLoad;
    needLoad = rhs.needLoad;
}

Texture& Texture::operator=(const Texture& rhs){
    render = rhs.render;
    framebuffer = rhs.framebuffer;
    type = rhs.type;
    id = rhs.id;
    for (int i = 0; i < 6; i++){
        paths[i] = rhs.paths[i];
        data[i] = rhs.data[i];
    }
    loadFromPath = rhs.loadFromPath;
    releaseDataAfterLoad = rhs.releaseDataAfterLoad;
    needLoad = rhs.needLoad;

    return *this; 
}

Texture::~Texture(){

}

void Texture::setPath(std::string path){
    destroy();

    this->paths[0] = path;
    this->id = path;
    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_2D;
    this->loadFromPath = true;
    this->releaseDataAfterLoad = true;
    this->needLoad = true;
}

void Texture::setData(TextureData data, std::string id){
    destroy();

    this->data[0] = data;
    this->id = id;
    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_2D;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = true;
}

void Texture::setCubePath(size_t index, std::string path){
    destroy();

    this->paths[index] = path;

    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_CUBE;
    this->loadFromPath = true;
    this->releaseDataAfterLoad = true;
    this->needLoad = true;

    std::string id = "cube";
    for (int f = 0; f < 6; f++){
        id = id + "|" + paths[f];
    }
    this->id = id;
}

void Texture::setCubePaths(std::string front, std::string back, std::string left, std::string right, std::string up, std::string down){
    destroy();

    this->paths[5] = front;
    this->paths[4] = back;
    this->paths[1] = left;
    this->paths[0] = right;
    this->paths[2] = up;
    this->paths[3] = down;

    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_CUBE;
    this->loadFromPath = true;
    this->releaseDataAfterLoad = true;
    this->needLoad = true;

    std::string id = "cube";
    for (int f = 0; f < 6; f++){
        id = id + "|" + paths[f];
    }
    this->id = id;
}

void Texture::setFramebuffer(FramebufferRender* framebuffer){
    destroy();

    this->framebuffer = framebuffer;
    this->id.clear();
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = false;
}

bool Texture::load(){

    if (!needLoad)
        return false;

    render = TexturePool::get(id);
    if (render)
        return true;

    int numFaces = 1;
	if (type == TextureType::TEXTURE_CUBE){
		numFaces = 6;
	}

    if (loadFromPath){
	    for (int f = 0; f < numFaces; f++){
            if (paths[f].empty() && type == TextureType::TEXTURE_CUBE){
            	Log::Error("Cube texture is missing textures");
			    return false;
            }
    	    data[f].loadTextureFromFile(paths[f].c_str());
	    }
    }

	render = TexturePool::get(id, type, data);

    if (releaseDataAfterLoad){
        for (int f = 0; f < numFaces; f++){
    	    data[f].releaseImageData();
	    }
    }

    needLoad = false;

    return true;
}

void Texture::destroy(){
    if (!id.empty() && render){
	    render.reset();
	    TexturePool::remove(id);
    }
}

TextureRender* Texture::getRender(){
    if (framebuffer)
        return &framebuffer->getColorTexture();

    if (needLoad && !render)
        load();

    if (!needLoad && !render)
        return NULL;

    return render.get();
}

TextureData& Texture::getData(size_t index){
    return data[index];
}

void Texture::setReleaseDataAfterLoad(bool releaseDataAfterLoad){
    this->releaseDataAfterLoad = releaseDataAfterLoad;
}

bool Texture::empty(){
    if (!needLoad && !render)
        return true;

    return false;
}

bool Texture::hasTextureFrame(){
    if (framebuffer)
        return true;

    return false;
}