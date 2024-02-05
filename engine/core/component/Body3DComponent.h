#ifndef BODY3D_COMPONENT_H
#define BODY3D_COMPONENT_H

#include "Engine.h"

namespace JPH{
    class Body;
    class Shape;
}

namespace Supernova{

    enum class CollisionShape3DType{
        SPHERE,
        BOX,
        CAPSULE,
        TAPERED_CAPSULE,
        CYLINDER,
        CONVEX_HULL,
        MESH,
        HEIGHTFIELD
    };

    // shape in jolt
    struct CollisionShape3D{
        const JPH::Shape* shape = NULL;
        Vector3 position = Vector3::ZERO;
        Quaternion rotation = Quaternion::IDENTITY;

        CollisionShape3DType type = CollisionShape3DType::SPHERE;
    };

    struct Body3DComponent{
        JPH::Body *body = NULL;

        CollisionShape3D shapes[MAX_SHAPES];
        int numShapes = 0;
        
        BodyType type = BodyType::DYNAMIC;
        bool newBody = true;
    };

}

#endif //BODY3D_COMPONENT_H