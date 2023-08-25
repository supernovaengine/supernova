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
        ROPE,
        FRICTION,
        MOTOR
    };

    struct Joint2DComponent{
        b2Joint* joint = NULL;

        Entity bodyA;
        Entity bodyB;
        Joint2DType type = Joint2DType::DISTANCE;
        bool collideConnected = false;

        Vector2 worldAnchorOnBodyA;
        Vector2 worldAnchorOnBodyB;

        bool needUpdate = true;
    };

}

#endif //JOINT2D_COMPONENT_H