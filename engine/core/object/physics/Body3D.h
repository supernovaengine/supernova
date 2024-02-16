//
// (c) 2023 Eduardo Doria.
//

#ifndef BODY3D_H
#define BODY3D_H

#include "EntityHandle.h"
#include "math/Vector2.h"
#include "component/Body3DComponent.h"

namespace JPH{
    class Body;
}

namespace Supernova{

    class Object;

    class Body3D: public EntityHandle{
    protected:
        void checkBody(const Body3DComponent& body) const;

    public:
        Body3D(Scene* scene, Entity entity);
        virtual ~Body3D();

        Body3D(const Body3D& rhs);
        Body3D& operator=(const Body3D& rhs);

        JPH::Body* getJoltBody() const;

        Object getAttachedObject();

        void load();

        int createBoxShape(float width, float height, float depth);
        int createBoxShape(Vector3 position, Quaternion rotation, float width, float height, float depth);
        int createSphereShape(float radius);
        int createSphereShape(Vector3 position, Quaternion rotation, float radius);
        int createCapsuleShape(float halfHeight, float radius);
        int createCapsuleShape(Vector3 position, Quaternion rotation, float halfHeight, float radius);
        int createTaperedCapsuleShape(float halfHeight, float topRadius, float bottomRadius);
        int createTaperedCapsuleShape(Vector3 position, Quaternion rotation, float halfHeight, float topRadius, float bottomRadius);
        int createCylinderShape(float halfHeight, float radius);
        int createCylinderShape(Vector3 position, Quaternion rotation, float halfHeight, float radius);
        int createConvexHullShape(std::vector<Vector3> vertices);
        int createConvexHullShape(Vector3 position, Quaternion rotation, std::vector<Vector3> vertices);
        int createMeshShape();
        int createMeshShape(std::vector<Vector3> vertices, std::vector<uint16_t> indices);
        int createMeshShape(Vector3 position, Quaternion rotation, std::vector<Vector3> vertices, std::vector<uint16_t> indices);
        int createHeightFieldShape();
        int createHeightFieldShape(unsigned int samplesSize);
        //TODO:
        // MutableCompoundShape

        void setShapeDensity(float density);
        void setShapeDensity(size_t index, float density);
        float getShapeDensity() const;
        float getShapeDensity(size_t index) const;

        void setType(BodyType type);
        BodyType getType() const;

        uint32_t getID() const;

        bool canBeKinematicOrDynamic() const;

        void activate();
        void deactivate();

        bool isSensor() const;
        void setIsSensor(bool sensor);

        void setAllowedAllMotion();
        void setAllowedOnlyTranslation();
        void setAllowedOnlyRotation();
        void setAllowedOnly2DPlane();

        void addAllowedTransactionX();
        void addAllowedTransactionY();
        void addAllowedTransactionZ();
        void addAllowedRotationX();
        void addAllowedRotationY();
        void addAllowedRotationZ();

        float getMass() const;
        void setMass(float mass);

        float getGravityFactor() const;
        void setGravityFactor(float gravityFactor);

        unsigned int getCollisionGroupID() const;
        void setCollisionGroupID(unsigned int group);

        bool isAllowSleeping() const;
        void setAllowSleeping(bool allowSleeping);

        float getFriction() const;
        void setFriction(float friction);

        float getRestitution() const;
        void setRestitution(float restitution);

        Vector3 getLinearVelocity() const;
        void setLinearVelocity(Vector3 linearVelocity);
        void setLinearVelocityClamped(Vector3 linearVelocity);

        Vector3 getAngularVelocity() const;
        void setAngularVelocity(Vector3 angularVelocity);
        void setAngularVelocityClamped(Vector3 angularVelocity);

        // center of mass
        Vector3 getPointVelocityCOM(Vector3 pointRelativeToCOM) const;
        Vector3 getPointVelocity(Vector3 point) const;

        Vector3 getAccumulatedForce() const;
        Vector3 getAccumulatedTorque() const;
        Matrix4 getInverseInertia() const;

        void applyForce(const Vector3& force);
        void applyForce(const Vector3& force, const Vector3& point);
        void applyTorque(const Vector3& torque);
        void applyImpulse(const Vector3& impulse);
        void applyImpulse(const Vector3& impulse, const Vector3& point);
        void applyAngularImpulse(const Vector3& angularImpulse);
        bool applyBuoyancyImpulse(const Vector3& surfacePosition, const Vector3& surfaceNormal, const float buoyancy, const float linearDrag, const float angularDrag, const Vector3& fluidVelocity, const Vector3& gravity, const float deltaTime);

        Vector3 getCenterOfMassPosition();
    };
}

#endif //BODY3D_H