//
// (c) 2023 Eduardo Doria.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "SubSystem.h"
#include "component/Body2DComponent.h"
#include "component/Joint2DComponent.h"

class b2World;
class b2Body;

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

		b2Body* getBody(Entity entity);

		bool loadBody2D(Body2DComponent& body);
		void destroyBody2D(Body2DComponent& body);

		bool loadShape2D(Body2DComponent& body, size_t index);
		void destroyShape2D(Body2DComponent& body, size_t index);

		bool loadJoint2D(Joint2DComponent& joint);
		void destroyJoint2D(Joint2DComponent& joint);

        void initDistanceJoint(Entity entity, Entity bodyA, Entity bodyB, Vector2 worldAnchorOnBodyA, Vector2 worldAnchorOnBodyB);

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //PHYSICSSYSTEM_H