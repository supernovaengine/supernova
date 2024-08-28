//
// (c) 2024 Eduardo Doria.
//

#ifndef BODY2D_COMPONENT_H
#define BODY2D_COMPONENT_H

#include "Engine.h"
#include "box2d/box2d.h"

#define MAX_SHAPES 10

namespace Supernova{

    enum class CollisionShape2DType{
        POLYGON,
        CIRCLE,
        EDGE,
        CHAIN
    };

    // fixture in Box2D
    struct CollisionShape2D{
        b2ShapeId shape;

        CollisionShape2DType type = CollisionShape2DType::POLYGON;
    };

    struct Body2DComponent{
        b2BodyId body;

        CollisionShape2D shapes[MAX_SHAPES];
        size_t numShapes = 0;

        BodyType type = BodyType::DYNAMIC;
        bool newBody = true;
    };

}

#endif //BODY2D_COMPONENT_H