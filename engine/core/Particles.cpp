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
    
    useTextureRects = false;

}

Particles::~Particles(){

}

void Particles::addParticle(){
    positions.push_back(Vector3(0.0, 0.0, 0.0));
    pointSizes.push_back(1);
    normals.push_back(Vector3(0.0, 0.0, 1.0));
    colors.push_back(*material.getColor());
    textureRects.push_back(TextureRect());

    fillScaledSizeVector();
    normalizeTextureRects();
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

void Particles::setParticleColor(int particle, Vector4 color){
    colors[particle] = color;
    renderManager.getRender()->updatePointColors();
}

void Particles::setParticleColor(int particle, float red, float green, float blue, float alpha){
    setParticleColor(particle, Vector4(red, green, blue, alpha));
}

void Particles::setParticleFrame(int particle, std::string id){
    textureRects[particle] = framesRect[id];
    
    if (loaded && !useTextureRects){
        useTextureRects = true;
        reload();
    }else{
        useTextureRects = true;
        normalizeTextureRects();
        renderManager.getRender()->updateTextureRects();
    }
}

void Particles::setParticleFrame(int particle, int id){
    setParticleFrame(particle, std::to_string(id));
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

void Particles::normalizeTextureRects(){
    if (this->texWidth != 0 && this->texHeight != 0) {
        for (int i=0; i < textureRects.size(); i++){
            if (!textureRects[i].isNormalized()){
                textureRects[i].setRect(textureRects[i].getX() / (float) texWidth,
                                        textureRects[i].getY() / (float) texHeight,
                                        textureRects[i].getWidth() / (float) texWidth,
                                        textureRects[i].getHeight() / (float) texHeight);
            }
        }
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

void Particles::addFrame(std::string id, float x, float y, float width, float height){
    framesRect[id] = TextureRect(x, y, width, height);
}

void Particles::removeFrame(std::string id){
    framesRect.erase(id);
}

std::vector<Vector3>* Particles::getPositions(){
    return &positions;
}

std::vector<Vector3>* Particles::getNormals(){
    return &normals;
}

std::vector<TextureRect>* Particles::getTextureRects(){
    if (useTextureRects){
        return &textureRects;
    }else{
        return NULL;
    }
}

std::vector<float>* Particles::getPointSizes(){
    return &pointSizes;
}

std::vector<Vector4>* Particles::getColors(){
    return &colors;
}

bool Particles::render(){
    return renderManager.draw();
}

bool Particles::load(){
    
    

    while (positions.size() > normals.size()){
        normals.push_back(Vector3(0,0,0));
    }
    
    while (positions.size() > textureRects.size()){
        textureRects.push_back(TextureRect());
    }

    while (positions.size() > pointSizes.size()){
        pointSizes.push_back(1);
    }

    while (positions.size() > colors.size()){
        colors.push_back(Vector4(0.0, 0.0, 0.0, 1.0));
    }


    fillScaledSizeVector();


    renderManager.getRender()->setParticles(this);

    if ((material.getTextures().size() > 0) && (textureRects.size() > 0)){
        TextureManager::loadTexture(material.getTextures()[0]);
        
        texWidth = TextureManager::getTextureWidth(material.getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(material.getTextures()[0]);
        
        normalizeTextureRects();
    }
    
    renderManager.load();

    return ConcreteObject::load();
}

bool Particles::draw(){

    return ConcreteObject::draw();
}
