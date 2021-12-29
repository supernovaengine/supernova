#include "TexturePool.h"

#include "Engine.h"
#include "Log.h"

using namespace Supernova;

textures_t& TexturePool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static textures_t* map = new textures_t();
    return *map;
};

std::shared_ptr<TextureRender> TexturePool::get(std::string id){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	return NULL;
}

std::shared_ptr<TextureRender> TexturePool::get(std::string id, TextureType type, TextureData* data){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	int numFaces = 1;
	if (type == TextureType::TEXTURE_CUBE){
		numFaces = 6;
	}

	TextureDataSize texData[6];

	for (int f = 0; f < numFaces; f++){
		texData[f] = {data[f].getSize(), data[f].getData()};
	}

	const auto resource =  std::make_shared<TextureRender>();
	resource->createTexture(id, data[0].getWidth(), data[0].getHeight(), data[0].getColorFormat(), type, numFaces, texData);
	shared = resource;

	return resource;
}

void TexturePool::remove(std::string id){
	if (getMap().count(id)){
		auto& shared = getMap()[id];
		if (shared.use_count() <= 1){
			shared->destroyTexture();
			getMap().erase(id);
		}
	}else{
		Log::Debug("Trying to destroy a not existent texture: %s", id.c_str());
	}
}
