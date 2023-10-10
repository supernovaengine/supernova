//
// (c) 2023 Eduardo Doria.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "SubSystem.h"
#include "component/Body2DComponent.h"
#include "component/Joint2DComponent.h"
#include "object/physics/Contact2D.h"
#include "object/physics/Manifold2D.h"
#include "object/physics/ContactImpulse2D.h"

class b2World;
class b2Body;
class b2Shape;
class b2JointDef;


namespace JPH{
	class Shape;
	class PhysicsSystem;
	class TempAllocatorImpl;
	class JobSystemThreadPool;
};
class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class ObjectLayerPairFilterImpl;

namespace Supernova{

	class Box2DContactListener;
	class Box2DContactFilter;
	

	class PhysicsSystem : public SubSystem {

	private:
		b2World* world2D;
		float pointsToMeterScale;

		Box2DContactListener* contactListener2D;
		Box2DContactFilter* contactFilter2D;

        JPH::TempAllocatorImpl* temp_allocator;
        JPH::JobSystemThreadPool* job_system;
		JPH::PhysicsSystem* world3D;

		BPLayerInterfaceImpl* broad_phase_layer_interface;
		ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter;
		ObjectLayerPairFilterImpl* object_vs_object_layer_filter;

		void updateBody2DPosition(Signature signature, Entity entity, Body2DComponent& body, bool updateAnyway);
		void updateBody3DPosition(Signature signature, Entity entity, Body3DComponent& body, bool updateAnyway);

		void createGenericJoltBody(Entity entity, Body3DComponent& body, BodyType type, const JPH::Shape* shape);

	public:
		PhysicsSystem(Scene* scene);
		virtual ~PhysicsSystem();

		float getPointsToMeterScale() const;
		void setPointsToMeterScale(float pointsToMeterScale);

		FunctionSubscribe<void(Contact2D)> beginContact2D;
		FunctionSubscribe<void(Contact2D)> endContact2D;
		FunctionSubscribe<void(Contact2D, Manifold2D)> preSolve2D;
		FunctionSubscribe<void(Contact2D, ContactImpulse2D)> postSolve2D;

		FunctionSubscribe<bool(Body2D, size_t, Body2D, size_t)> shouldCollide2D;

		void createBody2D(Entity entity);
		void removeBody2D(Entity entity);

		int createRectShape2D(Entity entity, float width, float height);
		int createCenteredRectShape2D(Entity entity, float width, float height, Vector2 center, float angle);
		int createPolygonShape2D(Entity entity, std::vector<Vector2> vertices);
		int createCircleShape2D(Entity entity, Vector2 center, float radius);
		int createTwoSidedEdgeShape2D(Entity entity, Vector2 vertice1, Vector2 vertice2);
		int createOneSidedEdgeShape2D(Entity entity, Vector2 vertice0, Vector2 vertice1, Vector2 vertice2, Vector2 vertice3);
		int createLoopChainShape2D(Entity entity, std::vector<Vector2> vertices);
		int createChainShape2D(Entity entity, std::vector<Vector2> vertices, Vector2 prevVertex, Vector2 nextVertex);

		void removeAllShapes2D(Entity entity);

		void createBody3D(Entity entity);
		void removeBody3D(Entity entity);

		void createBoxShape3D(Entity entity, BodyType type, float width, float height, float depth);
		void createSphereShape3D(Entity entity, BodyType type, float radius);

		b2World* getWorld2D() const;
		JPH::PhysicsSystem* getWorld3D() const;

		b2Body* getBody(Entity entity);

		bool loadBody2D(Entity entity);
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

		bool loadDistanceJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchorA, Vector3 anchorB);

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //PHYSICSSYSTEM_H