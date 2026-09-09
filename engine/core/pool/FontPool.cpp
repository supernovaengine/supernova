// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "FontPool.h"

#include "Engine.h"
#include "Log.h"

#include <vector>

using namespace doriax;

fonts_t& FontPool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static fonts_t* map = new fonts_t();
    return *map;
};

std::shared_ptr<STBText> FontPool::get(const std::string& id){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	return NULL;
}

std::shared_ptr<STBText> FontPool::get(const std::string& id, const std::string& fontpath, const std::vector<std::string>& fallbackPaths, unsigned int fontSize){
	auto& shared = getMap()[id];

	if (shared.use_count() > 0){
		return shared;
	}

	const auto resource = std::make_shared<STBText>();
    resource->load(fontpath, fallbackPaths, fontSize);

	shared = resource;

	return resource;
}

void FontPool::remove(const std::string& id){
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

void FontPool::clearUnused(){
	std::vector<std::string> ids;
	ids.reserve(getMap().size());
	for (const auto& [id, _] : getMap()) {
		ids.push_back(id);
	}
	for (const auto& id : ids) {
		remove(id);
	}
}
