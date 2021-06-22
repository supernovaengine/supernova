#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "math/Vector3.h"
#include "component/CameraComponent.h"
#include "component/Transform.h"
#include "render/FramebufferRender.h"

namespace Supernova{

    enum class LightType{
        DIRECTIONAL,
        POINT,
        SPOT
    };

    struct LightComponent{
        LightType type = LightType::DIRECTIONAL;

        Vector3 direction;
        Vector3 worldDirection;

        Vector3 color = Vector3(1.0, 1.0, 1.0);

        float range = 0.0;
        float intensity = 1.0;

        float innerConeCos = 0.0;
        float outerConeCos = 0.0;

        bool shadows = true;
        Matrix4 lightViewProjectionMatrix;
        FramebufferRender lightFb;
    };
    
}

#endif //LIGHT_COMPONENT_H