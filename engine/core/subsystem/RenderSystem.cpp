#include "RenderSystem.h"
#include "object/Camera.h"
#include "math/Quaternion.h"
#include "Scene.h"
#include "Engine.h"
#include "System.h"
#include "render/Render.h"
#include "render/ObjectRender.h"
#include "pool/ShaderPool.h"
#include "pool/TexturePool.h"
#include "math/Vector3.h"
#include "math/Angle.h"
#include <memory>

#define NUM_LIGHTS 3

using namespace Supernova;

TextureRender RenderSystem::emptyWhite;
TextureRender RenderSystem::emptyBlack;
TextureRender RenderSystem::emptyNormal;

bool RenderSystem::emptyTexturesCreated = false;

RenderSystem::RenderSystem(Scene* scene): SubSystem(scene){
	signature.set(scene->getComponentType<Transform>());
	signature.set(scene->getComponentType<MeshComponent>());

	hasLights = true;
}

void RenderSystem::load(){
	createEmptyTextures();
}

void RenderSystem::createEmptyTextures(){
	if (!emptyTexturesCreated){
		TextureDataSize texData[1];

    	uint32_t pixels[64];

		texData[0].data = (void*)pixels;
		texData[0].size = 8 * 8 * 4;

		for (int i = 0; i < 64; i++) {
        	pixels[i] = 0xFFFFFFFF;
    	}

		emptyWhite.createTexture("empty|white", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, texData);

		for (int i = 0; i < 64; i++) {
        	pixels[i] = 0xFF000000;
    	}

		emptyBlack.createTexture("empty|black", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, texData);

		for (int i = 0; i < 64; i++) {
        	pixels[i] = 0xFF808080;
    	}

		emptyNormal.createTexture("empty|normal", 8, 8, ColorFormat::RGBA, TextureType::TEXTURE_2D, 1, texData);

		emptyTexturesCreated = true;
	}
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

		render->beginLoad(mesh.submeshes[i].primitiveType);

		TextureRender* textureRender = NULL;
		textureRender = mesh.submeshes[i].material.baseColorTexture.getRender();
		if (textureRender)
			render->loadTexture(textureRender, TextureSamplerType::BASECOLOR);
		else
			render->loadTexture(&emptyWhite, TextureSamplerType::BASECOLOR);

		if (hasLights){
			TextureRender* textureRender = NULL;
			textureRender = mesh.submeshes[i].material.metallicRoughnessTexture.getRender();
			if (textureRender)	
				render->loadTexture(textureRender, TextureSamplerType::METALLICROUGHNESS);
			else
				render->loadTexture(&emptyWhite, TextureSamplerType::METALLICROUGHNESS);

			textureRender = mesh.submeshes[i].material.normalTexture.getRender();
			if (textureRender){
				render->loadTexture(textureRender, TextureSamplerType::NORMAL);
				mesh.submeshes[i].hasNormalMap = true;
			}else{
				//render->loadTexture(&emptyNormal, TextureSamplerType::NORMAL);
				mesh.submeshes[i].hasNormalMap = false;
			}

			textureRender = mesh.submeshes[i].material.occlusionTexture.getRender();
			if (textureRender)	
				render->loadTexture(textureRender, TextureSamplerType::OCCULSION);
			else
				render->loadTexture(&emptyWhite, TextureSamplerType::OCCULSION);

			textureRender = mesh.submeshes[i].material.emissiveTexture.getRender();
			if (textureRender)
				render->loadTexture(textureRender, TextureSamplerType::EMISSIVE);	
			else
				render->loadTexture(&emptyBlack, TextureSamplerType::EMISSIVE);
		}

		if (Engine::isAutomaticTransparency() && !mesh.transparency){
			if (mesh.submeshes[i].material.baseColorTexture.isTransparent() || mesh.submeshes[i].material.baseColorFactor.w != 1.0){
				mesh.transparency = true;
			}
		}

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

		if (hasLights){
			if (mesh.submeshes[i].hasTangent && mesh.submeshes[i].hasNormalMap){
				mesh.submeshes[i].shaderType = ShaderType::MESH_PBR;
			}else if (!mesh.submeshes[i].hasTangent && mesh.submeshes[i].hasNormalMap){
				mesh.submeshes[i].shaderType = ShaderType::MESH_PBR_NOTAN;
			}else if (mesh.submeshes[i].hasTangent && !mesh.submeshes[i].hasNormalMap){
				mesh.submeshes[i].shaderType = ShaderType::MESH_PBR_NONMAP;
			}else{
				mesh.submeshes[i].shaderType = ShaderType::MESH_PBR_NONMAP_NOTAN;
			}
		}else{
			mesh.submeshes[i].shaderType = ShaderType::MESH_PBR_UNLIT;
		}

		mesh.submeshes[i].shader = ShaderPool::get(mesh.submeshes[i].shaderType);
		render->loadShader(mesh.submeshes[i].shader.get());
	
		unsigned int vertexBufferCount = 0;
		unsigned int indexCount = 0;

		for (auto const& buf : mesh.buffers){
        	if (buf.first == mesh.defaultBuffer) {
				vertexBufferCount = buf.second->getCount();
        	}
        	if (buf.second->isRenderAttributes()) {
            	for (auto const &attr : buf.second->getAttributes()) {
					render->loadAttribute(attr.first, mesh.submeshes[i].shaderType, buf.second->getRender(), attr.second.getElements(), attr.second.getDataType(), buf.second->getStride(), attr.second.getOffset(), attr.second.getNormalized());
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
				render->loadAttribute(attr.first, mesh.submeshes[i].shaderType, bufferNameToRender[attr.second.getBuffer()], attr.second.getElements(), attr.second.getDataType(), bufferStride[attr.second.getBuffer()], attr.second.getOffset(), attr.second.getNormalized());
			}
		}

		if (indexCount > 0){
			mesh.submeshes[i].vertexCount = indexCount;
		}else{
			mesh.submeshes[i].vertexCount = vertexBufferCount;
		}
	
		mesh.loaded = true;

		render->endLoad();
	}

	return true;
}

typedef struct u_fs_pbrParams_t {
    Supernova::Vector4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    uint8_t _pad_24[8];
    Supernova::Vector3 emissiveFactor;
    uint8_t _pad_44[4];
    Supernova::Vector3 eyePos;
    uint8_t _pad_60[4];
} u_fs_pbrParams_t;

typedef struct u_lighting_t {
    Supernova::Vector4 direction_range[NUM_LIGHTS];
    Supernova::Vector4 color_intensity[NUM_LIGHTS];
    Supernova::Vector4 position_type[NUM_LIGHTS];
    Supernova::Vector4 inner_outer_ConeCos[NUM_LIGHTS];
} u_lighting_t;

void RenderSystem::drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform){
	if (mesh.loaded){
		for (int i = 0; i < mesh.numSubmeshes; i++){
			ObjectRender* render = &mesh.submeshes[i].render;
			if (render){

				u_lighting_t lights;
				lights.direction_range[0] = Vector4(0.8f, -0.2, 0.0, 0.0);
				lights.color_intensity[0] = Vector4(1.0f, 1.0f, 1.0f, 0.0);
				lights.position_type[0] = Vector4(0.0f, 0.0f, 0.0f, 0.0);
				lights.inner_outer_ConeCos[0] = Vector4(0.0f, 0.0f, 0.0f, 0.0);

				lights.direction_range[1] = Vector4(-0.5f, -0.5, 0.0, 0.0);
				lights.color_intensity[1] = Vector4(1.0f, 1.0f, 1.0f, 10000.0);
				lights.position_type[1] = Vector4(-50.0f, 80.0f, 80.0f, 2.0);
				lights.inner_outer_ConeCos[1] = Vector4(cos(Angle::degToRad(25)), cos(Angle::degToRad(35)), 0.0f, 0.0);

				lights.direction_range[2] = Vector4(0.0, 0.0, 0.0, 0.0);
				lights.color_intensity[2] = Vector4(1.0f, 1.0f, 1.0f, 10000.0);
				lights.position_type[2] = Vector4(300.0f, 30.0f, 80.0f, 1.0);
				lights.inner_outer_ConeCos[2] = Vector4(0.0f, 0.0f, 0.0f, 0.0);

				u_fs_pbrParams_t pbrParams_fs;
				pbrParams_fs.baseColorFactor = mesh.submeshes[i].material.baseColorFactor;
				pbrParams_fs.metallicFactor = mesh.submeshes[i].material.metallicFactor;
				pbrParams_fs.roughnessFactor = mesh.submeshes[i].material.roughnessFactor;
				pbrParams_fs.emissiveFactor = mesh.submeshes[i].material.emissiveFactor;
				pbrParams_fs.eyePos = camTransform.worldPosition;


				render->beginDraw();

				if (mesh.submeshes[i].shaderType == ShaderType::MESH_PBR || 
					mesh.submeshes[i].shaderType == ShaderType::MESH_PBR_NONMAP_NOTAN ||
					mesh.submeshes[i].shaderType == ShaderType::MESH_PBR_NOTAN ||
					mesh.submeshes[i].shaderType == ShaderType::MESH_PBR_NONMAP){
					render->applyUniform(UniformType::LIGHTING, UniformDataType::FLOAT, 16 * NUM_LIGHTS, &lights);
				}

				render->applyUniform(UniformType::PBR_FS_PARAMS, UniformDataType::FLOAT, 16, &pbrParams_fs);

				//model, normal and mvp matrix
				render->applyUniform(UniformType::PBR_VS_PARAMS, UniformDataType::FLOAT, 48, &transform.modelMatrix);

				render->draw(mesh.submeshes[i].vertexCount);
			}
		}
	}
}

bool RenderSystem::loadSky(SkyComponent& sky){

	ObjectRender* render = &sky.render;

	render->beginLoad(PrimitiveType::TRIANGLES);

	ShaderType shaderType = ShaderType::SKYBOX;

	sky.shader = ShaderPool::get(shaderType);
	render->loadShader(sky.shader.get());

	if (sky.texture.load()){
		render->loadTexture(sky.texture.getRender(), TextureSamplerType::SKYCUBE);
	} else {
		return false;
	}

	sky.buffer->getRender()->createBuffer(sky.buffer->getSize(), sky.buffer->getData(), sky.buffer->getBufferType(), false);

	if (sky.buffer->isRenderAttributes()) {
        for (auto const &attr : sky.buffer->getAttributes()) {
			render->loadAttribute(attr.first, shaderType, sky.buffer->getRender(), attr.second.getElements(), attr.second.getDataType(), sky.buffer->getStride(), attr.second.getOffset(), attr.second.getNormalized());
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
			render->applyUniform(UniformType::VIEWPROJECTIONSKY, UniformDataType::FLOAT, 16, &sky.skyViewProjectionMatrix);
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

	//TODO: Check if lights is on
	transform.normalMatrix = transform.modelMatrix.inverse().transpose();
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
		}
		transform.needUpdate = false;
	}
	camera.needUpdate = false;
}

void RenderSystem::draw(){
	
	sceneRender.startFrameBuffer();
	sceneRender.applyViewport(Engine::getViewRect());

	auto meshes = scene->getComponentArray<MeshComponent>();
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

	SkyComponent* sky = scene->findComponentFromIndex<SkyComponent>(0);
	if (sky){
		if (!sky->loaded){
			loadSky(*sky);
		}

		drawSky(*sky);
	}

	while (!transparentMeshes.empty()){
		TransparentMeshesData meshData = transparentMeshes.top();

		//Draw transparent meshes
		drawMesh(*meshData.mesh, *meshData.transform, cameraTransform);

		transparentMeshes.pop();
	}

	sceneRender.endFrameBuffer();
}

void RenderSystem::entityDestroyed(Entity entity){
	auto mesh = scene->findComponent<MeshComponent>(entity);
	
	if (mesh){
		for (int i = 0; i < mesh->numSubmeshes; i++){

			Submesh& submesh = mesh->submeshes[i];

			//Destroy shader
			submesh.shader.reset();
			ShaderPool::remove(submesh.shaderType);

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
		ShaderPool::remove(sky->shaderType);

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
