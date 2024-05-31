//
// (c) 2024 Eduardo Doria.
//

#include "ShaderPool.h"

#include "Log.h"
#include "Engine.h"
#include "shader/SBSReader.h"
#include "util/Base64.h"

#ifdef SOKOL_GLCORE33
#include "glsl330.h"
#endif
#ifdef SOKOL_GLES3
#include "glsl300es.h"
#endif
#ifdef SOKOL_D3D11
#include "hlsl5.h"
#endif
#if SOKOL_METAL || SUPERNOVA_APPLE
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include "msl21ios.h"
#elif TARGET_OS_MAC
#include "msl21macos.h"
#endif
#endif

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
	if (Engine::getGraphicBackend() == GraphicBackend::GLCORE33){
		return "glsl330";
	}else if (Engine::getGraphicBackend() == GraphicBackend::GLES3){
		return "glsl300es";
	}else if (Engine::getGraphicBackend() == GraphicBackend::METAL){
		if (Engine::getPlatform() == Platform::MacOS){
			return "msl21macos";
		}else if (Engine::getPlatform() == Platform::iOS){
			return "msl21ios";
		}
	}else if (Engine::getGraphicBackend() == GraphicBackend::D3D11){
		return "hlsl5";
	}

	return "<unknown>";
}


std::string ShaderPool::getShaderFile(std::string shaderStr){
	std::string filename = getShaderName(shaderStr);

	filename += ".sbs";

	return filename;
}

std::string ShaderPool::getShaderName(std::string shaderStr){
	std::string name = shaderStr;

	name += "_" + getShaderLangStr();

	return name;
}

std::string ShaderPool::getShaderStr(ShaderType shaderType, std::string properties){

	std::string str;

	if (shaderType == ShaderType::MESH){
		str = "mesh";
	}else if (shaderType == ShaderType::SKYBOX){
		str = "sky";
	}else if (shaderType == ShaderType::UI){
		str = "ui";
	}else if (shaderType == ShaderType::DEPTH){
		str = "depth";
	}else if (shaderType == ShaderType::POINTS){
		str = "points";
	}else if (shaderType == ShaderType::LINES){
		str = "lines";
	}

	if (str.empty())
		Log::error("Erro mapping shader type to string");

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

	std::string base64Shd = getBase64Shader(getShaderName(shaderStr));
	if (!base64Shd.empty() && sbs.read(Base64::decode(base64Shd))){ // from c header
		resource->createShader(sbs.getShaderData());
		//Log::debug("Create shader %s", shaderStr.c_str());
	}else if (sbs.read("shader://"+getShaderFile(shaderStr))){ // from file in assets/shaders dir
		resource->createShader(sbs.getShaderData());
		//Log::debug("Create shader %s", shaderStr.c_str());
	}else{
		getMissingShaders().push_back(shaderStr);
	}

	shared = resource;

	return resource;
}

void ShaderPool::remove(ShaderType shaderType, std::string properties){
	std::string shaderStr = getShaderStr(shaderType, properties);
	if (getMap().count(shaderStr)){
		auto& shared = getMap()[shaderStr];
		if (shared.use_count() <= 1){
			shared->destroyShader();
			//Log::debug("Remove shader %s", shaderStr.c_str());
			getMap().erase(shaderStr);
		}
	}else{
		if (Engine::isViewLoaded()){
			Log::debug("Trying to destroy a non existent shader");
		}
	}
}

std::string ShaderPool::getMeshProperties(bool unlit, bool uv1, bool uv2, 
						bool punctual, bool shadows, bool shadowsPCF, bool normals, bool normalMap, 
						bool tangents, bool vertexColorVec3, bool vertexColorVec4, bool textureRect, 
						bool fog, bool skinning, bool morphTarget, bool morphNormal, bool morphTangent,
						bool terrain){
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
	if (shadowsPCF)
		prop += "Pcf";
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
	if (textureRect)
		prop += "Txr";
	if (fog)
		prop += "Fog";
	if (skinning)
		prop += "Ski";
	if (morphTarget)
		prop += "Mta";
	if (morphNormal)
		prop += "Mnr";
	if (morphTangent)
		prop += "Mtg";
	if (terrain)
		prop += "Ter";

	return prop;
}

std::string ShaderPool::getDepthMeshProperties(bool skinning, bool morphTarget, bool morphNormal, bool morphTangent, bool terrain){
	std::string prop;

	if (skinning)
		prop += "Ski";
	if (morphTarget)
		prop += "Mta";
	if (morphNormal)
		prop += "Mnr";
	if (morphTangent)
		prop += "Mtg";
	if (terrain)
		prop += "Ter";

	return prop;
}

std::string ShaderPool::getUIProperties(bool texture, bool fontAtlasTexture, bool vertexColorVec3, bool vertexColorVec4){
	std::string prop;

	if (texture)
		prop += "Tex";
	if (fontAtlasTexture)
		prop += "Ftx";
	if (vertexColorVec3)
		prop += "Vc3";
	if (vertexColorVec4)
		prop += "Vc4";

	return prop;
}

std::string ShaderPool::getPointsProperties(bool texture, bool vertexColorVec3, bool vertexColorVec4, bool textureRect){
	std::string prop;

	if (texture)
		prop += "Tex";
	if (vertexColorVec3)
		prop += "Vc3";
	if (vertexColorVec4)
		prop += "Vc4";
	if (textureRect)
		prop += "Txr";

	return prop;
}

std::string ShaderPool::getLinesProperties(bool vertexColorVec3, bool vertexColorVec4){
	std::string prop;

	if (vertexColorVec3)
		prop += "Vc3";
	if (vertexColorVec4)
		prop += "Vc4";

	return prop;
}

void ShaderPool::clear(){
	for (auto& it: getMap()) {
		if (it.second)
			it.second->destroyShader();
	}
	//Log::debug("Remove all shaders");
	getMap().clear();
}
