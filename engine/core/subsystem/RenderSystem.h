//
// (c) 2024 Eduardo Doria.
//

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "SubSystem.h"
#include "component/MeshComponent.h"
#include "component/InstancedMeshComponent.h"
#include "component/ModelComponent.h"
#include "component/SkyComponent.h"
#include "component/UILayoutComponent.h"
#include "component/UIComponent.h"
#include "component/ImageComponent.h"
#include "component/CameraComponent.h"
#include "component/LightComponent.h"
#include "component/PointsComponent.h"
#include "component/LinesComponent.h"
#include "component/TerrainComponent.h"
#include "component/SpriteComponent.h"
#include "component/Transform.h"
#include "render/ObjectRender.h"
#include "render/CameraRender.h"
#include "render/BufferRender.h"
#include "render/FramebufferRender.h"
#include "Engine.h"
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
		Vector4 globalIllum; //global illumination
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

	typedef struct vs_depth_t {
		Matrix4 modelMatrix;
	    Matrix4 lightSpaceMatrix;
	} vs_depth_t;

	typedef struct check_load_t {
		Scene* scene;
		Entity entity;
	} check_load_t;

	class SUPERNOVA_API RenderSystem : public SubSystem {
	private:
		struct TransparentMeshesData{
			MeshComponent* mesh;
			InstancedMeshComponent* instmesh;
			TerrainComponent* terrain;
			Transform* transform;
			float distanceToCamera;
		};

		struct MeshComparison{
			bool const operator()(const TransparentMeshesData& lhs, const TransparentMeshesData& rhs) const{
				return lhs.distanceToCamera < rhs.distanceToCamera;
			}
		};

		Scene* scene;

		static uint32_t pixelsWhite[64];
		static uint32_t pixelsBlack[64];
		static uint32_t pixelsNormal[64];

		static TextureRender emptyWhite;
		static TextureRender emptyBlack;
		static TextureRender emptyCubeBlack;
		static TextureRender emptyNormal;

		static bool emptyTexturesCreated;
		
		bool hasLights;
		bool hasShadows;
		bool hasFog;
		bool hasMultipleCameras;

		fs_lighting_t fs_lighting;
		vs_shadows_t vs_shadows;
		fs_shadows_t fs_shadows;
		fs_fog_t fs_fog;

		static void changeLoaded(void* data);
		static void changeDestroy(void* data);

		void updateMVP(size_t index, Transform& transform, CameraComponent& camera, Transform& cameraTransform);

		void createFramebuffer(CameraComponent& camera);
		void createEmptyTextures();
		int checkLightsAndShadow();
		bool loadLights(int numLights);
		void processLights(Transform& cameraTransform);
		bool loadAndProcessFog();
		TextureShaderType getShadowMapByIndex(int index);
		TextureShaderType getShadowMapCubeByIndex(int index);
		void configureLightShadowNearFar(LightComponent& light, const CameraComponent& camera);
		Matrix4 getDirLightProjection(const Matrix4& viewMatrix, const Matrix4& sceneCameraInv);
		bool checkPBRFrabebufferUpdate(Material& material);
		bool loadPBRTextures(Material& material, ShaderData& shaderData, ObjectRender& render, bool shadows);
		void loadDepthTexture(Material& material, ShaderData& shaderData, ObjectRender& render);
		void loadTerrainTextures(TerrainComponent& terrain, ObjectRender& render, ShaderData& shaderData);
		Rect getScissorRect(UILayoutComponent& layout, ImageComponent& img, Transform& transform, CameraComponent& camera);

		// terrain
		bool terrainNodeLODSelect(TerrainComponent& terrain, Transform& transform, CameraComponent& camera, Transform& cameraTransform, TerrainNode& terrainNode, int lodLevel);
		AABB getTerrainNodeAABB(Transform& transform, TerrainNode& terrainNode);
		bool isTerrainNodeInSphere(Vector3 position, float radius, const AABB& box);

		float lerp(float a, float b, float fraction);

	protected:

		bool drawMesh(MeshComponent& mesh, Transform& transform, CameraComponent& camera, Transform& camTransform, bool renderToTexture, InstancedMeshComponent* instmesh, TerrainComponent* terrain);
		bool drawMeshDepth(MeshComponent& mesh, const float cameraFar, const Plane frustumPlanes[6], vs_depth_t vsDepthParams, InstancedMeshComponent* instmesh, TerrainComponent* terrain);
		void destroyMesh(Entity entity, MeshComponent& mesh);

		bool drawUI(UIComponent& ui, Transform& transform, bool renderToTexture);
		void destroyUI(Entity entity, UIComponent& ui);

		bool drawPoints(PointsComponent& points, Transform& transform, Transform& camTransform, bool renderToTexture);
		void destroyPoints(Entity entity, PointsComponent& points);

		bool drawLines(LinesComponent& lines, Transform& transform, Transform& camTransform, bool renderToTexture);
		void destroyLines(Entity entity, LinesComponent& lines);

		bool drawSky(SkyComponent& sky, bool renderToTexture);
		void destroySky(Entity entity, SkyComponent& sky);

		void destroyLight(LightComponent& light);
		void destroyCamera(CameraComponent& camera, bool entityDestroyed);
		
		void updateSkyViewProjection(SkyComponent& sky, CameraComponent& camera);
		void updateLightFromScene(LightComponent& light, Transform& transform, CameraComponent& camera);
		void updatePoints(PointsComponent& points, Transform& transform, CameraComponent& camera, Transform& camTransform);
		void updateTerrain(TerrainComponent& terrain, Transform& transform, CameraComponent& camera, Transform& cameraTransform);
		void updateCameraFrustumPlanes(const Matrix4 viewProjectionMatrix, Plane* frustumPlanes);
		void updateInstancedMesh(InstancedMeshComponent& instmesh, MeshComponent& mesh, Transform& transform, CameraComponent& camera, Transform& camTransform);

		void sortPoints(PointsComponent& points, Transform& transform, CameraComponent& camera, Transform& camTransform);
		void sortInstancedMesh(InstancedMeshComponent& instmesh, MeshComponent& mesh, Transform& transform, CameraComponent& camera, Transform& camTransform);

	public:

		RenderSystem(Scene* scene);
		virtual ~RenderSystem();

		bool loadMesh(Entity entity, MeshComponent& mesh, uint8_t pipelines, InstancedMeshComponent* instmesh, TerrainComponent* terrain);
		bool loadPoints(Entity entity, PointsComponent& points, uint8_t pipelines);
		bool loadLines(Entity entity, LinesComponent& lines, uint8_t pipelines);
		bool loadUI(Entity entity, UIComponent& ui, uint8_t pipelines, bool isText);
		bool loadSky(Entity entity, SkyComponent& sky, uint8_t pipelines);

		void updateFramebuffer(CameraComponent& camera);
		void updateTransform(Transform& transform);
		void updateCamera(CameraComponent& camera, Transform& transform);

		// camera
		void updateCameraSize(Entity entity);
		bool isInsideCamera(const float cameraFar, const Plane frustumPlanes[6], const AABB& box);
		bool isInsideCamera(CameraComponent& camera, const AABB& box);
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