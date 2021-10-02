#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "SubSystem.h"
#include "component/MeshComponent.h"
#include "component/SkyComponent.h"
#include "component/SpriteComponent.h"
#include "component/CameraComponent.h"
#include "component/LightComponent.h"
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

		SceneRender sceneRender;
		SceneRender depthRender;

		static TextureRender emptyWhite;
		static TextureRender emptyBlack;
		static TextureRender emptyCubeBlack;
		static TextureRender emptyNormal;

		static bool emptyTexturesCreated;
		
		bool hasLights;
		bool hasShadows;

		fs_lighting_t fs_lighting;
		vs_shadows_t vs_shadows;
		fs_shadows_t fs_shadows;

		std::map<std::string, BufferRender*> bufferNameToRender;
		std::priority_queue<TransparentMeshesData, std::vector<TransparentMeshesData>, MeshComparison> transparentMeshes;

		void createEmptyTextures();
		void checkLightsAndShadow();
		bool processLights();
		TextureShaderType getShadowMapByIndex(int index);
		TextureShaderType getShadowMapCubeByIndex(int index);
		void configureLightShadowNearFar(LightComponent& light, const CameraComponent& camera);
		Matrix4 getDirLightProjection(const Matrix4& viewMatrix, const Matrix4& sceneCameraInv);

		float lerp(float a, float b, float fraction);

	public:
		RenderSystem(Scene* scene);

		bool loadMesh(MeshComponent& mesh);
		void drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform);
		void drawMeshDepth(MeshComponent& mesh, Matrix4 modelLightSpaceMatrix);

		bool loadSprite(SpriteComponent& sprite);
		void drawSprite(SpriteComponent& sprite, Transform& transform);

		bool loadSky(SkyComponent& sky);
		void drawSky(SkyComponent& sky);

		void updateTransform(Transform& transform);
		void updateCamera(CameraComponent& camera, Transform& transform);
		void updateSkyViewProjection(CameraComponent& camera);
		void updateLightFromTransform(LightComponent& light, Transform& transform);
	
		virtual void load();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //RENDERSYSTEM_H