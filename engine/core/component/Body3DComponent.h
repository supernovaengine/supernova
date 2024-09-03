//
// (c) 2024 Eduardo Doria.
//

#ifndef BODY3D_COMPONENT_H
#define BODY3D_COMPONENT_H

#include "Engine.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"

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
        JPH::ShapeRefC shape = NULL;
        Vector3 position = Vector3::ZERO;
        Quaternion rotation = Quaternion::IDENTITY;

        Shape3DType type = Shape3DType::SPHERE;
    };

    struct Body3DComponent{
        JPH::BodyID body; 

        Shape3D shapes[MAX_SHAPES];
        size_t numShapes = 0;

        bool overrideMassProperties = false;
        Vector3 solidBoxSize;
        float solidBoxDensity;

        bool lockBody = true;
        
        BodyType type = BodyType::STATIC;
        bool newBody = true;
    };

}

#endif //BODY3D_COMPONENT_H