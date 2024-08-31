//
// (c) 2024 Eduardo Doria.
//

#ifndef BODY3D_COMPONENT_H
#define BODY3D_COMPONENT_H

#include "Engine.h"

namespace JPH{
    class Body;
    class Shape;
}

namespace Supernova{

    enum class Shape3DType{
        SPHERE,
        BOX,
        CAPSULE,
        TAPERED_CAPSULE,
        CYLINDER,
        CONVEX_HULL,
        MESH,
        HEIGHTFIELD
    };

    struct Shape3D{
        JPH::Shape* shape = NULL;
        Vector3 position = Vector3::ZERO;
        Quaternion rotation = Quaternion::IDENTITY;

        Shape3DType type = Shape3DType::SPHERE;
    };

    struct Body3DComponent{
        JPH::Body *body = NULL;

        Shape3D shapes[MAX_SHAPES];
        size_t numShapes = 0;

        bool overrideMassProperties = false;
        Vector3 solidBoxSize;
        float solidBoxDensity;
        
        BodyType type = BodyType::STATIC;
        bool newBody = true;
    };

}

#endif //BODY3D_COMPONENT_H