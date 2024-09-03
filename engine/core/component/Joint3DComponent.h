//
// (c) 2024 Eduardo Doria.
//

#ifndef JOINT3D_COMPONENT_H
#define JOINT3D_COMPONENT_H

#include "Engine.h"
#include "Jolt/Physics/Constraints/TwoBodyConstraint.h"

namespace Supernova{

    enum class Joint3DType{
        FIXED,
        DISTANCE,
        POINT,
        HINGE,
        CONE,
        PRISMATIC, // slider in Jolt
        SWINGTWIST,
        SIXDOF,
        PATH,
        GEAR,
        RACKANDPINON,
        PULLEY
    };

    struct Joint3DComponent{
        JPH::TwoBodyConstraint* joint = NULL;
        Joint3DType type = Joint3DType::FIXED;
    };

}

#endif //JOINT3D_COMPONENT_H