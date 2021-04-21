#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "SubSystem.h"
#include "component/MeshComponent.h"
#include "component/SkyComponent.h"
#include "component/CameraComponent.h"
#include "component/Transform.h"
#include "render/ObjectRender.h"
#include "render/SceneRender.h"
#include "render/BufferRender.h"
#include "Supernova.h"
#include <map>
#include <memory>
#include <queue>

namespace Supernova{

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

		static TextureRender emptyWhite;
		static TextureRender emptyBlack;
		static TextureRender emptyNormal;

		static bool emptyTexturesCreated;
		
		bool hasLights;

		std::map<std::string, BufferRender*> bufferNameToRender;
		std::priority_queue<TransparentMeshesData, std::vector<TransparentMeshesData>, MeshComparison> transparentMeshes;

		void createEmptyTextures();
		u_lighting_t collectLights();

	public:
		RenderSystem(Scene* scene);

		bool loadMesh(MeshComponent& mesh);
		void drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform, u_lighting_t& lights);

		bool loadSky(SkyComponent& sky);
		void drawSky(SkyComponent& sky);

		void updateTransform(Transform& transform);
		void updateCamera(CameraComponent& camera, Transform& transform);
		void updateSkyViewProjection(CameraComponent& camera);
	
		virtual void load();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //RENDERSYSTEM_H