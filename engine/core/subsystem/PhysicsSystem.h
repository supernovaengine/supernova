//
// (c) 2023 Eduardo Doria.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "SubSystem.h"
#include "component/Body2DComponent.h"

class b2World;

namespace Supernova{

	class PhysicsSystem : public SubSystem {

	private:
		b2World* world2D;

	public:
		PhysicsSystem(Scene* scene);
		virtual ~PhysicsSystem();

		void createBody2D(Entity entity);
		int addRectShape2D(Entity entity, float width, float height);

        void setShape2DDensity(Entity entity, size_t index, float density);
        void setShape2DFriction(Entity entity, size_t index, float friction);
        void setShape2DRestitution(Entity entity, size_t index, float restitution);

		bool loadBody2D(Body2DComponent& body);
		void destroyBody2D(Body2DComponent& body);

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //PHYSICSSYSTEM_H