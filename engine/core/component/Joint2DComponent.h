#ifndef JOINT2D_COMPONENT_H
#define JOINT2D_COMPONENT_H

#include "Supernova.h"

class b2JointDef;
class b2Joint;

namespace Supernova{

    enum class Joint2DType{
        DISTANCE,
        REVOLUTE,
        PRISMATIC,
        PULLEY,
        GEAR,
        MOUSE,
        WHEEL,
        WELD,
        FRICTION,
        MOTOR,
        ROPE
    };

    struct Joint2DComponent{
        b2Joint* joint = NULL;
        Joint2DType type = Joint2DType::DISTANCE;
    };

}

#endif //JOINT2D_COMPONENT_H