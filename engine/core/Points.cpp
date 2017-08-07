#include "Points.h"

#include "PrimitiveMode.h"
#include "Scene.h"
#include "Engine.h"

using namespace Supernova;

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

    render = NULL;
}

Points::~Points(){
    destroy();

    if (render)
        delete render;
}

void Points::updatePositions(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, positions.size(), &positions.front());
}

void Points::updateNormals(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, normals.size(), &normals.front());
}

void Points::updatePointColors(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, colors.size(), &colors.front());
}

void Points::updatePointSizes(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_POINTSIZES, pointSizes.size(), &pointSizes.front());
}

void Points::updateTextureRects(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, rectsData.size()/4, &rectsData.front());
}

void Points::addPoint(){
    positions.push_back(Vector3(0.0, 0.0, 0.0));
    pointSizes.push_back(1);
    normals.push_back(Vector3(0.0, 0.0, 1.0));
    colors.push_back(*material.getColor());
    textureRects.push_back(NULL);

    fillScaledSizeVector();
    updateNormalizedRectsData();
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
    if (loaded)
        updatePointColors();
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
        updateNormalizedRectsData();
        if (loaded)
            updateTextureRects();
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

    if (loaded)
        updatePointSizes();
}

void Points::updateNormalizedRectsData(){
    if (this->texWidth != 0 && this->texHeight != 0) {
        
        rectsData.clear();
        for (int i=0; i < textureRects.size(); i++){
            if (textureRects[i]) {
                if (!textureRects[i]->isNormalized()) {
                    rectsData.push_back(textureRects[i]->getX() / (float) texWidth);
                    rectsData.push_back(textureRects[i]->getY() / (float) texHeight);
                    rectsData.push_back(textureRects[i]->getWidth() / (float) texWidth);
                    rectsData.push_back(textureRects[i]->getHeight() / (float) texHeight);
                }else{
                    rectsData.push_back(textureRects[i]->getX());
                    rectsData.push_back(textureRects[i]->getY());
                    rectsData.push_back(textureRects[i]->getWidth());
                    rectsData.push_back(textureRects[i]->getHeight());
                }
            }
        }
        
    }
}

void Points::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updatePointScale();
}

void Points::updateMatrix(){
    ConcreteObject::updateMatrix();

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

bool Points::renderDraw(){
    if (!ConcreteObject::renderDraw())
        return false;
    
    render->prepareDraw();
    render->draw();
    render->finishDraw();
    
    return true;
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

    if (render == NULL)
        render = ObjectRender::newInstance();
    
    render->setPrimitiveType(S_PRIMITIVE_POINTS);
    render->setProgramShader(S_SHADER_POINTS);
    render->setDynamicBuffer(true);
    
    render->setTexture(material.getTexture());
    if (scene){
        render->setSceneRender(scene->getSceneRender());
        render->setLightRender(scene->getLightRender());
        render->setFogRender(scene->getFogRender());
    }

    if ((material.getTexture()) && (textureRects.size() > 0)){
        material.getTexture()->load();
        texWidth = material.getTexture()->getWidth();
        texHeight = material.getTexture()->getHeight();

        updateNormalizedRectsData();
    }
    
    render->addVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, 3, positions.size(), &positions.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, 3, normals.size(), &normals.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTSIZES, 1, pointSizesScaled.size(), &pointSizesScaled.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, 4, colors.size(), &colors.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, 4, rectsData.size()/4, &rectsData.front());
    
    render->addProperty(S_PROPERTY_MODELMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelMatrix);
    render->addProperty(S_PROPERTY_NORMALMATRIX, S_PROPERTYDATA_MATRIX4, 1, &normalMatrix);
    render->addProperty(S_PROPERTY_MVPMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelViewProjectionMatrix);
    render->addProperty(S_PROPERTY_CAMERAPOS, S_PROPERTYDATA_FLOAT3, 1, &cameraPosition);
    
    bool renderloaded = render->load();

    if (renderloaded)
        return ConcreteObject::load();
    else
        return false;
}

void Points::destroy(){
    
    ConcreteObject::destroy();

    if (render)
        render->destroy();
    
    for (int i=0; i < textureRects.size(); i++){
        delete textureRects[i];
    }
    
}
