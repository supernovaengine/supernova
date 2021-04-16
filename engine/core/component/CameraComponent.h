#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "math/Vector3.h"
#include "math/Matrix4.h"

namespace Supernova{

    enum class CameraType{
        CAMERA_2D,
        CAMERA_ORTHO,
        CAMERA_PERSPECTIVE
    };

    struct CameraComponent{

        CameraType type = CameraType::CAMERA_PERSPECTIVE;
        
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
        float perspectiveFar = 2000;

        bool automatic = true;
        bool needUpdate = true;
    };

}

#endif //CAMERA_COMPONENT_H