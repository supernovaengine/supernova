//
// (c) 2024 Eduardo Doria.
//

#ifndef BODY2D_COMPONENT_H
#define BODY2D_COMPONENT_H

#include "Engine.h"
#include "box2d/box2d.h"

#define MAX_SHAPES 10

namespace Supernova{

    enum class Shape2DType{
        POLYGON,
        CIRCLE,
        CAPSULE,
        SEGMENT,
        CHAIN // chain is a different type in Box2D 3.x
    };

    struct Shape2D{
        b2ShapeId shape = b2_nullShapeId;
        b2ChainId chain = b2_nullChainId;

        Shape2DType type = Shape2DType::POLYGON;
    };

    struct Body2DComponent{
        b2BodyId body;

        Shape2D shapes[MAX_SHAPES];
        size_t numShapes = 0;

        BodyType type = BodyType::STATIC;
        bool newBody = true;
    };

}

#endif //BODY2D_COMPONENT_H