//
// (c) 2021 Eduardo Doria.
//

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "SubSystem.h"
#include "component/MeshComponent.h"
#include "component/ModelComponent.h"
#include "component/SkyComponent.h"
#include "component/UIComponent.h"
#include "component/ImageComponent.h"
#include "component/CameraComponent.h"
#include "component/LightComponent.h"
#include "component/ParticlesComponent.h"
#include "component/TerrainComponent.h"
#include "component/Transform.h"
#include "render/ObjectRender.h"
#include "render/SceneRender.h"
#include "render/BufferRender.h"
#include "render/FramebufferRender.h"
#include "Supernova.h"
#include <map>
#include <memory>
#include <queue>

namespace Supernova{
	typedef struct fs_lighting_t {
	    Vector4 direction_range[MAX_LIGHTS];
	    Vector4 color_intensity[MAX_LIGHTS];
	    Vector4 position_type[MAX_LIGHTS];
	    Vector4 inCon_ouCon_shadows_cascades[MAX_LIGHTS];
		Vector4 eyePos;
	} fs_lighting_t;

	typedef struct fs_fog_t {
		Vector4 color_type;
		Vector4 density_start_end;
	} fs_fog_t;

	typedef struct vs_shadows_t {
	    Matrix4 lightViewProjectionMatrix[MAX_SHADOWSMAP];
	} vs_shadows_t;

	typedef struct fs_shadows_t {
        Vector4 bias_texSize_nearFar[MAX_SHADOWSMAP + MAX_SHADOWSCUBEMAP];
	} fs_shadows_t;

	class RenderSystem : public SubSystem {
	private:
		struct TransparentMeshesData{
			MeshComponent* mesh;
			Transform* transform;
			float distanceToCamera;
		};

		struct MeshComparison{
			bool const operator()(const TransparentMeshesData& lhs, const TransparentMeshesData& rhs) const{
				return lhs.distanceToCamera < rhs.distanceToCamera;
			}
		};

		Scene* scene;
		SceneRender sceneRender;
		SceneRender depthRender;

		static TextureRender emptyWhite;
		static TextureRender emptyBlack;
		static TextureRender emptyCubeBlack;
		static TextureRender emptyNormal;

		static bool emptyTexturesCreated;
		
		bool hasLights;
		bool hasShadows;
		bool hasFog;

		fs_lighting_t fs_lighting;
		vs_shadows_t vs_shadows;
		fs_shadows_t fs_shadows;
		fs_fog_t fs_fog;

		std::map<std::string, BufferRender*> bufferNameToRender;
		std::priority_queue<TransparentMeshesData, std::vector<TransparentMeshesData>, MeshComparison> transparentMeshes;

		void createEmptyTextures();
		void checkLightsAndShadow();
		bool processLights();
		bool processFog();
		TextureShaderType getShadowMapByIndex(int index);
		TextureShaderType getShadowMapCubeByIndex(int index);
		void configureLightShadowNearFar(LightComponent& light, const CameraComponent& camera);
		Matrix4 getDirLightProjection(const Matrix4& viewMatrix, const Matrix4& sceneCameraInv);
		void loadPBRTextures(Material& material, ShaderData& shaderData, ObjectRender& render, bool castShadows);
		Rect getScissorRect(UIComponent& ui, ImageComponent& img, Transform& transform, CameraComponent& camera);

		// terrain
		void setTerrainNodeIndex(TerrainComponent& terrain, TerrainNode& terrainNode, size_t size, size_t offset);
		bool terrainNodeLODSelect(TerrainComponent& terrain, Transform& transform, CameraComponent& camera, Transform& cameraTransform, TerrainNode& terrainNode, int lodLevel);
		AlignedBox getTerrainNodeAlignedBox(Transform& transform, TerrainNode& terrainNode);
		bool isTerrainNodeInSphere(Vector3 position, float radius, const AlignedBox& box);

		float lerp(float a, float b, float fraction);

	protected:

		bool loadMesh(MeshComponent& mesh);
		void drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform);
		void drawMeshDepth(MeshComponent& mesh, Matrix4 modelLightSpaceMatrix);
		void destroyMesh(MeshComponent& mesh);

		bool loadUI(UIComponent& ui, bool isText);
		void drawUI(UIComponent& ui, Transform& transform);
		void destroyUI(UIComponent& ui);

		bool loadParticles(ParticlesComponent& particles);
		void drawParticles(ParticlesComponent& particles, Transform& transform, Transform& camTransform);
		void destroyParticles(ParticlesComponent& particles);

		bool loadSky(SkyComponent& sky);
		void drawSky(SkyComponent& sky);
		void destroySky(SkyComponent& sky);

		void updateTransform(Transform& transform);
		void updateCamera(CameraComponent& camera, Transform& transform);
		void updateSkyViewProjection(CameraComponent& camera);
		void updateLightFromTransform(LightComponent& light, Transform& transform);
		void updateParticles(ParticlesComponent& particles, Transform& transform, CameraComponent& camera, Transform& camTransform);
		void updateTerrain(TerrainComponent& terrain, Transform& transform, CameraComponent& camera, Transform& cameraTransform);
		bool updateCameraFrustumPlanes(CameraComponent& camera);

	public:

		RenderSystem(Scene* scene);
		virtual ~RenderSystem();

		// camera
		float getCameraFar(CameraComponent& camera);
		bool isInsideCamera(CameraComponent& camera, const AlignedBox& box);
		bool isInsideCamera(CameraComponent& camera, const Vector3& point);
		bool isInsideCamera(CameraComponent& camera, const Vector3& center, const float& radius);
	
		virtual void load();
		virtual void destroy();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //RENDERSYSTEM_H