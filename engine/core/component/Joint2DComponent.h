//
// (c) 2024 Eduardo Doria.
//

#ifndef JOINT2D_COMPONENT_H
#define JOINT2D_COMPONENT_H

#include "Engine.h"

namespace Supernova{

    enum class Joint2DType{
        DISTANCE,
        REVOLUTE,
        PRISMATIC,
        //PULLEY,
        //GEAR,
        MOUSE,
        WHEEL,
        WELD,
        MOTOR
    };

    struct SUPERNOVA_API Joint2DComponent{
        b2JointId joint;
        Joint2DType type = Joint2DType::DISTANCE;
    };

}

#endif //JOINT2D_COMPONENT_H