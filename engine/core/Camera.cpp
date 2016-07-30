#include "Camera.h"
#include "Supernova.h"

Camera::Camera() : Object(){

    position = Vector3(0,0,1);
    view = Vector3(0,0,0);
    up = Vector3(0,1,0);

    //ORTHO
    left = 0;
    right = Supernova::getCanvasWidth();
    bottom = 0;
    top = Supernova::getCanvasHeight();

    //PERSPECTIVE
    y_fov = 0.75;
    aspect = (float) Supernova::getCanvasWidth() / (float) Supernova::getCanvasHeight();

    near = 1;
    far = 5000;

    projection = S_PERSPECTIVE;

    sceneBaseObject = NULL;

}

Camera::Camera(const Camera &camera){
    (*this) = camera;
}

Camera::Camera(int projection): Camera(){
    setProjection(projection);
}

Camera::~Camera() {
    // TODO Auto-generated destructor stub
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

    this->y_fov = c.y_fov;
    this->aspect = c.aspect;

    this->near = c.near;
    this->far = c.far;

    this->projection = c.projection;

    return *this;
}

void Camera::setView(const float x, const float y, const float z){
    setView(Vector3(x,y,z));
}

void Camera::setView(Vector3 view){
    if (this->view != view){
        this->view = view;
        update();
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
        update();
    }
}

Vector3 Camera::getUp(){
    return up;
}

void Camera::setProjection(int projection){
    if (this->projection != projection){
        this->projection = projection;
        update();
    }
}

void Camera::updateScreenSize(){
    float newRight = Supernova::getCanvasWidth();
    float newTop = Supernova::getCanvasHeight();
    float newAspect = (float) Supernova::getCanvasWidth() / (float) Supernova::getCanvasHeight();

    if ((right != newRight) || (top !=newTop) || (aspect != newAspect)){
        right = newRight;
        top = newTop;
        aspect = newAspect;
        update();
    }
}

void Camera::setOrtho(float left, float right, float bottom, float top, float near, float far){

    projection = S_ORTHO;

    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
    this->near = near;
    this->far = far;

    update();
}

void Camera::setPerspective(float y_fov, float aspect, float near, float far){

    projection = S_PERSPECTIVE;

    this->y_fov = y_fov;
    this->aspect = aspect;
    this->near = near;
    this->far = far;

    update();
}

void Camera::rotateView(float angle){
    if (angle != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, up);
        viewCenter = rotation * viewCenter;

        view = Vector3(viewCenter.x + position.x, viewCenter.y + position.y, viewCenter.z + position.z);

        update();
    }
}

void Camera::rotatePosition(float angle){
    if (angle != 0){
        Vector3 positionCenter(position.x - view.x, position.y - view.y, position.z - view.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, up);
        positionCenter = rotation * positionCenter;

        position = Vector3(positionCenter.x + view.x, positionCenter.y + view.y, positionCenter.z + view.z);

        update();
    }
}

void Camera::elevateView(float angle){
    if (angle != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, viewCenter.crossProduct(up));
        viewCenter = rotation * viewCenter;

        view = Vector3(viewCenter.x + position.x, viewCenter.y + position.y, viewCenter.z + position.z);

        update();
    }
}

void Camera::elevatePosition(float angle){
    if (angle != 0){
        Vector3 positionCenter(position.x - view.x, position.y - view.y, position.z - view.z);

        Matrix4 rotation;
        rotation = Matrix4::rotateMatrix(angle, positionCenter.crossProduct(up));
        positionCenter = rotation * positionCenter;

        position = Vector3(positionCenter.x + view.x, positionCenter.y + view.y, positionCenter.z + view.z);

        update();
    }
}

void Camera::moveForward(float distance){
    if (distance != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        viewCenter.normalise();

        view = view + (viewCenter.normalise() * distance);
        position = position + (viewCenter.normalise() * distance);

        update();
    }
}

void Camera::walkForward(float distance){
    if (distance != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Vector3 aux = viewCenter.dotProduct(up) * up / up.squaredLength();

        Vector3 walkVector = viewCenter - aux;

        view = view + (walkVector.normalise() * distance);
        position = position + (walkVector.normalise() * distance);

        update();
    }
}

void Camera::slide(float distance){
    if (distance != 0){
        Vector3 viewCenter(view.x - position.x, view.y - position.y, view.z - position.z);

        Vector3 slideVector = viewCenter.crossProduct(up);

        view = view + (slideVector.normalise() * distance);
        position = position + (slideVector.normalise() * distance);

        update();
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

    normalized_x = ((2 * x) / Supernova::getCanvasWidth()) -1;
    normalized_y = ((2 * y) / Supernova::getCanvasHeight()) -1;

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

void Camera::setSceneBaseObject(Object* baseObject){
    this->sceneBaseObject = baseObject;
}

void Camera::update(){

    Object::update();

    if (projection == S_ORTHO){
        projectionMatrix = Matrix4::orthoMatrix(left, right, bottom, top, near, far);
    }else if (projection == S_PERSPECTIVE){
        projectionMatrix = Matrix4::perspectiveMatrix(y_fov, aspect, near, far);
    }

    if (parent != NULL){
        worldView = modelMatrix * (view - position);
        worldUp = ((modelMatrix * up) - (modelMatrix * Vector3(0,0,0))).normalise();
    }else{
        worldView = view;
        worldUp = up;
    }

    viewMatrix = Matrix4::lookAtMatrix(worldPosition, worldView, worldUp);

    viewProjectionMatrix = viewMatrix * projectionMatrix;

    if (sceneBaseObject != NULL){
        sceneBaseObject->transform(getViewMatrix(), getViewProjectionMatrix(), &worldPosition);
    }

}
