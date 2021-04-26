#include "ShaderPool.h"

#include "Log.h"

using namespace Supernova;

shaders_t& ShaderPool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static shaders_t* map = new shaders_t();
    return *map;
};

std::shared_ptr<ShaderRender> ShaderPool::get(ShaderType shaderType){
	auto& shared = getMap()[shaderType];

	if (shared.use_count() > 0){
		return shared;
	}

	const auto resource =  std::make_shared<ShaderRender>();
	resource->createShader(shaderType);
	shared = resource;
	
	return resource;
}

void ShaderPool::remove(ShaderType shaderType){
	if (getMap().count(shaderType)){
		auto& shared = getMap()[shaderType];
		if (shared.use_count() <= 1){
			shared->destroyShader();
			getMap().erase(shaderType);
		}
	}else{
		Log::Debug("Trying to destroy a not existent shader");
	}
}
