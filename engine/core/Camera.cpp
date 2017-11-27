#include "Camera.h"
#include "Engine.h"
#include "math/Angle.h"

using namespace Supernova;

Camera::Camera() : Object(){

    position = Vector3(0,0,1);
    view = Vector3(0,0,0);
    up = Vector3(0,1,0);

    //ORTHO
    left = 0;
    right = Engine::getCanvasWidth();
    bottom = 0;
    top = Engine::getCanvasHeight();
    orthoNear = -10;
    orthoFar = 10;

    //PERSPECTIVE
    y_fov = 0.75;
    aspect = (float) Engine::getCanvasWidth() / (float) Engine::getCanvasHeight();
    perspectiveNear = 1;
    perspectiveFar = 5000;

    type = S_CAMERA_PERSPECTIVE;
    
    automatic = true;

    sceneObject = NULL;

}

Camera::Camera(const Camera &camera){
    (*this) = camera;
}

Camera::Camera(int type): Camera(){
    setType(type);
}

Camera::~Camera() {
}

Camera& Camera::operator=(const Camera &c){

    this->projectionMatrix = c.projectionMatrix;
    this->viewMatrix = c.viewMatrix;
    this->viewProjectionMatrix = c.viewProjectionMatrix;

    this->position = c.position;
    this->view = c.view;
    this->up = c.up;

    this->worldPosition = c.worldPosition;
    this->worldView = c.worldView;
    this->worldUp = c.worldUp;

    this->left = c.left;
    this->right = c.right;
    this->bottom = c.bottom;
    this->top = c.top;
    this->orthoNear = c.orthoNear;
    this->orthoFar = c.orthoFar;

    this->y_fov = c.y_fov;
    this->aspect = c.aspect;
    this->perspectiveNear = c.perspectiveNear;
    this->perspectiveFar = c.perspectiveFar;
    
    this->automatic = c.automatic;

    this->type = c.type;

    return *this;
}

void Camera::setView(const float x, const float y, const float z){
    setView(Vector3(x,y,z));
}

void Camera::setView(Vector3 view){
    if (this->view != view){
        this->view = view;
        updateMatrix();
    }
}

Vector3 Camera::getView(){
    return view;
}

void Camera::setUp(const float x, const float y, const float z){
    setUp(Vector3(x,y,z));
}

void Camera::setUp(Vector3 up){
    if (this->up != up){
        this->up = up;
        updateMatrix();
    }
}

Vector3 Camera::getUp(){
    return up;
}

void Camera::setType(int type){
    if (this->type != type){
        this->type = type;
        updateMatrix();
    }
}

int Camera::getType(){
    return type;
}

float Camera::getFarPlane(){
    if (type==S_CAMERA_PERSPECTIVE)
        return perspectiveFar;
    else
        return orthoFar;
}

void Camera::updateAutomaticSizes(Rect rect){
    if (automatic){
        float newLeft = rect.getX();
        float newBottom = rect.getY();
        float newRight = rect.getWidth();
        float newTop = rect.getHeight();
        float newAspect = rect.getWidth() / rect.getHeight();

        if ((left != newLeft) || (bottom != newBottom) || (right != newRight) || (top != newTop) || (aspect != newAspect)){
            left = newLeft;
            bottom = newBottom;
            right = newRight;
            top = newTop;
            aspect = newAspect;
            updateMatrix();
        }
    }
}

void Camera::setOrtho(float left, float right, float bottom, float top, float near, float far){

    type = S_CAMERA_ORTHO;

    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
    this->orthoNear = near;
    this->orthoFar = far;
    
    automatic = false;

    updateMatrix();
}

void Camera::setPerspective(float y_fov, float aspect, float near, float far){

    type = S_CAMERA_PERSPECTIVE;

    this->y_fov = Angle::defaultToRad(y_fov);
    this->aspect = aspect;
    this->perspectiveNear = near;
    this->perspectiveFar = far;
    
    automatic = false;

    updateMatrix();
}

void Camera::rotateView(float angle){
    if (angle != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, up);
        viewCenter = rotation * viewCenter;

        view = Vector3(viewCenter.x + position.x, viewCenter.y + position.y, viewCenter.z + position.z);

        updateMatrix();
    }
}

void Camera::rotatePosition(float angle){
    if (angle != 0){
        Vector3 positionCenter(position.x - view.x, position.y - view.y, position.z - view.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, up);
        positionCenter = rotation * positionCenter;

        position = Vector3(positionCenter.x + view.x, positionCenter.y + view.y, positionCenter.z + view.z);

        updateMatrix();
    }
}

void Camera::elevateView(float angle){
    if (angle != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, viewCenter.crossProduct(up));
        viewCenter = rotation * viewCenter;

        view = Vector3(viewCenter.x + position.x, viewCenter.y + position.y, viewCenter.z + position.z);

        updateMatrix();
    }
}

void Camera::elevatePosition(float angle){
    if (angle != 0){
        Vector3 positionCenter(position.x - view.x, position.y - view.y, position.z - view.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, positionCenter.crossProduct(up));
        positionCenter = rotation * positionCenter;

        position = Vector3(positionCenter.x + view.x, positionCenter.y + view.y, positionCenter.z + view.z);

        updateMatrix();
    }
}

void Camera::moveForward(float distance){
    if (distance != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        viewCenter.normalize();

        view = view + (viewCenter.normalize() * distance);
        position = position + (viewCenter.normalize() * distance);

        updateMatrix();
    }
}

void Camera::walkForward(float distance){
    if (distance != 0){

        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Vector3 aux = viewCenter.dotProduct(up) * up / up.squaredLength();

        Vector3 walkVector = viewCenter - aux;

        view = view + (walkVector.normalize() * distance);
        position = position + (walkVector.normalize() * distance);

        updateMatrix();

    }
}

void Camera::slide(float distance){
    if (distance != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Vector3 slideVector = viewCenter.crossProduct(up);

        view = view + (slideVector.normalize() * distance);
        position = position + (slideVector.normalize() * distance);

        updateMatrix();
    }
}

Matrix4* Camera::getProjectionMatrix(){
    return &projectionMatrix;
}

Matrix4* Camera::getViewMatrix(){
    return &viewMatrix;
}

Matrix4* Camera::getViewProjectionMatrix(){

    return &viewProjectionMatrix;

}

Ray Camera::pointsToRay(float x, float y) {

    float normalized_x, normalized_y;

    if (type == S_CAMERA_2D){
        normalized_x = ((2 * x) / Engine::getCanvasWidth()) -1;
        normalized_y = ((2 * y) / Engine::getCanvasHeight()) -1;
    }else{
        normalized_x = ((2 * x) / Engine::getCanvasWidth()) -1;
        normalized_y = -(((2 * y) / Engine::getCanvasHeight()) -1);
    }

    Vector4 near_point_ndc = {normalized_x, normalized_y, -1, 1};
    Vector4 far_point_ndc = {normalized_x, normalized_y,  1, 1};

    Vector4 near_point_world, far_point_world;
    Matrix4 inverseViewProjection = getViewProjectionMatrix()->getInverse();

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

void Camera::setSceneObject(Object* scene){
    this->sceneObject = scene;
}

void Camera::updateMatrix(){
    Object::updateMatrix();

    if (type == S_CAMERA_2D){ //use top-left orientation
        projectionMatrix = Matrix4::orthoMatrix(left, right, top, bottom, orthoNear, orthoFar);
    }else if (type == S_CAMERA_ORTHO){
        projectionMatrix = Matrix4::orthoMatrix(left, right, bottom, top, orthoNear, orthoFar);
    }else if (type == S_CAMERA_PERSPECTIVE){
        projectionMatrix = Matrix4::perspectiveMatrix(y_fov, aspect, perspectiveNear, perspectiveFar);
    }

    if (parent != NULL){
        worldView = modelMatrix * (view - position);
        worldUp = ((modelMatrix * up) - (modelMatrix * Vector3(0,0,0))).normalize();
    }else{
        worldView = view;
        worldUp = up;
    }
    
    if (type == S_CAMERA_2D){
        viewMatrix.identity();
    }else{
        viewMatrix = Matrix4::lookAtMatrix(worldPosition, worldView, worldUp);
    }

    viewProjectionMatrix = projectionMatrix * viewMatrix;

    if (sceneObject != NULL){
        sceneObject->updateVPMatrix(getViewMatrix(), getProjectionMatrix(), getViewProjectionMatrix(), &worldPosition);
    }
}
