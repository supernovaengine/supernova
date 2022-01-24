//
// (c) 2022 Eduardo Doria.
//

#ifndef MESHSYSTEM_H
#define MESHSYSTEM_H

#include "SubSystem.h"

#include "component/MeshComponent.h"
#include "component/SpriteComponent.h"
#include "component/CameraComponent.h"

namespace Supernova{

	class MeshSystem : public SubSystem {

    private:
        void createSprite(SpriteComponent& sprite, MeshComponent& mesh);
		void checkFlipY(SpriteComponent& sprite, CameraComponent& camera, MeshComponent& mesh);

	public:
		MeshSystem(Scene* scene);
		virtual ~MeshSystem();

		virtual void load();
        virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //MESHSYSTEM_H