#include "Object.h"
#include "Supernova.h"
#include "platform/Log.h"
#include "GUIObject.h"
#include "Light.h"


Object::Object(){
    loaded = false;
    parent = NULL;
    scene = NULL;

    viewMatrix = NULL;
    viewProjectionMatrix = NULL;
    cameraPosition = NULL;

    scale = Vector3(1,1,1);
    position = Vector3(0,0,0);
    rotation = Quaternion(1,0,0,0);
    center = Vector3(0,0,0);
}

Object::~Object(){
    destroy();
}

void Object::setSceneAndConfigure(Object* scene){
    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->scene = scene;
        (*it)->setSceneAndConfigure(scene);
    }
    
    if (Light* light_ptr = dynamic_cast<Light*>(this)){
        ((Scene*)scene)->addLight(light_ptr);
    }
    
    if (Scene* scene_ptr = dynamic_cast<Scene*>(this)){
        ((Scene*)scene)->addSubScene(scene_ptr);
    }
}

void Object::addObject(Object* obj){
    if (Scene* scene_ptr = dynamic_cast<Scene*>(obj)){
        scene_ptr->isChildScene = true;
    }
    
    if (scene != NULL)
        obj->setSceneAndConfigure(scene);
    
    if (obj->parent == NULL){
        objects.insert(objects.begin(), obj);

        obj->parent = this;

        obj->viewMatrix = viewMatrix;
        obj->viewProjectionMatrix = viewProjectionMatrix;
        obj->cameraPosition = cameraPosition;

        obj->update();
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
    }
    
    if (Scene* scene_ptr = dynamic_cast<Scene*>(obj)){
        scene_ptr->isChildScene = false;
    }
    
    std::vector<Object*>::iterator i = std::remove(objects.begin(), objects.end(), obj);
    objects.erase(i,objects.end());
    
    obj->parent = NULL;
    obj->scene = NULL;
    
    obj->viewMatrix = NULL;
    obj->viewProjectionMatrix = NULL;
    obj->cameraPosition = NULL;
    
    obj->update();
}

void Object::setPosition(const float x, const float y, const float z){
    setPosition(Vector3(x, y, z));
}

void Object::setPosition(Vector3 position){
    if (this->position != position){
        this->position = position;
        update();
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
        update();
    }
}

Quaternion Object::getRotation(){
    return this->rotation;
}

void Object::setScale(const float factor){
    setScale(Vector3(factor,factor,factor));
}

void Object::setScale(Vector3 scale){
    if (this->scale != scale){
        this->scale = scale;
        update();
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
        update();
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

void Object::transform(Matrix4* viewMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){

    this->viewMatrix = viewMatrix;
    this->viewProjectionMatrix = viewProjectionMatrix;
    this->cameraPosition = cameraPosition;

    updateMatrices();

    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->transform(viewMatrix, viewProjectionMatrix, cameraPosition);
    }

}

int Object::findObject(Object* object){
    
    for (int i=0; i < objects.size(); i++){
        if (objects[i] == object){
            return i;
        }
    }
    
    return -1;
}

void Object::moveFront(){
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
void Object::moveLast(){
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
void Object::moveUp(){
    if (parent != NULL){
        int pos = parent->findObject(this);
        
        if (pos > 0){
            Object* temp = parent->objects[pos];
            parent->objects[pos] = parent->objects[pos-1];
            parent->objects[pos-1] = temp;
        }
    }
    
}
void Object::moveDown(){
    if (parent != NULL){
        int pos = parent->findObject(this);
        
        if ((pos >= 0) && (pos < (parent->objects.size()-1))){
            Object* temp = parent->objects[pos];
            parent->objects[pos] = parent->objects[pos+1];
            parent->objects[pos+1] = temp;
        }
    }
}

void Object::update(){

    Matrix4 centerMatrix = Matrix4::translateMatrix(-center);
    Matrix4 scaleMatrix = Matrix4::scaleMatrix(scale);
    Matrix4 translateMatrix = Matrix4::translateMatrix(position);
    Matrix4 rotationMatrix = rotation.getRotationMatrix();

    this->modelMatrix = centerMatrix * scaleMatrix * rotationMatrix * translateMatrix;

    if (parent != NULL){
        Matrix4 parentCenterMatrix = Matrix4::translateMatrix(parent->center);
        this->modelMatrix = this->modelMatrix * parentCenterMatrix * parent->modelMatrix;
    }

    worldPosition = modelMatrix * Vector3(0,0,0);

    updateMatrices();

    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->update();
    }

}

void Object::updateMatrices(){
    if (this->viewProjectionMatrix != NULL){
        this->modelViewProjectionMatrix = this->modelMatrix * (*this->viewProjectionMatrix);
    }
}

bool Object::load(){

    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        (*it)->load();
    }

    loaded = true;

    return true;
}

bool Object::draw(){

    std::vector<Object*>::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it) {
        if (!(*it)->loaded)
            (*it)->load();
        (*it)->draw();
    }

    return true;
}

void Object::destroy(){

    parent->removeObject(this);

    while (objects.size() > 0){
        objects.back()->destroy();
    }

    loaded = false;

}
