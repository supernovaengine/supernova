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
#include <map>
#include <memory>
#include <queue>

namespace Supernova{

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

	public:
		RenderSystem(Scene* scene);

		bool loadMesh(MeshComponent& mesh);
		void drawMesh(MeshComponent& mesh, Transform& transform, Transform& camTransform);

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