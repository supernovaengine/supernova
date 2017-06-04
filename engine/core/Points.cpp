#include "Points.h"

#include "PrimitiveMode.h"
#include "Scene.h"
#include "Engine.h"
#include "render/TextureManager.h"

Points::Points(){

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

Points::~Points(){
    destroy();
}

void Points::addPoint(){
    positions.push_back(Vector3(0.0, 0.0, 0.0));
    pointSizes.push_back(1);
    normals.push_back(Vector3(0.0, 0.0, 1.0));
    colors.push_back(*material.getColor());
    textureRects.push_back(NULL);

    fillScaledSizeVector();
    normalizeTextureRects();
}

void Points::addPoint(Vector3 position){
    addPoint();

    setPointPosition((int)positions.size()-1, position);
}

void Points::setPointPosition(int point, Vector3 position){
    if (point >= 0 && point < positions.size()){
        positions[point] = position;
    }
}

void Points::setPointPosition(int point, float x, float y, float z){
    setPointPosition(point, Vector3(x, y, z));
}

void Points::setPointSize(int point, float size){

    if (point >= 0 && point < positions.size()){
        pointSizes[point] = size;
    }

    fillScaledSizeVector();
}

void Points::setPointColor(int point, Vector4 color){
    colors[point] = color;
    renderManager.getRender()->updatePointColors();
}

void Points::setPointColor(int point, float red, float green, float blue, float alpha){
    setPointColor(point, Vector4(red, green, blue, alpha));
}

void Points::setPointSprite(int point, std::string id){
    if (textureRects[point]){
        textureRects[point]->setRect(&framesRect[id]);
    }else {
        textureRects[point] = new Rect(framesRect[id]);
    }
    
    if (loaded && !useTextureRects){
        useTextureRects = true;
        reload();
    }else{
        useTextureRects = true;
        normalizeTextureRects();
        renderManager.getRender()->updateTextureRects();
    }
}

void Points::setPointSprite(int point, int id){
    setPointSprite(point, std::to_string(id));
}

void Points::updatePointScale(){
    if (sizeAttenuation) {
        pointScale = pointScaleFactor / (modelViewProjectionMatrix * Vector4(0, 0, 0, 1.0)).w;
    }else{
        pointScale = 1;
    }

    if (pointSizeReference == S_POINTSIZE_HEIGHT)
        pointScale *= (float)Engine::getScreenHeight() / (float)Engine::getCanvasHeight();
    if (pointSizeReference == S_POINTSIZE_WIDTH)
        pointScale *= (float)Engine::getScreenWidth() / (float)Engine::getCanvasWidth();

    fillScaledSizeVector();
}

void Points::fillScaledSizeVector(){
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

void Points::normalizeTextureRects(){
    if (this->texWidth != 0 && this->texHeight != 0) {
        for (int i=0; i < textureRects.size(); i++){
            if (textureRects[i]) {
                if (!textureRects[i]->isNormalized()) {
                    textureRects[i]->setRect(textureRects[i]->getX() / (float) texWidth,
                                            textureRects[i]->getY() / (float) texHeight,
                                            textureRects[i]->getWidth() / (float) texWidth,
                                            textureRects[i]->getHeight() / (float) texHeight);
                }
            }
        }
    }
}

void Points::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::transform(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updatePointScale();
}

void Points::update(){
    ConcreteObject::update();

    if (this->viewMatrix){
       this->normalMatrix = viewMatrix->getTranspose();
    }

    updatePointScale();
}

void Points::setSizeAttenuation(bool sizeAttenuation){
    this->sizeAttenuation = sizeAttenuation;
}

void Points::setPointScaleFactor(float pointScaleFactor){
    this->pointScaleFactor = pointScaleFactor;
}

void Points::setPointSizeReference(int pointSizeReference){
    this->pointSizeReference = pointSizeReference;
}

void Points::setMinPointSize(float minPointSize){
    this->minPointSize = minPointSize;
}

void Points::setMaxPointSize(float maxPointSize){
    this->maxPointSize = maxPointSize;
}

void Points::addSpriteFrame(std::string id, float x, float y, float width, float height){
    framesRect[id] = Rect(x, y, width, height);
}

void Points::removeSpriteFrame(std::string id){
    framesRect.erase(id);
}

std::vector<Vector3>* Points::getPositions(){
    return &positions;
}

std::vector<Vector3>* Points::getNormals(){
    return &normals;
}

std::vector<Rect*>* Points::getTextureRects(){
    return &textureRects;
}

std::vector<float>* Points::getPointSizes(){
    return &pointSizes;
}

std::vector<Vector4>* Points::getColors(){
    return &colors;
}

bool Points::render(){
    return renderManager.getRender()->draw();
}

bool Points::load(){
    
    

    while (positions.size() > normals.size()){
        normals.push_back(Vector3(0,0,0));
    }
    
    while (positions.size() > textureRects.size()){
        textureRects.push_back(NULL);
    }

    while (positions.size() > pointSizes.size()){
        pointSizes.push_back(1);
    }

    while (positions.size() > colors.size()){
        colors.push_back(Vector4(0.0, 0.0, 0.0, 1.0));
    }


    fillScaledSizeVector();


    renderManager.getRender()->setPoints(this);

    if ((material.getTextures().size() > 0) && (textureRects.size() > 0)){
        TextureManager::loadTexture(material.getTextures()[0]);
        
        texWidth = TextureManager::getTextureWidth(material.getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(material.getTextures()[0]);
        
        normalizeTextureRects();
    }
    
    renderManager.getRender()->load();

    return ConcreteObject::load();
}

bool Points::draw(){

    return ConcreteObject::draw();
}


void Points::destroy(){
    for (int i=0; i < textureRects.size(); i++){
        delete textureRects[i];
    }

    renderManager.getRender()->destroy();

    ConcreteObject::destroy();
}
