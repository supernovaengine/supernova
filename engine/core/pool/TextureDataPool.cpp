//
// (c) 2023 Eduardo Doria.
//

#include "TextureDataPool.h"

#include "Engine.h"
#include "Log.h"

using namespace Supernova;

texturesdata_t& TextureDataPool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static texturesdata_t* map = new texturesdata_t();
    return *map;
};

std::shared_ptr<std::array<TextureData,6>> TextureDataPool::get(std::string id){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	return NULL;
}

std::shared_ptr<std::array<TextureData,6>> TextureDataPool::get(std::string id, std::array<TextureData,6> data){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	const auto resource =  std::make_shared<std::array<TextureData,6>>(data);

	shared = resource;

	return resource;
}

void TextureDataPool::remove(std::string id){
	if (getMap().count(id)){
		auto& shared = getMap()[id];
		if (shared.use_count() <= 1){
			getMap().erase(id);
		}
	}else{
		if (Engine::isViewLoaded()){
			Log::debug("Trying to destroy a non existent texture data: %s", id.c_str());
		}
	}
}

void TextureDataPool::clear(){
	getMap().clear();
}
