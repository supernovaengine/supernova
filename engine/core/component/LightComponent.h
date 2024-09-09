//
// (c) 2024 Eduardo Doria.
//

#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "math/Vector3.h"
#include "component/CameraComponent.h"
#include "component/Transform.h"
#include "render/FramebufferRender.h"
#include "Engine.h"

namespace Supernova{

    enum class LightType{
        DIRECTIONAL,
        POINT,
        SPOT
    };

    struct LightCamera{
        CameraRender render;
        Matrix4 lightViewMatrix;
        Matrix4 lightProjectionMatrix;
        Matrix4 lightViewProjectionMatrix;
        Plane frustumPlanes[6];
        // for point light all cameras are same calculated value that is used by distanceToDepthValue in shader
        Vector2 nearFar = Vector2(0.0, 0.0);
    };

    struct LightComponent{
        LightType type = LightType::DIRECTIONAL;

        Vector3 direction;
        Vector3 worldDirection;

        Vector3 color = Vector3(1.0, 1.0, 1.0);

        float range = 0.0;
        float intensity = 1.0;

        float innerConeCos = 0.766044438; // cos(Angle::defaultToRad(80 / 2));
        float outerConeCos = 0.642787635; // cos(Angle::defaultToRad(100 / 2));

        bool shadows = false;
        float shadowBias = 0.001f;
        unsigned int mapResolution = 1024;
        Vector2 shadowCameraNearFar = Vector2(0.0f, 0.0f); // when zero it gets value from scene camera or light range
        unsigned int numShadowCascades = 3;

        LightCamera cameras[6];
        FramebufferRender framebuffer[MAX_SHADOWCASCADES];
        int shadowMapIndex;
    };
    
}

#endif //LIGHT_COMPONENT_H
