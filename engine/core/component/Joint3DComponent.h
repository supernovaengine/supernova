#ifndef JOINT3D_COMPONENT_H
#define JOINT3D_COMPONENT_H

#include "Supernova.h"

namespace JPH{
    class TwoBodyConstraint;
}

namespace Supernova{

    enum class Joint3DType{
        FIXED,
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

    struct Joint3DComponent{
        JPH::TwoBodyConstraint* joint = NULL;
        Joint3DType type = Joint3DType::FIXED;
    };

}

#endif //JOINT3D_COMPONENT_H