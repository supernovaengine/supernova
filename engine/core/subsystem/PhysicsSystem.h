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
		float pointsToMeterScale;

	public:
		PhysicsSystem(Scene* scene);
		virtual ~PhysicsSystem();

		void createBody2D(Entity entity);
		void removeBody2D(Entity entity);
		int addRectShape2D(Entity entity, float width, float height);

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