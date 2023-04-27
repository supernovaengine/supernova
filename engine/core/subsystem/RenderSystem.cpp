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
#include "util/Angle.h"
#include "buffer/ExternalBuffer.h"
#include "math/AlignedBox.h"
#include <memory>
#include <cmath>

using namespace Supernova;

uint32_t RenderSystem::pixelsWhite[64];
uint32_t RenderSystem::pixelsBlack[64];
uint32_t RenderSystem::pixelsNormal[64];

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
	hasFog = false;
	hasMultipleCameras = false;
}

RenderSystem::~RenderSystem(){
}

void RenderSystem::load(){
	createEmptyTextures();
	checkLightsAndShadow();
		
	update(0); // first update

	auto cameras = scene->getComponentArray<CameraComponent>();
	for (int i = 0; i < cameras->size(); i++){
		CameraComponent& camera = cameras->getComponentFromIndex(i);
		if (camera.renderToTexture){
			if (!camera.framebuffer->isCreated()){
				createFramebuffer(camera);
			}
		}
	}
}

void RenderSystem::destroy(){
	SkyComponent* sky = scene->findComponentFromIndex<SkyComponent>(0);
	if (sky){
		if (sky->loaded){
			destroySky(*sky);
		}
	}
	auto transforms = scene->getComponentArray<Transform>();
	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);
		Entity entity = transforms->getEntity(i);
		Signature signature = scene->getSignature(entity);

		if (signature.test(scene->getComponentType<MeshComponent>())){
			MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);
			if (mesh.loaded){
				destroyMesh(mesh);
			}
		}else if (signature.test(scene->getComponentType<TerrainComponent>())){
			TerrainComponent& terrain = scene->getComponent<TerrainComponent>(entity);
			if (terrain.loaded){
				destroyTerrain(terrain);
			}
		}else if (signature.test(scene->getComponentType<UIComponent>())){
			UIComponent& ui = scene->getComponent<UIComponent>(entity);
			if (ui.loaded){
				destroyUI(ui);
			}
		}else if (signature.test(scene->getComponentType<ParticlesComponent>())){
			ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);
			if (particles.loaded){
				destroyParticles(particles);
			}
		}
	}
}

void RenderSystem::createFramebuffer(CameraComponent& camera){
	camera.framebuffer->create();
}

void RenderSystem::updateFramebuffer(CameraComponent& camera){
	if (camera.framebuffer->isCreated()){
		camera.framebuffer->destroy();
	}

	createFramebuffer(camera);
}

void RenderSystem::createEmptyTextures(){
	if (!emptyTexturesCreated){
		void* data_array[6];
		size_t size_array[6];

		for (int i = 0; i < 64; i++) {
			pixelsNormal[i] = 0xFF808080;
		}

		for (int i = 0; i < 6; i++){
			data_array[i]  = (void*)pixelsNormal;
			size_array[i] = 8 * 8 * 4;
		}

		emptyNormal.createTexture(
				"empty|normal", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, data_array, size_array, 
				TextureFilter::NEAREST, TextureFilter::NEAREST, TextureWrap::REPEAT, TextureWrap::REPEAT);

		for (int i = 0; i < 64; i++) {
			pixelsWhite[i] = 0xFFFFFFFF;
		}

		for (int i = 0; i < 6; i++){
			data_array[i]  = (void*)pixelsWhite;
			size_array[i] = 8 * 8 * 4;
		}

		emptyWhite.createTexture(
				"empty|white", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, data_array, size_array, 
				TextureFilter::NEAREST, TextureFilter::NEAREST, TextureWrap::REPEAT, TextureWrap::REPEAT);

		for (int i = 0; i < 64; i++) {
			pixelsBlack[i] = 0xFF000000;
		}

		for (int i = 0; i < 6; i++){
			data_array[i]  = (void*)pixelsBlack;
			size_array[i] = 8 * 8 * 4;
		}

		emptyBlack.createTexture(
				"empty|black", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, data_array, size_array, 
				TextureFilter::NEAREST, TextureFilter::NEAREST, TextureWrap::REPEAT, TextureWrap::REPEAT);
		emptyCubeBlack.createTexture(
				"empty|cube|black", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_CUBE, 6, data_array, size_array, 
				TextureFilter::NEAREST, TextureFilter::NEAREST, TextureWrap::REPEAT, TextureWrap::REPEAT);

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

bool RenderSystem::loadLights(){
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
	
	for (int i = 0; i < numLights; i++){
		LightComponent& light = lights->getComponentFromIndex(i);

		light.shadowMapIndex = -1;
		
		if (light.shadows){
			if (light.numShadowCascades > MAX_SHADOWCASCADES){
				light.numShadowCascades = MAX_SHADOWCASCADES;
				Log::warn("Shadow cascades number is bigger than max value");
			}

			hasShadows = true; // Re-check shadows on, after checked in checkLightsAndShadow()
			if (light.type == LightType::POINT){
				if (!light.framebuffer[0].isCreated())
					light.framebuffer[0].createFramebuffer(
							TextureType::TEXTURE_CUBE, light.mapResolution, light.mapResolution, 
							TextureFilter::LINEAR, TextureFilter::LINEAR, TextureWrap::REPEAT, TextureWrap::REPEAT, true);

				if ((freeShadowCubeMap - MAX_SHADOWSMAP) < MAX_SHADOWSCUBEMAP){
					light.shadowMapIndex = freeShadowCubeMap++;
				}
			}else if (light.type == LightType::SPOT){
				if (!light.framebuffer[0].isCreated())
					light.framebuffer[0].createFramebuffer(
							TextureType::TEXTURE_2D, light.mapResolution, light.mapResolution, 
							TextureFilter::LINEAR, TextureFilter::LINEAR, TextureWrap::REPEAT, TextureWrap::REPEAT, true);

				if (freeShadowMap < MAX_SHADOWSMAP){
					light.shadowMapIndex = freeShadowMap++;
				}
			}else if (light.type == LightType::DIRECTIONAL){
				for (int c = 0; c < light.numShadowCascades; c++){
					if (!light.framebuffer[c].isCreated())
						light.framebuffer[c].createFramebuffer(
								TextureType::TEXTURE_2D, light.mapResolution, light.mapResolution, 
								TextureFilter::LINEAR, TextureFilter::LINEAR, TextureWrap::REPEAT, TextureWrap::REPEAT, true);
				}

				if ((freeShadowMap + light.numShadowCascades - 1) < MAX_SHADOWSMAP){
					light.shadowMapIndex = freeShadowMap;
					freeShadowMap += light.numShadowCascades;
				}
			}

		}

	}

	return true;
}

void RenderSystem::processLights(Transform& cameraTransform){
	auto lights = scene->getComponentArray<LightComponent>();

	int numLights = lights->size();
	if (numLights > MAX_LIGHTS)
		numLights = MAX_LIGHTS;

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

		if (light.shadows){
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
				Log::warn("There are no shadow maps available for all lights, some light shadow will be disabled");
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
}

bool RenderSystem::loadAndProcessFog(){

	FogComponent* fog = scene->findComponentFromIndex<FogComponent>(0);
	hasFog = false;
	if (fog){
		hasFog = true;

		int fogTypeI;
		if (fog->type == FogType::LINEAR){
			fogTypeI = 0;
		}else if (fog->type == FogType::EXPONENTIAL){
			fogTypeI = 1;
		}else if (fog->type == FogType::EXPONENTIALSQUARED){
			fogTypeI = 2;
		}

		fs_fog.color_type = Vector4(fog->color.x, fog->color.y, fog->color.z, fogTypeI);
		fs_fog.density_start_end = Vector4(fog->density, 0.0, fog->linearStart, fog->linearEnd);
	}

	return hasFog;
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

bool RenderSystem::checkPBRFrabebufferUpdate(Material& material){
	return (
		material.baseColorTexture.isFramebufferOutdated() ||
		material.metallicRoughnessTexture.isFramebufferOutdated() ||
		material.normalTexture.isFramebufferOutdated() ||
		material.occlusionTexture.isFramebufferOutdated() ||
		material.emissiveTexture.isFramebufferOutdated() );
}

void RenderSystem::loadPBRTextures(Material& material, ShaderData& shaderData, ObjectRender& render, bool castShadows){
	TextureRender* textureRender = NULL;
	int slotTex = -1;

	textureRender = material.baseColorTexture.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::BASECOLOR, ShaderStageType::FRAGMENT);
	if (textureRender)
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

	if (hasLights){
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

void RenderSystem::loadTerrainTextures(TerrainComponent& terrain, ShaderData& shaderData){
	TextureRender* textureRender = NULL;
	int slotTex = -1;

	textureRender = terrain.heightMap.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::HEIGHTMAP, ShaderStageType::VERTEX);
	if (textureRender)
		terrain.render.addTexture(slotTex, ShaderStageType::VERTEX, textureRender);
	else
		terrain.render.addTexture(slotTex, ShaderStageType::VERTEX, &emptyWhite);

	textureRender = terrain.blendMap.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::BLENDMAP, ShaderStageType::FRAGMENT);
	if (textureRender)
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyBlack);

	textureRender = terrain.textureDetailRed.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::TERRAINDETAIL_RED, ShaderStageType::FRAGMENT);
	if (textureRender)
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

	textureRender = terrain.textureDetailGreen.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::TERRAINDETAIL_GREEN, ShaderStageType::FRAGMENT);
	if (textureRender)
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

	textureRender = terrain.textureDetailBlue.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::TERRAINDETAIL_BLUE, ShaderStageType::FRAGMENT);
	if (textureRender)
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		terrain.render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);
}

bool RenderSystem::loadMesh(MeshComponent& mesh){

	std::map<std::string, Buffer*> buffers;
	bool allBuffersEmpty = true;

	if (mesh.buffer.getSize() > 0){
		buffers["vertices"] = &mesh.buffer;
		allBuffersEmpty = false;
	}
	if (mesh.indices.getSize() > 0){
		buffers["indices"] = &mesh.indices;
		allBuffersEmpty = false;
	}
	for (int i = 0; i < mesh.numExternalBuffers; i++){
		buffers[mesh.eBuffers[i].getName()] = &mesh.eBuffers[i];
		allBuffersEmpty = false;
	}

	if (mesh.vertexCount == 0){
		mesh.vertexCount = mesh.buffer.getCount();
	}

	if (allBuffersEmpty)
		return false;

	bufferNameToRender.clear();

	std::map<std::string, unsigned int> bufferStride;

	for (auto const& buf : buffers){
		buf.second->getRender()->createBuffer(buf.second->getSize(), buf.second->getData(), buf.second->getType(), buf.second->getUsage());
		bufferNameToRender[buf.first] = buf.second->getRender();
		bufferStride[buf.first] = buf.second->getStride();
	}

	for (int i = 0; i < mesh.numSubmeshes; i++){

		mesh.submeshes[i].hasNormalMap = false;
		mesh.submeshes[i].hasTangent = false;
		mesh.submeshes[i].hasVertexColor4 = false;
		mesh.submeshes[i].hasSkinning = false;
		mesh.submeshes[i].hasMorphTarget = false;
		mesh.submeshes[i].hasMorphNormal = false;

		ObjectRender& render = mesh.submeshes[i].render;

		render.beginLoad(mesh.submeshes[i].primitiveType);

		for (auto const& buf : buffers){
        	if (buf.second->isRenderAttributes()) {
            	for (auto const &attr : buf.second->getAttributes()) {
					if (attr.first == AttributeType::TEXCOORD1){
						mesh.submeshes[i].hasTexture1 = true;
					}
					if (attr.first == AttributeType::TANGENT){
						mesh.submeshes[i].hasTangent = true;
					}
					if (attr.first == AttributeType::COLOR){
						mesh.submeshes[i].hasVertexColor4 = true;
					}
					if (attr.first == AttributeType::BONEIDS || attr.first == AttributeType::BONEWEIGHTS){
						mesh.submeshes[i].hasSkinning = true;
					}
					if (attr.first == AttributeType::MORPHTARGET0){
						mesh.submeshes[i].hasMorphTarget = true;
					}
					if (attr.first == AttributeType::MORPHNORMAL0){
						mesh.submeshes[i].hasMorphNormal = true;
					}
					if (attr.first == AttributeType::MORPHTANGENT0){
						mesh.submeshes[i].hasMorphTangent = true;
					}
            	}
        	}
    	}
		for (auto const& attr : mesh.submeshes[i].attributes){
			if (attr.first == AttributeType::TEXCOORD1){
				mesh.submeshes[i].hasTexture1 = true;
			}
			if (attr.first == AttributeType::TANGENT){
				mesh.submeshes[i].hasTangent = true;
			}
			if (attr.first == AttributeType::COLOR){
				mesh.submeshes[i].hasVertexColor4 = true;
			}
			if (attr.first == AttributeType::BONEIDS || attr.first == AttributeType::BONEWEIGHTS){
				mesh.submeshes[i].hasSkinning = true;
			}
			if (attr.first == AttributeType::MORPHTARGET0){
				mesh.submeshes[i].hasMorphTarget = true;
			}
			if (attr.first == AttributeType::MORPHNORMAL0){
				mesh.submeshes[i].hasMorphNormal = true;
			}
			if (attr.first == AttributeType::MORPHTANGENT0){
				mesh.submeshes[i].hasMorphTangent = true;
			}
		}

		if (!mesh.submeshes[i].material.normalTexture.empty()){
			mesh.submeshes[i].hasNormalMap = true;
		}

		bool p_unlit = false;
		bool p_punctual = false;
		bool p_hasTexture1 = false;
		bool p_hasNormalMap = false;
		bool p_hasNormal = false;
		bool p_hasTangent = false;
		bool p_castShadows = false;
		bool p_shadowsPCF = false;

		if (mesh.submeshes[i].hasTexture1){
			p_hasTexture1 = true;
		}
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

		mesh.submeshes[i].shaderProperties = ShaderPool::getMeshProperties(
						p_unlit, p_hasTexture1, false, p_punctual, 
						p_castShadows, p_shadowsPCF, p_hasNormal, p_hasNormalMap, 
						p_hasTangent, false, mesh.submeshes[i].hasVertexColor4, mesh.submeshes[i].hasTextureRect, 
						hasFog, mesh.submeshes[i].hasSkinning, mesh.submeshes[i].hasMorphTarget, mesh.submeshes[i].hasMorphNormal, mesh.submeshes[i].hasMorphTangent,
						false);
		mesh.submeshes[i].shader = ShaderPool::get(ShaderType::MESH, mesh.submeshes[i].shaderProperties);
		if (hasShadows && mesh.castShadows){
			mesh.submeshes[i].depthShaderProperties = ShaderPool::getDepthMeshProperties(
				mesh.submeshes[i].hasSkinning, mesh.submeshes[i].hasMorphTarget, mesh.submeshes[i].hasMorphNormal, mesh.submeshes[i].hasMorphTangent, false);
			mesh.submeshes[i].depthShader = ShaderPool::get(ShaderType::DEPTH, mesh.submeshes[i].depthShaderProperties);
			if (!mesh.submeshes[i].depthShader->isCreated())
				return false;
		}
		if (!mesh.submeshes[i].shader->isCreated())
			return false;
		render.addShader(mesh.submeshes[i].shader.get());
		ShaderData& shaderData = mesh.submeshes[i].shader.get()->shaderData;

		mesh.submeshes[i].slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::PBR_VS_PARAMS, ShaderStageType::VERTEX);
		mesh.submeshes[i].slotFSParams = shaderData.getUniformBlockIndex(UniformBlockType::PBR_FS_PARAMS, ShaderStageType::FRAGMENT);
		if (hasFog){
			mesh.submeshes[i].slotFSFog = shaderData.getUniformBlockIndex(UniformBlockType::FS_FOG, ShaderStageType::FRAGMENT);
		}
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
		if (mesh.submeshes[i].hasSkinning){
			mesh.submeshes[i].slotVSSkinning = shaderData.getUniformBlockIndex(UniformBlockType::VS_SKINNING, ShaderStageType::VERTEX);
		}
		if (mesh.submeshes[i].hasMorphTarget){
			mesh.submeshes[i].slotVSMorphTarget = shaderData.getUniformBlockIndex(UniformBlockType::VS_MORPHTARGET, ShaderStageType::VERTEX);
		}

		loadPBRTextures(mesh.submeshes[i].material, shaderData, mesh.submeshes[i].render, mesh.castShadows);

		mesh.submeshes[i].needUpdateTexture = false;

		if (Engine::isAutomaticTransparency() && !mesh.transparent){
			if (mesh.submeshes[i].material.baseColorTexture.getData().isTransparent() || mesh.submeshes[i].material.baseColorFactor.w != 1.0){
				mesh.transparent = true;
			}
		}
	
		unsigned int indexCount = 0;

		for (auto const& buf : buffers){
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
			mesh.submeshes[i].vertexCount = mesh.vertexCount;
		}

		render.endLoad(PIP_DEFAULT | PIP_RTT);

		//----------Start depth shader---------------
		if (hasShadows && mesh.castShadows){
			ObjectRender& depthRender = mesh.submeshes[i].depthRender;

			depthRender.beginLoad(mesh.submeshes[i].primitiveType);

			depthRender.addShader(mesh.submeshes[i].depthShader.get());
			ShaderData& depthShaderData = mesh.submeshes[i].depthShader.get()->shaderData;

			mesh.submeshes[i].slotVSDepthParams = depthShaderData.getUniformBlockIndex(UniformBlockType::DEPTH_VS_PARAMS, ShaderStageType::VERTEX);

			if (mesh.submeshes[i].hasSkinning){
				mesh.submeshes[i].slotVSDepthSkinning = depthShaderData.getUniformBlockIndex(UniformBlockType::DEPTH_VS_SKINNING, ShaderStageType::VERTEX);
			}
			if (mesh.submeshes[i].hasMorphTarget){
				mesh.submeshes[i].slotVSDepthMorphTarget = depthShaderData.getUniformBlockIndex(UniformBlockType::DEPTH_VS_MORPHTARGET, ShaderStageType::VERTEX);
			}

			for (auto const& buf : buffers){
        		if (buf.second->isRenderAttributes()) {
					for (auto const &attr : buf.second->getAttributes()){
						if (attr.first == AttributeType::POSITION){
							depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized());
						}
						if (mesh.submeshes[i].hasSkinning){
							if (attr.first == AttributeType::BONEIDS || attr.first == AttributeType::BONEWEIGHTS){
								depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized());
							}
						}
						if (mesh.submeshes[i].hasMorphTarget){
							if (attr.first == AttributeType::MORPHTARGET0 || attr.first == AttributeType::MORPHTARGET1 ||
								attr.first == AttributeType::MORPHTARGET2 || attr.first == AttributeType::MORPHTARGET3 ||
								attr.first == AttributeType::MORPHTARGET0 || attr.first == AttributeType::MORPHTARGET4 ||
								attr.first == AttributeType::MORPHTARGET6 || attr.first == AttributeType::MORPHTARGET7 ||
								attr.first == AttributeType::MORPHNORMAL0 || attr.first == AttributeType::MORPHNORMAL1 ||
								attr.first == AttributeType::MORPHNORMAL2 || attr.first == AttributeType::MORPHNORMAL3 ||
								attr.first == AttributeType::MORPHTANGENT0 || attr.first == AttributeType::MORPHTANGENT1){
								depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized());
							}
						}
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
					depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
				}
				if (mesh.submeshes[i].hasSkinning){
					if (attr.first == AttributeType::BONEIDS || attr.first == AttributeType::BONEWEIGHTS){
						depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
					}
				}
				if (mesh.submeshes[i].hasMorphTarget){
					if (attr.first == AttributeType::MORPHTARGET0 || attr.first == AttributeType::MORPHTARGET1 ||
						attr.first == AttributeType::MORPHTARGET2 || attr.first == AttributeType::MORPHTARGET3 ||
						attr.first == AttributeType::MORPHTARGET0 || attr.first == AttributeType::MORPHTARGET4 ||
						attr.first == AttributeType::MORPHTARGET6 || attr.first == AttributeType::MORPHTARGET7 ||
						attr.first == AttributeType::MORPHNORMAL0 || attr.first == AttributeType::MORPHNORMAL1 ||
						attr.first == AttributeType::MORPHNORMAL2 || attr.first == AttributeType::MORPHNORMAL3 ||
						attr.first == AttributeType::MORPHTANGENT0 || attr.first == AttributeType::MORPHTANGENT1){
						depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
					}
				}
			}

			depthRender.endLoad(PIP_DEPTH);
		}
		//----------End depth shader---------------
	}

	mesh.needReload = false;
	mesh.loaded = true;

	return true;
}

void RenderSystem::drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform, bool renderToTexture){
	if (mesh.loaded){

		if (mesh.needUpdateBuffer){
			if (mesh.buffer.getUsage() != BufferUsage::IMMUTABLE)
				mesh.buffer.getRender()->updateBuffer(mesh.buffer.getSize(), mesh.buffer.getData());
			if (mesh.indices.getUsage() != BufferUsage::IMMUTABLE)
				mesh.indices.getRender()->updateBuffer(mesh.indices.getSize(), mesh.indices.getData());
			for (int i = 0; i < mesh.numExternalBuffers; i++){
				mesh.eBuffers[i].getRender()->updateBuffer(mesh.eBuffers[i].getSize(), mesh.eBuffers[i].getData());
			}

			mesh.needUpdateBuffer = false;
		}

		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender& render = mesh.submeshes[i].render;

			bool needUpdateFramebuffer = checkPBRFrabebufferUpdate(mesh.submeshes[i].material);

			if (mesh.submeshes[i].needUpdateTexture || needUpdateFramebuffer){
				ShaderData& shaderData = mesh.submeshes[i].shader.get()->shaderData;
				loadPBRTextures(mesh.submeshes[i].material, shaderData, mesh.submeshes[i].render, mesh.castShadows);

				mesh.submeshes[i].needUpdateTexture = false;
			}
			if (scene->isSceneAmbientLightEnabled()){
				mesh.submeshes[i].material.ambientFactor = scene->getAmbientLightFactor();
				mesh.submeshes[i].material.ambientLight = scene->getAmbientLightColor();
			}

			render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT);

			if (hasFog){
				render.applyUniformBlock(mesh.submeshes[i].slotFSFog, ShaderStageType::FRAGMENT, sizeof(float) * 8, &fs_fog);
			}

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

			if (mesh.submeshes[i].hasSkinning){
				render.applyUniformBlock(mesh.submeshes[i].slotVSSkinning, ShaderStageType::VERTEX, sizeof(float) * 16 * MAX_BONES, &mesh.bonesMatrix);
			}

			if (mesh.submeshes[i].hasMorphTarget){
				render.applyUniformBlock(mesh.submeshes[i].slotVSMorphTarget, ShaderStageType::VERTEX, sizeof(float) * MAX_MORPHTARGETS, &mesh.morphWeights);
			}

			render.applyUniformBlock(mesh.submeshes[i].slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 16, &mesh.submeshes[i].material);

			//model, normal and mvp matrix
			render.applyUniformBlock(mesh.submeshes[i].slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 48, &transform.modelMatrix);

			render.draw(mesh.submeshes[i].vertexCount);
		}
	}
}

void RenderSystem::drawMeshDepth(MeshComponent& mesh, vs_depth_t vsDepthParams){
	if (mesh.loaded && mesh.castShadows){
		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender& depthRender = mesh.submeshes[i].depthRender;

			depthRender.beginDraw(PIP_DEPTH);

			//model, mvp matrix
			depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthParams, ShaderStageType::VERTEX, sizeof(float) * 32, &vsDepthParams);

			if (mesh.submeshes[i].hasSkinning){
				depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthSkinning, ShaderStageType::VERTEX, sizeof(float) * 16 * MAX_BONES, &mesh.bonesMatrix);
			}
			if (mesh.submeshes[i].hasMorphTarget){
				depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthMorphTarget, ShaderStageType::VERTEX, sizeof(float) * MAX_MORPHTARGETS, &mesh.morphWeights);
			}

			depthRender.draw(mesh.submeshes[i].vertexCount);
		}
	}
}

void RenderSystem::destroyMesh(MeshComponent& mesh){
	if (!mesh.loaded)
		return;

	for (int i = 0; i < mesh.numSubmeshes; i++){

		Submesh& submesh = mesh.submeshes[i];

		//Destroy shader
		submesh.shader.reset();
		ShaderPool::remove(ShaderType::MESH, mesh.submeshes[i].shaderProperties);
		if (hasShadows && mesh.castShadows)
			ShaderPool::remove(ShaderType::DEPTH, mesh.submeshes[i].depthShaderProperties);

		//Destroy texture
		submesh.material.baseColorTexture.destroy();
		submesh.material.metallicRoughnessTexture.destroy();
		submesh.material.normalTexture.destroy();
		submesh.material.occlusionTexture.destroy();
		submesh.material.emissiveTexture.destroy();

		//Destroy render
		submesh.render.destroy();
		submesh.depthRender.destroy();

		//Shaders uniforms
		submesh.slotVSParams = -1;
		submesh.slotFSParams = -1;
		submesh.slotFSLighting = -1;
		submesh.slotVSSprite = -1;
		submesh.slotVSShadows = -1;
		submesh.slotFSShadows = -1;
		submesh.slotVSSkinning = -1;
		submesh.slotVSMorphTarget = -1;

		submesh.slotVSDepthParams = -1;
		submesh.slotVSDepthSkinning = -1;
		submesh.slotVSDepthMorphTarget = -1;
	}

	//Destroy buffer
	mesh.buffer.getRender()->destroyBuffer();
	mesh.indices.getRender()->destroyBuffer();
	for (int i = 0; i < mesh.numExternalBuffers; i++){
		mesh.eBuffers[i].getRender()->destroyBuffer();
	}

	mesh.loaded = false;
}

bool RenderSystem::loadTerrain(TerrainComponent& terrain){

	if (!terrain.heightMapLoaded)
		return false;

	if (terrain.buffer.getSize() == 0 || terrain.indices.getSize() == 0)
		return false;

	terrain.buffer.getRender()->createBuffer(terrain.buffer.getSize(), terrain.buffer.getData(), terrain.buffer.getType(), terrain.buffer.getUsage());
	terrain.indices.getRender()->createBuffer(terrain.indices.getSize(), terrain.indices.getData(), terrain.indices.getType(), terrain.indices.getUsage());

	terrain.render.beginLoad(PrimitiveType::TRIANGLES);

	bool p_unlit = false;
	bool p_punctual = false;
	bool p_hasNormal = false;
	bool p_castShadows = false;
	bool p_shadowsPCF = false;

	if (hasLights){
		p_punctual = true;

		p_hasNormal = true;
		if (hasShadows && terrain.castShadows){
			p_castShadows = true;
			if (scene->isShadowsPCF()){
				p_shadowsPCF = true;
			}
		}
	}else{
		p_unlit = true;
	}

	terrain.shaderProperties = ShaderPool::getMeshProperties(
					p_unlit, false, false, p_punctual, 
					p_castShadows, p_shadowsPCF, p_hasNormal, false, 
					false, false, false, false, 
					hasFog, false, false, false, false,
					true);
	terrain.shader = ShaderPool::get(ShaderType::MESH, terrain.shaderProperties);
	if (hasShadows && terrain.castShadows){
		terrain.depthShaderProperties = ShaderPool::getDepthMeshProperties(
			false, false, false, false, true);
		terrain.depthShader = ShaderPool::get(ShaderType::DEPTH, terrain.depthShaderProperties);
		if (!terrain.depthShader->isCreated())
			return false;
	}
	if (!terrain.shader->isCreated())
		return false;
	terrain.render.addShader(terrain.shader.get());
	ShaderData& shaderData = terrain.shader.get()->shaderData;

	terrain.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::PBR_VS_PARAMS, ShaderStageType::VERTEX);
	terrain.slotFSParams = shaderData.getUniformBlockIndex(UniformBlockType::PBR_FS_PARAMS, ShaderStageType::FRAGMENT);
	if (hasFog){
		terrain.slotFSFog = shaderData.getUniformBlockIndex(UniformBlockType::FS_FOG, ShaderStageType::FRAGMENT);
	}
	if (hasLights){
		terrain.slotFSLighting = shaderData.getUniformBlockIndex(UniformBlockType::FS_LIGHTING, ShaderStageType::FRAGMENT);
		if (hasShadows && terrain.castShadows){
			terrain.slotVSShadows = shaderData.getUniformBlockIndex(UniformBlockType::VS_SHADOWS, ShaderStageType::VERTEX);
			terrain.slotFSShadows = shaderData.getUniformBlockIndex(UniformBlockType::FS_SHADOWS, ShaderStageType::FRAGMENT);
		}
	}
	terrain.slotVSTerrain = shaderData.getUniformBlockIndex(UniformBlockType::TERRAIN_VS_PARAMS, ShaderStageType::VERTEX);
	terrain.slotVSTerrainNode = shaderData.getUniformBlockIndex(UniformBlockType::TERRAINNODE_VS_PARAMS, ShaderStageType::VERTEX);

	loadPBRTextures(terrain.material, shaderData, terrain.render, terrain.castShadows);
	loadTerrainTextures(terrain, shaderData);

	terrain.needUpdateTexture = false;

	for (auto const &attr : terrain.buffer.getAttributes()) {
		terrain.render.addAttribute(shaderData.getAttrIndex(attr.first), terrain.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), terrain.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized());
	}
	// empty to create index_type
	terrain.render.addIndex(terrain.indices.getRender(), AttributeDataType::UNSIGNED_SHORT, 0);

	terrain.render.endLoad(PIP_DEFAULT | PIP_RTT);

	//----------Start depth shader---------------
	if (hasShadows && terrain.castShadows){

		terrain.depthRender.beginLoad(PrimitiveType::TRIANGLES);

		terrain.depthRender.addShader(terrain.depthShader.get());
		ShaderData& depthShaderData = terrain.depthShader.get()->shaderData;

		terrain.slotVSDepthParams = depthShaderData.getUniformBlockIndex(UniformBlockType::DEPTH_VS_PARAMS, ShaderStageType::VERTEX);

		terrain.depthRender.addTexture(shaderData.getTextureIndex(TextureShaderType::HEIGHTMAP, ShaderStageType::VERTEX), ShaderStageType::VERTEX, terrain.heightMap.getRender());

		for (auto const &attr : terrain.buffer.getAttributes()) {
			if (attr.first == AttributeType::POSITION){
				terrain.depthRender.addAttribute(shaderData.getAttrIndex(attr.first), terrain.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), terrain.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized());
			}
		}
		// empty to create index_type
		terrain.depthRender.addIndex(terrain.indices.getRender(), AttributeDataType::UNSIGNED_SHORT, 0);

		terrain.depthRender.endLoad(PIP_DEPTH);
	}
	//----------End depth shader---------------

	terrain.needReload = false;
	terrain.loaded = true;

	return true;
}

void RenderSystem::drawTerrain(TerrainComponent& terrain, Transform& transform, Transform& camTransform, bool renderToTexture){
	if (terrain.loaded){
		bool needUpdateFramebuffer = checkPBRFrabebufferUpdate(terrain.material);

		if (terrain.needUpdateTexture || needUpdateFramebuffer){
			ShaderData& shaderData = terrain.shader.get()->shaderData;
			loadPBRTextures(terrain.material, shaderData, terrain.render, terrain.castShadows);
			loadTerrainTextures(terrain, shaderData);

			terrain.needUpdateTexture = false;
		}
		if (scene->isSceneAmbientLightEnabled()){
			terrain.material.ambientFactor = scene->getAmbientLightFactor();
			terrain.material.ambientLight = scene->getAmbientLightColor();
		}

		terrain.render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT);

		if (hasLights){
			terrain.render.applyUniformBlock(terrain.slotFSLighting, ShaderStageType::FRAGMENT, sizeof(float) * (16 * MAX_LIGHTS + 4), &fs_lighting);
			if (hasShadows && terrain.castShadows){
				terrain.render.applyUniformBlock(terrain.slotVSShadows, ShaderStageType::VERTEX, sizeof(float) * (16 * MAX_SHADOWSMAP), &vs_shadows);
				terrain.render.applyUniformBlock(terrain.slotFSShadows, ShaderStageType::FRAGMENT, sizeof(float) * (4 * (MAX_SHADOWSMAP + MAX_SHADOWSCUBEMAP)), &fs_shadows);
			}
		}

		terrain.render.applyUniformBlock(terrain.slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 16, &terrain.material);

		//model, normal and mvp matrix
		terrain.render.applyUniformBlock(terrain.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 48, &transform.modelMatrix);

		terrain.render.applyUniformBlock(terrain.slotVSTerrain, ShaderStageType::VERTEX, sizeof(float) * 8, &terrain.eyePos);

		for (int i = 0; i < terrain.numNodes; i++){
			if (terrain.nodes[i].visible){
				terrain.render.applyUniformBlock(terrain.slotVSTerrainNode, ShaderStageType::VERTEX, sizeof(float) * 8, &terrain.nodes[i].position);
				terrain.render.addIndex(terrain.indices.getRender(), terrain.nodes[i].indexAttribute.getDataType(), terrain.nodes[i].indexAttribute.getOffset());
				terrain.render.draw(terrain.nodes[i].indexAttribute.getCount());
			}
		}
	}
}

void RenderSystem::drawTerrainDepth(TerrainComponent& terrain, vs_depth_t vsDepthParams){
	if (terrain.loaded && terrain.castShadows){
		terrain.depthRender.beginDraw(PIP_DEPTH);

		//model, mvp matrix
		terrain.depthRender.applyUniformBlock(terrain.slotVSDepthParams, ShaderStageType::VERTEX, sizeof(float) * 32, &vsDepthParams);

		terrain.depthRender.applyUniformBlock(terrain.slotVSTerrain, ShaderStageType::VERTEX, sizeof(float) * 8, &terrain.eyePos);

		for (int i = 0; i < terrain.numNodes; i++){
			if (terrain.nodes[i].visible){
				terrain.depthRender.applyUniformBlock(terrain.slotVSTerrainNode, ShaderStageType::VERTEX, sizeof(float) * 8, &terrain.nodes[i].position);
				terrain.depthRender.addIndex(terrain.indices.getRender(), terrain.nodes[i].indexAttribute.getDataType(), terrain.nodes[i].indexAttribute.getOffset());
				terrain.depthRender.draw(terrain.nodes[i].indexAttribute.getCount());
			}
		}
	}
}

void RenderSystem::destroyTerrain(TerrainComponent& terrain){
	if (!terrain.loaded)
		return;

	terrain.heightMap.releaseData();

	//Destroy shader
	terrain.shader.reset();
	ShaderPool::remove(ShaderType::MESH, terrain.shaderProperties);
	if (hasShadows && terrain.castShadows)
		ShaderPool::remove(ShaderType::DEPTH, terrain.depthShaderProperties);

	//Destroy PBR texture
	terrain.material.baseColorTexture.destroy();
	terrain.material.metallicRoughnessTexture.destroy();
	terrain.material.normalTexture.destroy();
	terrain.material.occlusionTexture.destroy();
	terrain.material.emissiveTexture.destroy();

	//Destroy terrain texture
	terrain.heightMap.destroy();
	terrain.blendMap.destroy();
	terrain.textureDetailRed.destroy();
	terrain.textureDetailGreen.destroy();
	terrain.textureDetailBlue.destroy();

	//Destroy render
	terrain.render.destroy();
	terrain.depthRender.destroy();

	//Shaders uniforms
	terrain.slotVSParams = -1;
	terrain.slotFSParams = -1;
	terrain.slotFSLighting = -1;
	terrain.slotFSFog = -1;
	terrain.slotVSShadows = -1;
	terrain.slotFSShadows = -1;
	terrain.slotVSTerrain = -1;
	terrain.slotVSTerrainNode = -1;

	terrain.slotVSDepthParams = -1;

	//Destroy buffers
	terrain.buffer.getRender()->destroyBuffer();
	terrain.indices.getRender()->destroyBuffer();

	terrain.loaded = false;
}

bool RenderSystem::loadUI(UIComponent& uirender, bool isText){
	ObjectRender& render = uirender.render;

	render.beginLoad(uirender.primitiveType);

	TextureRender* textureRender = uirender.texture.getRender();

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

	uirender.shaderProperties = ShaderPool::getUIProperties(p_hasTexture, p_hasFontAtlasTexture, false, p_vertexColorVec4);
	uirender.shader = ShaderPool::get(ShaderType::UI, uirender.shaderProperties);
	if (!uirender.shader->isCreated())
		return false;
	render.addShader(uirender.shader.get());
	ShaderData& shaderData = uirender.shader.get()->shaderData;

	uirender.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::UI_VS_PARAMS, ShaderStageType::VERTEX);
	uirender.slotFSParams = shaderData.getUniformBlockIndex(UniformBlockType::UI_FS_PARAMS, ShaderStageType::FRAGMENT);

	size_t bufferSize;
	size_t minBufferSize;

	bufferSize = uirender.buffer.getSize();
	minBufferSize = uirender.minBufferCount * uirender.buffer.getStride();
	if (minBufferSize > bufferSize)
		bufferSize = minBufferSize;

	if (bufferSize == 0)
		return false;

	uirender.buffer.getRender()->createBuffer(bufferSize, uirender.buffer.getData(), uirender.buffer.getType(), uirender.buffer.getUsage());
	if (uirender.buffer.isRenderAttributes()) {
        for (auto const &attr : uirender.buffer.getAttributes()) {
			render.addAttribute(shaderData.getAttrIndex(attr.first), uirender.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), uirender.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized());
        }
    }

	bufferSize = uirender.indices.getSize();
	minBufferSize = uirender.minIndicesCount * uirender.indices.getStride();
	if (minBufferSize > bufferSize)
		bufferSize = minBufferSize;

	if (uirender.indices.getCount() > 0){
		uirender.indices.getRender()->createBuffer(bufferSize, uirender.indices.getData(), uirender.indices.getType(), uirender.indices.getUsage());
		uirender.vertexCount = uirender.indices.getCount();
		Attribute indexattr = uirender.indices.getAttributes()[AttributeType::INDEX];
		render.addIndex(uirender.indices.getRender(), indexattr.getDataType(), indexattr.getOffset());
	}else{
		uirender.vertexCount = uirender.buffer.getCount();
	}

	if (textureRender)
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::UI, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);
	
	uirender.needUpdateTexture = false;

	render.endLoad(PIP_DEFAULT | PIP_RTT);

	uirender.needReload = false;
	uirender.loaded = true;

	return true;
}

void RenderSystem::drawUI(UIComponent& uirender, Transform& transform, bool renderToTexture){
	if (uirender.loaded && uirender.buffer.getSize() > 0){

		if (uirender.needUpdateTexture || uirender.texture.isFramebufferOutdated()){
			ShaderData& shaderData = uirender.shader.get()->shaderData;
			TextureRender* textureRender = uirender.texture.getRender();
			if (textureRender)
				uirender.render.addTexture(shaderData.getTextureIndex(TextureShaderType::UI, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			uirender.needUpdateTexture = false;
		}

		if (uirender.needUpdateBuffer){
			uirender.buffer.getRender()->updateBuffer(uirender.buffer.getSize(), uirender.buffer.getData());

			if (uirender.indices.getCount() > 0){
				uirender.indices.getRender()->updateBuffer(uirender.indices.getSize(), uirender.indices.getData());
				uirender.vertexCount = uirender.indices.getCount();
			}else{
				uirender.vertexCount = uirender.buffer.getCount();
			}

			uirender.needUpdateBuffer = false;
		}

		ObjectRender& render = uirender.render;

		render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT);
		render.applyUniformBlock(uirender.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &transform.modelViewProjectionMatrix);
		//Color
		render.applyUniformBlock(uirender.slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 4, &uirender.color);
		render.draw(uirender.vertexCount);

	}
}

void RenderSystem::destroyUI(UIComponent& uirender){
	if (!uirender.loaded)
		return;

	//Destroy shader
	uirender.shader.reset();
	ShaderPool::remove(ShaderType::UI, uirender.shaderProperties);

	//Destroy texture
	uirender.texture.destroy();

	//Destroy render
	uirender.render.destroy();

	//Destroy buffer
	if (uirender.buffer.getSize() > 0){
		uirender.buffer.getRender()->destroyBuffer();
	}
	if (uirender.indices.getSize() > 0){
		uirender.indices.getRender()->destroyBuffer();
	}

	//Shaders uniforms
	uirender.slotVSParams = -1;
	uirender.slotFSParams = -1;

	uirender.loaded = false;
}

bool RenderSystem::loadParticles(ParticlesComponent& particles){
	ObjectRender& render = particles.render;

	render.beginLoad(PrimitiveType::POINTS);

	TextureRender* textureRender = particles.texture.getRender();

	if (Engine::isAutomaticTransparency() && !particles.transparent){
		if (particles.texture.getData().isTransparent()){ // Particle color is not tested here
			particles.transparent = true;
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

	particles.shaderProperties = ShaderPool::getPointsProperties(p_hasTexture, false, true, p_hasTextureRect);
	particles.shader = ShaderPool::get(ShaderType::POINTS, particles.shaderProperties);
	if (!particles.shader->isCreated())
		return false;
	render.addShader(particles.shader.get());
	ShaderData& shaderData = particles.shader.get()->shaderData;

	particles.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::POINTS_VS_PARAMS, ShaderStageType::VERTEX);

	particles.buffer.clearAll();
	particles.buffer.addAttribute(AttributeType::POSITION, 3, 0);
	particles.buffer.addAttribute(AttributeType::COLOR, 4, 3 * sizeof(float));
	particles.buffer.addAttribute(AttributeType::POINTSIZE, 1, 7 * sizeof(float));
	particles.buffer.addAttribute(AttributeType::POINTROTATION, 1, 8 * sizeof(float));
	particles.buffer.addAttribute(AttributeType::TEXTURERECT, 4, 9 * sizeof(float));
	particles.buffer.setStride(13 * sizeof(float));
	particles.buffer.setRenderAttributes(true);
	particles.buffer.setUsage(BufferUsage::STREAM);

	// Now buffer size is zero than it needed to be calculated
	size_t bufferSize = particles.maxParticles * particles.buffer.getStride();

	particles.buffer.getRender()->createBuffer(bufferSize, particles.buffer.getData(), particles.buffer.getType(), particles.buffer.getUsage());
	if (particles.buffer.isRenderAttributes()) {
        for (auto const &attr : particles.buffer.getAttributes()) {
			render.addAttribute(shaderData.getAttrIndex(attr.first), particles.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), particles.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized());
        }
    }

	if (textureRender)
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::POINTS, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

	particles.needUpdateTexture = false;

	render.endLoad(PIP_DEFAULT | PIP_RTT);

	particles.needReload = false;
	particles.loaded = true;

	return true;
}

void RenderSystem::drawParticles(ParticlesComponent& particles, Transform& transform, Transform& camTransform, bool renderToTexture){
	if (particles.loaded && particles.buffer.getSize() > 0){

		if (particles.needUpdateTexture || particles.texture.isFramebufferOutdated()){
			ShaderData& shaderData = particles.shader.get()->shaderData;
			TextureRender* textureRender = particles.texture.getRender();
			if (textureRender)
				particles.render.addTexture(shaderData.getTextureIndex(TextureShaderType::POINTS, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			particles.needUpdateTexture = false;
		}

		if (particles.needUpdateBuffer){
			particles.buffer.getRender()->updateBuffer(particles.buffer.getSize(), particles.buffer.getData());
			particles.needUpdateBuffer = false;
		}

		ObjectRender& render = particles.render;

		render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT);
		render.applyUniformBlock(particles.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &transform.modelViewProjectionMatrix);
		render.draw(particles.numVisible);
	}
}

void RenderSystem::destroyParticles(ParticlesComponent& particles){
	if (!particles.loaded)
		return;

	//Destroy shader
	particles.shader.reset();
	ShaderPool::remove(ShaderType::POINTS, particles.shaderProperties);

	//Destroy texture
	particles.texture.destroy();

	//Destroy render
	particles.render.destroy();

	//Destroy buffer
	particles.buffer.getRender()->destroyBuffer();

	//Shaders uniforms
	particles.slotVSParams = -1;

	particles.loaded = false;
}

bool RenderSystem::loadSky(SkyComponent& sky){

	createSky(sky);

	ObjectRender* render = &sky.render;

	render->beginLoad(PrimitiveType::TRIANGLES);

	sky.shader = ShaderPool::get(ShaderType::SKYBOX, "");
	if (!sky.shader->isCreated())
		return false;
	render->addShader(sky.shader.get());
	ShaderData& shaderData = sky.shader.get()->shaderData;

	sky.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::SKY_VS_PARAMS, ShaderStageType::VERTEX);
	sky.slotFSParams = shaderData.getUniformBlockIndex(UniformBlockType::SKY_FS_PARAMS, ShaderStageType::FRAGMENT);

	TextureRender* textureRender = sky.texture.getRender();
	if (textureRender){
		render->addTexture(shaderData.getTextureIndex(TextureShaderType::SKYCUBE, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);
	} else {
		return false;
	}

	sky.needUpdateTexture = false;

	sky.buffer.getRender()->createBuffer(sky.buffer.getSize(), sky.buffer.getData(), sky.buffer.getType(), sky.buffer.getUsage());

	if (sky.buffer.isRenderAttributes()) {
        for (auto const &attr : sky.buffer.getAttributes()) {
			render->addAttribute(shaderData.getAttrIndex(attr.first), sky.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), sky.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized());
        }
    }

	render->endLoad(PIP_DEFAULT | PIP_RTT);

	sky.loaded = true;

	return true;
}

void RenderSystem::drawSky(SkyComponent& sky, bool renderToTexture){
	if (sky.loaded){

		if (sky.needUpdateTexture || sky.texture.isFramebufferOutdated()){
			ShaderData& shaderData = sky.shader.get()->shaderData;
			TextureRender* textureRender = sky.texture.getRender();
			if (textureRender)
				sky.render.addTexture(shaderData.getTextureIndex(TextureShaderType::SKYCUBE, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			sky.needUpdateTexture = false;
		}

		ObjectRender& render = sky.render;

		render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT);
		render.applyUniformBlock(sky.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &sky.skyViewProjectionMatrix);
		render.applyUniformBlock(sky.slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 4, &sky.color);
		render.draw(36);
	}
}

void RenderSystem::destroySky(SkyComponent& sky){
	if (!sky.loaded)
		return;

	//Destroy shader
	sky.shader.reset();
	ShaderPool::remove(ShaderType::SKYBOX, "");

	//Destroy texture
	sky.texture.destroy();

	//Destroy render
	sky.render.destroy();

	//Destroy buffer
	sky.buffer.getRender()->destroyBuffer();

	//Shaders uniforms
	sky.slotVSParams = -1;
	sky.slotFSParams = -1;

	sky.loaded = false;
}

Rect RenderSystem::getScissorRect(UILayoutComponent& layout, ImageComponent& img, Transform& transform, CameraComponent& camera){
	int objScreenPosX = 0;
	int objScreenPosY = 0;
	int objScreenWidth = 0;
	int objScreenHeight = 0;

	if (!camera.renderToTexture) {

		float scaleX = transform.worldScale.x;
		float scaleY = transform.worldScale.y;

		float tempX = (2 * transform.worldPosition.x / (float) Engine::getCanvasWidth()) - 1;
		float tempY = (2 * transform.worldPosition.y / (float) Engine::getCanvasHeight()) - 1;

		float widthRatio = scaleX * (Engine::getViewRect().getWidth() / (float) Engine::getCanvasWidth());
		float heightRatio = scaleY * (Engine::getViewRect().getHeight() / (float) Engine::getCanvasHeight());

		objScreenPosX = (tempX * Engine::getViewRect().getWidth() + (float) System::instance().getScreenWidth()) / 2;
		objScreenPosY = (tempY * Engine::getViewRect().getHeight() + (float) System::instance().getScreenHeight()) / 2;
		objScreenWidth = layout.width * widthRatio;
		objScreenHeight = layout.height * heightRatio;

		if (camera.type == CameraType::CAMERA_2D)
			objScreenPosY = (float) System::instance().getScreenHeight() - objScreenHeight - objScreenPosY;

		if (!(img.patchMarginLeft == 0 && img.patchMarginTop == 0 && img.patchMarginRight == 0 && img.patchMarginBottom == 0)) {
			float borderScreenLeft = img.patchMarginLeft * widthRatio;
			float borderScreenTop = img.patchMarginTop * heightRatio;
			float borderScreenRight = img.patchMarginRight * widthRatio;
			float borderScreenBottom = img.patchMarginBottom * heightRatio;

			objScreenPosX += borderScreenLeft;
			objScreenPosY += borderScreenBottom; // scissor is bottom-left
			objScreenWidth -= (borderScreenLeft + borderScreenRight);
			objScreenHeight -= (borderScreenTop + borderScreenBottom);
		}

	}else {

		objScreenPosX = transform.worldPosition.x;
		objScreenPosY = transform.worldPosition.y;
		objScreenWidth = layout.width;
		objScreenHeight = layout.height;

		if (camera.type == CameraType::CAMERA_2D)
			objScreenPosY = (float) camera.framebuffer->getHeight() - objScreenHeight - objScreenPosY;

		if (!(img.patchMarginLeft == 0 && img.patchMarginTop == 0 && img.patchMarginRight == 0 && img.patchMarginBottom == 0)) {
			float borderScreenLeft = img.patchMarginLeft;
			float borderScreenTop = img.patchMarginTop;
			float borderScreenRight = img.patchMarginRight;
			float borderScreenBottom = img.patchMarginBottom;

			objScreenPosX += borderScreenLeft;
			objScreenPosY += borderScreenBottom; // scissor is bottom-left
			objScreenWidth -= (borderScreenLeft + borderScreenRight);
			objScreenHeight -= (borderScreenTop + borderScreenBottom);
		}

	}


	return Rect(objScreenPosX, objScreenPosY, objScreenWidth, objScreenHeight);
}

void RenderSystem::updateTransform(Transform& transform){
	if (!transform.staticObject){
		Matrix4 scaleMatrix = Matrix4::scaleMatrix(transform.scale);
		Matrix4 translateMatrix = Matrix4::translateMatrix(transform.position);
		Matrix4 rotationMatrix = transform.rotation.getRotationMatrix();

		transform.modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;
	}

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

	camera.needUpdateFrustumPlanes = true;
}

void RenderSystem::updateSkyViewProjection(SkyComponent& sky, CameraComponent& camera){
	Matrix4 skyViewMatrix = camera.viewMatrix;

	skyViewMatrix.set(3,0,0);
	skyViewMatrix.set(3,1,0);
	skyViewMatrix.set(3,2,0);
	skyViewMatrix.set(3,3,1);
	skyViewMatrix.set(2,3,0);
	skyViewMatrix.set(1,3,0);
	skyViewMatrix.set(0,3,0);

	sky.skyViewProjectionMatrix = camera.projectionMatrix * skyViewMatrix;
}

void RenderSystem::updateParticles(ParticlesComponent& particles, Transform& transform, CameraComponent& camera, Transform& camTransform, bool sortTransparentParticles){
	particles.shaderParticles.clear();
	particles.shaderParticles.reserve(particles.particles.size());

	// point particle sizes are in pixels, need to convert it to canvas size
	float sizeScaleW = System::instance().getScreenWidth() / (float)Engine::getCanvasWidth();
	float sizeScaleH = System::instance().getScreenHeight() / (float)Engine::getCanvasHeight();
	float sizeScale = std::max(sizeScaleW, sizeScaleH);

	particles.numVisible = 0;
	size_t particlesSize = (particles.particles.size() < particles.maxParticles)? particles.particles.size() : particles.maxParticles;
	for (int i = 0; i < particlesSize; i++){
		if (particles.particles[i].life > particles.particles[i].time){
			particles.shaderParticles.push_back({});
			particles.shaderParticles[particles.numVisible].position = particles.particles[i].position;
			particles.shaderParticles[particles.numVisible].color = particles.particles[i].color;
			particles.shaderParticles[particles.numVisible].size = particles.particles[i].size * sizeScale;
			particles.shaderParticles[particles.numVisible].rotation = particles.particles[i].rotation;
			particles.shaderParticles[particles.numVisible].textureRect = particles.particles[i].textureRect;
			particles.numVisible++;
		}
	}

	if (sortTransparentParticles){
		auto comparePoints = [&transform, &camTransform](const ParticleShaderData& a, const ParticleShaderData& b) -> bool {
			float distanceToCameraA = (camTransform.worldPosition - (transform.modelMatrix * a.position)).length();
			float distanceToCameraB = (camTransform.worldPosition - (transform.modelMatrix * b.position)).length();
			return distanceToCameraA > distanceToCameraB;
		};
		std::sort(particles.shaderParticles.begin(), particles.shaderParticles.end(), comparePoints);
	}

	if (particles.numVisible > 0){
		particles.buffer.setData((unsigned char*)(&particles.shaderParticles.at(0)), sizeof(ParticleShaderData)*particles.numVisible);
	}else{
		particles.buffer.setData((unsigned char*)nullptr, 0);
	}

	if (particles.loaded)
		particles.needUpdateBuffer = true;
}

void RenderSystem::updateTerrain(TerrainComponent& terrain, Transform& transform, CameraComponent& camera, Transform& cameraTransform){
	if (terrain.heightMapLoaded){
		for (int i = 0; i < terrain.numNodes; i++){
			terrain.nodes[i].visible = false;
		}

		for (int i = 0; i < (terrain.rootGridSize*terrain.rootGridSize); i++){
			terrainNodeLODSelect(terrain, transform, camera, cameraTransform, terrain.nodes[terrain.grid[i]], terrain.levels-1);
		}

		terrain.eyePos = Vector3(cameraTransform.worldPosition.x, cameraTransform.worldPosition.y, cameraTransform.worldPosition.z);
	}
}

void RenderSystem::setTerrainNodeIndex(TerrainComponent& terrain, TerrainNode& terrainNode, size_t size, size_t offset){
	terrainNode.indexAttribute.setElements(1);
	terrainNode.indexAttribute.setDataType(AttributeDataType::UNSIGNED_SHORT);
	terrainNode.indexAttribute.setCount(size);
	terrainNode.indexAttribute.setOffset(offset);
	terrainNode.indexAttribute.setNormalized(false);

	if (terrainNode.hasChilds){
		for (int i = 0; i < 4; i++){
			setTerrainNodeIndex(terrain, terrain.nodes[terrainNode.childs[i]], size, offset);
		}
	}
}

AlignedBox RenderSystem::getTerrainNodeAlignedBox(Transform& transform, TerrainNode& terrainNode){
    float halfSize = terrainNode.size/2;
    Vector3 worldHalfScale(halfSize * transform.worldScale.x, 1, halfSize * transform.worldScale.z);
    Vector3 worldPosition = transform.modelMatrix * Vector3(terrainNode.position.x, 0, terrainNode.position.y);

    Vector3 c1 = Vector3(worldPosition.x - worldHalfScale.x, terrainNode.minHeight, worldPosition.z - worldHalfScale.z);
    Vector3 c2 = Vector3(worldPosition.x + worldHalfScale.x, terrainNode.maxHeight, worldPosition.z + worldHalfScale.z);

    return AlignedBox(c1, c2);
};

bool RenderSystem::isTerrainNodeInSphere(Vector3 position, float radius, const AlignedBox& box) {
	float r2 = radius*radius;

	Vector3 c1 = box.getMinimum();
	Vector3 c2 = box.getMaximum();
	Vector3 distV;

	if (position.x < c1.x) distV.x = (position.x - c1.x);
	else if (position.x > c2.x) distV.x = (position.x - c2.x);
	if (position.y < c1.y) distV.y = (position.y - c1.y);
	else if (position.y > c2.y) distV.y = (position.y - c2.y);
	if (position.z < c1.z) distV.z = (position.z - c1.z);
	else if (position.z > c2.z) distV.z = (position.z - c2.z);

	float dist2 = distV.dotProduct(distV);

	return dist2 <= r2;
}

bool RenderSystem::terrainNodeLODSelect(TerrainComponent& terrain, Transform& transform, CameraComponent& camera, Transform& cameraTransform, TerrainNode& terrainNode, int lodLevel){
    terrainNode.currentRange = terrain.ranges[lodLevel];

    AlignedBox box = getTerrainNodeAlignedBox(transform, terrainNode);

    if (!isTerrainNodeInSphere(cameraTransform.worldPosition, terrain.ranges[lodLevel], box)) {
        return false;
    }

    if (!isInsideCamera(camera, box)) {
        return true;
    }

    if( lodLevel == 0 ) {
        //Full resolution
        terrainNode.resolution = terrain.resolution;
		terrainNode.visible = true;
		setTerrainNodeIndex(terrain, terrainNode, terrain.fullResNode.indexCount, terrain.fullResNode.indexOffset * sizeof(uint16_t));

        return true;
    } else {

        if( !isTerrainNodeInSphere(cameraTransform.worldPosition, terrain.ranges[lodLevel-1], box) ) {
            //Full resolution
			terrainNode.resolution = terrain.resolution;
			terrainNode.visible = true;
			setTerrainNodeIndex(terrain, terrainNode, terrain.fullResNode.indexCount, terrain.fullResNode.indexOffset * sizeof(uint16_t));
        } else {
            for (int i = 0; i < 4; i++) {
                TerrainNode& child = terrain.nodes[terrainNode.childs[i]];
				if (!terrainNodeLODSelect(terrain, transform, camera, cameraTransform, child, lodLevel-1)){
                    //Half resolution
                    child.resolution = terrain.resolution / 2;
                    child.currentRange = terrainNode.currentRange;
					child.visible = true;
					setTerrainNodeIndex(terrain, child, terrain.halfResNode.indexCount, terrain.halfResNode.indexOffset * sizeof(uint16_t));
                }
            }
        }

        return true;
    }

}

void RenderSystem::createSky(SkyComponent& sky){
	sky.buffer.clearAll();
	sky.buffer.addAttribute(AttributeType::POSITION, 3);

	Attribute* attVertex = sky.buffer.getAttribute(AttributeType::POSITION);

	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));

	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));

	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));

	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));

	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));

	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
	sky.buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
	sky.buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
}

void RenderSystem::updateCameraSize(Entity entity){
	Signature signature = scene->getSignature(entity);
	if (signature.test(scene->getComponentType<CameraComponent>())){
		CameraComponent& camera = scene->getComponent<CameraComponent>(entity);
		
		Rect rect;
		if (!camera.renderToTexture) {
			rect = Rect(0, 0, Engine::getCanvasWidth(), Engine::getCanvasHeight());
		}else{
			rect = Rect(0, 0, camera.framebuffer->getWidth(), camera.framebuffer->getHeight());
		}

		if (camera.automatic){
			float newLeft = rect.getX();
			float newBottom = rect.getY();
			float newRight = rect.getWidth();
			float newTop = rect.getHeight();
			float newAspect = rect.getWidth() / rect.getHeight();

			if ((camera.left != newLeft) || (camera.bottom != newBottom) || (camera.right != newRight) || (camera.top != newTop) || (camera.aspect != newAspect)){
				camera.left = newLeft;
				camera.bottom = newBottom;
				camera.right = newRight;
				camera.top = newTop;
				camera.aspect = newAspect;

				camera.needUpdate = true;
			}
		}
	}
}

float RenderSystem::getCameraFar(CameraComponent& camera){
	if (camera.type == CameraType::CAMERA_PERSPECTIVE){
		return camera.perspectiveFar;
	}else{
		return camera.orthoFar;
	}
}

bool RenderSystem::isInsideCamera(CameraComponent& camera, const AlignedBox& box){
    if (box.isNull() || box.isInfinite())
        return false;

    updateCameraFrustumPlanes(camera);

    Vector3 centre = box.getCenter();
    Vector3 halfSize = box.getHalfSize();

    for (int plane = 0; plane < 6; ++plane){
        if (plane == FRUSTUM_PLANE_FAR && getCameraFar(camera) == 0)
            continue;

        Plane::Side side = camera.frustumPlanes[plane].getSide(centre, halfSize);
        if (side == Plane::Side::NEGATIVE_SIDE){
            return false;
        }
    }

    return true;
}

bool RenderSystem::isInsideCamera(CameraComponent& camera, const Vector3& point){
    updateCameraFrustumPlanes(camera);

    for (int plane = 0; plane < 6; ++plane){
        if (plane == FRUSTUM_PLANE_FAR && getCameraFar(camera) == 0)
            continue;

        if (camera.frustumPlanes[plane].getSide(point) == Plane::Side::NEGATIVE_SIDE){
            return false;
        }
    }

    return true;
}

bool RenderSystem::isInsideCamera(CameraComponent& camera, const Vector3& center, const float& radius){
    updateCameraFrustumPlanes(camera);

    for (int plane = 0; plane < 6; ++plane){
        if (plane == FRUSTUM_PLANE_FAR && getCameraFar(camera) == 0)
            continue;

        if (camera.frustumPlanes[plane].getDistance(center) < -radius){
            return false;
        }
    }

    return true;
}

bool RenderSystem::updateCameraFrustumPlanes(CameraComponent& camera){
    if (!camera.needUpdateFrustumPlanes)
        return false;

    camera.needUpdateFrustumPlanes = false;

    camera.frustumPlanes[FRUSTUM_PLANE_LEFT].normal.x = camera.viewProjectionMatrix[0][3] + camera.viewProjectionMatrix[0][0];
    camera.frustumPlanes[FRUSTUM_PLANE_LEFT].normal.y = camera.viewProjectionMatrix[1][3] + camera.viewProjectionMatrix[1][0];
    camera.frustumPlanes[FRUSTUM_PLANE_LEFT].normal.z = camera.viewProjectionMatrix[2][3] + camera.viewProjectionMatrix[2][0];
    camera.frustumPlanes[FRUSTUM_PLANE_LEFT].d = camera.viewProjectionMatrix[3][3] + camera.viewProjectionMatrix[3][0];

    camera.frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.x = camera.viewProjectionMatrix[0][3] - camera.viewProjectionMatrix[0][0];
    camera.frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.y = camera.viewProjectionMatrix[1][3] - camera.viewProjectionMatrix[1][0];
    camera.frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.z = camera.viewProjectionMatrix[2][3] - camera.viewProjectionMatrix[2][0];
    camera.frustumPlanes[FRUSTUM_PLANE_RIGHT].d = camera.viewProjectionMatrix[3][3] - camera.viewProjectionMatrix[3][0];

    camera.frustumPlanes[FRUSTUM_PLANE_TOP].normal.x = camera.viewProjectionMatrix[0][3] - camera.viewProjectionMatrix[0][1];
    camera.frustumPlanes[FRUSTUM_PLANE_TOP].normal.y = camera.viewProjectionMatrix[1][3] - camera.viewProjectionMatrix[1][1];
    camera.frustumPlanes[FRUSTUM_PLANE_TOP].normal.z = camera.viewProjectionMatrix[2][3] - camera.viewProjectionMatrix[2][1];
    camera.frustumPlanes[FRUSTUM_PLANE_TOP].d = camera.viewProjectionMatrix[3][3] - camera.viewProjectionMatrix[3][1];

    camera.frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = camera.viewProjectionMatrix[0][3] + camera.viewProjectionMatrix[0][1];
    camera.frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = camera.viewProjectionMatrix[1][3] + camera.viewProjectionMatrix[1][1];
    camera.frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = camera.viewProjectionMatrix[2][3] + camera.viewProjectionMatrix[2][1];
    camera.frustumPlanes[FRUSTUM_PLANE_BOTTOM].d = camera.viewProjectionMatrix[3][3] + camera.viewProjectionMatrix[3][1];

    camera.frustumPlanes[FRUSTUM_PLANE_NEAR].normal.x = camera.viewProjectionMatrix[0][3] + camera.viewProjectionMatrix[0][2];
    camera.frustumPlanes[FRUSTUM_PLANE_NEAR].normal.y = camera.viewProjectionMatrix[1][3] + camera.viewProjectionMatrix[1][2];
    camera.frustumPlanes[FRUSTUM_PLANE_NEAR].normal.z = camera.viewProjectionMatrix[2][3] + camera.viewProjectionMatrix[2][2];
    camera.frustumPlanes[FRUSTUM_PLANE_NEAR].d = camera.viewProjectionMatrix[3][3] + camera.viewProjectionMatrix[3][2];

    camera.frustumPlanes[FRUSTUM_PLANE_FAR].normal.x = camera.viewProjectionMatrix[0][3] - camera.viewProjectionMatrix[0][2];
    camera.frustumPlanes[FRUSTUM_PLANE_FAR].normal.y = camera.viewProjectionMatrix[1][3] - camera.viewProjectionMatrix[1][2];
    camera.frustumPlanes[FRUSTUM_PLANE_FAR].normal.z = camera.viewProjectionMatrix[2][3] - camera.viewProjectionMatrix[2][2];
    camera.frustumPlanes[FRUSTUM_PLANE_FAR].d = camera.viewProjectionMatrix[3][3] - camera.viewProjectionMatrix[3][2];

    for (int i=0; i<6; i++){
        float length = camera.frustumPlanes[i].normal.normalizeL();
        camera.frustumPlanes[i].d /= length;
    }

    return true;
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

void RenderSystem::updateLightFromScene(LightComponent& light, Transform& transform, CameraComponent& camera){
	light.worldDirection = transform.worldRotation * light.direction;

	if (hasShadows && (light.intensity > 0)){
		
		Vector3 up = Vector3(0, 1, 0);
		if (light.worldDirection.crossProduct(up) == Vector3::ZERO){
			up = Vector3(0, 0, 1);
		}

		//TODO: perspective aspect based on shadow map size
		if (light.type == LightType::DIRECTIONAL){
			
			float shadowSplitLogFactor = .7f;

			Matrix4 projectionMatrix[MAX_SHADOWCASCADES];
			Matrix4 viewMatrix;

			viewMatrix = Matrix4::lookAtMatrix(transform.worldPosition, light.worldDirection + transform.worldPosition, up);

			//TODO: light directional cascades is only considering main camera
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
					Log::warn("Can not have multiple cascades shadows when using ortho scene camera. Reducing num shadow cascades to 1");
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

void RenderSystem::updateMVP(size_t index, Transform& transform, CameraComponent& camera, Transform& cameraTransform){
	if (transform.billboard && !transform.fakeBillboard){

		Vector3 camPos = cameraTransform.worldPosition;

		if (transform.cylindricalBillboard)
			camPos.y = transform.worldPosition.y;

		Matrix4 m1 = Matrix4::lookAtMatrix(camPos, transform.worldPosition, camera.worldUp).inverse();

		Quaternion oldRotation = transform.rotation;

		transform.rotation.fromRotationMatrix(m1);
		if (transform.parent != NULL_ENTITY){
			auto transformParent = scene->getComponent<Transform>(transform.parent);
			transform.rotation = transformParent.worldRotation.inverse() * transform.rotation;
		}

		if (transform.rotation != oldRotation){
			transform.needUpdate = true;

			std::vector<Entity> parentList;
			auto transforms = scene->getComponentArray<Transform>();
			for (int i = index; i < transforms->size(); i++){
				Transform& transform = transforms->getComponentFromIndex(i);

				// Finding childs
				if (i > index){
					if (std::find(parentList.begin(), parentList.end(), transform.parent) != parentList.end()){
						transform.needUpdate = true;
					}else{
						break;
					}
				}

				if (transform.needUpdate){
					Entity entity = transforms->getEntity(i);
					parentList.push_back(entity);
					updateTransform(transform);
				}
			}
		}

	}

	if (transform.billboard && transform.fakeBillboard){
		
		Matrix4 modelViewMatrix = camera.viewMatrix * transform.modelMatrix;

		modelViewMatrix.set(0, 0, transform.worldScale.x);
		modelViewMatrix.set(0, 1, 0.0);
		modelViewMatrix.set(0, 2, 0.0);

		if (!transform.cylindricalBillboard) {
			modelViewMatrix.set(1, 0, 0.0);
			modelViewMatrix.set(1, 1, transform.worldScale.y);
			modelViewMatrix.set(1, 2, 0.0);
		}

		modelViewMatrix.set(2, 0, 0.0);
		modelViewMatrix.set(2, 1, 0.0);
		modelViewMatrix.set(2, 2, transform.worldScale.z);

		transform.modelViewProjectionMatrix = camera.projectionMatrix * modelViewMatrix;

	}else{

		transform.modelViewProjectionMatrix = camera.viewProjectionMatrix * transform.modelMatrix;

	}

	transform.distanceToCamera = (cameraTransform.worldPosition - transform.worldPosition).length();
}

void RenderSystem::update(double dt){
	auto transforms = scene->getComponentArray<Transform>();
	auto cameras = scene->getComponentArray<CameraComponent>();

	std::vector<Entity> parentList;
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

	Entity mainCameraEntity = scene->getCamera();

	hasMultipleCameras = false;
	for (int i = 0; i < cameras->size(); i++){
		CameraComponent& camera = cameras->getComponentFromIndex(i);
		Entity cameraEntity = cameras->getEntity(i);
		Transform& cameraTransform = scene->getComponent<Transform>(cameraEntity);
		if (camera.renderToTexture && cameraEntity != mainCameraEntity){
			hasMultipleCameras = true;
		}

		if (cameraTransform.needUpdate){
			camera.needUpdate = true;
		}
		
		if (camera.needUpdate){
			updateCamera(camera, cameraTransform);
		}
	}

	CameraComponent& mainCamera =  scene->getComponent<CameraComponent>(mainCameraEntity);
	Transform& mainCameraTransform =  scene->getComponent<Transform>(mainCameraEntity);

	loadLights();
	loadAndProcessFog();

	SkyComponent* sky = scene->findComponentFromIndex<SkyComponent>(0);
	if (sky){
		if (mainCamera.needUpdate){
			if (!hasMultipleCameras){
				updateSkyViewProjection(*sky, mainCamera);
			}
		}

		if (!sky->loaded){
			loadSky(*sky);
		}
	}

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
		}else if (signature.test(scene->getComponentType<TerrainComponent>())){
			TerrainComponent& terrain = scene->getComponent<TerrainComponent>(entity);
			if (terrain.loaded && terrain.needReload){
				destroyTerrain(terrain);
			}
			if (!terrain.loaded){
				loadTerrain(terrain);
			}
		}else if (signature.test(scene->getComponentType<UIComponent>())){
			UIComponent& ui = scene->getComponent<UIComponent>(entity);
			if (!ui.loaded){
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
			}
		}else if (signature.test(scene->getComponentType<ParticlesComponent>())){
			ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);
			if (particles.loaded && particles.needReload){
				destroyParticles(particles);
			}
			if (!particles.loaded){
				loadParticles(particles);
			}
		}

		if (mainCamera.needUpdate || transform.needUpdate){

			// need to be updated for every camera
			if (!hasMultipleCameras){
				updateMVP(i, transform, mainCamera, mainCameraTransform);

				if (signature.test(scene->getComponentType<TerrainComponent>())){
					TerrainComponent& terrain = scene->getComponent<TerrainComponent>(entity);

					updateTerrain(terrain, transform, mainCamera, mainCameraTransform);
				}
			}

			// need to be updated ONLY for main camera
			if (signature.test(scene->getComponentType<LightComponent>())){
				LightComponent& light = scene->getComponent<LightComponent>(entity);
				
				updateLightFromScene(light, transform, mainCamera);
			}
			
			if (signature.test(scene->getComponentType<AudioComponent>())){
				AudioComponent& audio = scene->getComponent<AudioComponent>(entity);

				audio.needUpdate = true;
			}

		}

		if (transform.needUpdate){

			if (signature.test(scene->getComponentType<ModelComponent>())){
				ModelComponent& model = scene->getComponent<ModelComponent>(entity);

				model.inverseDerivedTransform = transform.modelMatrix.inverse();
			}

			if (signature.test(scene->getComponentType<BoneComponent>())){
				BoneComponent& bone = scene->getComponent<BoneComponent>(entity);

				if (bone.model != NULL_ENTITY){
					ModelComponent& model = scene->getComponent<ModelComponent>(bone.model);
					MeshComponent& mesh = scene->getComponent<MeshComponent>(bone.model);

					Matrix4 skinning = model.inverseDerivedTransform * transform.modelMatrix * bone.offsetMatrix;

					if (bone.index >= 0 && bone.index < MAX_BONES)
						mesh.bonesMatrix[bone.index] = skinning;
				}
			}

		}

        if (signature.test(scene->getComponentType<ParticlesComponent>())){
			ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);

			bool sortTransparentParticles = particles.transparent && mainCamera.type != CameraType::CAMERA_2D;

			if (particles.needUpdate || ((mainCamera.needUpdate || transform.needUpdate) && sortTransparentParticles)){
				if (!hasMultipleCameras || !sortTransparentParticles){
					updateParticles(particles, transform, mainCamera, mainCameraTransform, sortTransparentParticles);
				}
			}

			particles.needUpdate = false;
		}

		transform.needUpdate = false;
	}

	for (int i = 0; i < cameras->size(); i++){
		CameraComponent& camera = cameras->getComponentFromIndex(i);
		if (camera.needUpdate){
			camera.needUpdate = false;
		}
	}

	processLights(mainCameraTransform);
}

void RenderSystem::draw(){
	auto transforms = scene->getComponentArray<Transform>();
	auto cameras = scene->getComponentArray<CameraComponent>();

	//---------Depth shader----------
	if (hasShadows){
		auto lights = scene->getComponentArray<LightComponent>();
		auto meshes = scene->getComponentArray<MeshComponent>();
		auto terrains = scene->getComponentArray<TerrainComponent>();

		depthRender.setClearColor(Vector4(1.0, 1.0, 1.0, 1.0));
		
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
							if (transform->visible){
								drawMeshDepth(mesh, {transform->modelMatrix, light.cameras[c].lightViewProjectionMatrix});
							}
						}
					}
					for (int i = 0; i < terrains->size(); i++){
						TerrainComponent& terrain = terrains->getComponentFromIndex(i);
						Entity entity = terrains->getEntity(i);
						Transform* transform = scene->findComponent<Transform>(entity);

						if (transform){
							if (transform->visible){
								drawTerrainDepth(terrain, {transform->modelMatrix, light.cameras[c].lightViewProjectionMatrix});
							}
						}
					}
					depthRender.endFrameBuffer();
				}
			}
		}
	}

	for (int i = 0; i < cameras->size(); i++){
		Entity cameraEntity = cameras->getEntity(i);
		CameraComponent& camera = cameras->getComponentFromIndex(i);
		Transform& cameraTransform = scene->getComponent<Transform>(cameraEntity);

		bool isMainCamera = (cameraEntity == scene->getCamera());

		if (!isMainCamera && !camera.renderToTexture){
			continue; // camera is not used
		}

		if (scene->isMainScene() || camera.renderToTexture){
			sceneRender.setClearColor(scene->getBackgroundColor());
		}
		
		if (!camera.renderToTexture){
			sceneRender.startDefaultFrameBuffer(System::instance().getScreenWidth(), System::instance().getScreenHeight());
			sceneRender.applyViewport(Engine::getViewRect());
		}else{
			if (!camera.framebuffer->isCreated()){
				createFramebuffer(camera);
			}
			sceneRender.startFrameBuffer(&camera.framebuffer->getRender());
		}

		//---------Draw opaque meshes and UI----------
		bool hasActiveScissor = false;

		//---------Draw sky----------
		SkyComponent* sky = scene->findComponentFromIndex<SkyComponent>(0);
		if (sky){
			if (hasMultipleCameras){
				updateSkyViewProjection(*sky, camera);
			}

			if (!sky->loaded){
				loadSky(*sky);
			}

			drawSky(*sky, camera.renderToTexture);
		}

		for (int i = 0; i < transforms->size(); i++){
			Transform& transform = transforms->getComponentFromIndex(i);
			Entity entity = transforms->getEntity(i);
			Signature signature = scene->getSignature(entity);

			if (signature.test(scene->getComponentType<CameraComponent>())){
				continue;
			}

			if (hasMultipleCameras){
				updateMVP(i, transform, camera, cameraTransform);
			}

			// apply scissor on UI
			if (signature.test(scene->getComponentType<UILayoutComponent>())){
				UILayoutComponent& layout = scene->getComponent<UILayoutComponent>(entity);

				Rect parentScissor;
				bool hasParentScissor = false;

				if (transform.parent != NULL_ENTITY){
					Signature parentSignature = scene->getSignature(transform.parent);
					if (parentSignature.test(scene->getComponentType<UILayoutComponent>())){
						UILayoutComponent& parentLayout = scene->getComponent<UILayoutComponent>(transform.parent);

						parentScissor = parentLayout.scissor;
						if (!parentScissor.isZero()){
							if (!layout.ignoreScissor){
								sceneRender.applyScissor(parentScissor);
								hasActiveScissor = true;
							}
							layout.scissor = parentScissor;
							hasParentScissor = true;
						}
					}
				}

				if (signature.test(scene->getComponentType<ImageComponent>())){
					ImageComponent& img = scene->getComponent<ImageComponent>(entity);

					layout.scissor = getScissorRect(layout, img, transform, camera);
					if (hasParentScissor){
						layout.scissor = layout.scissor.fitOnRect(parentScissor);
					}
				}
			}

			if (signature.test(scene->getComponentType<MeshComponent>())){
				MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

				if (transform.visible){
					if (!mesh.transparent || !camera.transparentSort){
						//Draw opaque meshes if transparency is not necessary
						drawMesh(mesh, transform, cameraTransform, camera.renderToTexture);
					}else{
						transparentMeshes.push({&mesh, &transform, transform.distanceToCamera});
					}
				}

			}else if (signature.test(scene->getComponentType<TerrainComponent>())){
				TerrainComponent& terrain = scene->getComponent<TerrainComponent>(entity);

				if (hasMultipleCameras){
					updateTerrain(terrain, transform, camera, cameraTransform);
				}

				if (transform.visible){
					drawTerrain(terrain, transform, cameraTransform, camera.renderToTexture);
				}

			}else if (signature.test(scene->getComponentType<UIComponent>())){
				UIComponent& ui = scene->getComponent<UIComponent>(entity);

				bool isText = false;
				if (signature.test(scene->getComponentType<TextComponent>())){
					isText = true;
				}
				if (transform.visible)
					drawUI(ui, transform, camera.renderToTexture);

			}else if (signature.test(scene->getComponentType<ParticlesComponent>())){
				ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);

				bool sortTransparentParticles = particles.transparent && camera.type != CameraType::CAMERA_2D;

				if (hasMultipleCameras && sortTransparentParticles){
					updateParticles(particles, transform, camera, cameraTransform, sortTransparentParticles);
				}

				if (transform.visible)
					drawParticles(particles, transform, cameraTransform, camera.renderToTexture);

			}

			if (hasActiveScissor){
				sceneRender.applyScissor(Rect(0, 0, System::instance().getScreenWidth(), System::instance().getScreenHeight()));
				hasActiveScissor = false;
			}
		}

		//---------Draw transparent meshes----------
		while (!transparentMeshes.empty()){
			TransparentMeshesData meshData = transparentMeshes.top();

			//Draw transparent meshes
			drawMesh(*meshData.mesh, *meshData.transform, cameraTransform, camera.renderToTexture);

			transparentMeshes.pop();
		}

		sceneRender.endFrameBuffer();

	}

	//---------Missing some shaders----------
	if (ShaderPool::getMissingShaders().size() > 0){
		std::string misShaders;
		for (int i = 0; i < ShaderPool::getMissingShaders().size(); i++){
			if (!misShaders.empty())
				misShaders += "; ";
			misShaders += ShaderPool::getMissingShaders()[i];
		}
		Log::verbose(
			"\n"
			"-------------------\n"
			"Supernova is missing some shaders, you need to use Supershader tool to create these shaders in project assets directory.\n"
			"Go to directory \"tools/supershader\" and execute the command:\n"
			"\n"
			"> python3 supershader.py -s \"%s\" -l %s\n"
			"-------------------"
			, misShaders.c_str(), ShaderPool::getShaderLangStr().c_str());
		exit(1);
	}
}

void RenderSystem::entityDestroyed(Entity entity){
	Signature signature = scene->getSignature(entity);

	//TODO: Destroy lights?

	if (signature.test(scene->getComponentType<MeshComponent>())){
		destroyMesh(scene->getComponent<MeshComponent>(entity));
	}

	if (signature.test(scene->getComponentType<TerrainComponent>())){
		destroyTerrain(scene->getComponent<TerrainComponent>(entity));
	}

	if (signature.test(scene->getComponentType<UIComponent>())){
		destroyUI(scene->getComponent<UIComponent>(entity));
	}

	if (signature.test(scene->getComponentType<ParticlesComponent>())){
		destroyParticles(scene->getComponent<ParticlesComponent>(entity));
	}

	if (signature.test(scene->getComponentType<SkyComponent>())){
		destroySky(scene->getComponent<SkyComponent>(entity));
	}

	if (signature.test(scene->getComponentType<CameraComponent>())){
		CameraComponent& camera = scene->getComponent<CameraComponent>(entity);
		if (camera.framebuffer){
			delete camera.framebuffer; // destroy render is in destructor
		}
	}
}
