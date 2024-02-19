//
// (c) 2021 Eduardo Doria.
//

#include "Camera.h"

#include "util/Angle.h"
#include "Engine.h"
#include "subsystem/RenderSystem.h"

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
    cameraComponent.near = DEFAULT_ORTHO_NEAR;
    cameraComponent.far = DEFAULT_ORTHO_FAR;

    //PERSPECTIVE
    cameraComponent.yfov = 0.75;

    if (Engine::getCanvasWidth() != 0 && Engine::getCanvasHeight() != 0) {
        cameraComponent.aspect = (float) Engine::getCanvasWidth() / (float) Engine::getCanvasHeight();
    }else{
        cameraComponent.aspect = 1.0;
    }

    cameraComponent.near = DEFAULT_PERSPECTIVE_NEAR;
    cameraComponent.far = DEFAULT_PERSPECTIVE_FAR;

    // default CameraComponent is CAMERA_2D
    cameraComponent.type = CameraType::CAMERA_PERSPECTIVE;
}

Camera::Camera(Scene* scene, Entity entity): Object(scene, entity){
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
    camera.near = near;
    camera.far = far;
    
    camera.automatic = false;

    camera.needUpdate = true;
}

void Camera::setPerspective(float yfov, float aspect, float near, float far){
    CameraComponent& camera = getComponent<CameraComponent>();

    camera.type = CameraType::CAMERA_PERSPECTIVE;

    camera.yfov = Angle::defaultToRad(yfov);
    camera.aspect = aspect;
    camera.near = near;
    camera.far = far;
    
    camera.automatic = false;

    camera.needUpdate = true;
}

void Camera::setNear(float near){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.near != near){
        camera.near = near;

        camera.needUpdate = true;
    }
}

float Camera::getNear() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.near;
}

void Camera::setFar(float far){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.far != far){
        camera.far = far;

        camera.needUpdate = true;
    }
}

float Camera::getFar() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.far;
}

void Camera::setLeft(float left){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.left != left){
        camera.left = left;

        camera.automatic = false;

        camera.needUpdate = true;
    }
}

float Camera::getLeft() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.left;
}

void Camera::setRight(float right){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.right != right){
        camera.right = right;

        camera.automatic = false;

        camera.needUpdate = true;
    }
}

float Camera::getRight() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.right;
}

void Camera::setBottom(float bottom){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.bottom != bottom){
        camera.bottom = bottom;

        camera.automatic = false;

        camera.needUpdate = true;
    }
}

float Camera::getBottom() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.bottom;
}

void Camera::setTop(float top){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.top != top){
        camera.top = top;

        camera.automatic = false;

        camera.needUpdate = true;
    }
}

float Camera::getTop() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.top;
}

void Camera::setAspect(float aspect){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.aspect != aspect){
        camera.aspect = aspect;

        camera.automatic = false;

        camera.needUpdate = true;
    }
}

float Camera::getAspect() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.aspect;
}

void Camera::setYFov(float yfov){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.yfov != Angle::defaultToRad(yfov)){
        camera.yfov = Angle::defaultToRad(yfov);

        camera.needUpdate = true;
    }
}

float Camera::getYFov() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return Angle::radToDefault(camera.yfov);
}

void Camera::setType(CameraType type){
    CameraComponent& camera = getComponent<CameraComponent>();
    
    if (camera.type != type){
        camera.type = type;

        camera.needUpdate = true;
    }
}

CameraType Camera::getType() const{
    CameraComponent& camera = getComponent<CameraComponent>();
    
    return camera.type;
}

void Camera::setView(Vector3 view){
    CameraComponent& camera = getComponent<CameraComponent>();

    if (camera.view != view){
        camera.view = view;

        camera.needUpdate = true;
    }
}

void Camera::setView(const float x, const float y, const float z){
    setView(Vector3(x,y,z));
}

Vector3 Camera::getView() const{
    CameraComponent& camera = getComponent<CameraComponent>();
    return camera.view;
}

void Camera::setUp(Vector3 up){
    CameraComponent& camera = getComponent<CameraComponent>();
    
    if (camera.up != up){
        camera.up = up;

        camera.needUpdate = true;
    }
}

void Camera::setUp(const float x, const float y, const float z){
    setUp(Vector3(x,y,z));
}

Vector3 Camera::getUp() const{
    CameraComponent& camera = getComponent<CameraComponent>();
    return camera.up;
}

Vector3 Camera::getWorldView() const{
    CameraComponent& camera = getComponent<CameraComponent>();
    return camera.worldView;
}

Vector3 Camera::getWorldUp() const{
    CameraComponent& camera = getComponent<CameraComponent>();
    return camera.worldUp;
}

Vector3 Camera::getWorldRight() const{
    CameraComponent& camera = getComponent<CameraComponent>();
    return camera.worldRight;
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

        camera.needUpdate = true;
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

        transf.needUpdate = true;
        camera.needUpdate = true;
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

        camera.needUpdate = true;
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

        transf.needUpdate = true;
        camera.needUpdate = true;
    }
}

void Camera::moveForward(float distance){
    if (distance != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        viewCenter.normalize();

        camera.view = camera.view + (viewCenter * distance);
        transf.position = transf.position + (viewCenter * distance);

        transf.needUpdate = true;
        camera.needUpdate = true;
    }
}

void Camera::walkForward(float distance){
    if (distance != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        Vector3 aux = viewCenter.dotProduct(camera.up) * camera.up / camera.up.squaredLength();

        Vector3 walkVector = viewCenter - aux;

        walkVector.normalize();

        camera.view = camera.view + (walkVector * distance);
        transf.position = transf.position + (walkVector * distance);

        transf.needUpdate = true;
        camera.needUpdate = true;
    }
}

void Camera::slide(float distance){
    if (distance != 0){
        CameraComponent& camera = getComponent<CameraComponent>();
        Transform& transf = getComponent<Transform>();

        Vector3 viewCenter(camera.view.x - transf.position.x, camera.view.y - transf.position.y, camera.view.z - transf.position.z);

        Vector3 slideVector = viewCenter.crossProduct(camera.up);

        slideVector.normalize();

        camera.view = camera.view + (slideVector * distance);
        transf.position = transf.position + (slideVector * distance);

        transf.needUpdate = true;
        camera.needUpdate = true;
    }
}

void Camera::setRenderToTexture(bool renderToTexture){
    CameraComponent& camera = getComponent<CameraComponent>();

	camera.renderToTexture = renderToTexture;
}

bool Camera::isRenderToTexture() const{
    CameraComponent& camera = getComponent<CameraComponent>();

	return camera.renderToTexture;
}

Framebuffer* Camera::getFramebuffer(){
    CameraComponent& camera = getComponent<CameraComponent>();

	return camera.framebuffer;
}

void Camera::setFramebufferSize(int width, int height){
    CameraComponent& camera = getComponent<CameraComponent>();

	camera.framebuffer->setWidth(width);
	camera.framebuffer->setHeight(height);

	if (camera.renderToTexture){
		scene->getSystem<RenderSystem>()->updateFramebuffer(camera);
		scene->getSystem<RenderSystem>()->updateCameraSize(entity);
	}
}

void Camera::setFramebufferFilter(TextureFilter filter){
    CameraComponent& camera = getComponent<CameraComponent>();

	camera.framebuffer->setMinFilter(filter);
	camera.framebuffer->setMagFilter(filter);

	if (camera.renderToTexture){
		scene->getSystem<RenderSystem>()->updateFramebuffer(camera);
	}
}

void Camera::setTransparentSort(bool transparentSort){
    CameraComponent& camera = getComponent<CameraComponent>();

    camera.transparentSort = transparentSort;
}

bool Camera::isTransparentSort() const{
    CameraComponent& camera = getComponent<CameraComponent>();

    return camera.transparentSort;
}

Ray Camera::pointsToRay(float x, float y){
    CameraComponent& camera = getComponent<CameraComponent>();

    float normalized_x, normalized_y;

    if (camera.type == CameraType::CAMERA_2D){
        normalized_x = ((2 * x) / Engine::getCanvasWidth()) -1;
        normalized_y = ((2 * y) / Engine::getCanvasHeight()) -1;
    }else{
        normalized_x = ((2 * x) / Engine::getCanvasWidth()) -1;
        normalized_y = -(((2 * y) / Engine::getCanvasHeight()) -1);
    }

    Vector4 near_point_ndc = {normalized_x, normalized_y, -1, 1};
    Vector4 far_point_ndc = {normalized_x, normalized_y,  1, 1};

    Vector4 near_point_world, far_point_world;
    Matrix4 inverseViewProjection = camera.viewProjectionMatrix.inverse();

    near_point_world = inverseViewProjection * near_point_ndc;
    far_point_world = inverseViewProjection * far_point_ndc;

    near_point_world.divideByW();
    far_point_world.divideByW();

    Vector3 near_point_ray = {near_point_world[0], near_point_world[1], near_point_world[2]};
    Vector3 far_point_ray = {far_point_world[0], far_point_world[1], far_point_world[2]};
    Vector3 vector_between;
    vector_between = far_point_ray - near_point_ray;

    return Ray(near_point_ray, vector_between);
}

void Camera::updateCamera(){
    Transform& transform = getComponent<Transform>();
    CameraComponent& camera = getComponent<CameraComponent>();

    scene->getSystem<RenderSystem>()->updateCamera(camera, transform);
}