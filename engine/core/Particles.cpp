#include "Particles.h"

#include "PrimitiveMode.h"
#include "Scene.h"
#include "Supernova.h"
#include "render/TextureManager.h"

Particles::Particles(){

    pointScale = 1.0;
    sizeAttenuation = false;
    pointScaleFactor = 100;

    pointSizeReference = S_POINTSIZE_WIDTH;

    minPointSize = 1;
    maxPointSize = 1000;
    
    texWidth = 0;
    texHeight = 0;

    isSlicedTexture = false;
    slicesX = 1;
    slicesY = 1;

}

Particles::~Particles(){

}

void Particles::addParticle(){
    positions.push_back(Vector3(0.0, 0.0, 0.0));
    pointSizes.push_back(1);
    normals.push_back(Vector3(0.0, 0.0, 1.0));
    frames.push_back(0);
    colors.push_back(*material.getColor());
    textureRects.push_back(TextureRect(0.0, 0.5, 1.0, 0.5));

    fillScaledSizeVector();
    fillSlicesPosPixelsVector();
}

void Particles::addParticle(Vector3 position){
    addParticle();

    setParticlePosition((int)positions.size()-1, position);
}

void Particles::setParticlePosition(int particle, Vector3 position){
    if (particle >= 0 && particle < positions.size()){
        positions[particle] = position;
    }
}

void Particles::setParticlePosition(int particle, float x, float y, float z){
    setParticlePosition(particle, Vector3(x, y, z));
}

void Particles::setParticleSize(int particle, float size){

    if (particle >= 0 && particle < positions.size()){
        pointSizes[particle] = size;
    }

    fillScaledSizeVector();
}

void Particles::setParticleFrame(int particle, int frame){
    frames[particle] = frame;
    fillSlicesPosPixelsVector();
}

void Particles::setParticleColor(int particle, Vector4 color){
    colors[particle] = color;
    renderManager.getRender()->updatePointColors();
}

void Particles::setParticleColor(int particle, float red, float green, float blue, float alpha){
    setParticleColor(particle, Vector4(red, green, blue, alpha));
}

void Particles::updatePointScale(){
    if (sizeAttenuation) {
        pointScale = pointScaleFactor / (modelViewProjectionMatrix * Vector4(0, 0, 0, 1.0)).w;
    }else{
        pointScale = 1;
    }

    if (pointSizeReference == S_POINTSIZE_HEIGHT)
        pointScale *= (float)Supernova::getScreenHeight() / (float)Supernova::getCanvasHeight();
    if (pointSizeReference == S_POINTSIZE_WIDTH)
        pointScale *= (float)Supernova::getScreenWidth() / (float)Supernova::getCanvasWidth();

    fillScaledSizeVector();
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
    
    renderManager.getRender()->updatePointSizes();
}

void Particles::setTextureSlices(int slicesX, int slicesY){
    if (slicesX >= 1 && slicesY >= 1){
        this->slicesX = slicesX;
        this->slicesY = slicesY;

        if ((slicesX > 1 || slicesY > 1) && (material.getTextures().size() > 0)){
            isSlicedTexture = true;
            fillSlicesPosPixelsVector();
        }else{
            isSlicedTexture = false;
        }
    }
}

void Particles::fillSlicesPosPixelsVector(){
    if (texWidth > 0 && texHeight > 0){
        int slicesPosY;
        int slicesPosX;
        slicesPos.clear();
        for (int i = 0; i < frames.size(); i++){

            if (frames[i] < slicesX * slicesY){
                slicesPosY = floor(frames[i] / slicesX);
                slicesPosX = frames[i] - slicesX * slicesPosY;
            }else{
                slicesPosY = 0;
                slicesPosX = 0;
            }

            slicesPos.push_back(std::make_pair(slicesPosX * texWidth / slicesX, slicesPosY * texHeight / slicesY));
        }
    }
    
    renderManager.getRender()->updateSlicesPos();
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

void Particles::setSizeAttenuation(bool sizeAttenuation){
    this->sizeAttenuation = sizeAttenuation;
}

void Particles::setPointScaleFactor(float pointScaleFactor){
    this->pointScaleFactor = pointScaleFactor;
}

void Particles::setPointSizeReference(int pointSizeReference){
    this->pointSizeReference = pointSizeReference;
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
   // renderManager.getRender()->setSlicesPos(&slicesPos);
    renderManager.getRender()->setTextureRect(&textureRects);
    renderManager.getRender()->setPointColors(&colors);

    //renderManager.getRender()->setIsSlicedTexture(isSlicedTexture);

    renderManager.load();

    if ((material.getTextures().size() > 0) && (isSlicedTexture)){
        texWidth = TextureManager::getTextureWidth(material.getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(material.getTextures()[0]);
        
       // renderManager.getRender()->setTextureSize(texWidth, texHeight);
      //    renderManager.getRender()->setSliceSize(texWidth / slicesX, texHeight / slicesY);
        fillSlicesPosPixelsVector();
    }

    return ConcreteObject::load();
}

bool Particles::draw(){
    
    renderManager.getRender()->setModelMatrix(&modelMatrix);
    renderManager.getRender()->setNormalMatrix(&normalMatrix);
    renderManager.getRender()->setModelViewProjectionMatrix(&modelViewProjectionMatrix);
    renderManager.getRender()->setCameraPosition(cameraPosition);

    return ConcreteObject::draw();
}
