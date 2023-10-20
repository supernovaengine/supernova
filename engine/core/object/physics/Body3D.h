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
    public:
        Body3D(Scene* scene, Entity entity);
        virtual ~Body3D();

        Body3D(const Body3D& rhs);
        Body3D& operator=(const Body3D& rhs);

        JPH::Body* getJoltBody() const;

        Object getAttachedObject();

        void createBoxShape(BodyType type, float width, float height, float depth);
        void createSphereShape(BodyType type, float radius);

        void setLinearVelocity(Vector3 linearVelocity);
        Vector3 getLinearVelocity() const;

        void setType(BodyType type);
        BodyType getType() const;

        uint32_t getID() const;

        void applyForce(const Vector3& force, const Vector3& point);

    };
}

#endif //BODY3D_H