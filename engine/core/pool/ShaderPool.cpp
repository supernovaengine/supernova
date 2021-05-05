#include "ShaderPool.h"

#include "Log.h"
#include "Engine.h"
#include "shader/SGSReader.h"

using namespace Supernova;

shaders_t& ShaderPool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static shaders_t* map = new shaders_t();
    return *map;
};

std::string ShaderPool::getShaderFile(ShaderType shaderType){
	std::string filename = "";
	
	if (shaderType == ShaderType::SKYBOX){
		filename += "sky";
	}

	if (Engine::getPlatform() == Platform::LINUX){
		filename += "_glsl330";
	}

	filename += ".sgs";

	return filename;
}

std::shared_ptr<ShaderRender> ShaderPool::get(ShaderType shaderType){
	auto& shared = getMap()[shaderType];

	if (shared.use_count() > 0){
		return shared;
	}

	SGSReader sgs;
	sgs.read("shader://"+getShaderFile(shaderType));

	const auto resource =  std::make_shared<ShaderRender>();
	resource->createShader(sgs.getShaderData());
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
