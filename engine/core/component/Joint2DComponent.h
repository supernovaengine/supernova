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
        b2JointId joint = b2_nullJointId;
        Joint2DType type = Joint2DType::DISTANCE;

        Entity bodyA = NULL_ENTITY;
        Entity bodyB = NULL_ENTITY;

        Vector2 anchorA = Vector2::ZERO;
        Vector2 anchorB = Vector2::ZERO;
        Vector2 axis = Vector2::ZERO;
        Vector2 target = Vector2::ZERO;

        bool rope = false;
        bool needUpdateJoint = true;
    };

}

#endif //JOINT2D_COMPONENT_H