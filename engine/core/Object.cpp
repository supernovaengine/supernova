#include "Object.h"
#include "Log.h"
#include "GUIObject.h"
#include "Light.h"
#include "Scene.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Object::Object(){
    loaded = false;
    firstLoaded = false;
    
    parent = NULL;
    scene = NULL;

    ownedBodies = false;

    viewMatrix = NULL;
    projectionMatrix = NULL;
    viewProjectionMatrix = NULL;
    cameraPosition = Vector3(0,0,0);

    scale = Vector3(1,1,1);
    position = Vector3(0,0,0);
    rotation = Quaternion(1,0,0,0);
    center = Vector3(0,0,0);
}

Object::~Object(){
    
    if (parent)
        parent->removeObject(this);

    if (ownedBodies)
        delete body;
    else
        body->attachedObject = NULL;
    
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

    //This is for not add object in scene and subscenes
    if (this->scene == scene) {
        if (Light *light_ptr = dynamic_cast<Light *>(this)) {
            scene->addLight(light_ptr);
        }

        if (GUIObject *guiobject_ptr = dynamic_cast<GUIObject *>(this)) {
            scene->addGUIObject(guiobject_ptr);
        }

        if (SkyBox *sky_ptr = dynamic_cast<SkyBox *>(this)) {
            scene->setSky(sky_ptr);
        }
    }

    if (Scene *scene_ptr = dynamic_cast<Scene *>(this)) {
        scene->addSubScene(scene_ptr);
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
    
    if (obj->parent == NULL) {
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
        Log::Error("Object has a parent already");
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
    
    obj->updateMatrix();
}

void Object::setSceneDepth(bool depth){
    if (scene) {
        if (scene->getUserDefinedDepth() != S_OPTION_NO)
            scene->useDepth = depth;
    }
}

void Object::setPosition(const float x, const float y, const float z){
    setPosition(Vector3(x, y, z));
}

void Object::setPosition(const float x, const float y){
    setPosition(Vector3(x, y, this->position.z));
}

void Object::setPosition(Vector2 position){
    setPosition(Vector3(position.x, position.y, this->position.z));
}

void Object::setPosition(Vector3 position){
    if (body)
        body->setPosition(position);

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
    if (body)
        body->setRotation(rotation);

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

Vector3 Object::getWorldScale(){
    return this->worldScale;
}

void Object::setCenter(const float x, const float y, const float z){
    setCenter(Vector3(x, y, z));
}

void Object::setCenter(Vector3 center){
    if (this->center != center){
        this->center = center;
        updateMatrix();
    }
}

void Object::setCenter(const float x, const float y){
    setCenter(Vector3(x, y, getCenter().z));
}

void Object::setCenter(Vector2 center){
    setCenter(Vector3(center.x, center.y, getCenter().z));
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
    return cameraPosition;
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

void Object::addAction(Action* action){
    bool founded = false;

    std::vector<Action*>::iterator it;
    for (it = actions.begin(); it != actions.end(); ++it) {
        if (action == (*it))
            founded = true;
    }

    if (!founded){
        if (!action->object) {
            actions.push_back(action);
            action->object = this;
        }else{
            Log::Error("This action is attached to other object");
        }
    }
}

void Object::removeAction(Action* action){
    if (action->object == this){
        std::vector<Action*>::iterator i = std::remove(actions.begin(), actions.end(), action);
        actions.erase(i,actions.end());
        action->object = NULL;
    }else{
        Log::Error("This action is attached to other object");
    }
}

void Object::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    
    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    this->viewProjectionMatrix = viewProjectionMatrix;
    this->cameraPosition = *cameraPosition;
    
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

    this->modelMatrix = translateMatrix * rotationMatrix * scaleMatrix * centerMatrix;

    if (parent != NULL){
        Matrix4 parentCenterMatrix = Matrix4::translateMatrix(parent->center);
        this->modelMatrix = parent->modelMatrix * parentCenterMatrix * this->modelMatrix;
        worldRotation = parent->worldRotation * rotation;
        worldScale = Vector3(parent->worldScale.x * scale.x, parent->worldScale.y * scale.y, parent->worldScale.z * scale.z);
        worldPosition = modelMatrix * center;
    }else{
        worldRotation = rotation;
        worldScale = scale;
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
        this->modelViewProjectionMatrix = (*this->viewProjectionMatrix) * this->modelMatrix;
    }
}

bool Object::isIn3DScene(){
    if (scene && scene->is3D())
        return true;
    
    return false;
}

bool Object::isOwnedBodies(){
    return ownedBodies;
}

void Object::setOwnedBodies(bool ownedBodies){
    this->ownedBodies = ownedBodies;
}

void Object::attachBody(Body* body){
    if (!body->attachedObject){
        this->body = body;
        body->attachedObject = this;

        body->setPosition(position);
        body->setRotation(rotation);
    }else{
        Log::Error("Body is attached with other object already");
    }
}

void Object::detachBody(){
    this->body->attachedObject = NULL;
    this->body = NULL;
}

void Object::updateFromBody(){
    if (body){
        bool needUpdate = false;
        Vector3 bodyPosition = body->getPosition();
        Quaternion bodyRotation = body->getRotation();

        if (getPosition() != bodyPosition){
            position = bodyPosition;
            needUpdate = true;
        }

        if (getRotation() != bodyRotation){
            rotation = bodyRotation;
            needUpdate = true;
        }

        if (needUpdate)
            updateMatrix();
    }
}

bool Object::isLoaded(){
    return loaded;
}

bool Object::reload(){
    //TODO: Check if it is really necessary
    destroy();
    return load();
}

bool Object::load(){

    if ((position.z != 0) && isIn3DScene()){
        setSceneDepth(true);
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
        setSceneDepth(true);
    }

    for (int i = 0; i < actions.size(); i++){
        if (actions[i]->isRunning()) {
            actions[i]->step();
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
