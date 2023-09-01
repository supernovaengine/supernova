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
class b2Shape;
class b2JointDef;

namespace Supernova{

	class PhysicsSystem : public SubSystem {

	private:
		b2World* world2D;
		float pointsToMeterScale;

		void updateBodyPosition(Signature signature, Entity entity, Body2DComponent& body, bool updateAnyway);

	public:
		PhysicsSystem(Scene* scene);
		virtual ~PhysicsSystem();

		void createBody2D(Entity entity);
		void removeBody2D(Entity entity);
		int addRectShape2D(Entity entity, float width, float height);

		b2Body* getBody(Entity entity);

		bool loadBody2D(Body2DComponent& body);
		void destroyBody2D(Body2DComponent& body);

		bool loadShape2D(Body2DComponent& body, b2Shape* shape, size_t index);
		void destroyShape2D(Body2DComponent& body, size_t index);

		bool loadDistanceJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchorA, Vector2 anchorB);
		bool loadRevoluteJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor);
		bool loadPrismaticJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor, Vector2 axis);
		bool loadPulleyJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 groundAnchorA, Vector2 groundAnchorB, Vector2 anchorA, Vector2 anchorB, Vector2 axis, float ratio);
		bool loadGearJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Entity revoluteJoint, Entity prismaticJoint, float ratio);
		bool loadMouseJoint2D(Joint2DComponent& joint, Entity body, Vector2 target);
		bool loadWheelJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor, Vector2 axis);
		bool loadWeldJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor);
		bool loadFrictionJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor);
		bool loadMotorJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB);
		bool loadRopeJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchorA, Vector2 anchorB);
		void destroyJoint2D(Joint2DComponent& joint);

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //PHYSICSSYSTEM_H