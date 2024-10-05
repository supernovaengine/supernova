//
// (c) 2024 Eduardo Doria.
//

#include "Texture.h"

#include "Engine.h"
#include "Log.h"
#include "render/SystemRender.h"

using namespace Supernova;

Texture::Texture(){
    this->render = NULL;
    this->framebuffer = NULL;
    this->numFaces = 1;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = true;
    this->needLoad = false;

    this->minFilter = TextureFilter::LINEAR;
    this->magFilter = TextureFilter::LINEAR;
    this->wrapU = TextureWrap::REPEAT;
    this->wrapV = TextureWrap::REPEAT;
}

Texture::Texture(std::string path){
    this->render = NULL;
    this->framebuffer = NULL;
    this->paths[0] = path;
    this->id = path;
    this->type = TextureType::TEXTURE_2D;
    this->numFaces = 1;
    this->loadFromPath = true;
    this->releaseDataAfterLoad = true;
    this->needLoad = true;

    this->minFilter = TextureFilter::LINEAR;
    this->magFilter = TextureFilter::LINEAR;
    this->wrapU = TextureWrap::REPEAT;
    this->wrapV = TextureWrap::REPEAT;
}

Texture::Texture(std::string id, TextureData data){
    this->render = NULL;
    this->framebuffer = NULL;
    this->data = std::make_shared<std::array<TextureData,6>>();
    this->data->at(0) = data;
    //this->data = TextureDataPool::get(id, *this->data.get());
    this->id = id;
    this->type = TextureType::TEXTURE_2D;
    this->numFaces = 1;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = true;

    this->minFilter = TextureFilter::LINEAR;
    this->magFilter = TextureFilter::LINEAR;
    this->wrapU = TextureWrap::REPEAT;
    this->wrapV = TextureWrap::REPEAT;
}

Texture::Texture(const Texture& rhs){
    render = rhs.render;
    framebuffer = rhs.framebuffer;
    type = rhs.type;
    id = rhs.id;
    for (int i = 0; i < 6; i++){
        paths[i] = rhs.paths[i];
    }
    data = rhs.data;
    numFaces = rhs.numFaces;
    loadFromPath = rhs.loadFromPath;
    releaseDataAfterLoad = rhs.releaseDataAfterLoad;
    needLoad = rhs.needLoad;
    minFilter = rhs.minFilter;
    magFilter = rhs.magFilter;
    wrapU = rhs.wrapU;
    wrapV = rhs.wrapV;
}

Texture& Texture::operator=(const Texture& rhs){
    render = rhs.render;
    framebuffer = rhs.framebuffer;
    type = rhs.type;
    id = rhs.id;
    for (int i = 0; i < 6; i++){
        paths[i] = rhs.paths[i];
    }
    data = rhs.data;
    numFaces = rhs.numFaces;
    loadFromPath = rhs.loadFromPath;
    releaseDataAfterLoad = rhs.releaseDataAfterLoad;
    needLoad = rhs.needLoad;
    minFilter = rhs.minFilter;
    magFilter = rhs.magFilter;
    wrapU = rhs.wrapU;
    wrapV = rhs.wrapV;

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
        releaseDataAfterLoad == rhs.releaseDataAfterLoad &&
        minFilter == rhs.minFilter &&
        magFilter == rhs.magFilter &&
        wrapU == rhs.wrapU &&
        wrapV == rhs.wrapV
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
        releaseDataAfterLoad != rhs.releaseDataAfterLoad ||
        minFilter != rhs.minFilter ||
        magFilter != rhs.magFilter ||
        wrapU != rhs.wrapU ||
        wrapV != rhs.wrapV
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

void Texture::setData(std::string id, TextureData data){
    destroy();

    this->data = std::make_shared<std::array<TextureData,6>>();
    this->data->at(0) = data;
    //this->data = TextureDataPool::get(id, *this->data.get());
    this->id = id;
    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_2D;
    this->numFaces = 1;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = true;
}

void Texture::setId(std::string id){
    this->id = id;
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

void Texture::setCubeDatas(std::string id, TextureData front, TextureData back, TextureData left, TextureData right, TextureData up, TextureData down){
    destroy();

    this->data = std::make_shared<std::array<TextureData,6>>();
    this->data->at(5) = front;
    this->data->at(4) = back;
    this->data->at(1) = left;
    this->data->at(0) = right;
    this->data->at(2) = up;
    this->data->at(3) = down;

    this->id = id;
    this->framebuffer = NULL;
    this->type = TextureType::TEXTURE_CUBE;
    this->numFaces = 6;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = true;
}

void Texture::setFramebuffer(Framebuffer* framebuffer){
    destroy();

    this->framebuffer = framebuffer;
    this->id.clear();
    this->numFaces = 6;
    this->loadFromPath = false;
    this->releaseDataAfterLoad = false;
    this->needLoad = false;
}

bool Texture::load(){

    if (framebuffer)
        return true;

    if (data){
        return true;
    }else{
        data = TextureDataPool::get(id);
        if (data){
            return true;
        }
    }

    if (!needLoad)
        return false;

    numFaces = 1;
	if (type == TextureType::TEXTURE_CUBE){
		numFaces = 6;
	}

    if (loadFromPath){
        this->data = std::make_shared<std::array<TextureData,6>>();

        for (int f = 0; f < numFaces; f++){
            if (paths[f].empty() && type == TextureType::TEXTURE_CUBE){
                Log::error("Cube texture is missing textures");
                return false;
            }
            data->at(f).loadTextureFromFile(paths[f].c_str());

            if (Engine::getTextureStrategy() == TextureStrategy::FIT){
                data->at(f).fitPowerOfTwo();
            }else if (Engine::getTextureStrategy() == TextureStrategy::RESIZE){
                data->at(f).resizePowerOfTwo();
            }
        }
    }

    data = TextureDataPool::get(id, *data.get());

    needLoad = false;

    return true;
}

void Texture::destroy(){
    if (!id.empty()){

        if (render) {
            render.reset();
            render = NULL;
            TexturePool::remove(id);
        }

        if (data) {
            data.reset();
            data = NULL;
            TextureDataPool::remove(id);
        }

        if (!framebuffer){
            needLoad = true;
        }
    }
}

TextureRender* Texture::getRender(){
    if (framebuffer){
        lastFramebufferVersion = framebuffer->getVersion();
        return &framebuffer->getRender().getColorTexture();
    }

    render = TexturePool::get(id);

    if (render){
        data = TextureDataPool::get(id);
        return render.get();
    }

    load();

    if (!id.empty()){
        render = TexturePool::get(id, type, *data.get(), minFilter, magFilter, wrapU, wrapV);
    }

    if (data && releaseDataAfterLoad){
        for (int f = 0; f < numFaces; f++){
            SystemRender::scheduleCleanup(TextureData::cleanupTexture, &data->at(f));
        }
    }

    if (render){
        return render.get();
    }

    return NULL;
}

std::string Texture::getPath(size_t index){
    return paths[index];
}

TextureData& Texture::getData(size_t index){
    return data->at(index);
}

int Texture::getWidth(){
    if (this->framebuffer){
        return framebuffer->getWidth();
    }
    if (!data){
        return 0;
    }
    return getData().getOriginalWidth();
}

int Texture::getHeight(){
    if (this->framebuffer){
        return framebuffer->getHeight();
    }
    if (!data){
        return 0;
    }
    return getData().getOriginalHeight();
}

bool Texture::isTransparent(){
    if (!data){
        return false;
    }
    return getData().isTransparent();
}

void Texture::setReleaseDataAfterLoad(bool releaseDataAfterLoad){
    this->releaseDataAfterLoad = releaseDataAfterLoad;
}

bool Texture::isReleaseDataAfterLoad() const{
    return this->releaseDataAfterLoad;
}

void Texture::releaseData(){
    for (int f = 0; f < numFaces; f++){
        data->at(f).releaseImageData();
    }
}

bool Texture::empty(){
    if (!needLoad && !render)
        return true;

    return false;
}

bool Texture::isFramebuffer(){
    if (framebuffer)
        return true;

    return false;
}

bool Texture::isFramebufferOutdated(){
    if (framebuffer){
        return (lastFramebufferVersion != framebuffer->getVersion());
    }

    return false;
}

void Texture::setMinFilter(TextureFilter filter){
    if (framebuffer){
        framebuffer->setMinFilter(filter);
    }
    minFilter = filter;
}

TextureFilter Texture::getMinFilter() const{
    if (framebuffer){
        return framebuffer->getMinFilter();
    }
    return minFilter;
}

void Texture::setMagFilter(TextureFilter filter){
    if (framebuffer){
        framebuffer->setMagFilter(filter);
    }
    magFilter = filter;
}

TextureFilter Texture::getMagFilter() const{
    if (framebuffer){
        return framebuffer->getMagFilter();
    }
    return magFilter;
}

void Texture::setWrapU(TextureWrap wrapU){
    if (framebuffer){
        framebuffer->setWrapU(wrapU);
    }
    this->wrapU = wrapU;
}

TextureWrap Texture::getWrapU() const{
    if (framebuffer){
        return framebuffer->getWrapU();
    }
    return wrapU;
}

void Texture::setWrapV(TextureWrap wrapV){
    if (framebuffer){
        framebuffer->setWrapV(wrapV);
    }
    this->wrapV = wrapV;
}

TextureWrap Texture::getWrapV() const{
    if (framebuffer){
        return framebuffer->getWrapV();
    }
    return wrapV;
}