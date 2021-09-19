//
// (c) 2021 Eduardo Doria.
//

#include "Camera.h"

#include "math/Angle.h"
#include "Engine.h"

using namespace Supernova;

Camera::Camera(Scene* scene): Object(scene){
    addComponent<CameraComponent>({});

    CameraComponent& cameraComponent = getComponent<CameraComponent>();
    Transform& transform = getComponent<Transform>();

    transform.position = Vector3(0, 0, 1);

    //ORTHO
    cameraComponent.left = 0;
    cameraComponent.right = Engine::getCanvasWidth();
    cameraComponent.bottom = 0;
    cameraComponent.top = Engine::getCanvasHeight();
    cameraComponent.orthoNear = -10;
    cameraComponent.orthoFar = 10;

    //PERSPECTIVE
    cameraComponent.y_fov = 0.75;

    if (Engine::getCanvasWidth() != 0 && Engine::getCanvasHeight() != 0) {
        cameraComponent.aspect = (float) Engine::getCanvasWidth() / (float) Engine::getCanvasHeight();
    }else{
        cameraComponent.aspect = 1.0;
    }

    cameraComponent.perspectiveNear = 1;
    cameraComponent.perspectiveFar = 2000;

    cameraComponent.type = CameraType::CAMERA_2D;
}

void Camera::activate(){
    scene->setCamera(entity);
}

void Camera::setOrtho(float left, float right, float bottom, float top, float near, float far){
    CameraComponent& camera = getComponent<CameraComponent>();

    camera.type = CameraType::CAMERA_ORTHO;

    camera.left = left;
    camera.right = right;
    camera.bottom = bottom;
    camera.top = top;
    camera.orthoNear = near;
    camera.orthoFar = far;
    
    camera.automatic = false;

    updateCamera(camera);
}

void Camera::setPerspective(float y_fov, float aspect, float near, float far){
    CameraComponent& camera = getComponent<CameraComponent>();

    camera.type = CameraType::CAMERA_PERSPECTIVE;

    camera.y_fov = Angle::defaultToRad(y_fov);
    camera.aspect = aspect;
    camera.perspectiveNear = near;
    camera.perspectiveFar = far;
    
    camera.automatic = false;

    updateCamera(camera);
}

void Camera::setType(CameraType type){
    CameraComponent& camera = getComponent<CameraComponent>();
    
    if (camera.type != type){
        camera.type = type;

        updateCamera(camera);
    }
}

void Camera::setView(Vector3 view){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.view != view){
        camera.view = view;

        updateCamera(camera);
    }
}

void Camera::setView(const float x, const float y, const float z){
    setView(Vector3(x,y,z));
}

Vector3 Camera::getView(){
    CameraComponent& camera = getComponent<CameraComponent>();
    return camera.view;
}

void Camera::setUp(Vector3 up){
    CameraComponent& camera = getComponent<CameraComponent>();
    
    if (camera.up != up){
        camera.up = up;

        updateCamera(camera);
    }
}

void Camera::setUp(const float x, const float y, const float z){
    setUp(Vector3(x,y,z));
}

Vector3 Camera::getUp(){
    CameraComponent& camera = getComponent<CameraComponent>();
    return camera.up;
}

void Camera::rotateView(float angle){
    if (angle != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, camera.up);
        viewCenter = rotation * viewCenter;

        camera.view = Vector3(viewCenter.x + transf.position.x, viewCenter.y + transf.position.y, viewCenter.z + transf.position.z);

        updateCamera(camera);
    }
}

void Camera::rotatePosition(float angle){
    if (angle != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 positionCenter(transf.position.x - camera.view.x, transf.position.y - camera.view.y, transf.position.z - camera.view.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, camera.up);
        positionCenter = rotation * positionCenter;

        transf.position = Vector3(positionCenter.x + camera.view.x, positionCenter.y + camera.view.y, positionCenter.z + camera.view.z);

        updateTransform();
        updateCamera(camera);
    }
}

void Camera::elevateView(float angle){
    if (angle != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, viewCenter.crossProduct(camera.up));
        viewCenter = rotation * viewCenter;

        camera.view = Vector3(viewCenter.x + transf.position.x, viewCenter.y + transf.position.y, viewCenter.z + transf.position.z);

        updateCamera(camera);
    }
}

void Camera::elevatePosition(float angle){
    if (angle != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 positionCenter(transf.position.x - camera.view.x, transf.position.y - camera.view.y, transf.position.z - camera.view.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, positionCenter.crossProduct(camera.up));
        positionCenter = rotation * positionCenter;

        transf.position = Vector3(positionCenter.x + camera.view.x, positionCenter.y + camera.view.y, positionCenter.z + camera.view.z);

        updateTransform();
        updateCamera(camera);
    }
}

void Camera::moveForward(float distance){
    if (distance != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        viewCenter.normalize();

        camera.view = camera.view + (viewCenter.normalize() * distance);
        transf.position = transf.position + (viewCenter.normalize() * distance);

        updateTransform();
        updateCamera(camera);
    }
}

void Camera::walkForward(float distance){
    if (distance != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        Vector3 aux = viewCenter.dotProduct(camera.up) * camera.up / camera.up.squaredLength();

        Vector3 walkVector = viewCenter - aux;

        camera.view = camera.view + (walkVector.normalize() * distance);
        transf.position = transf.position + (walkVector.normalize() * distance);

        updateTransform();
        updateCamera(camera);
    }
}

void Camera::slide(float distance){
    if (distance != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        Vector3 slideVector = viewCenter.crossProduct(camera.up);

        camera.view = camera.view + (slideVector.normalize() * distance);
        transf.position = transf.position + (slideVector.normalize() * distance);

        updateTransform();
        updateCamera(camera);
    }
}

void Camera::updateCamera(CameraComponent& camera){
    camera.needUpdate = true;
}

void Camera::updateCamera(){
    CameraComponent& camera = getComponent<CameraComponent>();
    updateCamera(camera);
}