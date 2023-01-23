#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "math/Vector3.h"
#include "math/Matrix4.h"
#include "math/Plane.h"
#include "texture/Framebuffer.h"

namespace Supernova{

    enum class CameraType{
        CAMERA_2D,
        CAMERA_ORTHO,
        CAMERA_PERSPECTIVE
    };

    enum FrustumPlane{
        FRUSTUM_PLANE_NEAR   = 0,
        FRUSTUM_PLANE_FAR    = 1,
        FRUSTUM_PLANE_LEFT   = 2,
        FRUSTUM_PLANE_RIGHT  = 3,
        FRUSTUM_PLANE_TOP    = 4,
        FRUSTUM_PLANE_BOTTOM = 5
    };

    struct CameraComponent{

        CameraType type = CameraType::CAMERA_2D;
        
        Matrix4 projectionMatrix;
        Matrix4 viewMatrix;
        Matrix4 viewProjectionMatrix;

        Vector3 view = Vector3(0, 0, 0);
        Vector3 up = Vector3(0, 1, 0);

        Vector3 worldView;
        Vector3 worldUp;
        Vector3 worldRight;

        float left = 0;
        float right = 10;
        float bottom = 0;
        float top = 10;
        float orthoNear = -10;
        float orthoFar = 10;

        float y_fov = 0.75;
        float aspect = 1.0;
        float perspectiveNear = 1;
        float perspectiveFar = 200;

        bool needUpdateFrustumPlanes = true;
        Plane frustumPlanes[6];

		bool renderToTexture = false;
        // need to be a pointer to not lost reference when component changes position
        Framebuffer* framebuffer = new Framebuffer();

        bool transparentSort = true;

        bool automatic = true;
        bool needUpdate = true;
    };

}

#endif //CAMERA_COMPONENT_H