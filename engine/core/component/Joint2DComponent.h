#ifndef JOINT2D_COMPONENT_H
#define JOINT2D_COMPONENT_H

#include "Supernova.h"

class b2Joint;

namespace Supernova{

    enum class Joint2DType{
        DISTANCE,
        REVOLUTE
    };

    struct Joint2DComponent{
        b2Joint* joint = NULL;

        Entity bodyA;
        Entity bodyB;
        Joint2DType type = Joint2DType::DISTANCE;
        bool collideConnected = false;

		Vector2 localAnchorA = Vector2(0.0f, 0.0f);
		Vector2 localAnchorB = Vector2(0.0f, 0.0f);
		float referenceAngle = 0.0f;
		float lowerAngle = 0.0f;
		float upperAngle = 0.0f;
		float maxMotorTorque = 0.0f;
		float motorSpeed = 0.0f;
		float enableLimit = false;
		float enableMotor = false;

        bool needUpdate = true;
    };

}

#endif //JOINT2D_COMPONENT_H