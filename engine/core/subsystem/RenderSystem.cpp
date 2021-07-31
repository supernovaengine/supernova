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
#include <memory>
#include <cmath>

using namespace Supernova;

TextureRender RenderSystem::emptyWhite;
TextureRender RenderSystem::emptyBlack;
TextureRender RenderSystem::emptyCubeBlack;
TextureRender RenderSystem::emptyNormal;

bool RenderSystem::emptyTexturesCreated = false;

RenderSystem::RenderSystem(Scene* scene): SubSystem(scene){
	signature.set(scene->getComponentType<Transform>());
	signature.set(scene->getComponentType<MeshComponent>());

	hasLights = false;
	hasShadows = false;
}

void RenderSystem::load(){
	createEmptyTextures();
	checkLightsAndShadow();
	depthRender.setClearColor(Vector4(1.0, 1.0, 1.0, 1.0));
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

void RenderSystem::processLights(){
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
			hasShadows = true; // Re-check shadows on, after checked in checkLightsAndShadow()
			if (light.type == LightType::POINT){
				if (!light.framebuffer[0].isCreated())
					light.framebuffer[0].createFramebuffer(TextureType::TEXTURE_CUBE, light.mapResolution, light.mapResolution);

				if ((freeShadowCubeMap - MAX_SHADOWSMAP) < MAX_SHADOWSCUBEMAP){
					light.shadowMapIndex = freeShadowCubeMap++;
				}
			}else if (light.type == LightType::SPOT){
				if (!light.framebuffer[0].isCreated())
					light.framebuffer[0].createFramebuffer(TextureType::TEXTURE_2D, light.mapResolution, light.mapResolution);

				if (freeShadowMap < MAX_SHADOWSMAP){
					light.shadowMapIndex = freeShadowMap++;
				}
			}else if (light.type == LightType::DIRECTIONAL){
				for (int c = 0; c < light.numShadowCascades; c++){
					if (!light.framebuffer[c].isCreated())
						light.framebuffer[c].createFramebuffer(TextureType::TEXTURE_2D, light.mapResolution, light.mapResolution);
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
				Log::Warn("There are no shadow maps available");
			}
		}

		fs_lighting.direction_range[i] = Vector4(light.worldDirection.x, light.worldDirection.y, light.worldDirection.z, light.range);
		fs_lighting.color_intensity[i] = Vector4(light.color.x, light.color.y, light.color.z, light.intensity);
		fs_lighting.position_type[i] = Vector4(worldPosition.x, worldPosition.y, worldPosition.z, (float)type);
		fs_lighting.inCon_ouCon_shadows_cascades[i] = Vector4(light.innerConeCos, light.outerConeCos, light.shadowMapIndex, light.numShadowCascades);

	}

	// Setting intensity of other lights to zero
	for (int i = numLights; i < MAX_LIGHTS; i++){
		fs_lighting.color_intensity[i].w = 0.0;
	}
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

bool RenderSystem::loadMesh(MeshComponent& mesh){

	bufferNameToRender.clear();

	std::map<std::string, unsigned int> bufferStride;

	for (auto const& buf : mesh.buffers){
		buf.second->getRender()->createBuffer(buf.second->getSize(), buf.second->getData(), buf.second->getBufferType(), false);
		bufferNameToRender[buf.first] = buf.second->getRender();
		bufferStride[buf.first] = buf.second->getStride();
	}

	for (int i = 0; i < mesh.numSubmeshes; i++){

		mesh.submeshes[i].hasNormalMap = false;
		mesh.submeshes[i].hasTangent = false;
		mesh.submeshes[i].hasVertexColor = false;

		ObjectRender* render = &mesh.submeshes[i].render;

		render->beginLoad(mesh.submeshes[i].primitiveType, false);

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
			}
		}else{
			p_unlit = true;
		}

		mesh.submeshes[i].shaderProperties = ShaderPool::getMeshProperties(p_unlit, true, false, p_punctual, p_castShadows, p_hasNormal, p_hasNormalMap, p_hasTangent, false, true);
		mesh.submeshes[i].shader = ShaderPool::get(shaderType, mesh.submeshes[i].shaderProperties);
		if (hasShadows && mesh.castShadows){
			mesh.submeshes[i].depthShader = ShaderPool::get(ShaderType::DEPTH, "");
			if (!mesh.submeshes[i].depthShader->isCreated())
				return false;
		}
		if (!mesh.submeshes[i].shader->isCreated())
			return false;
		render->loadShader(mesh.submeshes[i].shader.get());
		ShaderData& shaderData = mesh.submeshes[i].shader.get()->shaderData;

		mesh.submeshes[i].slotVSParams = shaderData.getUniformIndex(UniformType::PBR_VS_PARAMS, ShaderStageType::VERTEX);
		mesh.submeshes[i].slotFSParams = shaderData.getUniformIndex(UniformType::PBR_FS_PARAMS, ShaderStageType::FRAGMENT);
		if (hasLights){
			mesh.submeshes[i].slotFSLighting = shaderData.getUniformIndex(UniformType::FS_LIGHTING, ShaderStageType::FRAGMENT);
			if (hasShadows && mesh.castShadows){
				mesh.submeshes[i].slotVSShadows = shaderData.getUniformIndex(UniformType::VS_SHADOWS, ShaderStageType::VERTEX);
				mesh.submeshes[i].slotFSShadows = shaderData.getUniformIndex(UniformType::FS_SHADOWS, ShaderStageType::FRAGMENT);
			}
		}

		TextureRender* textureRender = NULL;
		textureRender = mesh.submeshes[i].material.baseColorTexture.getRender();
		if (textureRender)
			render->loadTexture(shaderData.getTextureIndex(TextureShaderType::BASECOLOR, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, textureRender);
		else
			render->loadTexture(shaderData.getTextureIndex(TextureShaderType::BASECOLOR, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, &emptyWhite);

		if (hasLights){
			TextureRender* textureRender = NULL;
			int slotTex = -1;
			
			textureRender = mesh.submeshes[i].material.metallicRoughnessTexture.getRender();
			slotTex = shaderData.getTextureIndex(TextureShaderType::METALLICROUGHNESS, ShaderStageType::FRAGMENT);
			if (textureRender)
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
			else
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

			textureRender = mesh.submeshes[i].material.normalTexture.getRender();
			slotTex = shaderData.getTextureIndex(TextureShaderType::NORMAL, ShaderStageType::FRAGMENT);
			if (textureRender)
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
			else
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, &emptyNormal);

			textureRender = mesh.submeshes[i].material.occlusionTexture.getRender();
			slotTex = shaderData.getTextureIndex(TextureShaderType::OCCULSION, ShaderStageType::FRAGMENT);
			if (textureRender)	
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
			else
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, &emptyWhite);

			textureRender = mesh.submeshes[i].material.emissiveTexture.getRender();
			slotTex = shaderData.getTextureIndex(TextureShaderType::EMISSIVE, ShaderStageType::FRAGMENT);
			if (textureRender)	
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, textureRender);
			else
				render->loadTexture(slotTex, ShaderStageType::FRAGMENT, &emptyBlack);

			if (hasShadows && mesh.castShadows){
				size_t num2DShadows = 0;
				size_t numCubeShadows = 0;
				auto lights = scene->getComponentArray<LightComponent>();
				for (int l = 0; l < lights->size(); l++){
					LightComponent& light = lights->getComponentFromIndex(l);
					if (light.shadowMapIndex >= 0){
						if (light.type == LightType::POINT){
							slotTex = shaderData.getTextureIndex(getShadowMapCubeByIndex(light.shadowMapIndex), ShaderStageType::FRAGMENT);
							render->loadTexture(slotTex, ShaderStageType::FRAGMENT, light.framebuffer[0].getColorTexture());
							numCubeShadows++;
						}else if (light.type == LightType::SPOT){
							slotTex = shaderData.getTextureIndex(getShadowMapByIndex(light.shadowMapIndex), ShaderStageType::FRAGMENT);
							render->loadTexture(slotTex, ShaderStageType::FRAGMENT, light.framebuffer[0].getColorTexture());
							num2DShadows++;
						}else if (light.type == LightType::DIRECTIONAL){
							for (int c = 0; c < light.numShadowCascades; c++){
								slotTex = shaderData.getTextureIndex(getShadowMapByIndex(light.shadowMapIndex+c), ShaderStageType::FRAGMENT);
								render->loadTexture(slotTex, ShaderStageType::FRAGMENT, light.framebuffer[c].getColorTexture());
								num2DShadows++;
							}
						}
					}
				}
				if (MAX_SHADOWSMAP > num2DShadows){
					for (int s = num2DShadows; s < MAX_SHADOWSMAP; s++){
						slotTex = shaderData.getTextureIndex(getShadowMapByIndex(s), ShaderStageType::FRAGMENT);
						render->loadTexture(slotTex, ShaderStageType::FRAGMENT, &emptyBlack);
					}
				}
				if (MAX_SHADOWSCUBEMAP > numCubeShadows){
					for (int s = numCubeShadows; s < MAX_SHADOWSCUBEMAP; s++){
						slotTex = shaderData.getTextureIndex(getShadowMapCubeByIndex(s+MAX_SHADOWSMAP), ShaderStageType::FRAGMENT);
						render->loadTexture(slotTex, ShaderStageType::FRAGMENT, &emptyCubeBlack);
					}
				}
			}
		}

		if (Engine::isAutomaticTransparency() && !mesh.transparency){
			if (mesh.submeshes[i].material.baseColorTexture.isTransparent() || mesh.submeshes[i].material.baseColorFactor.w != 1.0){
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
					render->loadAttribute(shaderData.getAttrIndex(attr.first), buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized());
            	}
        	}
			if (buf.second->getBufferType() == BufferType::INDEX_BUFFER){
				indexCount = buf.second->getCount();
				Attribute indexattr = buf.second->getAttributes()[AttributeType::INDEX];
				render->loadIndex(buf.second->getRender(), indexattr.getDataType(), indexattr.getOffset());
			}
    	}

		for (auto const& attr : mesh.submeshes[i].attributes){
			if (attr.first == AttributeType::INDEX){
				indexCount = attr.second.getCount();
				render->loadIndex(bufferNameToRender[attr.second.getBuffer()], attr.second.getDataType(), attr.second.getOffset());
			}else{
				render->loadAttribute(shaderData.getAttrIndex(attr.first), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
			}
		}

		if (indexCount > 0){
			mesh.submeshes[i].vertexCount = indexCount;
		}else{
			mesh.submeshes[i].vertexCount = vertexBufferCount;
		}

		render->endLoad();

		//----------Start depth shader---------------
		if (hasShadows && mesh.castShadows){
			ObjectRender* depthRender = &mesh.submeshes[i].depthRender;

			depthRender->beginLoad(mesh.submeshes[i].primitiveType, true);

			depthRender->loadShader(mesh.submeshes[i].depthShader.get());
			ShaderData& depthShaderData = mesh.submeshes[i].depthShader.get()->shaderData;

			mesh.submeshes[i].slotVSDepthParams = depthShaderData.getUniformIndex(UniformType::DEPTH_VS_PARAMS, ShaderStageType::VERTEX);

			for (auto const& buf : mesh.buffers){
        		if (buf.second->isRenderAttributes()) {
					if (buf.second->getAttributes().count(AttributeType::POSITION)){
						Attribute posattr = buf.second->getAttributes()[AttributeType::POSITION];
						depthRender->loadAttribute(depthShaderData.getAttrIndex(AttributeType::POSITION), buf.second->getRender(), posattr.getElements(), posattr.getDataType(), buf.second->getStride(), posattr.getOffset(), posattr.getNormalized());
					}
        		}
				if (buf.second->getBufferType() == BufferType::INDEX_BUFFER){
					indexCount = buf.second->getCount();
					Attribute indexattr = buf.second->getAttributes()[AttributeType::INDEX];
					depthRender->loadIndex(buf.second->getRender(), indexattr.getDataType(), indexattr.getOffset());
				}
    		}

			for (auto const& attr : mesh.submeshes[i].attributes){
				if (attr.first == AttributeType::INDEX){
					depthRender->loadIndex(bufferNameToRender[attr.second.getBuffer()], attr.second.getDataType(), attr.second.getOffset());
				}else if (attr.first == AttributeType::POSITION){
					depthRender->loadAttribute(depthShaderData.getAttrIndex(AttributeType::POSITION), bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
				}
			}

			depthRender->endLoad();
		}
		//----------End depth shader---------------
	}

	mesh.loaded = true;

	return true;
}

void RenderSystem::drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform){
	if (mesh.loaded){
		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender* render = &mesh.submeshes[i].render;
			
			u_fs_pbrParams_t pbrParams_fs;
			pbrParams_fs.baseColorFactor = mesh.submeshes[i].material.baseColorFactor;
			pbrParams_fs.metallicFactor = mesh.submeshes[i].material.metallicFactor;
			pbrParams_fs.roughnessFactor = mesh.submeshes[i].material.roughnessFactor;
			pbrParams_fs.emissiveFactor = mesh.submeshes[i].material.emissiveFactor;
			pbrParams_fs.eyePos = camTransform.worldPosition;

			render->beginDraw();

			if (hasLights){
				render->applyUniform(mesh.submeshes[i].slotFSLighting, ShaderStageType::FRAGMENT, UniformDataType::FLOAT, 16 * MAX_LIGHTS, &fs_lighting);
				if (hasShadows && mesh.castShadows){
					render->applyUniform(mesh.submeshes[i].slotVSShadows, ShaderStageType::VERTEX, UniformDataType::FLOAT, 16 * (MAX_SHADOWSMAP), &vs_shadows);
					render->applyUniform(mesh.submeshes[i].slotFSShadows, ShaderStageType::FRAGMENT, UniformDataType::FLOAT, 4 * (MAX_SHADOWSMAP + MAX_SHADOWSCUBEMAP), &fs_shadows);
				}
			}

			render->applyUniform(mesh.submeshes[i].slotFSParams, ShaderStageType::FRAGMENT, UniformDataType::FLOAT, 16, &pbrParams_fs);

			//model, normal and mvp matrix
			render->applyUniform(mesh.submeshes[i].slotVSParams, ShaderStageType::VERTEX, UniformDataType::FLOAT, 48, &transform.modelMatrix);

			render->draw(mesh.submeshes[i].vertexCount);
		}
	}
}

void RenderSystem::drawMeshDepth(MeshComponent& mesh, Matrix4 modelLightSpaceMatrix){
	if (mesh.loaded && mesh.castShadows){
		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender* depthRender = &mesh.submeshes[i].depthRender;

			depthRender->beginDraw();
			
			//mvp matrix
			depthRender->applyUniform(mesh.submeshes[i].slotVSDepthParams, ShaderStageType::VERTEX, UniformDataType::FLOAT, 16, &modelLightSpaceMatrix);

			depthRender->draw(mesh.submeshes[i].vertexCount);
		}
	}
}

bool RenderSystem::loadSky(SkyComponent& sky){

	ObjectRender* render = &sky.render;

	render->beginLoad(PrimitiveType::TRIANGLES, false);

	ShaderType shaderType = ShaderType::SKYBOX;

	sky.shader = ShaderPool::get(shaderType, "");
	if (!sky.shader->isCreated())
		return false;
	render->loadShader(sky.shader.get());
	ShaderData& shaderData = sky.shader.get()->shaderData;

	sky.slotVSParams = shaderData.getUniformIndex(UniformType::VIEWPROJECTIONSKY, ShaderStageType::VERTEX);

	if (sky.texture.load()){
		render->loadTexture(shaderData.getTextureIndex(TextureShaderType::SKYCUBE, ShaderStageType::FRAGMENT), ShaderStageType::FRAGMENT, sky.texture.getRender());
	} else {
		return false;
	}

	sky.buffer->getRender()->createBuffer(sky.buffer->getSize(), sky.buffer->getData(), sky.buffer->getBufferType(), false);

	if (sky.buffer->isRenderAttributes()) {
        for (auto const &attr : sky.buffer->getAttributes()) {
			render->loadAttribute(shaderData.getAttrIndex(attr.first), sky.buffer->getRender(), attr.second.getElements(), attr.second.getDataType(), sky.buffer->getStride(), attr.second.getOffset(), attr.second.getNormalized());
        }
    }

	sky.loaded = true;

	render->endLoad();

	return true;
}

void RenderSystem::drawSky(SkyComponent& sky){
	if (sky.loaded){
		ObjectRender* render = &sky.render;
		if (render){
			render->beginDraw();
			render->applyUniform(sky.slotVSParams, ShaderStageType::VERTEX, UniformDataType::FLOAT, 16, &sky.skyViewProjectionMatrix);
			render->draw(36);
		}
	}
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

			Matrix4 projectionMatrix[light.numShadowCascades];
			Matrix4 viewMatrix;

			viewMatrix = Matrix4::lookAtMatrix(transform.worldPosition, light.worldDirection + transform.worldPosition, up);

			float zFar = camera.perspectiveFar;
            float zNear = camera.perspectiveNear;
            float fov = 0;
            float ratio = 1;

			float farPlaneOffset = (zFar - zNear) * 0.005;

            std::vector<float> splitFar;
            std::vector<float> splitNear;

			// Split perspective frustrum to create cascades
            if (camera.type == CameraType::CAMERA_PERSPECTIVE) {
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
            }

			// Get frustrum box and create light ortho
			for (int ca = 0; ca < light.numShadowCascades; ca++) {
				Matrix4 sceneCameraInv = (Matrix4::perspectiveMatrix(fov, ratio, splitNear[ca], splitFar[ca]+farPlaneOffset) * camera.viewMatrix).inverse();
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

				projectionMatrix[ca] = Matrix4::orthoMatrix(minX, maxX, minY, maxY, -maxZ, -minZ);

				light.cameras[ca].lightViewProjectionMatrix = projectionMatrix[ca] * viewMatrix;
				light.cameras[ca].nearFar = Vector2(splitNear[ca], splitFar[ca]);
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

	auto transforms = scene->getComponentArray<Transform>();
	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);
		
		if (transform.needUpdate){
			updateTransform(transform);
		}
	}

	if (camera.needUpdate){
		updateCamera(camera, cameraTransform);
		updateSkyViewProjection(camera);
	}

	for (int i = 0; i < transforms->size(); i++){
		Transform& transform = transforms->getComponentFromIndex(i);

		if (camera.needUpdate || transform.needUpdate){
			transform.modelViewProjectionMatrix = camera.viewProjectionMatrix * transform.modelMatrix;
			transform.distanceToCamera = (cameraTransform.worldPosition - transform.worldPosition).length();

			Entity entity = transforms->getEntity(i);
			LightComponent* light = scene->findComponent<LightComponent>(entity);
			if (light){
				updateLightFromTransform(*light, transform);
			}
		}
		transform.needUpdate = false;
	}
	camera.needUpdate = false;

	processLights();
}

void RenderSystem::draw(){
	auto meshes = scene->getComponentArray<MeshComponent>();

	//---------Depth shader----------
	if (hasShadows){
		auto lights = scene->getComponentArray<LightComponent>();
		
		for (int l = 0; l < lights->size(); l++){
			LightComponent& light = lights->getComponentFromIndex(l);

			if (light.intensity > 0){
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
	
	//---------Draw transparent meshes----------
	sceneRender.startDefaultFrameBuffer(System::instance().getScreenWidth(), System::instance().getScreenHeight());
	sceneRender.applyViewport(Engine::getViewRect());

	Transform& cameraTransform =  scene->getComponent<Transform>(scene->getCamera());
	
	for (int i = 0; i < meshes->size(); i++){
		MeshComponent& mesh = meshes->getComponentFromIndex(i);
		Entity entity = meshes->getEntity(i);
		Transform* transform = scene->findComponent<Transform>(entity);

		if (transform){
			if (!mesh.loaded){
				loadMesh(mesh);
			}
			if (!mesh.transparency){
				//Draw opaque meshes
				drawMesh(mesh, *transform, cameraTransform);
			}else{
				transparentMeshes.push({&mesh, transform, transform->distanceToCamera});
			}
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

	//---------Draw opaque meshes----------
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
			"> python supershader.py -s \"%s\" -p \"%s\" -l %s\n"
			"-------------------"
			, misShaders.c_str(), PROJECT_ROOT, ShaderPool::getShaderLangStr().c_str());
		exit(1);
	}
	
	SystemRender::commit();
}

void RenderSystem::entityDestroyed(Entity entity){
	auto mesh = scene->findComponent<MeshComponent>(entity);
	
	if (mesh){
		for (int i = 0; i < mesh->numSubmeshes; i++){

			Submesh& submesh = mesh->submeshes[i];

			//Destroy shader
			submesh.shader.reset();
			ShaderPool::remove(ShaderType::MESH, mesh->submeshes[i].shaderProperties);
			if (hasShadows && mesh->castShadows)
				ShaderPool::remove(ShaderType::DEPTH, "");

			//Destroy texture
			submesh.material.baseColorTexture.destroy();

			//Destroy render
			submesh.render.destroy();
		}

		//Destroy buffers
		for (auto const& buf : mesh->buffers){
			buf.second->getRender()->destroyBuffer();
		}
	}

	auto sky = scene->findComponent<SkyComponent>(entity);
	if (sky){
		//Destroy shader
		sky->shader.reset();
		ShaderPool::remove(sky->shaderType, "");

		//Destroy texture
		sky->texture.destroy();

		//Destroy render
		sky->render.destroy();

		//Destroy buffer
		if (sky->buffer){
			sky->buffer->getRender()->destroyBuffer();
		}
	}

}
