#ifndef BODY2D_COMPONENT_H
#define BODY2D_COMPONENT_H

#include "Supernova.h"

#define MAX_SHAPES 10

class b2Body;
class b2Shape;
class b2Fixture;

namespace Supernova{

    enum class CollisionShape2DType{
        POLYGON,
        CIRCLE,
        EDGE,
        CHAIN
    };

    enum class Body2DType{
        STATIC,
        kINEMATIC,
        DYNAMIC
    };

    struct CollisionShape2D{
        CollisionShape2DType type = CollisionShape2DType::POLYGON;

        b2Shape* shape = NULL;
        b2Fixture* fixture = NULL;
    };

    struct Body2DComponent{
        b2Body* body = NULL;

        Vector2 linearVelocity = Vector2(0.0f, 0.0f);
        float angularVelocity = 0.0f;
        float linearDamping = 0.0f;
        float angularDamping = 0.0f;
        bool allowSleep = true;
        bool awake = true;
        bool fixedRotation = false;
        bool bullet = false;
        Body2DType type = Body2DType::STATIC;
        bool enable = true;
        float gravityScale = 1.0f;

        CollisionShape2D shapes[MAX_SHAPES];
        int numShapes = 0;

        bool needUpdate = true;
    };

}

#endif //BODY2D_COMPONENT_H