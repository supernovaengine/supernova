//
// (c) 2023 Eduardo Doria.
//

#ifndef JoltPhysicsAux_h
#define JoltPhysicsAux_h

#include "subsystem/PhysicsSystem.h"
#include "object/physics/Body3D.h"
#include "object/physics/CollideShapeResult3D.h"
#include "object/physics/Contact3D.h"

#include "Jolt/Jolt.h"

#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"
#include "Jolt/Physics/Collision/GroupFilterTable.h"

#include "Jolt/Physics/Constraints/FixedConstraint.h"
#include "Jolt/Physics/Constraints/DistanceConstraint.h"
#include "Jolt/Physics/Constraints/PointConstraint.h"
#include "Jolt/Physics/Constraints/HingeConstraint.h"
#include "Jolt/Physics/Constraints/ConeConstraint.h"
#include "Jolt/Physics/Constraints/SliderConstraint.h"
#include "Jolt/Physics/Constraints/SwingTwistConstraint.h"
#include "Jolt/Physics/Constraints/SixDOFConstraint.h"
#include "Jolt/Physics/Constraints/PathConstraint.h"
#include "Jolt/Physics/Constraints/PathConstraintPathHermite.h"
#include "Jolt/Physics/Constraints/GearConstraint.h"
#include "Jolt/Physics/Constraints/RackAndPinionConstraint.h"
#include "Jolt/Physics/Constraints/PulleyConstraint.h"


// Based on: https://github.com/jrouwe/JoltPhysics/blob/master/HelloWorld/HelloWorld.cpp
namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};
namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr unsigned int NUM_LAYERS(2);
};


class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			//JPH_ASSERT(false);
			return false;
		}
	}
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface{
private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];

public:
	BPLayerInterfaceImpl(){
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual unsigned int GetNumBroadPhaseLayers() const override{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override{
		//JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			//JPH_ASSERT(false);
			return false;
		}
	}
};


namespace Supernova{

	class JoltActivationListener : public JPH::BodyActivationListener{
	private:
        Scene* scene;
        PhysicsSystem* physicsSystem;
	public:
        JoltActivationListener(Scene* scene, PhysicsSystem* physicsSystem){
            this->scene = scene;
            this->physicsSystem = physicsSystem;
        }

		virtual void OnBodyActivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override{
			physicsSystem->onBodyActivated3D(Body3D(scene, inBodyUserData));
		}

		virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override{
			physicsSystem->onBodyDeactivated3D(Body3D(scene, inBodyUserData));
		}
	};

	class JoltContactListener : public JPH::ContactListener{
    private:
        Scene* scene;
        PhysicsSystem* physicsSystem;
	public:
        JoltContactListener(Scene* scene, PhysicsSystem* physicsSystem){
            this->scene = scene;
            this->physicsSystem = physicsSystem;
        }

		// See: ContactListener
		virtual JPH::ValidateResult	OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override{
			Entity entity1 = inBody1.GetUserData();
			Body3D body1(scene, entity1);

			Entity entity2 = inBody2.GetUserData();
			Body3D body2(scene, entity2);

			if (!physicsSystem->shouldCollide3D.callRet(body1, body2, Vector3(inBaseOffset.GetX(), inBaseOffset.GetY(), inBaseOffset.GetZ()), CollideShapeResult3D(scene, &inCollisionResult), true)){
				return JPH::ValidateResult::RejectAllContactsForThisBodyPair;
			}

			return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override{
			Entity entity1 = inBody1.GetUserData();
			Body3D body1(scene, entity1);

			Entity entity2 = inBody2.GetUserData();
			Body3D body2(scene, entity2);

			physicsSystem->onContactAdded3D(body1, body2, Contact3D(scene, &inManifold, &ioSettings));
		}

		virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override{
			Entity entity1 = inBody1.GetUserData();
			Body3D body1(scene, entity1);

			Entity entity2 = inBody2.GetUserData();
			Body3D body2(scene, entity2);

			physicsSystem->onContactPersisted3D(body1, body2, Contact3D(scene, &inManifold, &ioSettings));
		}

		virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override{
			JPH::BodyInterface &body_interface = physicsSystem->getWorld3D()->GetBodyInterface();

			Body3D body1(scene, body_interface.GetUserData(inSubShapePair.GetBody1ID()));
			Body3D body2(scene, body_interface.GetUserData(inSubShapePair.GetBody2ID()));

			int32_t subShapeID1 = inSubShapePair.GetSubShapeID1().GetValue();
			int32_t subShapeID2 = inSubShapePair.GetSubShapeID2().GetValue();

			physicsSystem->onContactRemoved3D(body1, body2, (int)subShapeID1, (int)subShapeID2);
		}
	};

}

#endif //JoltPhysicsAux_h
