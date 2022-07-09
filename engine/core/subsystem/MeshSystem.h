//
// (c) 2022 Eduardo Doria.
//

#ifndef MESHSYSTEM_H
#define MESHSYSTEM_H

#include "SubSystem.h"

#include "component/MeshComponent.h"
#include "component/ModelComponent.h"
#include "component/SpriteComponent.h"
#include "component/MeshPolygonComponent.h"
#include "component/CameraComponent.h"
#include "component/TerrainComponent.h"

namespace Supernova{

	class MeshSystem : public SubSystem {

    private:
        void createSprite(SpriteComponent& sprite, MeshComponent& mesh);
		void createMeshPolygon(MeshPolygonComponent& polygon, MeshComponent& mesh);

		void changeFlipY(bool& flipY, CameraComponent& camera, MeshComponent& mesh);

		// Terrain
		size_t getTerrainGridArraySize(int rootGridSize, int levels);
		float getTerrainHeight(TerrainComponent& terrain, float x, float y);
		float maxTerrainHeightArea(TerrainComponent& terrain, float x, float z, float w, float h);
		float minTerrainHeightArea(TerrainComponent& terrain, float x, float z, float w, float h);
		TerrainNodeIndex createPlaneNodeBuffer(TerrainComponent& terrain, int width, int height, int widthSegments, int heightSegments);
		void createTerrain(TerrainComponent& terrain);
		void createTerrainNode(TerrainComponent& terrain, float x, float y, float size, int lodDepth);

	public:
		MeshSystem(Scene* scene);
		virtual ~MeshSystem();

		void destroyModel(ModelComponent& model);

		virtual void load();
		virtual void destroy();
        virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //MESHSYSTEM_H