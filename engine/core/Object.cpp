#include "Object.h"
#include "platform/Log.h"
#include "GUIObject.h"
#include "Light.h"
#include "Scene.h"


using namespace Supernova;

Object::Object(){
    loaded = false;
    firstLoaded = false;
    
    parent = NULL;
    scene = NULL;

    viewMatrix = NULL;
    projectionMatrix = NULL;
    viewProjectionMatrix = NULL;
    cameraPosition = NULL;

    scale = Vector3(1,1,1);
    position = Vector3(0,0,0);
    rotation = Quaternion(1,0,0,0);
    center = Vector3(0,0,0);
}

Object::~Object(){
    
    if (parent)
        parent->removeObject(this);
    
    destroy();
}

void Object::setSceneAndConfigure(Scene* scene){
    if (this->scene == NULL){
        this->scene = scene;
        if (loaded)
            reload();
    }
    
    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->setSceneAndConfigure(scene);
    }
    
    if (Light* light_ptr = dynamic_cast<Light*>(this)){
        ((Scene*)scene)->addLight(light_ptr);
    }
    
    if (Scene* scene_ptr = dynamic_cast<Scene*>(this)){
        ((Scene*)scene)->addSubScene(scene_ptr);
    }
    
    if (GUIObject* guiobject_ptr = dynamic_cast<GUIObject*>(this)){
        ((Scene*)scene)->addGUIObject(guiobject_ptr);
    }

    if (SkyBox* sky_ptr = dynamic_cast<SkyBox*>(this)){
        ((Scene*)scene)->setSky(sky_ptr);
    }
}

void Object::removeScene(){
    this->scene = NULL;
    
    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->removeScene();
    }
}

void Object::addObject(Object* obj){
    if (Scene* scene_ptr = dynamic_cast<Scene*>(obj)){
        scene_ptr->childScene = true;
    }
    
    if (obj->parent == NULL){
        objects.push_back(obj);

        obj->parent = this;

        obj->viewMatrix = viewMatrix;
        obj->projectionMatrix = projectionMatrix;
        obj->viewProjectionMatrix = viewProjectionMatrix;
        obj->cameraPosition = cameraPosition;
        obj->modelViewProjectionMatrix = modelViewProjectionMatrix;
        
        obj->firstLoaded = false;
        
        if (scene != NULL)
            obj->setSceneAndConfigure(scene);

        obj->updateMatrix();
    }else{
        Log::Error(LOG_TAG, "Object has a parent already");
    }
    
}

void Object::removeObject(Object* obj){
    
    if (scene != NULL){
        
        if (Light* light_ptr = dynamic_cast<Light*>(this)){
            ((Scene*)scene)->removeLight(light_ptr);
        }
        
        if (Scene* scene_ptr = dynamic_cast<Scene*>(this)){
            ((Scene*)scene)->removeSubScene(scene_ptr);
        }
        
        if (GUIObject* guiobject_ptr = dynamic_cast<GUIObject*>(this)){
            ((Scene*)scene)->removeGUIObject(guiobject_ptr);
        }
    }
    
    if (Scene* scene_ptr = dynamic_cast<Scene*>(obj)){
        scene_ptr->childScene = false;
    }
    
    std::vector<Object*>::iterator i = std::remove(objects.begin(), objects.end(), obj);
    objects.erase(i,objects.end());
    
    obj->parent = NULL;
    obj->removeScene();
    
    obj->viewMatrix = NULL;
    obj->viewProjectionMatrix = NULL;
    obj->cameraPosition = NULL;
    
    obj->updateMatrix();
}

void Object::setDepth(bool depth){
    if (scene != NULL) {
        ((Scene*)scene)->useDepth = depth;
    }
}

void Object::setPosition(const float x, const float y, const float z){
    setPosition(Vector3(x, y, z));
}

void Object::setPosition(Vector3 position){
    if (this->position != position){
        this->position = position;
        updateMatrix();
    }
}

Vector3 Object::getPosition(){
    return position;
}

Vector3 Object::getWorldPosition(){
    return worldPosition;
}

void Object::setRotation(const float xAngle, const float yAngle, const float zAngle){
    Quaternion qx, qy, qz;

    qx.fromAngleAxis(xAngle, Vector3(1,0,0));
    qy.fromAngleAxis(yAngle, Vector3(0,1,0));
    qz.fromAngleAxis(zAngle, Vector3(0,0,1));

    setRotation(qz * (qy * qx)); //order ZYX
}

void Object::setRotation(Quaternion rotation){
    if (this->rotation != rotation){
        this->rotation = rotation;
        updateMatrix();
    }
}

Quaternion Object::getRotation(){
    return this->rotation;
}

Quaternion Object::getWorldRotation(){
    return this->worldRotation;
}

void Object::setScale(const float factor){
    setScale(Vector3(factor,factor,factor));
}

void Object::setScale(Vector3 scale){
    if (this->scale != scale){
        this->scale = scale;
        updateMatrix();
    }
}

Vector3 Object::getScale(){
    return this->scale;
}

void Object::setCenter(const float x, const float y, const float z){
    this->center = Vector3(x, y, z);
}

void Object::setCenter(Vector3 center){
    if (this->center != center){
        this->center = center;
        updateMatrix();
    }
}

Vector3 Object::getCenter(){
    return center;
}

Matrix4 Object::getModelMatrix(){
    return modelMatrix;
}

Matrix4 Object::getModelViewProjectMatrix(){
    return modelViewProjectionMatrix;
}

Vector3 Object::getCameraPosition(){
    if (cameraPosition != NULL)
        return *cameraPosition;
    else
        return Vector3();
}

Scene* Object::getScene(){
    return scene;
}

Object* Object::getParent(){
    return parent;
}

int Object::findObject(Object* object){
    
    for (int i=0; i < objects.size(); i++){
        if (objects[i] == object){
            return i;
        }
    }
    
    return -1;
}

void Object::moveToFront(){
    if (parent != NULL){
        int pos = parent->findObject(this);

        if ((pos >= 0) && (pos < (parent->objects.size()-1))){
            Object* temp = parent->objects[pos];

            for (int i = pos; i < (parent->objects.size()-1); i++){
                parent->objects[i] = parent->objects[i+1];
            }
            parent->objects[parent->objects.size()-1] = temp;
        }
    }
}

void Object::moveToBack(){
    if (parent != NULL){
        int pos = parent->findObject(this);
    
        if (pos > 0){
            Object* temp = parent->objects[pos];
            
            for (int i = pos; i > 0; i--){
                parent->objects[i] = parent->objects[i-1];
            }
            parent->objects[0] = temp;
        }
    }
}

void Object::moveDown(){
    if (parent != NULL){
        int pos = parent->findObject(this);
        
        if (pos > 0){
            Object* temp = parent->objects[pos];
            parent->objects[pos] = parent->objects[pos-1];
            parent->objects[pos-1] = temp;
        }
    }
    
}
void Object::moveUp(){
    if (parent != NULL){
        int pos = parent->findObject(this);
        
        if ((pos >= 0) && (pos < (parent->objects.size()-1))){
            Object* temp = parent->objects[pos];
            parent->objects[pos] = parent->objects[pos+1];
            parent->objects[pos+1] = temp;
        }
    }
}


void Object::addTimeline (Timeline* timeline){
    bool founded = false;

    std::vector<Timeline*>::iterator it;
    for (it = timelines.begin(); it != timelines.end(); ++it) {
        if (timeline == (*it))
            founded = true;
    }

    if (!founded){
        if (!timeline->parent) {
            timelines.push_back(timeline);
            timeline->parent = this;
        }else{
            Log::Error(LOG_TAG, "This timeline is attached to other object");
        }
    }
}

void Object::removeTimeline (Timeline* timeline){
    std::vector<Timeline*>::iterator i = std::remove(timelines.begin(), timelines.end(), timeline);
    timelines.erase(i,timelines.end());
    timeline->parent = NULL;
}

void Object::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    
    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    this->viewProjectionMatrix = viewProjectionMatrix;
    this->cameraPosition = cameraPosition;
    
    updateMVPMatrix();
    
    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);
    }
    
}

void Object::updateMatrix(){

    Matrix4 centerMatrix = Matrix4::translateMatrix(-center);
    Matrix4 scaleMatrix = Matrix4::scaleMatrix(scale);
    Matrix4 translateMatrix = Matrix4::translateMatrix(position);
    Matrix4 rotationMatrix = rotation.getRotationMatrix();

    this->modelMatrix = centerMatrix * scaleMatrix * rotationMatrix * translateMatrix;

    if (parent != NULL){
        Matrix4 parentCenterMatrix = Matrix4::translateMatrix(parent->center);
        this->modelMatrix = this->modelMatrix * parentCenterMatrix * parent->modelMatrix;
        worldRotation = parent->worldRotation * rotation;
        worldPosition = modelMatrix * Vector3(0,0,0);
    }else{
        worldRotation = rotation;
        worldPosition = position;
    }

    updateMVPMatrix();

    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->updateMatrix();
    }

}

void Object::updateMVPMatrix(){
    if (this->viewProjectionMatrix != NULL){
        this->modelViewProjectionMatrix = this->modelMatrix * (*this->viewProjectionMatrix);
    }
}

bool Object::isIn3DScene(){
    if (scene && scene->is3D())
        return true;
    
    return false;
}

bool Object::isLoaded(){
    return loaded;
}

bool Object::reload(){
    
    destroy();
    
    return load();
}

bool Object::load(){

    if ((position.z != 0) && isIn3DScene()){
        setDepth(true);
    }

    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->load();
    }

    loaded = true;
    firstLoaded = true;

    return loaded;

}

bool Object::draw(){
    if (position.z != 0){
        setDepth(true);
    }

    for (int i = 0; i < timelines.size(); i++){
        if (timelines[i]->isStarted()) {
            timelines[i]->step();
        }else{
            removeTimeline(timelines[i]);
        }
    }
    
    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        if ((*it)->scene != (*it)){ //if not a scene object
            if (!(*it)->firstLoaded)
                (*it)->load();
            if ((*it)->loaded)
                (*it)->draw();
        }
    }
    
    return loaded;
}

void Object::destroy(){

    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it){
        (*it)->destroy();
    }

    loaded = false;

}
