//
// (c) 2023 Eduardo Doria.
//

#ifndef CollideShapeResult3D_H
#define CollideShapeResult3D_H

#include "Entity.h"
#include "Body3D.h"

namespace JPH{
    class CollideShapeResult;
}

namespace Supernova{

    class CollideShapeResult3D{
    private:
        Scene* scene;
        const JPH::CollideShapeResult* collideShapeResult;

    public:
        CollideShapeResult3D(Scene* scene, const JPH::CollideShapeResult* collideShapeResult);
        virtual ~CollideShapeResult3D();

        CollideShapeResult3D(const CollideShapeResult3D& rhs);
        CollideShapeResult3D& operator=(const CollideShapeResult3D& rhs);

        const JPH::CollideShapeResult* getJoltCollideShapeResult() const;

        Vector3 getContactPointOnA() const;
        Vector3 getContactPointOnB() const;
        Vector3 getPenetrationAxis() const;
        int32_t getSubShapeID1() const;
        int32_t getSubShapeID2() const;
        int32_t getBodyID2() const;
    };
}

#endif //CollideShapeResult3D_H