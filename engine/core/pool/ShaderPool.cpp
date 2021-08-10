#include "ShaderPool.h"

#include "Log.h"
#include "Engine.h"
#include "shader/SBSReader.h"

using namespace Supernova;

shaders_t& ShaderPool::getMap(){
    //To prevent similar problem of static init fiasco but on deinitialization
    //https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
    static shaders_t* map = new shaders_t();
    return *map;
};

std::vector<std::string>& ShaderPool::getMissingShaders(){
    static std::vector<std::string>* missingshaders = new std::vector<std::string>();
    return *missingshaders;
};

std::string ShaderPool::getShaderLangStr(){
	if (Engine::getPlatform() == Platform::LINUX){
		return "glsl330";
	}else if (Engine::getPlatform() == Platform::WEB){
		return "glsl100";
	}else if (Engine::getPlatform() == Platform::MACOS){
		return "msl21_macos";
	}else if (Engine::getPlatform() == Platform::IOS){
		return "msl21_ios";
	}else if (Engine::getPlatform() == Platform::WINDOWS){
		return "hlsl5";
	}

	return "<unknown>";
}


std::string ShaderPool::getShaderFile(std::string shaderStr){
	std::string filename = shaderStr;

	filename += "_" + getShaderLangStr();

	filename += ".sbs";

	return filename;
}

std::string ShaderPool::getShaderStr(ShaderType shaderType, std::string properties){

	std::string str;

	if (shaderType == ShaderType::MESH){
		str = "mesh";
	}else if (shaderType == ShaderType::SKYBOX){
		str = "sky";
	}else if (shaderType == ShaderType::DEPTH){
		str = "depth";
	}else if (shaderType == ShaderType::LINES){
		str = "lines";
	}else if (shaderType == ShaderType::POINTS){
		str = "points";
	}

	if (str.empty())
		Log::Error("Erro mapping shader type to string");

	if (!properties.empty())
		str += "_" + properties;

	return str;
}

std::shared_ptr<ShaderRender> ShaderPool::get(ShaderType shaderType, std::string properties){
	std::string shaderStr = getShaderStr(shaderType, properties);
	auto& shared = getMap()[shaderStr];

	if (shared.use_count() > 0){
		return shared;
	}

	SBSReader sbs;
	const auto resource =  std::make_shared<ShaderRender>();

	if (sbs.read("shader://"+getShaderFile(shaderStr))){
		resource->createShader(sbs.getShaderData());
	}else{
		getMissingShaders().push_back(shaderStr);
	}
	
	shared = resource;

	return resource;
}

void ShaderPool::remove(ShaderType shaderType, std::string properties){
	std::string shaderStr = getShaderStr(shaderType, properties);
	int teste = getMap().count(shaderStr);
	if (getMap().count(shaderStr)){
		auto& shared = getMap()[shaderStr];
		if (shared.use_count() <= 1){
			shared->destroyShader();
			getMap().erase(shaderStr);
		}
	}else{
		Log::Debug("Trying to destroy a not existent shader");
	}
}

std::string ShaderPool::getMeshProperties(bool unlit, bool uv1, bool uv2, 
						bool punctual, bool shadows, bool normals, bool normalMap, 
						bool tangents, bool vertexColorVec3, bool vertexColorVec4){
	std::string prop;

	if (unlit)
		prop += "Ult";
	if (uv1)
		prop += "Uv1";
	if (uv2)
		prop += "Uv2";
	if (punctual)
		prop += "Puc";
	if (shadows)
		prop += "Shw";
	if (normals)
		prop += "Nor";
	if (normalMap)
		prop += "Nmp";
	if (tangents)
		prop += "Tan";
	if (vertexColorVec3)
		prop += "Vc3";
	if (vertexColorVec4)
		prop += "Vc4";

	return prop;
}
