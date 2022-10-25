#include "Texture.h"

#include "Engine.h"
#include "Log.h"

using namespace Supernova;

Texture::Texture(){
    this->renderAndData = NULL;
    this->framebuffer = NULL;
    this->numFaces = 1;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = true;
    this->needLoad = false;
}

Texture::Texture(std::string path){
    this->renderAndData = NULL;
    this->framebuffer = NULL;
    this->paths[0] = path;
    this->id = path;
    this->type = TextureType::TEXTURE_2D;
    this->numFaces = 1;
    this->loadFromPath = true;
    this->releaseDataAfterLoad = true;
    this->needLoad = true;
}

Texture::Texture(TextureData data, std::string id){
    this->renderAndData = NULL;
    this->framebuffer = NULL;
    this->data[0] = data;
    this->id = id;
    this->type = TextureType::TEXTURE_2D;
    this->numFaces = 1;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = true;
}

Texture::Texture(const Texture& rhs){
    renderAndData = rhs.renderAndData;
    framebuffer = rhs.framebuffer;
    type = rhs.type;
    id = rhs.id;
    for (int i = 0; i < 6; i++){
        paths[i] = rhs.paths[i];
        data[i] = rhs.data[i];
    }
    numFaces = rhs.numFaces;
    loadFromPath = rhs.loadFromPath;
    releaseDataAfterLoad = rhs.releaseDataAfterLoad;
    needLoad = rhs.needLoad;
}

Texture& Texture::operator=(const Texture& rhs){
    renderAndData = rhs.renderAndData;
    framebuffer = rhs.framebuffer;
    type = rhs.type;
    id = rhs.id;
    for (int i = 0; i < 6; i++){
        paths[i] = rhs.paths[i];
        data[i] = rhs.data[i];
    }
    numFaces = rhs.numFaces;
    loadFromPath = rhs.loadFromPath;
    releaseDataAfterLoad = rhs.releaseDataAfterLoad;
    needLoad = rhs.needLoad;

    return *this; 
}

bool Texture::operator == ( const Texture& rhs ) const{
    return (
        framebuffer == rhs.framebuffer &&
        type == rhs.type &&
        id == rhs.id &&
        paths[0] == rhs.paths[0] &&
        paths[1] == rhs.paths[1] &&
        paths[2] == rhs.paths[2] &&
        paths[3] == rhs.paths[3] &&
        paths[4] == rhs.paths[4] &&
        paths[5] == rhs.paths[5] &&
        numFaces == rhs.numFaces &&
        loadFromPath == rhs.loadFromPath &&
        releaseDataAfterLoad == rhs.releaseDataAfterLoad
     );
}

bool Texture::operator != ( const Texture& rhs ) const{
    return (
        framebuffer != rhs.framebuffer ||
        type != rhs.type ||
        id != rhs.id ||
        paths[0] != rhs.paths[0] ||
        paths[1] != rhs.paths[1] ||
        paths[2] != rhs.paths[2] ||
        paths[3] != rhs.paths[3] ||
        paths[4] != rhs.paths[4] ||
        paths[5] != rhs.paths[5] ||
        numFaces == rhs.numFaces ||
        loadFromPath != rhs.loadFromPath ||
        releaseDataAfterLoad != rhs.releaseDataAfterLoad
    );
}

Texture::~Texture(){

}

void Texture::setPath(std::string path){
    destroy();

    this->paths[0] = path;
    this->id = path;
    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_2D;
    this->numFaces = 1;
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
    this->numFaces = 1;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = true;
}

void Texture::setCubePath(size_t index, std::string path){
    destroy();

    this->paths[index] = path;

    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_CUBE;
    this->numFaces = 6;
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
    this->numFaces = 6;
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
    this->numFaces = 6;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = false;
}

bool Texture::load(){

    if (!needLoad)
        return false;

    renderAndData = TexturePool::get(id);
    if (renderAndData){
        for (int f = 0; f < 6; f++){
            data[f] = renderAndData->data[f];
        }
        return true;
    }

    numFaces = 1;
	if (type == TextureType::TEXTURE_CUBE){
		numFaces = 6;
	}

    if (loadFromPath){
	    for (int f = 0; f < numFaces; f++){
            if (paths[f].empty() && type == TextureType::TEXTURE_CUBE){
            	Log::error("Cube texture is missing textures");
			    return false;
            }
    	    data[f].loadTextureFromFile(paths[f].c_str());

            if (Engine::getTextureStrategy() == TextureStrategy::FIT){
                data[f].fitPowerOfTwo();
            }else if (Engine::getTextureStrategy() == TextureStrategy::RESAMPLE){
                data[f].resamplePowerOfTwo();
            }
	    }
    }

	renderAndData = TexturePool::get(id, type, data);

    if (releaseDataAfterLoad){
        for (int f = 0; f < numFaces; f++){
    	    data[f].releaseImageData();
	    }
    }

    needLoad = false;

    return true;
}

void Texture::destroy(){
    if (!id.empty() && renderAndData){
	    renderAndData.reset();
	    TexturePool::remove(id);
    }
}

TextureRender* Texture::getRender(){
    if (framebuffer)
        return &framebuffer->getColorTexture();

    if (needLoad && !renderAndData)
        load();

    if (!needLoad && !renderAndData)
        return NULL;

    return &renderAndData->render;
}

std::string Texture::getPath(size_t index){
    return paths[index];
}

TextureData& Texture::getData(size_t index){
    return data[index];
}

void Texture::setReleaseDataAfterLoad(bool releaseDataAfterLoad){
    this->releaseDataAfterLoad = releaseDataAfterLoad;
}

void Texture::releaseData(){
    for (int f = 0; f < numFaces; f++){
        data[f].releaseImageData();
    }
}

bool Texture::empty(){
    if (!needLoad && !renderAndData)
        return true;

    return false;
}

bool Texture::hasTextureFrame(){
    if (framebuffer)
        return true;

    return false;
}