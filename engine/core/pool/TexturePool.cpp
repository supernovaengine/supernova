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

std::shared_ptr<TexturePoolData> TexturePool::get(std::string id){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	return NULL;
}

std::shared_ptr<TexturePoolData> TexturePool::get(std::string id, TextureType type, TextureData data[6], TextureFilter minFilter, TextureFilter magFilter){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	int numFaces = 1;
	if (type == TextureType::TEXTURE_CUBE){
		numFaces = 6;
	}

	const auto resource =  std::make_shared<TexturePoolData>();

	void* data_array[6];
	size_t size_array[6];

	for (int f = 0; f < numFaces; f++){
		data_array[f] = data[f].getData();
		size_array[f] = (size_t)data[f].getSize();

		resource->data[f] = data[f];
	}

	resource->render.createTexture(id, data[0].getWidth(), data[0].getHeight(), data[0].getColorFormat(), type, numFaces, data_array, size_array, minFilter, magFilter);
	shared = resource;

	return resource;
}

void TexturePool::remove(std::string id){
	if (getMap().count(id)){
		auto& shared = getMap()[id];
		if (shared.use_count() <= 1){
			shared->render.destroyTexture();
			getMap().erase(id);
		}
	}else{
		Log::debug("Trying to destroy a not existent texture: %s", id.c_str());
	}
}
