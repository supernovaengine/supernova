//
// (c) 2024 Eduardo Doria.
//

#ifndef BODY3D_COMPONENT_H
#define BODY3D_COMPONENT_H

#include "Engine.h"
#include "ecs/Entity.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"

#ifndef MAX_SHAPES
#define MAX_SHAPES 10
#endif

#define MAX_SHAPE_VERTICES_3D 256
#define MAX_SHAPE_INDICES_3D 768

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

    enum class Shape3DSource{
        NONE,
        RAW_VERTICES,
        RAW_MESH,
        ENTITY_MESH,
        ENTITY_HEIGHTFIELD
    };

    struct SUPERNOVA_API Shape3D{
        JPH::ShapeRefC shape = NULL;
        Vector3 position = Vector3::ZERO;
        Quaternion rotation = Quaternion::IDENTITY;

        Shape3DType type = Shape3DType::SPHERE;

        float width = 1.0f;
        float height = 1.0f;
        float depth = 1.0f;

        float radius = 0.5f;
        float halfHeight = 0.5f;
        float topRadius = 0.5f;
        float bottomRadius = 0.5f;
        float density = 1000.0f;

        Shape3DSource source = Shape3DSource::NONE;
        Entity sourceEntity = NULL_ENTITY;
        unsigned int samplesSize = 0;

        Vector3 vertices[MAX_SHAPE_VERTICES_3D];
        uint16_t indices[MAX_SHAPE_INDICES_3D];
        uint16_t numVertices = 0;
        uint16_t numIndices = 0;
    };

    struct Body3DComponent{
        JPH::BodyID body;

        Shape3D shapes[MAX_SHAPES];
        size_t numShapes = 0;

        bool needReloadBody = true;
        bool needUpdateShapes = true;

        bool overrideMassProperties = false;
        Vector3 solidBoxSize;
        float solidBoxDensity;

        bool lockBody = true;
        
        BodyType type = BodyType::STATIC;
        bool newBody = true;
    };

}

#endif //BODY3D_COMPONENT_H