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

    isSpriteSheet = false;
    spritesX = 1;
    spritesY = 1;

}

Particles::~Particles(){

}

void Particles::addParticle(){
    positions.push_back(Vector3(0.0, 0.0, 0.0));
    pointSizes.push_back(1);
    normals.push_back(Vector3(0.0, 0.0, 1.0));
    sprites.push_back(0);
    colors.push_back(*material.getColor());

    fillScaledSizeVector();
    fillSpritePosPixelsVector();
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

void Particles::setParticleSprite(int particle, int sprite){
    sprites[particle] = sprite;
    fillSpritePosPixelsVector();
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

void Particles::setSpriteSheet(int spritesX, int spritesY){
    if (spritesX >= 1 && spritesY >= 1){
        this->spritesX = spritesX;
        this->spritesY = spritesY;

        if ((spritesX > 1 || spritesY > 1) && (material.getTextures().size() > 0)){
            isSpriteSheet = true;
            fillSpritePosPixelsVector();
        }else{
            isSpriteSheet = false;
        }
    }
}

void Particles::fillSpritePosPixelsVector(){
    if (texWidth > 0 && texHeight > 0){
        int spritesPosY;
        int spritesPosX;
        spritesPixelsPos.clear();
        for (int i = 0; i < sprites.size(); i++){
            
            if (sprites[i] < spritesX * spritesY){
                spritesPosY = floor(sprites[i] / spritesX);
                spritesPosX = (sprites[i]) - (spritesX) * spritesPosY;
            }else{
                spritesPosY = 0;
                spritesPosX = 0;
            }
            
            spritesPixelsPos.push_back(std::make_pair(spritesPosX * texWidth / spritesX, spritesPosY * texHeight / spritesY));
        }
    }
    
    renderManager.getRender()->updateSpritePos();
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
    renderManager.getRender()->setPointSpritesPos(&spritesPixelsPos);
    renderManager.getRender()->setPointColors(&colors);

    //renderManager.getRender()->setIsPoints(true);
    //renderManager.getRender()->setPrimitiveMode(S_POINTS);
    renderManager.getRender()->setIsSpriteSheet(isSpriteSheet);

    renderManager.load();

    if ((material.getTextures().size() > 0) && (isSpriteSheet)){
        texWidth = TextureManager::getTextureWidth(material.getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(material.getTextures()[0]);
        
        renderManager.getRender()->setTextureSize(texWidth, texHeight);
        renderManager.getRender()->setSpriteSize(texWidth / spritesX, texHeight / spritesY);
        fillSpritePosPixelsVector();
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
