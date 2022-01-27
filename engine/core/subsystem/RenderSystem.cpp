//
// (c) 2021 Eduardo Doria.
//

#include "RenderSystem.h"
#include "object/Camera.h"
#include "math/Quaternion.h"
#include "Scene.h"
#include "Engine.h"
#include "System.h"
#include "render/Render.h"
#include "render/ObjectRender.h"
#include "render/SystemRender.h"
#include "pool/ShaderPool.h"
#include "pool/TexturePool.h"
#include "math/Vector3.h"
#include "math/Angle.h"
#include "buffer/ExternalBuffer.h"
#include <memory>
#include <cmath>

#ifndef PROJECT_ROOT
#define PROJECT_ROOT "<PROJECT_ROOT>"
#endif

using namespace Supernova;

TextureRender RenderSystem::emptyWhite;
TextureRender RenderSystem::emptyBlack;
TextureRender RenderSystem::emptyCubeBlack;
TextureRender RenderSystem::emptyNormal;

bool RenderSystem::emptyTexturesCreated = false;

RenderSystem::RenderSystem(Scene* scene): SubSystem(scene){
	signature.set(scene->getComponentType<Transform>());

	this->scene = scene;

	hasLights = false;
	hasShadows = false;
}

RenderSystem::~RenderSystem(){
}

void RenderSystem::load(){
	createEmptyTextures();
	checkLightsAndShadow();

	if (scene->isMainScene() || scene->isRenderToTexture()){
		sceneRender.setClearColor(scene->getColor());
	}
	depthRender.setClearColor(Vector4(1.0, 1.0, 1.0, 1.0));

	if (scene->isRenderToTexture()){
		if (!scene->getFramebuffer().isCreated())
			scene->getFramebuffer().createFramebuffer(TextureType::TEXTURE_2D, scene->getFramebufferWidth(), scene->getFramebufferHeight(), false);
	}
}

void RenderSystem::createEmptyTextures(){
	if (!emptyTexturesCreated){
		TextureDataSize texData[6];

    	uint32_t pixels[64];

		for (int i = 0; i < 6; i++){
			texData[i].data = (void*)pixels;
			texData[i].size = 8 * 8 * 4;
		}

		for (int i = 0; i < 64; i++) {
        	pixels[i] = 0xFF808080;
    	}

		emptyNormal.createTexture("empty|normal", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, texData);

		for (int i = 0; i < 64; i++) {
        	pixels[i] = 0xFFFFFFFF;
    	}

		emptyWhite.createTexture("empty|white", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, texData);

		for (int i = 0; i < 64; i++) {
        	pixels[i] = 0xFF000000;
    	}

		emptyBlack.createTexture("empty|black", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, texData);
		emptyCubeBlack.createTexture("empty|cube|black", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_CUBE, 6, texData);

		emptyTexturesCreated = true;
	}
}

void RenderSystem::checkLightsAndShadow(){
	auto lights = scene->getComponentArray<LightComponent>();

	int numLights = lights->size();
	if (numLights > MAX_LIGHTS)
		numLights = MAX_LIGHTS;

	if (numLights > 0)
		hasLights = true;
	
	for (int i = 0; i < numLights; i++){
		LightComponent& light = lights->getComponentFromIndex(i);
		if (light.shadows){
			hasShadows = true;
		}
	}
}

bool RenderSystem::processLights(){
	hasLights = false;
	hasShadows = false;

	int freeShadowMap = 0;
	int freeShadowCubeMap = MAX_SHADOWSMAP;

	auto lights = scene->getComponentArray<LightComponent>();

	int numLights = lights->size();
	if (numLights > MAX_LIGHTS)
		numLights = MAX_LIGHTS;

	if (numLights > 0)
		hasLights = true; // Re-check lights on, after checked in checkLightsAndShadow()

	Entity camera = scene->getCamera();
	if (camera == NULL_ENTITY){
		return false;
	}

	Transform& cameraTransform =  scene->getComponent<Transform>(camera);
	
	for (int i = 0; i < numLights; i++){
		LightComponent& light = lights->getComponentFromIndex(i);
		Entity entity = lights->getEntity(i);
		Transform* transform = scene->findComponent<Transform>(entity);

		Vector3 worldPosition;
		if (transform){
			worldPosition = Vector3(transform->worldPosition);
		}

		int type = 0;
		if (light.type == LightType::POINT)
			type = 1;
		if (light.type == LightType::SPOT)
			type = 2;

		light.shadowMapIndex = -1;
		
		if (light.shadows){
			if (light.numShadowCascades > MAX_SHADOWCASCADES){
				light.numShadowCascades = MAX_SHADOWCASCADES;
				Log::Warn("Shadow cascades number is bigger than max value");
			}

			hasShadows = true; // Re-check shadows on, after checked in checkLightsAndShadow()
			if (light.type == LightType::POINT){
				if (!light.framebuffer[0].isCreated())
					light.framebuffer[0].createFramebuffer(TextureType::TEXTURE_CUBE, light.mapResolution, light.mapResolution, true);

				if ((freeShadowCubeMap - MAX_SHADOWSMAP) < MAX_SHADOWSCUBEMAP){
					light.shadowMapIndex = freeShadowCubeMap++;
				}
			}else if (light.type == LightType::SPOT){
				if (!light.framebuffer[0].isCreated())
					light.framebuffer[0].createFramebuffer(TextureType::TEXTURE_2D, light.mapResolution, light.mapResolution, true);

				if (freeShadowMap < MAX_SHADOWSMAP){
					light.shadowMapIndex = freeShadowMap++;
				}
			}else if (light.type == LightType::DIRECTIONAL){
				for (int c = 0; c < light.numShadowCascades; c++){
					if (!light.framebuffer[c].isCreated())
						light.framebuffer[c].createFramebuffer(TextureType::TEXTURE_2D, light.mapResolution, light.mapResolution, true);
				}

				if ((freeShadowMap + light.numShadowCascades - 1) < MAX_SHADOWSMAP){
					light.shadowMapIndex = freeShadowMap;
					freeShadowMap += light.numShadowCascades;
				}
			}

			if (light.shadowMapIndex >= 0){
				size_t numMaps = (light.type == LightType::DIRECTIONAL) ? light.numShadowCascades : 1;
				for (int c = 0; c < numMaps; c++){
					if (light.type != LightType::POINT){
						vs_shadows.lightViewProjectionMatrix[light.shadowMapIndex+c] = light.cameras[c].lightViewProjectionMatrix;
					}
					fs_shadows.bias_texSize_nearFar[light.shadowMapIndex+c] = Vector4(light.shadowBias, light.mapResolution, light.cameras[c].nearFar.x, light.cameras[c].nearFar.y);
				}
			}else{
				light.shadows = false;
				Log::Warn("There are no shadow maps available for all lights, some light shadow will be disabled");
			}
		}

		fs_lighting.direction_range[i] = Vector4(light.worldDirection.x, light.worldDirection.y, light.worldDirection.z, light.range);
		fs_lighting.color_intensity[i] = Vector4(light.color.x, light.color.y, light.color.z, light.intensity);
		fs_lighting.position_type[i] = Vector4(worldPosition.x, worldPosition.y, worldPosition.z, (float)type);
		fs_lighting.inCon_ouCon_shadows_cascades[i] = Vector4(light.innerConeCos, light.outerConeCos, light.shadowMapIndex, light.numShadowCascades);
		fs_lighting.eyePos = Vector4(cameraTransform.worldPosition.x, cameraTransform.worldPosition.y, cameraTransform.worldPosition.z, 0.0);
	}

	// Setting intensity of other lights to zero
	for (int i = numLights; i < MAX_LIGHTS; i++){
		fs_lighting.color_intensity[i].w = 0.0;
	}

	return true;
}

TextureShaderType RenderSystem::getShadowMapByIndex(int index){
	if (index == 0){
		return TextureShaderType::SHADOWMAP1;
	}else if (index == 1){
		return TextureShaderType::SHADOWMAP2;
	}else if (index == 2){
		return TextureShaderType::SHADOWMAP3;
	}else if (index == 3){
		return TextureShaderType::SHADOWMAP4;
	}else if (index == 4){
		return TextureShaderType::SHADOWMAP5;
	}else if (index == 5){
		return TextureShaderType::SHADOWMAP6;
	}else if (index == 6){
		return TextureShaderType::SHADOWMAP7;
	}else if (index == 7){
		return TextureShaderType::SHADOWMAP8;
	}

	return TextureShaderType::SHADOWMAP1;
}

TextureShaderType RenderSystem::getShadowMapCubeByIndex(int index){
	index -= MAX_SHADOWSMAP;
	if (index == 0){
		return TextureShaderType::SHADOWCUBEMAP1;
	}

	return TextureShaderType::SHADOWCUBEMAP1;
}

void RenderSystem::loadPBRTextures(Material& material, ShaderData& shaderData, ObjectRender& render, bool castShadows){
	TextureRender* textureRender = NULL;
	textureRender = material.baseColorTexture.getRender();
	if (textureRender)
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::BASECOLOR, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);
	else
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::BASECOLOR, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, &emptyWhite);

	if (hasLights){
		TextureRender* textureRender = NULL;
		int slotTex = -1;

		textureRender = material.metallicRoughnessTexture.getRender();
		slotTex = shaderData.getTextureIndex(TextureShaderType::METALLICROUGHNESS, ShaderStageType::FRAGMENT);
		if (textureRender)
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
		else
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

		textureRender = material.normalTexture.getRender();
		slotTex = shaderData.getTextureIndex(TextureShaderType::NORMAL, ShaderStageType::FRAGMENT);
		if (textureRender)
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
		else
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyNormal);

		textureRender = material.occlusionTexture.getRender();
		slotTex = shaderData.getTextureIndex(TextureShaderType::OCCULSION, ShaderStageType::FRAGMENT);
		if (textureRender)
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
		else
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

		textureRender = material.emissiveTexture.getRender();
		slotTex = shaderData.getTextureIndex(TextureShaderType::EMISSIVE, ShaderStageType::FRAGMENT);
		if (textureRender)
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
		else
			render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyBlack);

		if (hasShadows && castShadows){
			size_t num2DShadows = 0;
			size_t numCubeShadows = 0;
			auto lights = scene->getComponentArray<LightComponent>();
			for (int l = 0; l < lights->size(); l++){
				LightComponent& light = lights->getComponentFromIndex(l);
				if (light.shadowMapIndex >= 0){
					if (light.type == LightType::POINT){
						slotTex = shaderData.getTextureIndex(getShadowMapCubeByIndex(light.shadowMapIndex), ShaderStageType::FRAGMENT);
						render.addTexture(slotTex, ShaderStageType::FRAGMENT, &light.framebuffer[0].getColorTexture());
						numCubeShadows++;
					}else if (light.type == LightType::SPOT){
						slotTex = shaderData.getTextureIndex(getShadowMapByIndex(light.shadowMapIndex), ShaderStageType::FRAGMENT);
						render.addTexture(slotTex, ShaderStageType::FRAGMENT, &light.framebuffer[0].getColorTexture());
						num2DShadows++;
					}else if (light.type == LightType::DIRECTIONAL){
						for (int c = 0; c < light.numShadowCascades; c++){
							slotTex = shaderData.getTextureIndex(getShadowMapByIndex(light.shadowMapIndex+c), ShaderStageType::FRAGMENT);
							render.addTexture(slotTex, ShaderStageType::FRAGMENT, &light.framebuffer[c].getColorTexture());
							num2DShadows++;
						}
					}
				}
			}
			if (MAX_SHADOWSMAP > num2DShadows){
				for (int s = num2DShadows; s < MAX_SHADOWSMAP; s++){
					slotTex = shaderData.getTextureIndex(getShadowMapByIndex(s), ShaderStageType::FRAGMENT);
					render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyBlack);
				}
			}
			if (MAX_SHADOWSCUBEMAP > numCubeShadows){
				for (int s = numCubeShadows; s < MAX_SHADOWSCUBEMAP; s++){
					slotTex = shaderData.getTextureIndex(getShadowMapCubeByIndex(s+MAX_SHADOWSMAP), ShaderStageType::FRAGMENT);
					render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyCubeBlack);
				}
			}
		}

	}
}

bool RenderSystem::loadMesh(MeshComponent& mesh){

	bufferNameToRender.clear();

	std::map<std::string, unsigned int> bufferStride;

	for (auto const& buf : mesh.buffers){
		buf.second->getRender()->createBuffer(buf.second->getSize(), buf.second->getData(), buf.second->getType(), buf.second->getUsage());
		bufferNameToRender[buf.first] = buf.second->getRender();
		bufferStride[buf.first] = buf.second->getStride();
	}

	for (int i = 0; i < mesh.numSubmeshes; i++){

		mesh.submeshes[i].hasNormalMap = false;
		mesh.submeshes[i].hasTangent = false;
		mesh.submeshes[i].hasVertexColor = false;

		ObjectRender& render = mesh.submeshes[i].render;

		render.beginLoad(mesh.submeshes[i].primitiveType, false, scene->isRenderToTexture());

		for (auto const& buf : mesh.buffers){
        	if (buf.second->isRenderAttributes()) {
            	for (auto const &attr : buf.second->getAttributes()) {
					if (attr.first == AttributeType::TANGENT){
						mesh.submeshes[i].hasTangent = true;
					}
					if (attr.first == AttributeType::COLOR){
						mesh.submeshes[i].hasVertexColor = true;
					}
            	}
        	}
    	}
		for (auto const& attr : mesh.submeshes[i].attributes){
			if (attr.first == AttributeType::TANGENT){
				mesh.submeshes[i].hasTangent = true;
			}
			if (attr.first == AttributeType::COLOR){
				mesh.submeshes[i].hasVertexColor = true;
			}
		}

		if (!mesh.submeshes[i].material.normalTexture.empty()){
			mesh.submeshes[i].hasNormalMap = true;
		}


		ShaderType shaderType = ShaderType::MESH;

		bool p_unlit = false;
		bool p_punctual = false;
		bool p_hasNormalMap = false;
		bool p_hasNormal = false;
		bool p_hasTangent = false;
		bool p_castShadows = false;
		bool p_shadowsPCF = false;
		bool p_textureRect = false;
		bool p_vertexColorVec4 = false;

		if (hasLights){
			p_punctual = true;

			p_hasNormal = true;
			if (mesh.submeshes[i].hasTangent){
				p_hasTangent = true;
			}
			if (mesh.submeshes[i].hasNormalMap){
				p_hasNormalMap = true;
			}
			if (hasShadows && mesh.castShadows){
				p_castShadows = true;
				if (scene->isShadowsPCF()){
					p_shadowsPCF = true;
				}
			}
		}else{
			p_unlit = true;
		}
		if (mesh.submeshes[i].hasTextureRect){
			p_textureRect = true;
		}
		if (mesh.submeshes[i].hasVertexColor){
			p_vertexColorVec4 = true;
		}

		mesh.submeshes[i].shaderProperties = ShaderPool::getMeshProperties(p_unlit, true, false, p_punctual, p_castShadows, p_shadowsPCF, p_hasNormal, p_hasNormalMap, p_hasTangent, false, p_vertexColorVec4, p_textureRect);
		mesh.submeshes[i].shader = ShaderPool::get(shaderType, mesh.submeshes[i].shaderProperties);
		if (hasShadows && mesh.castShadows){
			mesh.submeshes[i].depthShader = ShaderPool::get(ShaderType::DEPTH, "");
			if (!mesh.submeshes[i].depthShader->isCreated())
				return false;
		}
		if (!mesh.submeshes[i].shader->isCreated())
			return false;
		render.addShader(mesh.submeshes[i].shader.get());
		ShaderData& shaderData = mesh.submeshes[i].shader.get()->shaderData;

		mesh.submeshes[i].slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::PBR_VS_PARAMS, ShaderStageType::VERTEX);
		mesh.submeshes[i].slotFSParams = shaderData.getUniformBlockIndex(UniformBlockType::PBR_FS_PARAMS, ShaderStageType::FRAGMENT);
		if (hasLights){
			mesh.submeshes[i].slotFSLighting = shaderData.getUniformBlockIndex(UniformBlockType::FS_LIGHTING, ShaderStageType::FRAGMENT);
			if (hasShadows && mesh.castShadows){
				mesh.submeshes[i].slotVSShadows = shaderData.getUniformBlockIndex(UniformBlockType::VS_SHADOWS, ShaderStageType::VERTEX);
				mesh.submeshes[i].slotFSShadows = shaderData.getUniformBlockIndex(UniformBlockType::FS_SHADOWS, ShaderStageType::FRAGMENT);
			}
		}
		if (mesh.submeshes[i].hasTextureRect){
			mesh.submeshes[i].slotVSSprite = shaderData.getUniformBlockIndex(UniformBlockType::SPRITE_VS_PARAMS, ShaderStageType::VERTEX);
		}

		loadPBRTextures(mesh.submeshes[i].material, shaderData, mesh.submeshes[i].render, mesh.castShadows);

		mesh.submeshes[i].needUpdateTexture = false;

		if (Engine::isAutomaticTransparency() && !mesh.transparency){
			if (mesh.submeshes[i].material.baseColorTexture.getData().isTransparent() || mesh.submeshes[i].material.baseColorFactor.w != 1.0){
				mesh.transparency = true;
			}
		}
	
		unsigned int vertexBufferCount = 0;
		unsigned int indexCount = 0;

		for (auto const& buf : mesh.buffers){
        	if (buf.first == mesh.defaultBuffer) {
				vertexBufferCount = buf.second->getCount();
        	}
        	if (buf.second->isRenderAttributes()) {
            	for (auto const &attr : buf.second->getAttributes()) {
					render.addAttribute(shaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized());
            	}
        	}
			if (buf.second->getType() == BufferType::INDEX_BUFFER){
				indexCount = buf.second->getCount();
				Attribute indexattr = buf.second->getAttributes()[AttributeType::INDEX];
				render.addIndex(buf.second->getRender(), indexattr.getDataType(), indexattr.getOffset());
			}
    	}

		for (auto const& attr : mesh.submeshes[i].attributes){
			if (attr.first == AttributeType::INDEX){
				indexCount = attr.second.getCount();
				render.addIndex(bufferNameToRender[attr.second.getBuffer()], attr.second.getDataType(), attr.second.getOffset());
			}else{
				render.addAttribute(shaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
			}
		}

		if (indexCount > 0){
			mesh.submeshes[i].vertexCount = indexCount;
		}else{
			mesh.submeshes[i].vertexCount = vertexBufferCount;
		}

		render.endLoad();

		//----------Start depth shader---------------
		if (hasShadows && mesh.castShadows){
			ObjectRender& depthRender = mesh.submeshes[i].depthRender;

			depthRender.beginLoad(mesh.submeshes[i].primitiveType, true, scene->isRenderToTexture());

			depthRender.addShader(mesh.submeshes[i].depthShader.get());
			ShaderData& depthShaderData = mesh.submeshes[i].depthShader.get()->shaderData;

			mesh.submeshes[i].slotVSDepthParams = depthShaderData.getUniformBlockIndex(UniformBlockType::DEPTH_VS_PARAMS, ShaderStageType::VERTEX);

			for (auto const& buf : mesh.buffers){
        		if (buf.second->isRenderAttributes()) {
					if (buf.second->getAttributes().count(AttributeType::POSITION)){
						Attribute posattr = buf.second->getAttributes()[AttributeType::POSITION];
						depthRender.addAttribute(depthShaderData.getAttrIndex(AttributeType::POSITION), buf.second->getRender(), posattr.getElements(), posattr.getDataType(), buf.second->getStride(), posattr.getOffset(), posattr.getNormalized());
					}
        		}
				if (buf.second->getType() == BufferType::INDEX_BUFFER){
					indexCount = buf.second->getCount();
					Attribute indexattr = buf.second->getAttributes()[AttributeType::INDEX];
					depthRender.addIndex(buf.second->getRender(), indexattr.getDataType(), indexattr.getOffset());
				}
    		}

			for (auto const& attr : mesh.submeshes[i].attributes){
				if (attr.first == AttributeType::INDEX){
					depthRender.addIndex(bufferNameToRender[attr.second.getBuffer()], attr.second.getDataType(), attr.second.getOffset());
				}else if (attr.first == AttributeType::POSITION){
					depthRender.addAttribute(depthShaderData.getAttrIndex(AttributeType::POSITION), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
				}
			}

			depthRender.endLoad();
		}
		//----------End depth shader---------------
	}

	mesh.needReload = false;
	mesh.loaded = true;

	return true;
}

void RenderSystem::drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform){
	if (mesh.loaded){

		if (mesh.needUpdateBuffer){
			for (auto const& buf : mesh.buffers){
				if (buf.second->getUsage() != BufferUsage::IMMUTABLE)
					buf.second->getRender()->updateBuffer(buf.second->getSize(), buf.second->getData());
			}

			mesh.needUpdateBuffer = false;
		}

		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender& render = mesh.submeshes[i].render;

			if (mesh.submeshes[i].needUpdateTexture){
				ShaderData& shaderData = mesh.submeshes[i].shader.get()->shaderData;
				loadPBRTextures(mesh.submeshes[i].material, shaderData, mesh.submeshes[i].render, mesh.castShadows);

				mesh.submeshes[i].needUpdateTexture = false;
			}

			render.beginDraw();

			if (hasLights){
				render.applyUniformBlock(mesh.submeshes[i].slotFSLighting, ShaderStageType::FRAGMENT, sizeof(float) * (16 * MAX_LIGHTS + 4), &fs_lighting);
				if (hasShadows && mesh.castShadows){
					render.applyUniformBlock(mesh.submeshes[i].slotVSShadows, ShaderStageType::VERTEX, sizeof(float) * (16 * MAX_SHADOWSMAP), &vs_shadows);
					render.applyUniformBlock(mesh.submeshes[i].slotFSShadows, ShaderStageType::FRAGMENT, sizeof(float) * (4 * (MAX_SHADOWSMAP + MAX_SHADOWSCUBEMAP)), &fs_shadows);
				}
			}

			if (mesh.submeshes[i].hasTextureRect){
				render.applyUniformBlock(mesh.submeshes[i].slotVSSprite, ShaderStageType::VERTEX, sizeof(float) * 4, &mesh.submeshes[i].textureRect);
			}
			size_t teste = sizeof(float);
			render.applyUniformBlock(mesh.submeshes[i].slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 16, &mesh.submeshes[i].material);

			//model, normal and mvp matrix
			render.applyUniformBlock(mesh.submeshes[i].slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 48, &transform.modelMatrix);

			render.draw(mesh.submeshes[i].vertexCount);
		}
	}
}

void RenderSystem::drawMeshDepth(MeshComponent& mesh, Matrix4 modelLightSpaceMatrix){
	if (mesh.loaded && mesh.castShadows){
		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender& depthRender = mesh.submeshes[i].depthRender;

			depthRender.beginDraw();

			//mvp matrix
			depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthParams, ShaderStageType::VERTEX, sizeof(float) * 16, &modelLightSpaceMatrix);

			depthRender.draw(mesh.submeshes[i].vertexCount);
		}
	}
}

void RenderSystem::destroyMesh(MeshComponent& mesh){
	for (int i = 0; i < mesh.numSubmeshes; i++){

		Submesh& submesh = mesh.submeshes[i];

		//Destroy shader
		submesh.shader.reset();
		ShaderPool::remove(ShaderType::MESH, mesh.submeshes[i].shaderProperties);
		if (hasShadows && mesh.castShadows)
			ShaderPool::remove(ShaderType::DEPTH, "");

		//Destroy texture
		submesh.material.baseColorTexture.destroy();
		submesh.material.metallicRoughnessTexture.destroy();
		submesh.material.normalTexture.destroy();
		submesh.material.occlusionTexture.destroy();
		submesh.material.emissiveTexture.destroy();

		//Destroy render
		submesh.render.destroy();

		//Shaders uniforms
		submesh.slotVSParams = -1;
		submesh.slotFSParams = -1;
		submesh.slotFSLighting = -1;
		submesh.slotVSSprite = -1;
		submesh.slotVSShadows = -1;
		submesh.slotFSShadows = -1;
		submesh.slotVSDepthParams = -1;
	}

	//Destroy buffers
	for (auto const& buf : mesh.buffers){
		buf.second->getRender()->destroyBuffer();
	}

	mesh.loaded = false;
}

bool RenderSystem::loadUI(UIComponent& ui, bool isText){
	ObjectRender& render = ui.render;

	render.beginLoad(ui.primitiveType, false, scene->isRenderToTexture());

	TextureRender* textureRender = ui.texture.getRender();

	bool p_hasTexture = false;
	bool p_vertexColorVec4 = true;
	bool p_hasFontAtlasTexture = false;
	if (textureRender){
		if (isText){
			p_vertexColorVec4 = false;
			p_hasFontAtlasTexture = true;
		}else{
			p_hasTexture = true;
		}
	}
	
	ShaderType shaderType = ShaderType::UI;

	ui.shaderProperties = ShaderPool::getUIProperties(p_hasTexture, p_hasFontAtlasTexture, false, p_vertexColorVec4);
	ui.shader = ShaderPool::get(shaderType, ui.shaderProperties);
	if (!ui.shader->isCreated())
		return false;
	render.addShader(ui.shader.get());
	ShaderData& shaderData = ui.shader.get()->shaderData;

	ui.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::UI_VS_PARAMS, ShaderStageType::VERTEX);
	ui.slotFSParams = shaderData.getUniformBlockIndex(UniformBlockType::UI_FS_PARAMS, ShaderStageType::FRAGMENT);

	size_t bufferSize;
	size_t minBufferSize;

	bufferSize = ui.buffer->getSize();
	minBufferSize = ui.minBufferCount * ui.buffer->getStride();
	if (minBufferSize > bufferSize)
		bufferSize = minBufferSize;

	ui.buffer->getRender()->createBuffer(bufferSize, ui.buffer->getData(), ui.buffer->getType(), ui.buffer->getUsage());
	if (ui.buffer->isRenderAttributes()) {
        for (auto const &attr : ui.buffer->getAttributes()) {
			render.addAttribute(shaderData.getAttrIndex(attr.first), ui.buffer->getRender(), attr.second.getElements(), attr.second.getDataType(), ui.buffer->getStride(), attr.second.getOffset(), attr.second.getNormalized());
        }
    }

	bufferSize = ui.indices->getSize();
	minBufferSize = ui.minIndicesCount * ui.indices->getStride();
	if (minBufferSize > bufferSize)
		bufferSize = minBufferSize;

	if (ui.indices->getCount() > 0){
		ui.indices->getRender()->createBuffer(bufferSize, ui.indices->getData(), ui.indices->getType(), ui.indices->getUsage());
		ui.vertexCount = ui.indices->getCount();
		Attribute indexattr = ui.indices->getAttributes()[AttributeType::INDEX];
		render.addIndex(ui.indices->getRender(), indexattr.getDataType(), indexattr.getOffset());
	}else{
		ui.vertexCount = ui.buffer->getCount();
	}

	if (textureRender)
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::UI, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);
	
	ui.needUpdateTexture = false;

	render.endLoad();

	ui.needReload = false;
	ui.loaded = true;

	return true;
}

void RenderSystem::drawUI(UIComponent& ui, Transform& transform){
	if (ui.loaded && ui.buffer && ui.buffer->getSize() > 0){

		if (ui.needUpdateTexture){
			ShaderData& shaderData = ui.shader.get()->shaderData;
			TextureRender* textureRender = ui.texture.getRender();
			if (textureRender)
				ui.render.addTexture(shaderData.getTextureIndex(TextureShaderType::UI, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			ui.needUpdateTexture = false;
		}

		if (ui.needUpdateBuffer){
			ui.buffer->getRender()->updateBuffer(ui.buffer->getSize(), ui.buffer->getData());

			if (ui.indices->getCount() > 0){
				ui.indices->getRender()->updateBuffer(ui.indices->getSize(), ui.indices->getData());
				ui.vertexCount = ui.indices->getCount();
			}else{
				ui.vertexCount = ui.buffer->getCount();
			}

			ui.needUpdateBuffer = false;
		}

		ObjectRender& render = ui.render;

		render.beginDraw();
		render.applyUniformBlock(ui.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &transform.modelViewProjectionMatrix);
		//Color
		render.applyUniformBlock(ui.slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 4, &ui.color);
		render.draw(ui.vertexCount);

	}
}

void RenderSystem::destroyUI(UIComponent& ui){
	//Destroy shader
	ui.shader.reset();
	ShaderPool::remove(ShaderType::UI, ui.shaderProperties);

	//Destroy texture
	ui.texture.destroy();

	//Destroy render
	ui.render.destroy();

	//Destroy buffer
	if (ui.buffer){
		ui.buffer->getRender()->destroyBuffer();
	}
	if (ui.indices){
		ui.indices->getRender()->destroyBuffer();
	}

	//Shaders uniforms
	ui.slotVSParams = -1;
	ui.slotFSParams = -1;

	ui.loaded = false;
}

bool RenderSystem::loadParticles(ParticlesComponent& particles){
	ObjectRender& render = particles.render;

	render.beginLoad(PrimitiveType::POINTS, false, scene->isRenderToTexture());

	TextureRender* textureRender = particles.texture.getRender();

	if (Engine::isAutomaticTransparency() && !particles.transparency){
		if (particles.texture.getData().isTransparent()){ // Particle color is not tested here
			particles.transparency = true;
		}
	}

	bool p_hasTexture = false;
	bool p_hasTextureRect = false;
	if (textureRender){
		p_hasTexture = true;
		if (particles.hasTextureRect){
			p_hasTextureRect = true;
		}
	}

	ShaderType shaderType = ShaderType::POINTS;

	particles.shaderProperties = ShaderPool::getPointsProperties(p_hasTexture, false, true, p_hasTextureRect);
	particles.shader = ShaderPool::get(shaderType, particles.shaderProperties);
	if (!particles.shader->isCreated())
		return false;
	render.addShader(particles.shader.get());
	ShaderData& shaderData = particles.shader.get()->shaderData;

	particles.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::POINTS_VS_PARAMS, ShaderStageType::VERTEX);

	// Now buffer size is zero than it needed to be calculated
	size_t bufferSize = particles.maxParticles * particles.buffer->getStride();

	particles.buffer->getRender()->createBuffer(bufferSize, particles.buffer->getData(), particles.buffer->getType(), particles.buffer->getUsage());
	if (particles.buffer->isRenderAttributes()) {
        for (auto const &attr : particles.buffer->getAttributes()) {
			render.addAttribute(shaderData.getAttrIndex(attr.first), particles.buffer->getRender(), attr.second.getElements(), attr.second.getDataType(), particles.buffer->getStride(), attr.second.getOffset(), attr.second.getNormalized());
        }
    }

	if (textureRender)
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::POINTS, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

	particles.needUpdateTexture = false;

	render.endLoad();

	particles.needReload = false;
	particles.loaded = true;

	return true;
}

void RenderSystem::drawParticles(ParticlesComponent& particles, Transform& transform, Transform& camTransform){
	if (particles.loaded && particles.buffer && particles.buffer->getSize() > 0){

		if (particles.needUpdateTexture){
			ShaderData& shaderData = particles.shader.get()->shaderData;
			TextureRender* textureRender = particles.texture.getRender();
			if (textureRender)
				particles.render.addTexture(shaderData.getTextureIndex(TextureShaderType::POINTS, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			particles.needUpdateTexture = false;
		}

		if (particles.needUpdateBuffer){
			particles.buffer->getRender()->updateBuffer(particles.buffer->getSize(), particles.buffer->getData());
			particles.needUpdateBuffer = false;
		}

		ObjectRender& render = particles.render;

		render.beginDraw();
		render.applyUniformBlock(particles.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &transform.modelViewProjectionMatrix);
		render.draw(particles.particles.size());
	}
}

void RenderSystem::destroyParticles(ParticlesComponent& particles){
	//Destroy shader
	particles.shader.reset();
	ShaderPool::remove(ShaderType::POINTS, particles.shaderProperties);

	//Destroy texture
	particles.texture.destroy();

	//Destroy render
	particles.render.destroy();

	//Destroy buffer
	if (particles.buffer){
		particles.buffer->getRender()->destroyBuffer();
	}

	//Shaders uniforms
	particles.slotVSParams = -1;

	particles.loaded = false;
}

bool RenderSystem::loadSky(SkyComponent& sky){

	ObjectRender* render = &sky.render;

	render->beginLoad(PrimitiveType::TRIANGLES, false, scene->isRenderToTexture());

	ShaderType shaderType = ShaderType::SKYBOX;

	sky.shader = ShaderPool::get(shaderType, "");
	if (!sky.shader->isCreated())
		return false;
	render->addShader(sky.shader.get());
	ShaderData& shaderData = sky.shader.get()->shaderData;

	sky.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::VIEWPROJECTIONSKY, ShaderStageType::VERTEX);

	TextureRender* textureRender = sky.texture.getRender();
	if (textureRender){
		render->addTexture(shaderData.getTextureIndex(TextureShaderType::SKYCUBE, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);
	} else {
		return false;
	}

	sky.needUpdateTexture = false;

	sky.buffer->getRender()->createBuffer(sky.buffer->getSize(), sky.buffer->getData(), sky.buffer->getType(), sky.buffer->getUsage());

	if (sky.buffer->isRenderAttributes()) {
        for (auto const &attr : sky.buffer->getAttributes()) {
			render->addAttribute(shaderData.getAttrIndex(attr.first), sky.buffer->getRender(), attr.second.getElements(), attr.second.getDataType(), sky.buffer->getStride(), attr.second.getOffset(), attr.second.getNormalized());
        }
    }

	render->endLoad();

	sky.loaded = true;

	return true;
}

void RenderSystem::drawSky(SkyComponent& sky){
	if (sky.loaded){

		if (sky.needUpdateTexture){
			ShaderData& shaderData = sky.shader.get()->shaderData;
			TextureRender* textureRender = sky.texture.getRender();
			if (textureRender)
				sky.render.addTexture(shaderData.getTextureIndex(TextureShaderType::SKYCUBE, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			sky.needUpdateTexture = false;
		}

		ObjectRender& render = sky.render;

		render.beginDraw();
		render.applyUniformBlock(sky.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &sky.skyViewProjectionMatrix);
		render.draw(36);
	}
}

void RenderSystem::destroySky(SkyComponent& sky){
	//Destroy shader
	sky.shader.reset();
	ShaderPool::remove(ShaderType::SKYBOX, "");

	//Destroy texture
	sky.texture.destroy();

	//Destroy render
	sky.render.destroy();

	//Destroy buffer
	if (sky.buffer){
		sky.buffer->getRender()->destroyBuffer();
	}

	//Shaders uniforms
	sky.slotVSParams = -1;

	sky.loaded = false;
}

Rect RenderSystem::getScissorRect(UIComponent& ui, ImageComponent& img, Transform& transform, CameraComponent& camera){
	int objScreenPosX = 0;
	int objScreenPosY = 0;
	int objScreenWidth = 0;
	int objScreenHeight = 0;

	if (!scene->isRenderToTexture()) {

		float scaleX = transform.worldScale.x;
		float scaleY = transform.worldScale.y;

		float tempX = (2 * transform.worldPosition.x / (float) Engine::getCanvasWidth()) - 1;
		float tempY = (2 * transform.worldPosition.y / (float) Engine::getCanvasHeight()) - 1;

		float widthRatio = scaleX * (Engine::getViewRect().getWidth() / (float) Engine::getCanvasWidth());
		float heightRatio = scaleY * (Engine::getViewRect().getHeight() / (float) Engine::getCanvasHeight());

		objScreenPosX = (tempX * Engine::getViewRect().getWidth() + (float) System::instance().getScreenWidth()) / 2;
		objScreenPosY = (tempY * Engine::getViewRect().getHeight() + (float) System::instance().getScreenHeight()) / 2;
		objScreenWidth = ui.width * widthRatio;
		objScreenHeight = ui.height * heightRatio;

		if (camera.type == CameraType::CAMERA_2D)
			objScreenPosY = (float) System::instance().getScreenHeight() - objScreenHeight - objScreenPosY;

		if (!(img.patchMarginLeft == 0 && img.patchMarginTop == 0 && img.patchMarginRight == 0 && img.patchMarginBottom == 0)) {
			float borderScreenLeft = img.patchMarginLeft * widthRatio;
			float borderScreenTop = img.patchMarginTop * heightRatio;
			float borderScreenRight = img.patchMarginRight * widthRatio;
			float borderScreenBottom = img.patchMarginBottom * heightRatio;

			objScreenPosX += borderScreenLeft;
			objScreenPosY += borderScreenTop;
			objScreenWidth -= (borderScreenLeft + borderScreenRight);
			objScreenHeight -= (borderScreenTop + borderScreenBottom);
		}

	}else {

		objScreenPosX = transform.worldPosition.x;
		objScreenPosY = transform.worldPosition.y;
		objScreenWidth = ui.width;
		objScreenHeight = ui.height;

		if (camera.type == CameraType::CAMERA_2D)
			objScreenPosY = (float) scene->getFramebufferHeight() - objScreenHeight - objScreenPosY;

		if (!(img.patchMarginLeft == 0 && img.patchMarginTop == 0 && img.patchMarginRight == 0 && img.patchMarginBottom == 0)) {
			float borderScreenLeft = img.patchMarginLeft;
			float borderScreenTop = img.patchMarginTop;
			float borderScreenRight = img.patchMarginRight;
			float borderScreenBottom = img.patchMarginBottom;

			objScreenPosX += borderScreenLeft;
			objScreenPosY += borderScreenTop;
			objScreenWidth -= (borderScreenLeft + borderScreenRight);
			objScreenHeight -= (borderScreenTop + borderScreenBottom);
		}

	}


	return Rect(objScreenPosX, objScreenPosY, objScreenWidth, objScreenHeight);
}

void RenderSystem::updateTransform(Transform& transform){
	Matrix4 scaleMatrix = Matrix4::scaleMatrix(transform.scale);
    Matrix4 translateMatrix = Matrix4::translateMatrix(transform.position);
    Matrix4 rotationMatrix = transform.rotation.getRotationMatrix();

	transform.modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;

	if (transform.parent != NULL_ENTITY){
		auto transformParent = scene->getComponent<Transform>(transform.parent);

		transform.modelMatrix = transformParent.modelMatrix * transform.modelMatrix;

    	transform.worldRotation = transformParent.worldRotation * transform.rotation;
		transform.worldScale = transformParent.worldScale * transform.scale;
    	transform.worldPosition = transform.modelMatrix * Vector3(0,0,0);
	}else{
		transform.worldRotation = transform.rotation;
    	transform.worldScale = transform.scale;
    	transform.worldPosition = transform.position;
	}

	if (hasLights){
		transform.normalMatrix = transform.modelMatrix.inverse().transpose();
	}
}

void RenderSystem::updateCamera(CameraComponent& camera, Transform& transform){
	//Update ProjectionMatrix
	if (camera.type == CameraType::CAMERA_2D){
		camera.projectionMatrix = Matrix4::orthoMatrix(camera.left, camera.right, camera.top, camera.bottom, camera.orthoNear, camera.orthoFar);
	}else if (camera.type == CameraType::CAMERA_ORTHO) {
		camera.projectionMatrix = Matrix4::orthoMatrix(camera.left, camera.right, camera.bottom, camera.top, camera.orthoNear, camera.orthoFar);
	}else if (camera.type == CameraType::CAMERA_PERSPECTIVE){
		camera.projectionMatrix = Matrix4::perspectiveMatrix(camera.y_fov, camera.aspect, camera.perspectiveNear, camera.perspectiveFar);
	}

	if (transform.parent != NULL_ENTITY){
        camera.worldView = transform.modelMatrix * (camera.view - transform.position);
        camera.worldUp = ((transform.modelMatrix * camera.up) - (transform.modelMatrix * Vector3(0,0,0))).normalize();
    }else{
        camera.worldView = camera.view;
        camera.worldUp = camera.up;
    }

	//Update ViewMatrix
	if (camera.type == CameraType::CAMERA_2D){
        camera.viewMatrix.identity();
    }else{
        camera.viewMatrix = Matrix4::lookAtMatrix(transform.worldPosition, camera.worldView, camera.worldUp);
    }

	camera.worldRight = Vector3(camera.viewMatrix[0][0], camera.viewMatrix[1][0], camera.viewMatrix[2][0]);

	//Update ViewProjectionMatrix
	camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
}

void RenderSystem::updateSkyViewProjection(CameraComponent& camera){
	SkyComponent* sky = scene->findComponentFromIndex<SkyComponent>(0);
	if (sky){
		Matrix4 skyViewMatrix = camera.viewMatrix;

		skyViewMatrix.set(3,0,0);
    	skyViewMatrix.set(3,1,0);
    	skyViewMatrix.set(3,2,0);
    	skyViewMatrix.set(3,3,1);
    	skyViewMatrix.set(2,3,0);
    	skyViewMatrix.set(1,3,0);
    	skyViewMatrix.set(0,3,0);

		sky->skyViewProjectionMatrix = camera.projectionMatrix * skyViewMatrix;
	}
}

void RenderSystem::updateParticles(ParticlesComponent& particles, Transform& transform, CameraComponent& camera, Transform& camTransform){
	particles.shaderParticles.clear();
	particles.shaderParticles.reserve(particles.particles.size());

	particles.numVisible = 0;
	size_t particlesSize = (particles.particles.size() < particles.maxParticles)? particles.particles.size() : particles.maxParticles;
	for (int i = 0; i < particlesSize; i++){
		if (particles.particles[i].life > particles.particles[i].time){
			particles.shaderParticles.push_back({});
			particles.shaderParticles[particles.numVisible].position = particles.particles[i].position;
			particles.shaderParticles[particles.numVisible].color = particles.particles[i].color;
			particles.shaderParticles[particles.numVisible].size = particles.particles[i].size;
			particles.shaderParticles[particles.numVisible].rotation = particles.particles[i].rotation;
			particles.shaderParticles[particles.numVisible].textureRect = particles.particles[i].textureRect;
			particles.numVisible++;
		}
	}

	if (particles.transparency && camera.type != CameraType::CAMERA_2D){
		auto comparePoints = [&transform, &camTransform](const ParticleShaderData& a, const ParticleShaderData& b) -> bool {
			float distanceToCameraA = (camTransform.worldPosition - (transform.modelMatrix * a.position)).length();
			float distanceToCameraB = (camTransform.worldPosition - (transform.modelMatrix * b.position)).length();
			return distanceToCameraA > distanceToCameraB;
		};
		std::sort(particles.shaderParticles.begin(), particles.shaderParticles.end(), comparePoints);
	}

	if (particles.numVisible > 0){
		((ExternalBuffer*)particles.buffer)->setData((unsigned char*)(&particles.shaderParticles.at(0)), sizeof(ParticleShaderData)*particles.numVisible);
	}else{
		((ExternalBuffer*)particles.buffer)->setData((unsigned char*)nullptr, 0);
	}
	particles.needUpdateBuffer = true;
}

void RenderSystem::configureLightShadowNearFar(LightComponent& light, const CameraComponent& camera){
	if (light.shadowCameraNearFar.x == 0.0){
		if (camera.type == CameraType::CAMERA_PERSPECTIVE)
			light.shadowCameraNearFar.x = camera.perspectiveNear;
		else
			light.shadowCameraNearFar.x = camera.orthoNear;
	}
	if (light.shadowCameraNearFar.y == 0.0){
		if (light.range == 0.0){
			if (camera.type == CameraType::CAMERA_PERSPECTIVE)
				light.shadowCameraNearFar.y = camera.perspectiveFar;
			else
				light.shadowCameraNearFar.y = camera.orthoFar;
		}else{
			light.shadowCameraNearFar.y = light.range;
		}
	}
}

float RenderSystem::lerp(float a, float b, float fraction) {
    return (a * (1.0f - fraction)) + (b * fraction);
}

Matrix4 RenderSystem::getDirLightProjection(const Matrix4& viewMatrix, const Matrix4& sceneCameraInv){
	Matrix4 t = viewMatrix * sceneCameraInv;
	std::vector<Vector4> v = {
			t * Vector4(-1.f, 1.f, -1.f, 1.f),
			t * Vector4(1.f, 1.f, -1.f, 1.f),
			t * Vector4(1.f, -1.f, -1.f, 1.f),
			t * Vector4(-1.f, -1.f, -1.f, 1.f),
			t * Vector4(-1.f, 1.f, 1.f, 1.f),
			t * Vector4(1.f, 1.f, 1.f, 1.f),
			t * Vector4(1.f, -1.f, 1.f, 1.f),
			t * Vector4(-1.f, -1.f, 1.f, 1.f)
	};

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();

	for (auto& p : v){
		p = p / p.w;

		if (p.x < minX) minX = p.x;
		if (p.x > maxX) maxX = p.x;
		if (p.y < minY) minY = p.y;
		if (p.y > maxY) maxY = p.y;
		if (p.z < minZ) minZ = p.z;
		if (p.z > maxZ) maxZ = p.z;
	}

	return Matrix4::orthoMatrix(minX, maxX, minY, maxY, -maxZ, -minZ);
}

void RenderSystem::updateLightFromTransform(LightComponent& light, Transform& transform){
	light.worldDirection = transform.worldRotation * light.direction;

	if (hasShadows && (light.intensity > 0)){
		
		Vector3 up = Vector3(0, 1, 0);
		if (light.worldDirection.crossProduct(up) == Vector3::ZERO){
			up = Vector3(0, 0, 1);
		}

		CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());

		//TODO: perspective aspect based on shadow map size
		if (light.type == LightType::DIRECTIONAL){
			
			float shadowSplitLogFactor = .7f;

			Matrix4 projectionMatrix[MAX_SHADOWCASCADES];
			Matrix4 viewMatrix;

			viewMatrix = Matrix4::lookAtMatrix(transform.worldPosition, light.worldDirection + transform.worldPosition, up);

			if (camera.type == CameraType::CAMERA_PERSPECTIVE) {

				float zFar = camera.perspectiveFar;
				float zNear = camera.perspectiveNear;
				float fov = 0;
				float ratio = 1;

				float farPlaneOffset = (zFar - zNear) * 0.005;

				std::vector<float> splitFar;
				std::vector<float> splitNear;

				// Split perspective frustrum to create cascades
                Matrix4 projection = camera.projectionMatrix;
                Matrix4 invProjection = projection.inverse();
                std::vector<Vector4> v1 = {
                    invProjection * Vector4(-1.f, 1.f, -1.f, 1.f),
                    invProjection * Vector4(1.f, 1.f, -1.f, 1.f),
                    invProjection * Vector4(1.f, -1.f, -1.f, 1.f),
                    invProjection * Vector4(-1.f, -1.f, -1.f, 1.f),
                    invProjection * Vector4(-1.f, 1.f, 1.f, 1.f),
                    invProjection * Vector4(1.f, 1.f, 1.f, 1.f),
                    invProjection * Vector4(1.f, -1.f, 1.f, 1.f),
                    invProjection * Vector4(-1.f, -1.f, 1.f, 1.f)
                };

                zFar = std::min(zFar, -(v1[4] / v1[4].w).z);
                zNear = -(v1[0] / v1[0].w).z;
                fov = atanf(1.f / projection[1][1]) * 2.f;
                ratio = projection[1][1] / projection[0][0];

				splitFar.resize(light.numShadowCascades);
				splitNear.resize(light.numShadowCascades);
				splitNear[0] = zNear;
				splitFar[light.numShadowCascades - 1] = zFar;
                float j = 1.f;
                for (auto i = 0u; i < light.numShadowCascades - 1; ++i, j+= 1.f){
                    splitFar[i] = lerp(
                            zNear + (j / (float)light.numShadowCascades) * (zFar - zNear),
                            zNear * powf(zFar / zNear, j / (float)light.numShadowCascades),
                            shadowSplitLogFactor
                    );
                    splitNear[i + 1] = splitFar[i];
                }

				// Get frustrum box and create light ortho
				for (int ca = 0; ca < light.numShadowCascades; ca++) {
					Matrix4 sceneCameraInv = (Matrix4::perspectiveMatrix(fov, ratio, splitNear[ca], splitFar[ca]+farPlaneOffset) * camera.viewMatrix).inverse();

					projectionMatrix[ca] = getDirLightProjection(viewMatrix, sceneCameraInv);

					light.cameras[ca].lightViewProjectionMatrix = projectionMatrix[ca] * viewMatrix;
					light.cameras[ca].nearFar = Vector2(splitNear[ca], splitFar[ca]);
				}
				
			} else {

				if (light.numShadowCascades > 1){
					light.numShadowCascades = 1;
					Log::Warn("Can not have multiple cascades shadows when using ortho scene camera. Reducing num shadow cascades to 1");
				}

				Matrix4 sceneCameraInv = camera.viewProjectionMatrix.inverse();

				projectionMatrix[0] = getDirLightProjection(viewMatrix, sceneCameraInv);

				light.cameras[0].lightViewProjectionMatrix = projectionMatrix[0] * viewMatrix;
				light.cameras[0].nearFar = Vector2(-1, 1);
				
			}

		}else if (light.type == LightType::SPOT){
			Matrix4 projectionMatrix;
			Matrix4 viewMatrix;

			configureLightShadowNearFar(light, camera);

			viewMatrix = Matrix4::lookAtMatrix(transform.worldPosition, light.worldDirection + transform.worldPosition, up);

			projectionMatrix = Matrix4::perspectiveMatrix(acos(light.outerConeCos)*2, 1, light.shadowCameraNearFar.x, light.shadowCameraNearFar.y);

			light.cameras[0].lightViewProjectionMatrix = projectionMatrix * viewMatrix;
			light.cameras[0].nearFar = light.shadowCameraNearFar;
		}else if (light.type == LightType::POINT){
			Matrix4 projectionMatrix;
			Matrix4 viewMatrix[6];

			configureLightShadowNearFar(light, camera);

			viewMatrix[0] = Matrix4::lookAtMatrix(transform.worldPosition, Vector3( 1.f, 0.f, 0.f) + transform.worldPosition, Vector3(0.f, -1.f, 0.f));
			viewMatrix[1] = Matrix4::lookAtMatrix(transform.worldPosition, Vector3(-1.f, 0.f, 0.f) + transform.worldPosition, Vector3(0.f, -1.f, 0.f));
			viewMatrix[2] = Matrix4::lookAtMatrix(transform.worldPosition, Vector3( 0.f, 1.f, 0.f) + transform.worldPosition, Vector3(0.f,  0.f, 1.f));
			viewMatrix[3] = Matrix4::lookAtMatrix(transform.worldPosition, Vector3( 0.f,-1.f, 0.f) + transform.worldPosition, Vector3(0.f,  0.f,-1.f));
			viewMatrix[4] = Matrix4::lookAtMatrix(transform.worldPosition, Vector3( 0.f, 0.f, 1.f) + transform.worldPosition, Vector3(0.f, -1.f, 0.f));
			viewMatrix[5] = Matrix4::lookAtMatrix(transform.worldPosition, Vector3( 0.f, 0.f,-1.f) + transform.worldPosition, Vector3(0.f, -1.f, 0.f));

			projectionMatrix = Matrix4::perspectiveMatrix(Angle::degToRad(90), 1, light.shadowCameraNearFar.x, light.shadowCameraNearFar.y);

			Vector2 calculedNearFar;
			float nfsub = light.shadowCameraNearFar.y - light.shadowCameraNearFar.x;
			calculedNearFar.x = (light.shadowCameraNearFar.y + light.shadowCameraNearFar.x) / nfsub * 0.5f + 0.5f;
			calculedNearFar.y =-(light.shadowCameraNearFar.y * light.shadowCameraNearFar.x) / nfsub;

			for (int f = 0; f < 6; f++){
				light.cameras[f].lightViewProjectionMatrix = projectionMatrix * viewMatrix[f];
				light.cameras[f].nearFar = calculedNearFar;
			}
		}
		
		
	}
}

void RenderSystem::update(double dt){
	CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());
	Transform& cameraTransform =  scene->getComponent<Transform>(scene->getCamera());

	if (cameraTransform.needUpdate){
		camera.needUpdate = true;
	}

	std::vector<Entity> parentList;
	auto transforms = scene->getComponentArray<Transform>();

	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);

		// Finding childs
		if (std::find(parentList.begin(), parentList.end(), transform.parent) != parentList.end()){
			transform.needUpdate = true;
		}

		if (transform.needUpdate){
			Entity entity = transforms->getEntity(i);
			parentList.push_back(entity);
			updateTransform(transform);
		}
	}

	if (camera.needUpdate){
		updateCamera(camera, cameraTransform);
		updateSkyViewProjection(camera);
	}

	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);

		Entity entity = transforms->getEntity(i);
		Signature signature = scene->getSignature(entity);

		if (camera.needUpdate || transform.needUpdate){

			bool usingFakeBillboard = false;

			if (signature.test(scene->getComponentType<SpriteComponent>())){
				SpriteComponent& sprite = scene->getComponent<SpriteComponent>(entity);

				if (sprite.billboard && !sprite.fakeBillboard){

					Vector3 camPos = cameraTransform.worldPosition;

					if (sprite.cylindricalBillboard)
						camPos.y = transform.worldPosition.y;

					Matrix4 m1 = Matrix4::lookAtMatrix(camPos, transform.worldPosition, camera.worldUp);

					Quaternion oldRotation = transform.rotation;

					transform.rotation.fromRotationMatrix(m1);
					if (transform.parent != NULL_ENTITY){
						auto transformParent = scene->getComponent<Transform>(transform.parent);
						transform.rotation = transformParent.worldRotation.inverse() * transform.rotation;
					}

					if (transform.rotation != oldRotation){
						updateTransform(transform);
					}

				}

				if (sprite.billboard && sprite.fakeBillboard){
					Matrix4 modelViewMatrix = camera.viewMatrix * transform.modelMatrix;

					modelViewMatrix.set(0, 0, transform.worldScale.x);
					modelViewMatrix.set(0, 1, 0.0);
					modelViewMatrix.set(0, 2, 0.0);

					if (!sprite.cylindricalBillboard) {
						modelViewMatrix.set(1, 0, 0.0);
						modelViewMatrix.set(1, 1, transform.worldScale.y);
						modelViewMatrix.set(1, 2, 0.0);
					}

					modelViewMatrix.set(2, 0, 0.0);
					modelViewMatrix.set(2, 1, 0.0);
					modelViewMatrix.set(2, 2, transform.worldScale.z);

					transform.modelViewProjectionMatrix = camera.projectionMatrix * modelViewMatrix;
					usingFakeBillboard = true;
				}
			}

			if (!usingFakeBillboard){
				transform.modelViewProjectionMatrix = camera.viewProjectionMatrix * transform.modelMatrix;
			}
			transform.distanceToCamera = (cameraTransform.worldPosition - transform.worldPosition).length();

        	if (signature.test(scene->getComponentType<LightComponent>())){
				LightComponent& light = scene->getComponent<LightComponent>(entity);

				updateLightFromTransform(light, transform);
			}
		}

        if (signature.test(scene->getComponentType<ParticlesComponent>())){
			ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);
			
			if (particles.needUpdate || camera.needUpdate || transform.needUpdate){
				updateParticles(particles, transform, camera, cameraTransform);
			}

			particles.needUpdate = false;
		}

		transform.needUpdate = false;
	}
	camera.needUpdate = false;

	processLights();
}

void RenderSystem::draw(){
	auto transforms = scene->getComponentArray<Transform>();

	//---------Depth shader----------
	if (hasShadows){
		auto lights = scene->getComponentArray<LightComponent>();
		auto meshes = scene->getComponentArray<MeshComponent>();
		
		for (int l = 0; l < lights->size(); l++){
			LightComponent& light = lights->getComponentFromIndex(l);

			if (light.intensity > 0 && light.shadows){
				size_t cameras = 1;
				if (light.type == LightType::POINT){
					cameras = 6;
				}else if (light.type == LightType::DIRECTIONAL){
					cameras = light.numShadowCascades;
				}

				for (int c = 0; c < cameras; c++){
					size_t face = 0;
					size_t fb = c;
					if (light.type == LightType::POINT){
						face = c;
						fb = 0;
					}

					depthRender.startFrameBuffer(&light.framebuffer[fb], face);
					for (int i = 0; i < meshes->size(); i++){
						MeshComponent& mesh = meshes->getComponentFromIndex(i);
						Entity entity = meshes->getEntity(i);
						Transform* transform = scene->findComponent<Transform>(entity);

						if (transform){
							if (mesh.loaded && mesh.needReload){
								destroyMesh(mesh);
							}
							if (!mesh.loaded){
								loadMesh(mesh);
							}
							drawMeshDepth(mesh, light.cameras[c].lightViewProjectionMatrix * transform->modelMatrix);
						}
					}
					depthRender.endFrameBuffer();
				}
			}
		}
	}
	
	if (!scene->isRenderToTexture()){
		sceneRender.startDefaultFrameBuffer(System::instance().getScreenWidth(), System::instance().getScreenHeight());
		sceneRender.applyViewport(Engine::getViewRect());
	}else{
		sceneRender.startFrameBuffer(&scene->getFramebuffer());
	}

	Transform& cameraTransform =  scene->getComponent<Transform>(scene->getCamera());
	CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());

	//---------Draw opaque meshes and UI----------
	std::vector<Entity> parentListScissor;
	Rect activeScissor;

	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);
		Entity entity = transforms->getEntity(i);
		Signature signature = scene->getSignature(entity);

        if (signature.test(scene->getComponentType<MeshComponent>())){
			MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

			if (mesh.loaded && mesh.needReload){
				destroyMesh(mesh);
			}
			if (!mesh.loaded){
				loadMesh(mesh);
			}
			if (!mesh.transparency || camera.type == CameraType::CAMERA_2D){
				//Draw opaque meshes if 3D camera
				drawMesh(mesh, transform, cameraTransform);
			}else{
				transparentMeshes.push({&mesh, &transform, transform.distanceToCamera});
			}

		}else if (signature.test(scene->getComponentType<UIComponent>())){
			UIComponent& ui = scene->getComponent<UIComponent>(entity);

			if (std::find(parentListScissor.begin(), parentListScissor.end(), transform.parent) == parentListScissor.end()){
				activeScissor = Rect(0,0,System::instance().getScreenWidth(), System::instance().getScreenHeight());
				sceneRender.applyScissor(activeScissor);
				parentListScissor.clear();
			}

			bool isText = false;
			if (signature.test(scene->getComponentType<TextComponent>())){
				isText = true;
			}
			if (ui.loaded && ui.needReload){
				destroyUI(ui);
			}
			if (!ui.loaded){
				loadUI(ui, isText);
			}
			drawUI(ui, transform);

			if (signature.test(scene->getComponentType<ImageComponent>())){
				ImageComponent& img = scene->getComponent<ImageComponent>(entity);

				activeScissor = getScissorRect(ui, img, transform, camera).fitOnRect(activeScissor);
				sceneRender.applyScissor(activeScissor);
				parentListScissor.push_back(entity);
			}

		}else if (signature.test(scene->getComponentType<ParticlesComponent>())){
			ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);

			if (particles.loaded && particles.needReload){
				destroyParticles(particles);
			}
			if (!particles.loaded){
				loadParticles(particles);
			}
			drawParticles(particles, transform, cameraTransform);

		}
	}

	//---------Draw sky----------
	SkyComponent* sky = scene->findComponentFromIndex<SkyComponent>(0);
	if (sky){
		if (!sky->loaded){
			loadSky(*sky);
		}

		drawSky(*sky);
	}

	//---------Draw transparent meshes----------
	while (!transparentMeshes.empty()){
		TransparentMeshesData meshData = transparentMeshes.top();

		//Draw transparent meshes
		drawMesh(*meshData.mesh, *meshData.transform, cameraTransform);

		transparentMeshes.pop();
	}

	sceneRender.endFrameBuffer();

	//---------Missing some shaders----------
	if (ShaderPool::getMissingShaders().size() > 0){
		std::string misShaders;
		for (int i = 0; i < ShaderPool::getMissingShaders().size(); i++){
			if (!misShaders.empty())
				misShaders += "; ";
			misShaders += ShaderPool::getMissingShaders()[i];
		}
		Log::Verbose(
			"\n"
			"-------------------\n"
			"Supernova is missing some shaders, you need to use Supershader tool to create these shaders in project assets directory.\n"
			"Go to directory \"tools/supershader\" and execute the command:\n"
			"\n"
			"> python3 supershader.py -s \"%s\" -p \"%s\" -l %s\n"
			"-------------------"
			, misShaders.c_str(), PROJECT_ROOT, ShaderPool::getShaderLangStr().c_str());
		exit(1);
	}
}

void RenderSystem::entityDestroyed(Entity entity){
	Signature signature = scene->getSignature(entity);

	if (signature.test(scene->getComponentType<MeshComponent>())){
		destroyMesh(scene->getComponent<MeshComponent>(entity));
	}

	//TODO: Destroy lights? Framebuffer

	if (signature.test(scene->getComponentType<UIComponent>())){
		destroyUI(scene->getComponent<UIComponent>(entity));
	}

	if (signature.test(scene->getComponentType<ParticlesComponent>())){
		destroyParticles(scene->getComponent<ParticlesComponent>(entity));
	}

	if (signature.test(scene->getComponentType<SkyComponent>())){
		destroySky(scene->getComponent<SkyComponent>(entity));
	}
}
