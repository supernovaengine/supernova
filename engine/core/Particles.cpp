#include "Particles.h"

#include "PrimitiveMode.h"
#include "Scene.h"

Particles::Particles(){

    pointScale = 1.0;
    sizeAttenuation = false;
    pointScaleFactor = 200;

    minPointSize = 1;
    maxPointSize = 1000;

    positions.push_back(Vector3(-3, 1, 20));
    positions.push_back(Vector3(0, 1, 20));
    positions.push_back(Vector3(3, 1, 20));
    
    pointSizes.push_back(30);
    pointSizes.push_back(60);
    pointSizes.push_back(30);

    normals.push_back(Vector3(0,0,1.0));
    normals.push_back(Vector3(0,0,1.0));
    normals.push_back(Vector3(0,0,1.0));
}

Particles::~Particles(){

}

void Particles::updatePointScale(){
    if (sizeAttenuation) {
        pointScale = 200 / (modelViewProjectionMatrix * Vector4(0, 0, 0, 1.0)).w;
    }else{
        pointScale = 1;
    }
}

void Particles::fillScaledSizeVector(){
    pointSizesScaled.clear();
    for (int i = 0; i < pointSizes.size(); i++){
        float pointSizeScaledVal = pointSizes[i] * pointScale;
        if (pointSizeScaledVal < minPointSize)
            pointSizeScaledVal = minPointSize;
        if (pointSizeScaledVal > maxPointSize)
            pointSizeScaledVal = maxPointSize;
        pointSizesScaled.push_back(pointSizeScaledVal);
    }
}

void Particles::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::transform(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updatePointScale();
}

void Particles::update(){
    ConcreteObject::update();

    if (this->viewMatrix){
       this->normalMatrix = viewMatrix->getTranspose();
    }

    updatePointScale();
}

void Particles::setPointScale(float pointScale){
    this->pointScale = pointScale;
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
    
    fillScaledSizeVector();

    if (scene != NULL){
        renderManager.getRender()->setSceneRender(((Scene*)scene)->getSceneRender());
    }

    renderManager.getRender()->setPositions(&positions);
    renderManager.getRender()->setNormals(&normals);
    renderManager.getRender()->setMaterial(&material);
    
    renderManager.getRender()->setPointSizes(&pointSizesScaled);

    renderManager.getRender()->setIsPoints(true);
    renderManager.getRender()->setPrimitiveMode(S_POINTS);

    renderManager.load();

    return ConcreteObject::load();
}

bool Particles::draw(){
    
    fillScaledSizeVector();

    renderManager.getRender()->setModelMatrix(&modelMatrix);
    renderManager.getRender()->setNormalMatrix(&normalMatrix);
    renderManager.getRender()->setModelViewProjectionMatrix(&modelViewProjectionMatrix);
    renderManager.getRender()->setCameraPosition(cameraPosition);
    
    renderManager.getRender()->updatePointSizes();

    return ConcreteObject::draw();
}
