#include "Particles.h"

#include "PrimitiveMode.h"
#include "Scene.h"

Particles::Particles(){

    pointScale = 1.0;
    pointSize = 30.0;
    sizeAttenuation = false;
    pointScaleFactor = 200;

    minPointSize = 1;
    maxPointSize = 1000;

    positions.push_back(Vector3(0, 5, 0));
    positions.push_back(Vector3(0, 8, 0));
    positions.push_back(Vector3(0, 10, 0));
}

Particles::~Particles(){

}

void Particles::updatePointDistance(){
    if (sizeAttenuation) {
        pointScale = 200 / (modelViewProjectionMatrix * Vector4(0, 0, 0, 1.0)).w;
    }else{
        pointScale = 1;
    }
}

void Particles::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::transform(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updatePointDistance();
}

void Particles::update(){
    ConcreteObject::update();

    updatePointDistance();
}

void Particles::setPointScale(float pointScale){
    this->pointScale = pointScale;
}

void Particles::setPointSize(float pointSize){
    this->pointSize = pointSize;
}

void Particles::setSizeAttenuation(bool sizeAttenuation){
    this->sizeAttenuation = sizeAttenuation;
}

void Particles::setPointScaleFactor(float pointScaleFactor){
    this->pointScaleFactor = pointScaleFactor;
}

void Particles::setMinPointSize(float minPointSize){
    this->minPointSize = minPointSize;
}

void Particles::setMaxPointSize(float maxPointSize){
    this->maxPointSize = maxPointSize;
}

bool Particles::render(){
    return renderManager.draw();
}

bool Particles::load(){

    if (scene != NULL){
        renderManager.getRender()->setSceneRender(((Scene*)scene)->getSceneRender());
    }

    renderManager.getRender()->setPositions(&positions);
    renderManager.getRender()->setNormals(&normals);
    renderManager.getRender()->setMaterial(&material);

    renderManager.getRender()->setIsPoints(true);
    renderManager.getRender()->setPrimitiveMode(S_POINTS);

    renderManager.load();

    return ConcreteObject::load();
}

bool Particles::draw(){

    float pointSizeScaled = pointSize * pointScale;
    if (pointSizeScaled < minPointSize)
        pointSizeScaled = minPointSize;
    if (pointSizeScaled > maxPointSize)
        pointSizeScaled = maxPointSize;

    renderManager.getRender()->setPointSize(pointSizeScaled);

    renderManager.getRender()->setModelMatrix(&modelMatrix);
    renderManager.getRender()->setNormalMatrix(&normalMatrix);
    renderManager.getRender()->setModelViewProjectionMatrix(&modelViewProjectionMatrix);
    renderManager.getRender()->setCameraPosition(cameraPosition);

    return ConcreteObject::draw();
}
