#ifndef BODY2D_COMPONENT_H
#define BODY2D_COMPONENT_H

#include "Supernova.h"

#define MAX_SHAPES 10
#define MAX_SHAPE_VERTICES 30

class b2Body;

namespace Supernova{

    enum class CollisionShape2DType{
        POLYGON,
        CIRCLE,
        EDGE,
        CHAIN
    };

    struct CollisionShape2D{
        CollisionShape2DType type = CollisionShape2DType::POLYGON;
        float radius = 0;
        float center = 0;
        Vector2 vertices[MAX_SHAPE_VERTICES];
    };

    struct Body2DComponent{
        b2Body* groundBody = NULL;

        CollisionShape2D shapes[MAX_SHAPES];
    };

}

#endif //BODY2D_COMPONENT_H