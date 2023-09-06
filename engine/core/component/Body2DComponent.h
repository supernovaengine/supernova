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

    // fixture in Box2D
    struct CollisionShape2D{
        b2Fixture* fixture = NULL;

        CollisionShape2DType type = CollisionShape2DType::POLYGON;
    };

    struct Body2DComponent{
        b2Body* body = NULL;

        CollisionShape2D shapes[MAX_SHAPES];
        int numShapes = 0;

        bool newBody = true;
    };

}

#endif //BODY2D_COMPONENT_H