//
// (c) 2024 Eduardo Doria.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "SubSystem.h"
#include "component/Body2DComponent.h"
#include "component/Joint2DComponent.h"
#include "object/physics/Contact2D.h"
#include "object/physics/Manifold2D.h"
#include "object/physics/ContactImpulse2D.h"
#include "object/physics/Body3D.h"
#include "object/physics/CollideShapeResult3D.h"
#include "object/physics/Contact3D.h"

class b2World;
class b2Body;
class b2Shape;
class b2JointDef;


namespace JPH{
	class Shape;
	class PhysicsSystem;
	class TempAllocatorImpl;
	class JobSystemThreadPool;
	class ShapeSettings;
};
class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class ObjectLayerPairFilterImpl;

namespace Supernova{

	class Box2DContactListener;
	class Box2DContactFilter;

	class JoltActivationListener;
	class JoltContactListener;
	

	class PhysicsSystem : public SubSystem {

	private:
		Vector3 gravity;

		b2World* world2D;
		float pointsToMeterScale2D;

		Box2DContactListener* contactListener2D;
		Box2DContactFilter* contactFilter2D;

		JoltActivationListener* activationListener3D;
		JoltContactListener* contactListener3D;

        JPH::TempAllocatorImpl* temp_allocator;
        JPH::JobSystemThreadPool* job_system;
		JPH::PhysicsSystem* world3D;

		BPLayerInterfaceImpl* broad_phase_layer_interface;
		ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter;
		ObjectLayerPairFilterImpl* object_vs_object_layer_filter;

		void updateBody2DPosition(Signature signature, Entity entity, Body2DComponent& body);
		void updateBody3DPosition(Signature signature, Entity entity, Body3DComponent& body);

		void createGenericJoltBody(Entity entity, Body3DComponent& body, const JPH::Shape* shape);

	public:
		PhysicsSystem(Scene* scene);
		virtual ~PhysicsSystem();

		Vector3 getGravity() const;
		void setGravity(Vector3 gravity);
		void setGravity(float x, float y);
		void setGravity(float x, float y, float z);

		float getPointsToMeterScale2D() const;
		void setPointsToMeterScale2D(float pointsToMeterScale2D);

		FunctionSubscribe<void(Contact2D)> beginContact2D;
		FunctionSubscribe<void(Contact2D)> endContact2D;
		FunctionSubscribe<void(Contact2D, Manifold2D)> preSolve2D;
		FunctionSubscribe<void(Contact2D, ContactImpulse2D)> postSolve2D;

		FunctionSubscribe<bool(Body2D, unsigned long, Body2D, unsigned long)> shouldCollide2D;

		FunctionSubscribe<void(Body3D)> onBodyActivated3D;
		FunctionSubscribe<void(Body3D)> onBodyDeactivated3D;
		FunctionSubscribe<void(Body3D, Body3D, Contact3D)> onContactAdded3D;
		FunctionSubscribe<void(Body3D, Body3D, Contact3D)> onContactPersisted3D;
		FunctionSubscribe<void(Body3D, Body3D, size_t, size_t)> onContactRemoved3D;

		FunctionSubscribe<bool(Body3D, Body3D, Vector3, CollideShapeResult3D)> shouldCollide3D;

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

		int createBoxShape3D(Entity entity, Vector3 position, Quaternion rotation, float width, float height, float depth);
		int createSphereShape3D(Entity entity, Vector3 position, Quaternion rotation, float radius);
		int createCapsuleShape3D(Entity entity, Vector3 position, Quaternion rotation, float halfHeight, float radius);
		int createTaperedCapsuleShape3D(Entity entity, Vector3 position, Quaternion rotation, float halfHeight, float topRadius, float bottomRadius);
		int createCylinderShape3D(Entity entity, Vector3 position, Quaternion rotation, float halfHeight, float radius);
		int createConvexHullShape3D(Entity entity, Vector3 position, Quaternion rotation, std::vector<Vector3> vertices);
		int createMeshShape3D(Entity entity, Vector3 position, Quaternion rotation, std::vector<Vector3> vertices, std::vector<uint16_t> indices);
		int createMeshShape3D(Entity entity, MeshComponent& mesh);
		int createHeightFieldShape3D(Entity entity, TerrainComponent& terrain, unsigned int samplesSize);

		b2World* getWorld2D() const;
		JPH::PhysicsSystem* getWorld3D() const;

		b2Body* getBody(Entity entity);

		bool loadBody2D(Entity entity);
		void destroyBody2D(Body2DComponent& body);

		bool loadBody3D(Entity entity);
		void destroyBody3D(Body3DComponent& body);

		int loadShape2D(Body2DComponent& body, b2Shape* shape);
		void destroyShape2D(Body2DComponent& body, size_t index);

		int loadShape3D(Body3DComponent& body, const Vector3& position, const Quaternion& rotation, JPH::ShapeSettings* shapeSettings);
		void destroyShape3D(Body3DComponent& body, size_t index);

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

		bool loadFixedJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB);
		bool loadDistanceJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchorA, Vector3 anchorB);
		bool loadPointJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor);
		bool loadHingeJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor, Vector3 axis, Vector3 normal);
		bool loadConeJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor, Vector3 twistAxis);
		bool loadPrismaticJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 sliderAxis, float limitsMin, float limitsMax);
		bool loadSwingTwistJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor, Vector3 twistAxis, Vector3 planeAxis, float normalHalfConeAngle, float planeHalfConeAngle, float twistMinAngle, float twistMaxAngle);
		bool loadSixDOFJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchorA, Vector3 anchorB, Vector3 axisX, Vector3 axisY);
		bool loadPathJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, std::vector<Vector3> positions, std::vector<Vector3> tangents, std::vector<Vector3> normals, Vector3 pathPosition, bool isLooping);
		bool loadGearJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Entity hingeA, Entity hingeB, int numTeethGearA, int numTeethGearB);
		bool loadRackAndPinionJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Entity hinge, Entity slider, int numTeethRack, int numTeethGear, int rackLength);
		bool loadPulleyJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchorA, Vector3 anchorB, Vector3 fixedPointA, Vector3 fixedPointB);
		void destroyJoint3D(Joint3DComponent& joint);

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //PHYSICSSYSTEM_H