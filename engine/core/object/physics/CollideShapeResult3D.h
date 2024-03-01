//
// (c) 2024 Eduardo Doria.
//

#ifndef CollideShapeResult3D_H
#define CollideShapeResult3D_H

#include "Entity.h"
#include "Body3D.h"

namespace JPH{
    class Body;
    class CollideShapeResult;
}

namespace Supernova{

    class CollideShapeResult3D{
    private:
        Scene* scene;
        const JPH::Body* body1;
        const JPH::Body* body2;
        const JPH::CollideShapeResult* collideShapeResult;

    public:
        CollideShapeResult3D(Scene* scene, const JPH::Body* body1, const JPH::Body* body2, const JPH::CollideShapeResult* collideShapeResult);
        virtual ~CollideShapeResult3D();

        CollideShapeResult3D(const CollideShapeResult3D& rhs);
        CollideShapeResult3D& operator=(const CollideShapeResult3D& rhs);

        const JPH::CollideShapeResult* getJoltCollideShapeResult() const;

        Vector3 getContactPointOnA() const;
        Vector3 getContactPointOnB() const;
        Vector3 getPenetrationAxis() const;
        size_t getShapeIndex1() const;
        size_t getShapeIndex2() const;
    };
}

#endif //CollideShapeResult3D_H