//
// (c) 2023 Eduardo Doria.
//

#include "FontPool.h"

#include "Engine.h"
#include "Log.h"

using namespace Supernova;

fonts_t& FontPool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static fonts_t* map = new fonts_t();
    return *map;
};

std::shared_ptr<STBText> FontPool::get(std::string id){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	return NULL;
}

std::shared_ptr<STBText> FontPool::get(std::string id, std::string fontpath, unsigned int fontSize){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	const auto resource = std::make_shared<STBText>();
    resource->load(fontpath, fontSize);

	shared = resource;

	return resource;
}

void FontPool::remove(std::string id){
	if (getMap().count(id)){
		auto& shared = getMap()[id];
		if (shared.use_count() <= 1){
			getMap().erase(id);
		}
	}
}

void FontPool::clear(){
	getMap().clear();
}
