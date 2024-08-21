//
// (c) 2024 Eduardo Doria.
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
#include "math/AABB.h"
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
}

RenderSystem::~RenderSystem(){
}

void RenderSystem::load(){
	hasLights = false;
	hasShadows = false;
	hasFog = false;
	hasMultipleCameras = false;

	createEmptyTextures();
		
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
	emptyWhite.destroyTexture();
	emptyBlack.destroyTexture();
	emptyCubeBlack.destroyTexture();
	emptyNormal.destroyTexture();

	emptyTexturesCreated = false;

	auto skys = scene->getComponentArray<SkyComponent>();
	if (skys->size() > 0){
		SkyComponent& sky = skys->getComponentFromIndex(0);
		Entity entity = skys->getEntity(0);
		if (sky.loaded){
			destroySky(entity, sky);
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
				destroyMesh(entity, mesh);
			}
		}else if (signature.test(scene->getComponentType<UIComponent>())){
			UIComponent& ui = scene->getComponent<UIComponent>(entity);
			if (ui.loaded){
				destroyUI(entity, ui);
			}
		}else if (signature.test(scene->getComponentType<PointsComponent>())){
			PointsComponent& points = scene->getComponent<PointsComponent>(entity);
			if (points.loaded){
				destroyPoints(entity, points);
			}
		}else if (signature.test(scene->getComponentType<LinesComponent>())){
			LinesComponent& lines = scene->getComponent<LinesComponent>(entity);
			if (lines.loaded){
				destroyLines(entity, lines);
			}
		}else if (signature.test(scene->getComponentType<LightComponent>())){
			LightComponent& light = scene->getComponent<LightComponent>(entity);
			destroyLight(light);
		}else if (signature.test(scene->getComponentType<CameraComponent>())){
			CameraComponent& camera = scene->getComponent<CameraComponent>(entity);
			destroyCamera(camera, false);
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

int RenderSystem::checkLightsAndShadow(){
	hasLights = false;
	hasShadows = false;

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

	return numLights;
}

bool RenderSystem::loadLights(int numLights){
	int freeShadowMap = 0;
	int freeShadowCubeMap = MAX_SHADOWSMAP;

	auto lights = scene->getComponentArray<LightComponent>();
	
	for (int i = 0; i < numLights; i++){
		LightComponent& light = lights->getComponentFromIndex(i);

		light.shadowMapIndex = -1;
		
		if (light.shadows){
			if (light.numShadowCascades > MAX_SHADOWCASCADES){
				light.numShadowCascades = MAX_SHADOWCASCADES;
				Log::warn("Shadow cascades number is bigger than max value");
			}

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

void RenderSystem::loadPBRTextures(Material& material, ShaderData& shaderData, ObjectRender& render, bool shadows){
	TextureRender* textureRender = NULL;
	std::pair<int, int> slotTex(-1, -1);

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

		if (hasShadows && shadows){
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

void RenderSystem::loadDepthTexture(Material& material, ShaderData& shaderData, ObjectRender& render){
	TextureRender* textureDepthRender = material.baseColorTexture.getRender();
	std::pair<int, int> slotTex = shaderData.getTextureIndex(TextureShaderType::DEPTHTEXTURE, ShaderStageType::FRAGMENT);
	if (textureDepthRender)
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureDepthRender);
	else
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);
}

void RenderSystem::loadTerrainTextures(TerrainComponent& terrain, ObjectRender& render, ShaderData& shaderData){
	TextureRender* textureRender = NULL;
	std::pair<int, int> slotTex(-1, -1);

	textureRender = terrain.heightMap.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::HEIGHTMAP, ShaderStageType::VERTEX);
	if (textureRender)
		render.addTexture(slotTex, ShaderStageType::VERTEX, textureRender);
	else
		render.addTexture(slotTex, ShaderStageType::VERTEX, &emptyWhite);

	textureRender = terrain.blendMap.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::BLENDMAP, ShaderStageType::FRAGMENT);
	if (textureRender)
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyBlack);

	textureRender = terrain.textureDetailRed.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::TERRAINDETAIL_RED, ShaderStageType::FRAGMENT);
	if (textureRender)
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

	textureRender = terrain.textureDetailGreen.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::TERRAINDETAIL_GREEN, ShaderStageType::FRAGMENT);
	if (textureRender)
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

	textureRender = terrain.textureDetailBlue.getRender();
	slotTex = shaderData.getTextureIndex(TextureShaderType::TERRAINDETAIL_BLUE, ShaderStageType::FRAGMENT);
	if (textureRender)
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
	else
		render.addTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);
}

bool RenderSystem::loadMesh(Entity entity, MeshComponent& mesh, uint8_t pipelines, InstancedMeshComponent* instmesh, TerrainComponent* terrain){

	if (!Engine::isViewLoaded()) 
		return false;

	if (terrain && !terrain->heightMapLoaded)
		return false;

	if (terrain && instmesh){
		Log::warn("Terrain cannot be an InstancedMesh");
	}

	std::map<std::string, Buffer*> buffers;
	std::map<std::string, BufferRender*> bufferNameToRender;
	bool allBuffersEmpty = true;

	if (mesh.buffer.getSize() > 0){
		buffers["vertices"] = &mesh.buffer;
		allBuffersEmpty = false;
		if (mesh.buffer.getUsage() != BufferUsage::IMMUTABLE){
			mesh.needUpdateBuffer = true;
		}
	}
	if (mesh.indices.getSize() > 0){
		buffers["indices"] = &mesh.indices;
		allBuffersEmpty = false;
		if (mesh.indices.getUsage() != BufferUsage::IMMUTABLE){
			mesh.needUpdateBuffer = true;
		}
	}
	for (int i = 0; i < mesh.numExternalBuffers; i++){
		buffers[mesh.eBuffers[i].getName()] = &mesh.eBuffers[i];
		allBuffersEmpty = false;
		if (mesh.eBuffers[i].getUsage() != BufferUsage::IMMUTABLE){
			mesh.needUpdateBuffer = true;
		}
	}

	if (mesh.vertexCount == 0){
		mesh.vertexCount = mesh.buffer.getCount();
	}

	if (allBuffersEmpty)
		return false;

	std::map<std::string, unsigned int> bufferStride;

	for (auto const& buf : buffers){
		buf.second->getRender()->createBuffer(buf.second->getSize(), buf.second->getData(), buf.second->getType(), buf.second->getUsage());
		bufferNameToRender[buf.first] = buf.second->getRender();
		bufferStride[buf.first] = buf.second->getStride();
	}

	if (instmesh){
		// Now buffer size is zero than it needed to be calculated
		size_t bufferSize = instmesh->maxInstances * instmesh->buffer.getStride();
		instmesh->buffer.getRender()->createBuffer(bufferSize, instmesh->buffer.getData(), instmesh->buffer.getType(), instmesh->buffer.getUsage());

		instmesh->needUpdateBuffer = true;
	}

	if (terrain){
		for (int s = 0; s < 2; s++){
			size_t bufferSize = pow(terrain->rootGridSize, 2) * pow(terrain->levels, 2) * terrain->nodesbuffer[s].getStride() / 2;
			terrain->nodesbuffer[s].getRender()->createBuffer(bufferSize, terrain->nodesbuffer[s].getData(), terrain->nodesbuffer[s].getType(), terrain->nodesbuffer[s].getUsage());
		}

		// using same material in both terrain submeshes
		mesh.submeshes[1].material = mesh.submeshes[0].material;
	}

	for (int i = 0; i < mesh.numSubmeshes; i++){

		ObjectRender& render = mesh.submeshes[i].render;

		mesh.submeshes[i].hasNormalMap = false;
		mesh.submeshes[i].hasTangent = false;
		mesh.submeshes[i].hasVertexColor4 = false;
		mesh.submeshes[i].hasSkinning = false;
		mesh.submeshes[i].hasMorphTarget = false;
		mesh.submeshes[i].hasMorphNormal = false;

		render.beginLoad(mesh.submeshes[i].primitiveType);

		for (auto const& buf : buffers){
        	if (buf.second->isRenderAttributes()) {
            	for (auto const &attr : buf.second->getAttributes()) {
					if (attr.first == AttributeType::TEXCOORD1){
						mesh.submeshes[i].hasTexCoord1 = true;
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
				mesh.submeshes[i].hasTexCoord1 = true;
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
		bool p_receiveShadows = false;
		bool p_shadowsPCF = false;

		if (mesh.submeshes[i].hasTexCoord1 || mesh.submeshes[i].material.baseColorTexture.getRender()){
			p_hasTexture1 = true;
		}
		if (terrain && terrain->blendMap.getRender()){
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
			if (hasShadows && mesh.receiveShadows){
				p_receiveShadows = true;
				if (scene->isShadowsPCF()){
					p_shadowsPCF = true;
				}
			}
		}else{
			p_unlit = true;
		}

		mesh.submeshes[i].shaderProperties = ShaderPool::getMeshProperties(
						p_unlit, p_hasTexture1, false, p_punctual, 
						p_receiveShadows, p_shadowsPCF, p_hasNormal, p_hasNormalMap, 
						p_hasTangent, false, mesh.submeshes[i].hasVertexColor4, mesh.submeshes[i].hasTextureRect, 
						hasFog, mesh.submeshes[i].hasSkinning, mesh.submeshes[i].hasMorphTarget, mesh.submeshes[i].hasMorphNormal, mesh.submeshes[i].hasMorphTangent,
						(terrain)?true:false, (instmesh)?true:false);
		mesh.submeshes[i].shader = ShaderPool::get(ShaderType::MESH, mesh.submeshes[i].shaderProperties);
		if (hasShadows && mesh.castShadows){
			mesh.submeshes[i].depthShaderProperties = ShaderPool::getDepthMeshProperties(
				mesh.submeshes[i].hasDepthTexture, mesh.submeshes[i].hasSkinning, mesh.submeshes[i].hasMorphTarget, 
				mesh.submeshes[i].hasMorphNormal, mesh.submeshes[i].hasMorphTangent, false, (instmesh)?true:false);
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
			if (hasShadows && mesh.receiveShadows){
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

		loadPBRTextures(mesh.submeshes[i].material, shaderData, mesh.submeshes[i].render, mesh.receiveShadows);

		if (terrain){
			mesh.submeshes[i].slotVSTerrain = shaderData.getUniformBlockIndex(UniformBlockType::TERRAIN_VS_PARAMS, ShaderStageType::VERTEX);

			loadTerrainTextures(*terrain, mesh.submeshes[i].render, shaderData);

			terrain->needUpdateTexture = false;

			for (auto const &attr : terrain->nodesbuffer[i].getAttributes()) {
				render.addAttribute(shaderData.getAttrIndex(attr.first), terrain->nodesbuffer[i].getRender(), attr.second.getElements(), attr.second.getDataType(), terrain->nodesbuffer[i].getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
			}
		}

		mesh.submeshes[i].needUpdateTexture = false;

		if (Engine::isAutomaticTransparency() && !mesh.transparent){
			if (mesh.submeshes[i].material.baseColorTexture.isTransparent() || mesh.submeshes[i].material.baseColorFactor.w != 1.0){
				mesh.transparent = true;
			}
		}
	
		unsigned int indexCount = 0;

		for (auto const& buf : buffers){
        	if (buf.second->isRenderAttributes()) {
				if (buf.second->getType() == BufferType::INDEX_BUFFER){
					indexCount = buf.second->getCount();
					Attribute indexattr = buf.second->getAttributes()[AttributeType::INDEX];
					render.addIndex(buf.second->getRender(), indexattr.getDataType(), indexattr.getOffset());
				}else{
					for (auto const &attr : buf.second->getAttributes()) {
						render.addAttribute(shaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
					}
				}
        	}
    	}

		for (auto const& attr : mesh.submeshes[i].attributes){
			if (bufferNameToRender.count(attr.second.getBuffer())){
				if (attr.first == AttributeType::INDEX){
					indexCount = attr.second.getCount();
					render.addIndex(bufferNameToRender[attr.second.getBuffer()], attr.second.getDataType(), attr.second.getOffset());
				}else{
					render.addAttribute(shaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
				}
			}else{
				Log::error("Cannot load submesh attribute from buffer name: %s", attr.second.getBuffer().c_str());
			}
		}

		if (instmesh){
			for (auto const &attr : instmesh->buffer.getAttributes()) {
				render.addAttribute(shaderData.getAttrIndex(attr.first), instmesh->buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), instmesh->buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
			}
		}

		if (indexCount > 0){
			mesh.submeshes[i].vertexCount = indexCount;
		}else{
			mesh.submeshes[i].vertexCount = mesh.vertexCount;
		}

		if (!render.endLoad(pipelines)){
			return false;
		}

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

			if (mesh.submeshes[i].hasDepthTexture){
				loadDepthTexture(mesh.submeshes[i].material, depthShaderData, depthRender);
			}

			if (terrain){
				mesh.submeshes[i].slotVSDepthTerrain = depthShaderData.getUniformBlockIndex(UniformBlockType::DEPTH_TERRAIN_VS_PARAMS, ShaderStageType::VERTEX);

				for (auto const &attr : terrain->nodesbuffer[i].getAttributes()) {
					depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), terrain->nodesbuffer[i].getRender(), attr.second.getElements(), attr.second.getDataType(), terrain->nodesbuffer[i].getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
				}
			}

			for (auto const& buf : buffers){
        		if (buf.second->isRenderAttributes()) {

					if (buf.second->getType() == BufferType::INDEX_BUFFER){
						indexCount = buf.second->getCount();
						Attribute indexattr = buf.second->getAttributes()[AttributeType::INDEX];
						depthRender.addIndex(buf.second->getRender(), indexattr.getDataType(), indexattr.getOffset());
					}else{
						for (auto const &attr : buf.second->getAttributes()){
							if (attr.first == AttributeType::POSITION || attr.first == AttributeType::TEXCOORD1){
								depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
							}
							if (mesh.submeshes[i].hasSkinning){
								if (attr.first == AttributeType::BONEIDS || attr.first == AttributeType::BONEWEIGHTS){
									depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
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
									depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
								}
							}
						}
					}
        		}
    		}

			for (auto const& attr : mesh.submeshes[i].attributes){
				if (bufferNameToRender.count(attr.second.getBuffer())){
					if (attr.first == AttributeType::INDEX){
						depthRender.addIndex(bufferNameToRender[attr.second.getBuffer()], attr.second.getDataType(), attr.second.getOffset());
					}else if (attr.first == AttributeType::POSITION){
						depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
					}
					if (mesh.submeshes[i].hasSkinning){
						if (attr.first == AttributeType::BONEIDS || attr.first == AttributeType::BONEWEIGHTS){
							depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
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
							depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
						}
					}
				}else{
					Log::error("Cannot load (depth) submesh attribute from buffer name: %s", attr.second.getBuffer().c_str());
				}
			}

			if (instmesh){
				for (auto const &attr : instmesh->buffer.getAttributes()) {
					depthRender.addAttribute(depthShaderData.getAttrIndex(attr.first), instmesh->buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), instmesh->buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
				}
			}

			if (!depthRender.endLoad(PIP_DEPTH)){
				return false;
			}
		}
		//----------End depth shader---------------
	}

	mesh.needReload = false;
	mesh.loadCalled = true;
	SystemRender::addQueueCommand(&changeLoaded, new check_load_t{scene, entity});

	return true;
}

bool RenderSystem::drawMesh(MeshComponent& mesh, Transform& transform, CameraComponent& camera, Transform& camTransform, bool renderToTexture, InstancedMeshComponent* instmesh, TerrainComponent* terrain){
	if (mesh.loaded){

		if (!terrain && mesh.worldAABB != AABB::ZERO && !isInsideCamera(camera, mesh.worldAABB)) {
			return false;
		}

		if (mesh.needUpdateBuffer){
			if (mesh.buffer.getUsage() != BufferUsage::IMMUTABLE)
				mesh.buffer.getRender()->updateBuffer(mesh.buffer.getSize(), mesh.buffer.getData());
			if (mesh.indices.getUsage() != BufferUsage::IMMUTABLE)
				mesh.indices.getRender()->updateBuffer(mesh.indices.getSize(), mesh.indices.getData());
			for (int i = 0; i < mesh.numExternalBuffers; i++){
				if (mesh.eBuffers[i].getUsage() != BufferUsage::IMMUTABLE)
					mesh.eBuffers[i].getRender()->updateBuffer(mesh.eBuffers[i].getSize(), mesh.eBuffers[i].getData());
			}

			mesh.needUpdateBuffer = false;
		}
		unsigned int instanceCount = 1;
		if (instmesh){
			instanceCount = instmesh->numVisible;

			if (instmesh->needUpdateBuffer){
				instmesh->buffer.getRender()->updateBuffer(instmesh->buffer.getSize(), instmesh->buffer.getData());

				instmesh->needUpdateBuffer = false;
			}
		}

		if (terrain && terrain->needUpdateNodesBuffer){
			for (int s = 0; s < 2; s++){
				terrain->nodesbuffer[s].getRender()->updateBuffer(terrain->nodesbuffer[s].getSize(), terrain->nodesbuffer[s].getData());
			}

			terrain->needUpdateNodesBuffer = false;
		}

		if (terrain && terrain->needUpdateTerrain){
			for (int s = 0; s < 2; s++){
				ShaderData& shaderData = mesh.submeshes[s].shader.get()->shaderData;
				loadTerrainTextures(*terrain, mesh.submeshes[s].render, shaderData);
			}

			terrain->needUpdateTerrain = false;
		}

		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender& render = mesh.submeshes[i].render;

			if (terrain){
				instanceCount = terrain->nodesbuffer[i].getCount();
			}

			bool needUpdateFramebuffer = checkPBRFrabebufferUpdate(mesh.submeshes[i].material);

			if (mesh.submeshes[i].needUpdateTexture || needUpdateFramebuffer){
				ShaderData& shaderData = mesh.submeshes[i].shader.get()->shaderData;
				loadPBRTextures(mesh.submeshes[i].material, shaderData, mesh.submeshes[i].render, mesh.receiveShadows);

				if (mesh.submeshes[i].hasDepthTexture){
					ShaderData& depthShaderData = mesh.submeshes[i].depthShader.get()->shaderData;
					loadDepthTexture(mesh.submeshes[i].material, depthShaderData, mesh.submeshes[i].depthRender);
				}

				mesh.submeshes[i].needUpdateTexture = false;
			}

			if (scene->isSceneAmbientLightEnabled()){
				mesh.submeshes[i].material.ambientFactor = scene->getAmbientLightFactor();
				mesh.submeshes[i].material.ambientLight = scene->getAmbientLightColor();
			}

			if (!render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT)){
				mesh.needReload = true;
				return false;
			}

			if (hasFog){
				render.applyUniformBlock(mesh.submeshes[i].slotFSFog, ShaderStageType::FRAGMENT, sizeof(float) * 8, &fs_fog);
			}

			if (hasLights){
				render.applyUniformBlock(mesh.submeshes[i].slotFSLighting, ShaderStageType::FRAGMENT, sizeof(float) * (16 * MAX_LIGHTS + 4), &fs_lighting);
				if (hasShadows && mesh.receiveShadows){
					render.applyUniformBlock(mesh.submeshes[i].slotVSShadows, ShaderStageType::VERTEX, sizeof(float) * (16 * MAX_SHADOWSMAP), &vs_shadows);
					render.applyUniformBlock(mesh.submeshes[i].slotFSShadows, ShaderStageType::FRAGMENT, sizeof(float) * (4 * (MAX_SHADOWSMAP + MAX_SHADOWSCUBEMAP)), &fs_shadows);
				}
			}

			if (mesh.submeshes[i].hasTextureRect){
				render.applyUniformBlock(mesh.submeshes[i].slotVSSprite, ShaderStageType::VERTEX, sizeof(float) * 4, &mesh.submeshes[i].textureRect);
			}

			if (mesh.submeshes[i].hasSkinning){
				render.applyUniformBlock(mesh.submeshes[i].slotVSSkinning, ShaderStageType::VERTEX, sizeof(float) * 16 * MAX_BONES + (sizeof(float) * 4), &mesh.bonesMatrix);
			}

			if (mesh.submeshes[i].hasMorphTarget){
				if (!mesh.submeshes[i].hasMorphNormal && !mesh.submeshes[i].hasMorphTangent){
					render.applyUniformBlock(mesh.submeshes[i].slotVSMorphTarget, ShaderStageType::VERTEX, sizeof(float) * MAX_MORPHTARGETS, &mesh.morphWeights);
				}else{
					render.applyUniformBlock(mesh.submeshes[i].slotVSMorphTarget, ShaderStageType::VERTEX, sizeof(float) * MAX_MORPHTARGETS / 2, &mesh.morphWeights);
				}
			}

			if (hasLights){
				render.applyUniformBlock(mesh.submeshes[i].slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 16, &mesh.submeshes[i].material);
			}else{
				render.applyUniformBlock(mesh.submeshes[i].slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 4, &mesh.submeshes[i].material);
			}

			if (terrain){
				render.applyUniformBlock(mesh.submeshes[i].slotVSTerrain, ShaderStageType::VERTEX, sizeof(float) * 8, &(terrain->eyePos));
			}

			//model, normal and mvp matrix
			render.applyUniformBlock(mesh.submeshes[i].slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 48, &transform.modelMatrix);

			render.draw(mesh.submeshes[i].vertexCount, instanceCount);
		}
	}

	return true;
}

bool RenderSystem::drawMeshDepth(MeshComponent& mesh, const float cameraFar, const Plane frustumPlanes[6], vs_depth_t vsDepthParams, InstancedMeshComponent* instmesh, TerrainComponent* terrain){
	if (mesh.loaded && mesh.castShadows){

		if (mesh.worldAABB != AABB::ZERO && !isInsideCamera(cameraFar, frustumPlanes, mesh.worldAABB)) {
			return false;
		}

		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender& depthRender = mesh.submeshes[i].depthRender;

			if (!depthRender.beginDraw(PIP_DEPTH)){
				mesh.needReload = true;
				return false;
			}

			unsigned int instanceCount = 1;
			if (instmesh){
				instanceCount = instmesh->numVisible;
			}

			//model, mvp matrix
			depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthParams, ShaderStageType::VERTEX, sizeof(float) * 32, &vsDepthParams);

			if (mesh.submeshes[i].hasSkinning){
				depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthSkinning, ShaderStageType::VERTEX, sizeof(float) * 16 * MAX_BONES + (sizeof(float) * 4), &mesh.bonesMatrix);
			}
			if (mesh.submeshes[i].hasMorphTarget){
				if (!mesh.submeshes[i].hasMorphNormal && !mesh.submeshes[i].hasMorphTangent){
					depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthMorphTarget, ShaderStageType::VERTEX, sizeof(float) * MAX_MORPHTARGETS, &mesh.morphWeights);
				}else{
					depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthMorphTarget, ShaderStageType::VERTEX, sizeof(float) * MAX_MORPHTARGETS / 2, &mesh.morphWeights);
				}
			}

			if (terrain){
				depthRender.applyUniformBlock(mesh.submeshes[i].slotVSDepthTerrain, ShaderStageType::VERTEX, sizeof(float) * 8, &(terrain->eyePos));
			}

			depthRender.draw(mesh.submeshes[i].vertexCount, instanceCount);
		}
	}

	return true;
}

void RenderSystem::destroyMesh(Entity entity, MeshComponent& mesh){
	InstancedMeshComponent* instmesh = scene->findComponent<InstancedMeshComponent>(entity);
	TerrainComponent* terrain = scene->findComponent<TerrainComponent>(entity);

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

		if (terrain){
			//Destroy terrain texture
			terrain->heightMap.destroy();
			terrain->blendMap.destroy();
			terrain->textureDetailRed.destroy();
			terrain->textureDetailGreen.destroy();
			terrain->textureDetailBlue.destroy();

			//Destroy terrain buffer
			for (int s = 0; s < 2; s++){
				terrain->nodesbuffer[s].getRender()->destroyBuffer();
			}
		}

		if (instmesh){
			//Destroy instmesh buffer
			instmesh->buffer.getRender()->destroyBuffer();
		}

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
		submesh.slotVSTerrain = -1;

		submesh.slotVSDepthParams = -1;
		submesh.slotVSDepthSkinning = -1;
		submesh.slotVSDepthMorphTarget = -1;
		submesh.slotVSDepthTerrain = -1;
	}

	//Destroy buffer
	//mesh.buffer.clearAll();
	mesh.buffer.getRender()->destroyBuffer();
	//mesh.indices.clearAll();
	mesh.indices.getRender()->destroyBuffer();
	for (int i = 0; i < mesh.numExternalBuffers; i++){
		//mesh.eBuffers[i].clearAll();
		mesh.eBuffers[i].getRender()->destroyBuffer();
	}

	SystemRender::addQueueCommand(&changeDestroy, new check_load_t{scene, entity});
}

bool RenderSystem::loadUI(Entity entity, UIComponent& uirender, uint8_t pipelines, bool isText){

	if (!Engine::isViewLoaded()) 
		return false;

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
			render.addAttribute(shaderData.getAttrIndex(attr.first), uirender.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), uirender.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
        }
    }
	if (uirender.buffer.getUsage() != BufferUsage::IMMUTABLE){
		uirender.needUpdateBuffer = true;
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
		if (uirender.indices.getUsage() != BufferUsage::IMMUTABLE){
			uirender.needUpdateBuffer = true;
		}
	}else{
		uirender.vertexCount = uirender.buffer.getCount();
	}

	if (textureRender)
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::UI, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);
	
	uirender.needUpdateTexture = false;

	if (!render.endLoad(pipelines)){
		return false;
	}

	uirender.needReload = false;
	uirender.loadCalled = true;
	SystemRender::addQueueCommand(&changeLoaded, new check_load_t{scene, entity});

	return true;
}

bool RenderSystem::drawUI(UIComponent& uirender, Transform& transform, bool renderToTexture){
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

		if (!render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT)){
			uirender.needReload = true;
			return false;
		}
		render.applyUniformBlock(uirender.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &transform.modelViewProjectionMatrix);
		//Color
		render.applyUniformBlock(uirender.slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 4, &uirender.color);
		render.draw(uirender.vertexCount, 1);

	}

	return true;
}

void RenderSystem::destroyUI(Entity entity, UIComponent& uirender){
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
		//uirender.buffer.clearAll();
		uirender.buffer.getRender()->destroyBuffer();
	}
	if (uirender.indices.getSize() > 0){
		//uirender.indices.clearAll();
		uirender.indices.getRender()->destroyBuffer();
	}

	//Shaders uniforms
	uirender.slotVSParams = -1;
	uirender.slotFSParams = -1;

	SystemRender::addQueueCommand(&changeDestroy, new check_load_t{scene, entity});
}

bool RenderSystem::loadPoints(Entity entity, PointsComponent& points, uint8_t pipelines){

	if (!Engine::isViewLoaded()) 
		return false;

	ObjectRender& render = points.render;

	render.beginLoad(PrimitiveType::POINTS);

	TextureRender* textureRender = points.texture.getRender();

	if (Engine::isAutomaticTransparency() && !points.transparent){
		if (points.texture.isTransparent()){ // Particle color is not tested here
			points.transparent = true;
		}
	}

	bool p_hasTexture = false;
	bool p_hasTextureRect = false;
	if (textureRender){
		p_hasTexture = true;
		if (points.hasTextureRect){
			p_hasTextureRect = true;
		}
	}

	points.shaderProperties = ShaderPool::getPointsProperties(p_hasTexture, false, true, p_hasTextureRect);
	points.shader = ShaderPool::get(ShaderType::POINTS, points.shaderProperties);
	if (!points.shader->isCreated())
		return false;
	render.addShader(points.shader.get());
	ShaderData& shaderData = points.shader.get()->shaderData;

	points.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::POINTS_VS_PARAMS, ShaderStageType::VERTEX);

	points.buffer.clear();
	points.buffer.addAttribute(AttributeType::POSITION, 3, 0);
	points.buffer.addAttribute(AttributeType::COLOR, 4, 3 * sizeof(float));
	points.buffer.addAttribute(AttributeType::POINTSIZE, 1, 7 * sizeof(float));
	points.buffer.addAttribute(AttributeType::POINTROTATION, 1, 8 * sizeof(float));
	points.buffer.addAttribute(AttributeType::TEXTURERECT, 4, 9 * sizeof(float));
	points.buffer.setStride(13 * sizeof(float));
	points.buffer.setRenderAttributes(true);
	points.buffer.setUsage(BufferUsage::STREAM);

	// Now buffer size is zero than it needed to be calculated
	size_t bufferSize = points.maxPoints * points.buffer.getStride();

	points.buffer.getRender()->createBuffer(bufferSize, points.buffer.getData(), points.buffer.getType(), points.buffer.getUsage());
	if (points.buffer.isRenderAttributes()) {
        for (auto const &attr : points.buffer.getAttributes()) {
			render.addAttribute(shaderData.getAttrIndex(attr.first), points.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), points.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
        }
    }

	points.needUpdateBuffer = true;

	if (textureRender)
		render.addTexture(shaderData.getTextureIndex(TextureShaderType::POINTS, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

	points.needUpdateTexture = false;

	if (!render.endLoad(pipelines)){
		return false;
	}

	points.needReload = false;
	points.loadCalled = true;
	SystemRender::addQueueCommand(&changeLoaded, new check_load_t{scene, entity});

	return true;
}

bool RenderSystem::loadLines(Entity entity, LinesComponent& lines, uint8_t pipelines){

	if (!Engine::isViewLoaded()) 
		return false;

	if (lines.lines.size() == 0)
		return false;

	ObjectRender& render = lines.render;

	render.beginLoad(PrimitiveType::LINES);

	lines.shaderProperties = ShaderPool::getLinesProperties(false, true);
	lines.shader = ShaderPool::get(ShaderType::LINES, lines.shaderProperties);
	if (!lines.shader->isCreated())
		return false;
	render.addShader(lines.shader.get());
	ShaderData& shaderData = lines.shader.get()->shaderData;

	lines.slotVSParams = shaderData.getUniformBlockIndex(UniformBlockType::LINES_VS_PARAMS, ShaderStageType::VERTEX);

	lines.buffer.clear();
	lines.buffer.addAttribute(AttributeType::POSITION, 3, 0);
	lines.buffer.addAttribute(AttributeType::COLOR, 4, 3 * sizeof(float));
	lines.buffer.setStride(7 * sizeof(float));
	lines.buffer.setRenderAttributes(true);
	lines.buffer.setUsage(BufferUsage::DYNAMIC);

	lines.buffer.setData((unsigned char*)(&lines.lines.at(0)), sizeof(LineData)*lines.lines.size());

	size_t bufferSize = lines.maxLines * lines.buffer.getStride();

	if (bufferSize == 0)
		return false;

	lines.buffer.getRender()->createBuffer(bufferSize, lines.buffer.getData(), lines.buffer.getType(), lines.buffer.getUsage());
	if (lines.buffer.isRenderAttributes()) {
        for (auto const &attr : lines.buffer.getAttributes()) {
			render.addAttribute(shaderData.getAttrIndex(attr.first), lines.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), lines.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized(), attr.second.getPerInstance());
        }
    }
	if (lines.buffer.getUsage() != BufferUsage::IMMUTABLE){
		lines.needUpdateBuffer = true;
	}

	if (!render.endLoad(pipelines)){
		return false;
	}

	lines.needReload = false;
	lines.loadCalled = true;
	SystemRender::addQueueCommand(&changeLoaded, new check_load_t{scene, entity});

	return true;
}

bool RenderSystem::drawPoints(PointsComponent& points, Transform& transform, Transform& camTransform, bool renderToTexture){
	if (points.loaded && points.buffer.getSize() > 0){

		if (points.needUpdateTexture || points.texture.isFramebufferOutdated()){
			ShaderData& shaderData = points.shader.get()->shaderData;
			TextureRender* textureRender = points.texture.getRender();
			if (textureRender)
				points.render.addTexture(shaderData.getTextureIndex(TextureShaderType::POINTS, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			points.needUpdateTexture = false;
		}

		if (points.needUpdateBuffer){
			points.buffer.getRender()->updateBuffer(points.buffer.getSize(), points.buffer.getData());
			points.needUpdateBuffer = false;
		}

		ObjectRender& render = points.render;

		if (!render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT)){
			points.needReload = true;
			return false;
		}
		render.applyUniformBlock(points.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &transform.modelViewProjectionMatrix);
		render.draw(points.numVisible, 1);
	}

	return true;
}

void RenderSystem::destroyPoints(Entity entity, PointsComponent& points){
	if (!points.loaded)
		return;

	//Destroy shader
	points.shader.reset();
	ShaderPool::remove(ShaderType::POINTS, points.shaderProperties);

	//Destroy texture
	points.texture.destroy();

	//Destroy render
	points.render.destroy();

	//Destroy buffer
	//points.buffer.clearAll();
	points.buffer.getRender()->destroyBuffer();

	//Shaders uniforms
	points.slotVSParams = -1;

	SystemRender::addQueueCommand(&changeDestroy, new check_load_t{scene, entity});
}

bool RenderSystem::drawLines(LinesComponent& lines, Transform& transform, Transform& camTransform, bool renderToTexture){
	if (lines.loaded && lines.buffer.getSize() > 0){

		if (lines.needUpdateBuffer){
			lines.buffer.getRender()->updateBuffer(lines.buffer.getSize(), lines.buffer.getData());
			lines.needUpdateBuffer = false;
		}

		ObjectRender& render = lines.render;

		if (!render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT)){
			lines.needReload = true;
			return false;
		}
		render.applyUniformBlock(lines.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &transform.modelViewProjectionMatrix);
		render.draw(lines.lines.size() * 2, 1);
	}

	return true;
}

void RenderSystem::destroyLines(Entity entity, LinesComponent& lines){
	if (!lines.loaded)
		return;

	//Destroy shader
	lines.shader.reset();
	ShaderPool::remove(ShaderType::LINES, lines.shaderProperties);

	//Destroy render
	lines.render.destroy();

	//Destroy buffer
	lines.buffer.getRender()->destroyBuffer();

	//Shaders uniforms
	lines.slotVSParams = -1;

	SystemRender::addQueueCommand(&changeDestroy, new check_load_t{scene, entity});
}

bool RenderSystem::loadSky(Entity entity, SkyComponent& sky, uint8_t pipelines){

	if (!Engine::isViewLoaded()) 
		return false;	

	sky.buffer.clear();
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
			render->addAttribute(shaderData.getAttrIndex(attr.first), sky.buffer.getRender(), attr.second.getElements(), attr.second.getDataType(), sky.buffer.getStride(), attr.second.getOffset(), attr.second.getNormalized(),  attr.second.getPerInstance());
        }
    }

	if (!render->endLoad(pipelines)){
		return false;
	}

	sky.loadCalled = true;
	SystemRender::addQueueCommand(&changeLoaded, new check_load_t{scene, entity});

	return true;
}

bool RenderSystem::drawSky(SkyComponent& sky, bool renderToTexture){
	if (sky.loaded){

		if (sky.needUpdateTexture || sky.texture.isFramebufferOutdated()){
			ShaderData& shaderData = sky.shader.get()->shaderData;
			TextureRender* textureRender = sky.texture.getRender();
			if (textureRender)
				sky.render.addTexture(shaderData.getTextureIndex(TextureShaderType::SKYCUBE, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);

			sky.needUpdateTexture = false;
		}

		ObjectRender& render = sky.render;

		if (!render.beginDraw((renderToTexture)?PIP_RTT:PIP_DEFAULT)){
			sky.needReload = true;
			return false;
		}
		render.applyUniformBlock(sky.slotVSParams, ShaderStageType::VERTEX, sizeof(float) * 16, &sky.skyViewProjectionMatrix);
		render.applyUniformBlock(sky.slotFSParams, ShaderStageType::FRAGMENT, sizeof(float) * 4, &sky.color);
		render.draw(36, 1);
	}

	return true;
}

void RenderSystem::destroySky(Entity entity, SkyComponent& sky){
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
	//sky.buffer.clearAll();
	sky.buffer.getRender()->destroyBuffer();

	//Shaders uniforms
	sky.slotVSParams = -1;
	sky.slotFSParams = -1;

	SystemRender::addQueueCommand(&changeDestroy, new check_load_t{scene, entity});
}

void RenderSystem::destroyLight(LightComponent& light){
	for (int i = 0; i < MAX_SHADOWCASCADES; i++) {
		light.framebuffer[i].destroyFramebuffer();
	}
}

void RenderSystem::destroyCamera(CameraComponent& camera, bool entityDestroyed){
	if (camera.framebuffer){
		if (entityDestroyed){
			delete camera.framebuffer; // destroy framebuffer in destructor
		}else{
			camera.framebuffer->destroy();
		}
	}
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
		Matrix4 translateMatrix = Matrix4::translateMatrix(transform.position);
		Matrix4 rotationMatrix = transform.rotation.getRotationMatrix();
		Matrix4 scaleMatrix = Matrix4::scaleMatrix(transform.scale);

		transform.localMatrix = translateMatrix * rotationMatrix * scaleMatrix;
	}

	if (transform.parent != NULL_ENTITY){
		Transform& transformParent = scene->getComponent<Transform>(transform.parent);

		transform.modelMatrix = transformParent.modelMatrix * transform.localMatrix;

		transform.modelMatrix.decompose(transform.worldPosition, transform.worldScale, transform.worldRotation);
	}else{
		transform.modelMatrix = transform.localMatrix;

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
		camera.projectionMatrix = Matrix4::orthoMatrix(camera.left, camera.right, camera.top, camera.bottom, camera.near, camera.far);
	}else if (camera.type == CameraType::CAMERA_ORTHO) {
		camera.projectionMatrix = Matrix4::orthoMatrix(camera.left, camera.right, camera.bottom, camera.top, camera.near, camera.far);
	}else if (camera.type == CameraType::CAMERA_PERSPECTIVE){
		camera.projectionMatrix = Matrix4::perspectiveMatrix(camera.yfov, camera.aspect, camera.near, camera.far);
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

	updateCameraFrustumPlanes(camera.viewProjectionMatrix, camera.frustumPlanes);
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

	Matrix4 rotationMatrix = Quaternion(0, sky.rotation, 0).getRotationMatrix();

	sky.skyViewProjectionMatrix = camera.projectionMatrix * skyViewMatrix * rotationMatrix;

	sky.needUpdateSky = false;
}

void RenderSystem::updatePoints(PointsComponent& points, Transform& transform, CameraComponent& camera, Transform& camTransform){
	points.renderPoints.clear();
	points.renderPoints.reserve(points.points.size());

	// point particle sizes are in pixels, need to convert it to canvas size
	float sizeScaleW = System::instance().getScreenWidth() / (float)Engine::getCanvasWidth();
	float sizeScaleH = System::instance().getScreenHeight() / (float)Engine::getCanvasHeight();
	float sizeScale = std::max(sizeScaleW, sizeScaleH);

	points.numVisible = 0;
	size_t pointsSize = (points.points.size() < points.maxPoints)? points.points.size() : points.maxPoints;
	for (int i = 0; i < pointsSize; i++){
		if (points.points[i].visible){
			points.renderPoints.push_back({});
			points.renderPoints[points.numVisible].position = points.points[i].position;
			points.renderPoints[points.numVisible].color = points.points[i].color;
			points.renderPoints[points.numVisible].size = points.points[i].size * sizeScale;
			points.renderPoints[points.numVisible].rotation = points.points[i].rotation;
			points.renderPoints[points.numVisible].textureRect = points.points[i].textureRect;
			points.numVisible++;
		}
	}

	if (points.numVisible > 0){
		points.buffer.setData((unsigned char*)(&points.renderPoints.at(0)), sizeof(PointRenderData)*points.numVisible);
	}else{
		points.buffer.setData((unsigned char*)nullptr, 0);
	}

	if (points.loaded)
		points.needUpdateBuffer = true;
}

void RenderSystem::sortPoints(PointsComponent& points, Transform& transform, CameraComponent& camera, Transform& camTransform){
	Vector3 camDir = (camTransform.worldPosition - camera.worldView).normalize();

	auto comparePoints = [&transform, &camDir](const PointRenderData& a, const PointRenderData& b) -> bool {
		return (transform.modelMatrix * a.position).dotProduct(camDir) < (transform.modelMatrix * b.position).dotProduct(camDir);
	};
	std::sort(points.renderPoints.begin(), points.renderPoints.end(), comparePoints);

	if (points.loaded)
		points.needUpdateBuffer = true;
}

void RenderSystem::updateTerrain(TerrainComponent& terrain, Transform& transform, CameraComponent& camera, Transform& cameraTransform){
	if (terrain.heightMapLoaded){
		for (int i = 0; i < terrain.numNodes; i++){
			terrain.nodes[i].visible = false;
		}

		for (int s = 0; s < 2; s++){
			terrain.nodesbuffer[s].clear();
		}

		for (int i = 0; i < (terrain.rootGridSize*terrain.rootGridSize); i++){
			terrainNodeLODSelect(terrain, transform, camera, cameraTransform, terrain.nodes[terrain.grid[i]], terrain.levels-1);
		}

		terrain.needUpdateNodesBuffer = true;

		terrain.eyePos = Vector3(cameraTransform.worldPosition.x, cameraTransform.worldPosition.y, cameraTransform.worldPosition.z);
	}
}

AABB RenderSystem::getTerrainNodeAABB(Transform& transform, TerrainNode& terrainNode){
    float halfSize = terrainNode.size/2;
    Vector3 worldHalfScale(halfSize * transform.worldScale.x, 1, halfSize * transform.worldScale.z);
    Vector3 worldPosition = transform.modelMatrix * Vector3(terrainNode.position.x, 0, terrainNode.position.y);

    Vector3 c1 = Vector3(worldPosition.x - worldHalfScale.x, terrainNode.minHeight, worldPosition.z - worldHalfScale.z);
    Vector3 c2 = Vector3(worldPosition.x + worldHalfScale.x, terrainNode.maxHeight, worldPosition.z + worldHalfScale.z);

    return AABB(c1, c2);
};

bool RenderSystem::isTerrainNodeInSphere(Vector3 position, float radius, const AABB& box) {
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

    AABB box = getTerrainNodeAABB(transform, terrainNode);

    if (!isTerrainNodeInSphere(cameraTransform.worldPosition, terrain.ranges[lodLevel], box)) {
		// no node or child nodes were selected
        return false;
    }

    if (!isInsideCamera(camera, box)) {
		// return true to parent node does not select itself over area
        return true;
    }

    if( lodLevel == 0 ) {
        //Full resolution
        terrainNode.resolution = terrain.resolution;
		terrainNode.visible = true;
		terrain.nodesbuffer[0].addVector2(AttributeType::TERRAINNODEPOSITION, terrainNode.position);
		terrain.nodesbuffer[0].addFloat(AttributeType::TERRAINNODESIZE, terrainNode.size);
		terrain.nodesbuffer[0].addFloat(AttributeType::TERRAINNODERANGE, terrainNode.currentRange);
		terrain.nodesbuffer[0].addFloat(AttributeType::TERRAINNODERESOLUTION, terrainNode.resolution);

        return true;
    } else {

        if( !isTerrainNodeInSphere(cameraTransform.worldPosition, terrain.ranges[lodLevel-1], box) ) {
            //Full resolution
			terrainNode.resolution = terrain.resolution;
			terrainNode.visible = true;
			terrain.nodesbuffer[0].addVector2(AttributeType::TERRAINNODEPOSITION, terrainNode.position);
			terrain.nodesbuffer[0].addFloat(AttributeType::TERRAINNODESIZE, terrainNode.size);
			terrain.nodesbuffer[0].addFloat(AttributeType::TERRAINNODERANGE, terrainNode.currentRange);
			terrain.nodesbuffer[0].addFloat(AttributeType::TERRAINNODERESOLUTION, terrainNode.resolution);
        } else {
            for (int i = 0; i < 4; i++) {
                TerrainNode& child = terrain.nodes[terrainNode.childs[i]];
				if (!terrainNodeLODSelect(terrain, transform, camera, cameraTransform, child, lodLevel-1)){
                    //Half resolution
                    child.resolution = terrain.resolution / 2;
                    child.currentRange = terrainNode.currentRange;
					child.visible = true;
					terrain.nodesbuffer[1].addVector2(AttributeType::TERRAINNODEPOSITION, child.position);
					terrain.nodesbuffer[1].addFloat(AttributeType::TERRAINNODESIZE, child.size);
					terrain.nodesbuffer[1].addFloat(AttributeType::TERRAINNODERANGE, child.currentRange);
					terrain.nodesbuffer[1].addFloat(AttributeType::TERRAINNODERESOLUTION, child.resolution);
                }
            }
        }

        return true;
    }

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

bool RenderSystem::isInsideCamera(const float cameraFar, const Plane frustumPlanes[6], const AABB& box){
    if (box.isNull() || box.isInfinite())
        return false;

    Vector3 centre = box.getCenter();
    Vector3 halfSize = box.getHalfSize();

    for (int plane = 0; plane < 6; ++plane){
        if (plane == FRUSTUM_PLANE_FAR && cameraFar == 0)
            continue;

        Plane::Side side = frustumPlanes[plane].getSide(centre, halfSize);
        if (side == Plane::Side::NEGATIVE_SIDE){
            return false;
        }
    }

    return true;
}

bool RenderSystem::isInsideCamera(CameraComponent& camera, const AABB& box){
    return isInsideCamera(camera.far, camera.frustumPlanes, box);
}

bool RenderSystem::isInsideCamera(CameraComponent& camera, const Vector3& point){
    for (int plane = 0; plane < 6; ++plane){
        if (plane == FRUSTUM_PLANE_FAR && camera.far == 0)
            continue;

        if (camera.frustumPlanes[plane].getSide(point) == Plane::Side::NEGATIVE_SIDE){
            return false;
        }
    }

    return true;
}

bool RenderSystem::isInsideCamera(CameraComponent& camera, const Vector3& center, const float& radius){
    for (int plane = 0; plane < 6; ++plane){
        if (plane == FRUSTUM_PLANE_FAR && camera.far == 0)
            continue;

        if (camera.frustumPlanes[plane].getDistance(center) < -radius){
            return false;
        }
    }

    return true;
}

void RenderSystem::updateCameraFrustumPlanes(const Matrix4 viewProjectionMatrix, Plane* frustumPlanes){

    frustumPlanes[FRUSTUM_PLANE_LEFT].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0];
    frustumPlanes[FRUSTUM_PLANE_LEFT].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0];
    frustumPlanes[FRUSTUM_PLANE_LEFT].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0];
    frustumPlanes[FRUSTUM_PLANE_LEFT].d = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];

    frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0];
    frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0];
    frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0];
    frustumPlanes[FRUSTUM_PLANE_RIGHT].d = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];

    frustumPlanes[FRUSTUM_PLANE_TOP].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1];
    frustumPlanes[FRUSTUM_PLANE_TOP].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1];
    frustumPlanes[FRUSTUM_PLANE_TOP].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1];
    frustumPlanes[FRUSTUM_PLANE_TOP].d = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];

    frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1];
    frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1];
    frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1];
    frustumPlanes[FRUSTUM_PLANE_BOTTOM].d = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];

    frustumPlanes[FRUSTUM_PLANE_NEAR].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2];
    frustumPlanes[FRUSTUM_PLANE_NEAR].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2];
    frustumPlanes[FRUSTUM_PLANE_NEAR].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2];
    frustumPlanes[FRUSTUM_PLANE_NEAR].d = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];

    frustumPlanes[FRUSTUM_PLANE_FAR].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2];
    frustumPlanes[FRUSTUM_PLANE_FAR].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2];
    frustumPlanes[FRUSTUM_PLANE_FAR].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2];
    frustumPlanes[FRUSTUM_PLANE_FAR].d = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];

    for (int i=0; i<6; i++){
        float length = frustumPlanes[i].normal.normalizeL();
        frustumPlanes[i].d /= length;
    }
}

void RenderSystem::updateInstancedMesh(InstancedMeshComponent& instmesh, MeshComponent& mesh, Transform& transform, CameraComponent& camera, Transform& camTransform){
	instmesh.renderInstances.clear();
	instmesh.renderInstances.reserve(instmesh.instances.size());

	Quaternion bRotation;
	if (instmesh.instancedBillboard){
		Vector3 camPos = camTransform.worldPosition;
		if (instmesh.instancedCylindricalBillboard){
			camPos.y = transform.worldPosition.y;
		}

		Matrix4 m1 = camera.viewMatrix.inverse();
		bRotation.fromRotationMatrix(m1);
		bRotation = transform.worldRotation.inverse() * bRotation;
	}

	mesh.aabb = AABB::ZERO;
	instmesh.numVisible = 0;
	size_t instancesSize = (instmesh.instances.size() < instmesh.maxInstances)? instmesh.instances.size() : instmesh.maxInstances;
	for (int i = 0; i < instancesSize; i++){
		if (instmesh.instances[i].visible){
			Matrix4 translateMatrix = Matrix4::translateMatrix(instmesh.instances[i].position);
			Matrix4 rotationMatrix;
			Matrix4 scaleMatrix = Matrix4::scaleMatrix(instmesh.instances[i].scale);

			if (instmesh.instancedBillboard){
				rotationMatrix = bRotation.getRotationMatrix();
			}else{
				rotationMatrix = instmesh.instances[i].rotation.getRotationMatrix();
			}

			Matrix4 instanceMatrix = translateMatrix * rotationMatrix * scaleMatrix;

			instmesh.renderInstances.push_back({});
			instmesh.renderInstances[instmesh.numVisible].instanceMatrix = instanceMatrix;
			instmesh.renderInstances[instmesh.numVisible].color = instmesh.instances[i].color;
			instmesh.renderInstances[instmesh.numVisible].textureRect = instmesh.instances[i].textureRect;
			instmesh.numVisible++;

			mesh.aabb.merge(instanceMatrix * mesh.verticesAABB);
		}
	}

	mesh.worldAABB = transform.localMatrix * mesh.aabb;

	if (instmesh.numVisible > 0){
		instmesh.buffer.setData((unsigned char*)(&instmesh.renderInstances.at(0)), sizeof(InstanceRenderData)*instmesh.numVisible);
	}else{
		instmesh.buffer.setData((unsigned char*)nullptr, 0);
	}

	if (mesh.loaded)
		instmesh.needUpdateBuffer = true;
}

void RenderSystem::sortInstancedMesh(InstancedMeshComponent& instmesh, MeshComponent& mesh, Transform& transform, CameraComponent& camera, Transform& camTransform){
	Vector3 camDir = camDir = (camTransform.worldPosition - camera.worldView).normalize();

	auto comparePoints = [&transform, &camDir](const InstanceRenderData& a, const InstanceRenderData& b) -> bool {
		Vector3 positionA = Vector3(a.instanceMatrix[3][0], a.instanceMatrix[3][1], a.instanceMatrix[3][2]);
		Vector3 positionB = Vector3(b.instanceMatrix[3][0], b.instanceMatrix[3][1], b.instanceMatrix[3][2]);

		return (transform.modelMatrix * positionA).dotProduct(camDir) < (transform.modelMatrix * positionB).dotProduct(camDir);
	};
	std::sort(instmesh.renderInstances.begin(), instmesh.renderInstances.end(), comparePoints);

	if (mesh.loaded)
		instmesh.needUpdateBuffer = true;
}

void RenderSystem::configureLightShadowNearFar(LightComponent& light, const CameraComponent& camera){
	if (light.shadowCameraNearFar.x == 0.0){
		light.shadowCameraNearFar.x = camera.near;
	}
	if (light.shadowCameraNearFar.y == 0.0){
		if (light.range == 0.0){
			light.shadowCameraNearFar.y = camera.far;
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

				float zFar = camera.far;
				float zNear = camera.near;
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

					light.cameras[ca].lightViewMatrix = viewMatrix;
					light.cameras[ca].lightProjectionMatrix = projectionMatrix[ca];
					light.cameras[ca].lightViewProjectionMatrix = projectionMatrix[ca] * viewMatrix;
					light.cameras[ca].nearFar = Vector2(splitNear[ca], splitFar[ca]);
					updateCameraFrustumPlanes(light.cameras[ca].lightViewProjectionMatrix, light.cameras[ca].frustumPlanes);
				}
				
			} else {

				if (light.numShadowCascades > 1){
					light.numShadowCascades = 1;
					Log::warn("Can not have multiple cascades shadows when using ortho scene camera. Reducing num shadow cascades to 1");
				}

				Matrix4 sceneCameraInv = camera.viewProjectionMatrix.inverse();

				projectionMatrix[0] = getDirLightProjection(viewMatrix, sceneCameraInv);

				light.cameras[0].lightViewMatrix = viewMatrix;
				light.cameras[0].lightProjectionMatrix = projectionMatrix[0];
				light.cameras[0].lightViewProjectionMatrix = projectionMatrix[0] * viewMatrix;
				light.cameras[0].nearFar = Vector2(-1, 1);
				updateCameraFrustumPlanes(light.cameras[0].lightViewProjectionMatrix, light.cameras[0].frustumPlanes);

			}

		}else if (light.type == LightType::SPOT){
			Matrix4 projectionMatrix;
			Matrix4 viewMatrix;

			configureLightShadowNearFar(light, camera);

			viewMatrix = Matrix4::lookAtMatrix(transform.worldPosition, light.worldDirection + transform.worldPosition, up);

			projectionMatrix = Matrix4::perspectiveMatrix(acos(light.outerConeCos)*2, 1, light.shadowCameraNearFar.x, light.shadowCameraNearFar.y);

			light.cameras[0].lightViewMatrix = viewMatrix;
			light.cameras[0].lightProjectionMatrix = projectionMatrix;
			light.cameras[0].lightViewProjectionMatrix = projectionMatrix * viewMatrix;
			light.cameras[0].nearFar = light.shadowCameraNearFar;
			updateCameraFrustumPlanes(light.cameras[0].lightViewProjectionMatrix, light.cameras[0].frustumPlanes);

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
				light.cameras[f].lightViewMatrix = viewMatrix[f];
				light.cameras[f].lightProjectionMatrix = projectionMatrix;
				light.cameras[f].lightViewProjectionMatrix = projectionMatrix * viewMatrix[f];
				light.cameras[f].nearFar = calculedNearFar;
				updateCameraFrustumPlanes(light.cameras[f].lightViewProjectionMatrix, light.cameras[f].frustumPlanes);
			}
		}
		
		
	}
}

void RenderSystem::changeLoaded(void* data){
	if (!Engine::isViewLoaded())
		return;

	check_load_t* loadObj = (check_load_t*)data;

	Scene* scene = loadObj->scene;
	Entity entity = loadObj->entity;

	Signature signature = scene->getSignature(entity);

	if (signature.test(scene->getComponentType<MeshComponent>())){
		MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

		mesh.loaded = true;

	}else if (signature.test(scene->getComponentType<UIComponent>())){
		UIComponent& uirender = scene->getComponent<UIComponent>(entity);

		uirender.loaded = true;

	}else if (signature.test(scene->getComponentType<PointsComponent>())){
		PointsComponent& points = scene->getComponent<PointsComponent>(entity);

		points.loaded = true;

	}else if (signature.test(scene->getComponentType<LinesComponent>())){
		LinesComponent& lines = scene->getComponent<LinesComponent>(entity);

		lines.loaded = true;

	}else if (signature.test(scene->getComponentType<SkyComponent>())){
		SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

		sky.loaded = true;
	}

	delete (check_load_t*)data;
}

void RenderSystem::changeDestroy(void* data){
	check_load_t* loadObj = (check_load_t*)data;

	Scene* scene = loadObj->scene;
	Entity entity = loadObj->entity;

	Signature signature = scene->getSignature(entity);

	if (signature.test(scene->getComponentType<MeshComponent>())){
		MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

		mesh.loaded = false;
		mesh.loadCalled = false;

	}else if (signature.test(scene->getComponentType<UIComponent>())){
		UIComponent& uirender = scene->getComponent<UIComponent>(entity);

		uirender.loaded = false;
		uirender.loadCalled = false;

	}else if (signature.test(scene->getComponentType<PointsComponent>())){
		PointsComponent& points = scene->getComponent<PointsComponent>(entity);

		points.loaded = false;
		points.loadCalled = false;

	}else if (signature.test(scene->getComponentType<LinesComponent>())){
		LinesComponent& lines = scene->getComponent<LinesComponent>(entity);

		lines.loaded = false;
		lines.loadCalled = false;

	}else if (signature.test(scene->getComponentType<SkyComponent>())){
		SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

		sky.loaded = false;
		sky.loadCalled = false;
	}

	delete (check_load_t*)data;
}

void RenderSystem::updateMVP(size_t index, Transform& transform, CameraComponent& camera, Transform& cameraTransform){
	if (transform.billboard && !transform.fakeBillboard){

		Vector3 camPos = cameraTransform.worldPosition;

		if (transform.cylindricalBillboard)
			camPos.y = transform.worldPosition.y;

		if ((camPos - transform.worldPosition).crossProduct(camera.worldUp).length() != 0){ // check if not parallel
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
	int numLights = checkLightsAndShadow();

	auto transforms = scene->getComponentArray<Transform>();
	auto cameras = scene->getComponentArray<CameraComponent>();

	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);

		if (transform.parent != NULL_ENTITY){
			Transform& transformParent = scene->getComponent<Transform>(transform.parent);

			if (transformParent.needUpdate){
				transform.needUpdate = true;
			}

			if (transformParent.needUpdateChildVisibility){
				transform.visible = transformParent.visible;
				transform.needUpdateChildVisibility = true;
			}
		}

		if (transform.needUpdate){
			updateTransform(transform);
		}
	}

	Entity mainCameraEntity = scene->getCamera();
	uint8_t pipelines = 0;

	hasMultipleCameras = false;
	for (int i = 0; i < cameras->size(); i++){
		CameraComponent& camera = cameras->getComponentFromIndex(i);
		Entity cameraEntity = cameras->getEntity(i);
		Transform& cameraTransform = scene->getComponent<Transform>(cameraEntity);
		if (camera.renderToTexture && cameraEntity != mainCameraEntity){
			hasMultipleCameras = true;
		}

		if (cameraEntity == mainCameraEntity && !camera.renderToTexture){
			pipelines |= PIP_DEFAULT;
		}

		if (camera.renderToTexture){
			pipelines |= PIP_RTT;
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

	loadLights(numLights);
	loadAndProcessFog();

	auto skys = scene->getComponentArray<SkyComponent>();
	if (skys->size() > 0){
		SkyComponent& sky = skys->getComponentFromIndex(0);
		Entity entity = skys->getEntity(0);
		if (mainCamera.needUpdate || sky.needUpdateSky){
			if (!hasMultipleCameras){
				updateSkyViewProjection(sky, mainCamera);
			}
		}

		if (sky.loaded && sky.needReload){
			destroySky(entity, sky);
		}
		if (!sky.loadCalled){
			loadSky(entity, sky, pipelines);
		}
	}

	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);

		Entity entity = transforms->getEntity(i);
		Signature signature = scene->getSignature(entity);

		if (signature.test(scene->getComponentType<MeshComponent>())){
			MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

			TerrainComponent* terrain = scene->findComponent<TerrainComponent>(entity);

			InstancedMeshComponent* instmesh = scene->findComponent<InstancedMeshComponent>(entity);
			if (instmesh){
				bool sortTransparentInstances = mesh.transparent && mainCamera.type != CameraType::CAMERA_2D;

				if (instmesh->needUpdateInstances && !instmesh->instancedBillboard){
					updateInstancedMesh(*instmesh, mesh, transform, mainCamera, mainCameraTransform);
				}

				if (instmesh->needUpdateInstances || ((mainCamera.needUpdate || transform.needUpdate) && sortTransparentInstances)){
					if (!hasMultipleCameras || !sortTransparentInstances){
						if (instmesh->instancedBillboard){
							updateInstancedMesh(*instmesh, mesh, transform, mainCamera, mainCameraTransform);
						}
						sortInstancedMesh(*instmesh, mesh, transform, mainCamera, mainCameraTransform);
					}
				}

				instmesh->needUpdateInstances = false;
			}
			if (mesh.loaded && mesh.needReload){
				destroyMesh(entity, mesh);
			}
			if (!mesh.loadCalled){
				loadMesh(entity, mesh, pipelines, instmesh, terrain);
			}
		}else if (signature.test(scene->getComponentType<UIComponent>())){
			UIComponent& ui = scene->getComponent<UIComponent>(entity);
			if (!ui.loaded){
				bool isText = false;
				if (signature.test(scene->getComponentType<TextComponent>())){
					isText = true;
				}
				if (ui.loaded && ui.needReload){
					destroyUI(entity, ui);
				}
				if (!ui.loadCalled){
					loadUI(entity, ui, pipelines, isText);
				}
			}
		}else if (signature.test(scene->getComponentType<PointsComponent>())){
			PointsComponent& points = scene->getComponent<PointsComponent>(entity);
			if (points.loaded && points.needReload){
				destroyPoints(entity, points);
			}
			if (!points.loadCalled){
				loadPoints(entity, points, pipelines);
			}
		}else if (signature.test(scene->getComponentType<LinesComponent>())){
			LinesComponent& lines = scene->getComponent<LinesComponent>(entity);
			if (lines.loaded && lines.needReload){
				destroyLines(entity, lines);
			}
			if (!lines.loadCalled){
				loadLines(entity, lines, pipelines);
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

			if (signature.test(scene->getComponentType<MeshComponent>())){
				MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

				mesh.worldAABB = transform.localMatrix * mesh.aabb;
			}

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

        if (signature.test(scene->getComponentType<PointsComponent>())){
			PointsComponent& points = scene->getComponent<PointsComponent>(entity);

			bool sortTransparentPoints = points.transparent && mainCamera.type != CameraType::CAMERA_2D;

			if (points.needUpdate){
				updatePoints(points, transform, mainCamera, mainCameraTransform);
			}

			if (points.needUpdate || ((mainCamera.needUpdate || transform.needUpdate) && sortTransparentPoints)){
				if (!hasMultipleCameras || !sortTransparentPoints){
					sortPoints(points, transform, mainCamera, mainCameraTransform);
				}
			}

			points.needUpdate = false;
		}

		transform.needUpdateChildVisibility = false;
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
	std::priority_queue<TransparentMeshesData, std::vector<TransparentMeshesData>, MeshComparison> transparentMeshes;

	auto transforms = scene->getComponentArray<Transform>();
	auto cameras = scene->getComponentArray<CameraComponent>();

	//---------Depth shader----------
	if (hasShadows){
		auto lights = scene->getComponentArray<LightComponent>();
		auto meshes = scene->getComponentArray<MeshComponent>();
		auto terrains = scene->getComponentArray<TerrainComponent>();
		
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
					light.cameras[c].render.setClearColor(Vector4(1.0, 1.0, 1.0, 1.0));

					light.cameras[c].render.startFrameBuffer(&light.framebuffer[fb], face);
					for (int i = 0; i < meshes->size(); i++){
						MeshComponent& mesh = meshes->getComponentFromIndex(i);
						Entity entity = meshes->getEntity(i);
						Transform* transform = scene->findComponent<Transform>(entity);

						if (transform){
							if (transform->visible){
								InstancedMeshComponent* instmesh = scene->findComponent<InstancedMeshComponent>(entity);
								TerrainComponent* terrain = scene->findComponent<TerrainComponent>(entity);

								vs_depth_t vsDepthParams;

								if (transform->billboard && mesh.enableShadowsBillboard){
									Matrix4 modelViewMatrix = light.cameras[c].lightViewMatrix * transform->modelMatrix;

									modelViewMatrix.set(0, 0, transform->worldScale.x);
									modelViewMatrix.set(0, 1, 0.0);
									modelViewMatrix.set(0, 2, 0.0);

									if (!transform->cylindricalBillboard) {
										modelViewMatrix.set(1, 0, 0.0);
										modelViewMatrix.set(1, 1, transform->worldScale.y);
										modelViewMatrix.set(1, 2, 0.0);
									}

									modelViewMatrix.set(2, 0, 0.0);
									modelViewMatrix.set(2, 1, 0.0);
									modelViewMatrix.set(2, 2, transform->worldScale.z);

									vsDepthParams = {modelViewMatrix, light.cameras[c].lightProjectionMatrix};
								}else{
									vsDepthParams = {transform->modelMatrix, light.cameras[c].lightViewProjectionMatrix};
								}

								drawMeshDepth(mesh, light.cameras[c].nearFar.y, light.cameras[c].frustumPlanes, vsDepthParams, instmesh, terrain);
							}
						}
					}

					light.cameras[c].render.endFrameBuffer();
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

		if (Engine::getMainScene() == scene || camera.renderToTexture){
			camera.render.setClearColor(scene->getBackgroundColor());
		}
		
		if (!camera.renderToTexture){
			camera.render.startFrameBuffer();
			camera.render.applyViewport(Engine::getViewRect());
		}else{
			if (!camera.framebuffer->isCreated()){
				createFramebuffer(camera);
			}
			camera.render.startFrameBuffer(&camera.framebuffer->getRender());
		}

		//---------Draw opaque meshes and UI----------
		bool hasActiveScissor = false;

		//---------Draw sky----------
		auto skys = scene->getComponentArray<SkyComponent>();
		if (skys->size() > 0){
			SkyComponent& sky = skys->getComponentFromIndex(0);
			Entity entity = skys->getEntity(0);
			if (hasMultipleCameras){
				updateSkyViewProjection(sky, camera);
			}

			drawSky(sky, camera.renderToTexture);
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

				if (transform.parent != NULL_ENTITY){
					Signature parentSignature = scene->getSignature(transform.parent);
					if (parentSignature.test(scene->getComponentType<UILayoutComponent>())){
						UILayoutComponent& parentLayout = scene->getComponent<UILayoutComponent>(transform.parent);

						parentScissor = parentLayout.scissor;
						if (!parentScissor.isZero()){
							if (!layout.ignoreScissor){
								camera.render.applyScissor(parentScissor);
								layout.scissor = parentScissor;

								hasActiveScissor = true;
							}
						}
					}
				}

				if (signature.test(scene->getComponentType<ImageComponent>())){
					ImageComponent& img = scene->getComponent<ImageComponent>(entity);

					layout.scissor = getScissorRect(layout, img, transform, camera);
					if (hasActiveScissor){
						layout.scissor = layout.scissor.fitOnRect(parentScissor);
					}
				}
			}

			if (signature.test(scene->getComponentType<MeshComponent>())){
				MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

				if (transform.visible){

					InstancedMeshComponent* instmesh = scene->findComponent<InstancedMeshComponent>(entity);
					if (instmesh){
						bool sortTransparentInstances = mesh.transparent && camera.type != CameraType::CAMERA_2D;

						if (hasMultipleCameras && sortTransparentInstances){
							if (instmesh->instancedBillboard){
								updateInstancedMesh(*instmesh, mesh, transform, camera, cameraTransform);
							}
							sortInstancedMesh(*instmesh, mesh, transform, camera, cameraTransform);
						}
					}

					TerrainComponent* terrain = scene->findComponent<TerrainComponent>(entity);
					if (terrain && hasMultipleCameras){
						updateTerrain(*terrain, transform, camera, cameraTransform);
					}

					if (!mesh.transparent || !camera.transparentSort){
						//Draw opaque meshes if transparency is not necessary
						drawMesh(mesh, transform, camera, cameraTransform, camera.renderToTexture, instmesh, terrain);
					}else{
						transparentMeshes.push({&mesh, instmesh, terrain, &transform, transform.distanceToCamera});
					}
				}

			}else if (signature.test(scene->getComponentType<UIComponent>())){
				UIComponent& ui = scene->getComponent<UIComponent>(entity);

				bool isText = false;
				if (signature.test(scene->getComponentType<TextComponent>())){
					isText = true;
				}
				if (transform.visible)
					drawUI(ui, transform, camera.renderToTexture);

			}else if (signature.test(scene->getComponentType<PointsComponent>())){
				PointsComponent& points = scene->getComponent<PointsComponent>(entity);

				bool sortTransparentPoints = points.transparent && camera.type != CameraType::CAMERA_2D;

				if (hasMultipleCameras && sortTransparentPoints){
					sortPoints(points, transform, camera, cameraTransform);
				}

				if (transform.visible)
					drawPoints(points, transform, cameraTransform, camera.renderToTexture);

			}else if (signature.test(scene->getComponentType<LinesComponent>())){
				LinesComponent& lines = scene->getComponent<LinesComponent>(entity);

				if (transform.visible)
					drawLines(lines, transform, cameraTransform, camera.renderToTexture);

			}

			if (hasActiveScissor){
				if (!camera.renderToTexture){
					camera.render.applyScissor(Rect(0, 0, System::instance().getScreenWidth(), System::instance().getScreenHeight()));
				}else{
					camera.render.applyScissor(Rect(0, 0, camera.framebuffer->getWidth(), camera.framebuffer->getHeight()));
				}
				hasActiveScissor = false;
			}
		}

		//---------Draw transparent meshes----------
		while (!transparentMeshes.empty()){
			TransparentMeshesData meshData = transparentMeshes.top();

			//Draw transparent meshes
			drawMesh(*meshData.mesh, *meshData.transform, camera, cameraTransform, camera.renderToTexture, meshData.instmesh, meshData.terrain);

			transparentMeshes.pop();
		}

		camera.render.endFrameBuffer();

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
			"Go to directory \"tools/\" and execute the command:\n"
			"\n"
			"> python3 supershader.py -s \"%s\" -l %s\n"
			"-------------------"
			, misShaders.c_str(), ShaderPool::getShaderLangStr().c_str());
		exit(1);
	}
}

void RenderSystem::entityDestroyed(Entity entity){
	Signature signature = scene->getSignature(entity);

	if (signature.test(scene->getComponentType<LightComponent>())){
		destroyLight(scene->getComponent<LightComponent>(entity));
	}

	if (signature.test(scene->getComponentType<CameraComponent>())){
		destroyCamera(scene->getComponent<CameraComponent>(entity), true);
	}

	if (signature.test(scene->getComponentType<MeshComponent>())){
		destroyMesh(entity, scene->getComponent<MeshComponent>(entity));
	}

	if (signature.test(scene->getComponentType<UIComponent>())){
		destroyUI(entity, scene->getComponent<UIComponent>(entity));
	}

	if (signature.test(scene->getComponentType<PointsComponent>())){
		destroyPoints(entity, scene->getComponent<PointsComponent>(entity));
	}

	if (signature.test(scene->getComponentType<LinesComponent>())){
		destroyLines(entity, scene->getComponent<LinesComponent>(entity));
	}

	if (signature.test(scene->getComponentType<SkyComponent>())){
		destroySky(entity, scene->getComponent<SkyComponent>(entity));
	}
}
