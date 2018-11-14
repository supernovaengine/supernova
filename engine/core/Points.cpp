#include "Points.h"

#include "Scene.h"
#include "Engine.h"
#include "math/Angle.h"

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

    automaticUpdate = true;
    pertmitSortTransparentPoints = true;

    buffers.resize(1);
}

Points::~Points(){
    destroy();

    if (render)
        delete render;
}

bool Points::shouldSort(){
   return (transparent && scene && scene->isUseDepth() && scene->getUserDefinedTransparency() != S_OPTION_NO && pertmitSortTransparentPoints);
}

void Points::copyBuffer(){
    buffers[0].clearAll();
    buffers[0].setName("points");
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_POINTSIZES, 1);
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, 4);
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_POINTROTATIONS, 1);
    if (useTextureRects)
        buffers[0].addAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, 4);

    std::vector<Point> sortedPoints = points;

    if (shouldSort()) {
        auto comparePoints = [this](const Point a, const Point b) -> bool {
            float distanceToCameraA = (this->cameraPosition - (modelMatrix * a.position)).length();
            float distanceToCameraB = (this->cameraPosition - (modelMatrix * b.position)).length();
            return distanceToCameraA > distanceToCameraB;
        };

        std::sort(sortedPoints.begin(), sortedPoints.end(), comparePoints);
    }

    for (int i=0; i < sortedPoints.size(); i++){
        if (sortedPoints[i].visible) {
            buffers[0].addValue(S_VERTEXATTRIBUTE_VERTICES, sortedPoints[i].position);
            buffers[0].addValue(S_VERTEXATTRIBUTE_NORMALS, sortedPoints[i].normal);
            buffers[0].addValue(S_VERTEXATTRIBUTE_POINTSIZES, sortedPoints[i].size);
            buffers[0].addValue(S_VERTEXATTRIBUTE_POINTCOLORS, sortedPoints[i].color);
            buffers[0].addValue(S_VERTEXATTRIBUTE_POINTROTATIONS, sortedPoints[i].rotation);
            if (useTextureRects)
                buffers[0].addValue(S_VERTEXATTRIBUTE_TEXTURERECTS, sortedPoints[i].textureRect.getVector());
        }
    }
}

void Points::updatePoints(){
    copyBuffer();

    if (loaded)
        updateBuffer(0);
}

void Points::normalizeTextureRects(){
    if (useTextureRects && this->texWidth != 0 && this->texHeight != 0) {
        for (int i=0; i < points.size(); i++) {
            if (!points[i].textureRect.isNormalized()) {
                points[i].textureRect.setRect(points[i].textureRect.getX() / (float) texWidth,
                                              points[i].textureRect.getY() / (float) texHeight,
                                              points[i].textureRect.getWidth() / (float) texWidth,
                                              points[i].textureRect.getHeight() / (float) texHeight);
            }

        }
    }
}

void Points::addPoint(){
    points.push_back({Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), Rect(0.0, 0.0, 1.0, 1.0), 1, *material.getColor(), 0.0, true});
}

void Points::addPoint(Vector3 position){
    addPoint();
    setPointPosition((int)points.size()-1, position);
}

void Points::clearPoints(){
    points.clear();
}

void Points::setPointPosition(int point, Vector3 position){
    if (point >= 0 && point < points.size()){
        
        bool changed = false;
        if (points[point].position != position)
            changed = true;

        points[point].position = position;

        if (loaded && changed && automaticUpdate){
            updatePoints();
        }
    }
}

void Points::setPointPosition(int point, float x, float y, float z){
    setPointPosition(point, Vector3(x, y, z));
}

void Points::setPointSize(int point, float size){
    if (point >= 0 && point < points.size()){
        
        bool changed = false;
        if (points[point].size != size)
            changed = true;

        points[point].size = size;

        if (loaded && changed && automaticUpdate){
            updatePoints();
        }
    }
}

void Points::setPointColor(int point, Vector4 color){
    if (point >= 0 && point < points.size()) {
        
        bool changed = false;
        if (points[point].color != color)
            changed = true;

        points[point].color = color;

        if (color.w != 1){
            transparent = true;
        }

        if (loaded && changed && automaticUpdate){
            updatePoints();
        }
    }
}

void Points::setPointColor(int point, float red, float green, float blue, float alpha){
    setPointColor(point, Vector4(red, green, blue, alpha));
}

void Points::setPointRotation(int point, float rotation){
    if (point >= 0 && point < points.size()) {

        bool changed = false;
        if (points[point].rotation != Angle::defaultToRad(rotation))
            changed = true;

        points[point].rotation = Angle::defaultToRad(rotation);

        if (loaded && changed && automaticUpdate){
            updatePoints();
        }
    }
}

void Points::setPointSprite(int point, int id){
    if (point >= 0 && point < points.size() && id >= 0) {

            bool changed = false;
            if (framesRect.count(id) > 0) {
                if (points[point].textureRect != framesRect[id].rect) {
                    changed = true;
                    points[point].textureRect.setRect(framesRect[id].rect);
                }
            }else{
                changed = true;
                points[point].textureRect.setRect(Rect(0, 0, 1, 1));
            }

            if (!useTextureRects) {
                useTextureRects = true;
                if (loaded)
                    reload();
            } else {
                if (loaded && changed && automaticUpdate) {
                    normalizeTextureRects();
                    updatePoints();
                }
            }

    }
}

void Points::setPointSprite(int point, std::string name){
    std::vector<int> frameslist = findFramesByString(name);
    if (frameslist.size() > 0){
        setPointSprite(point, frameslist[0]);
    }
}

void Points::setPointVisible(int point, bool visible){
    if (point >= 0 && point < points.size()) {
        
        bool changed = false;
        if (points[point].visible != visible)
            changed = true;

        points[point].visible = visible;
        
        if (loaded && changed && automaticUpdate){
            updatePoints();
        }
        
    }
}

Vector3 Points::getPointPosition(int point){
    if (point >= 0 && point < points.size()){
        return points[point].position;
    }
    
    return Vector3(0,0,0);
}

float Points::getPointSize(int point){
    if (point >= 0 && point < points.size()){
        return points[point].size;
    }
    
    return -1;
}

Vector4 Points::getPointColor(int point){
    if (point >= 0 && point < points.size()){
        return points[point].color;
    }
    
    return Vector4(0,0,0,0);
}

float Points::getPointRotation(int point){
    if (point >= 0 && point < points.size()){
        return Angle::radToDefault(points[point].rotation);
    }

    return 0.0;
}

void Points::updatePointScale(){
    float newPointScale;
    
    if (sizeAttenuation) {
        newPointScale = pointScaleFactor / (modelViewProjectionMatrix * Vector4(0.0, 0.0, 0.0, 1.0)).w;
    }else{
        newPointScale = 1;
    }
    
    if (pointSizeReference == S_POINTSIZE_HEIGHT)
        newPointScale *= (float)Engine::getScreenHeight() / (float)Engine::getCanvasHeight();
    if (pointSizeReference == S_POINTSIZE_WIDTH)
        newPointScale *= (float)Engine::getScreenWidth() / (float)Engine::getCanvasWidth();

    if (newPointScale != pointScale){
    
        pointScale = newPointScale;
        
        if (loaded){
            updatePoints();
        }
    }
}

void Points::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updatePointScale();
    if (shouldSort())
        updatePoints();
}

void Points::updateMatrix(){
    ConcreteObject::updateMatrix();

    if (this->viewMatrix){
       this->normalMatrix = viewMatrix->transpose();
    }

    updatePointScale();
    if (shouldSort())
        updatePoints();
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

std::vector<int> Points::findFramesByString(std::string id){
    std::vector<int> frameslist;
    for (int i = 0; i < framesRect.size(); i++){

        std::size_t found = framesRect[i].id.find(id);
        if (found!=std::string::npos)
            frameslist.push_back(i);
    }
    return frameslist;
}

void Points::addSpriteFrame(int id, std::string name, Rect rect){
    framesRect[id] = {name, rect};
}

void Points::addSpriteFrame(std::string name, float x, float y, float width, float height){
    int id = 0;
    while ( framesRect.count(id) > 0 ) {
        id++;
    }
    framesRect[id] = {name, Rect(x, y, width, height)};
    addSpriteFrame(id, name, Rect(x, y, width, height));
}

void Points::addSpriteFrame(float x, float y, float width, float height){
    addSpriteFrame("", x, y,  width, height);
}

void Points::addSpriteFrame(Rect rect){
    addSpriteFrame(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void Points::removeSpriteFrame(int id){
    framesRect.erase(id);
}

void Points::removeSpriteFrame(std::string name){
    std::vector<int> frameslist = findFramesByString(name);

    while (frameslist.size() > 0) {
        framesRect.erase(frameslist[0]);
        frameslist.clear();
        frameslist = findFramesByString(name);
    }
}

void Points::setPertmitSortTransparentPoints(bool pertmitSortTransparentPoints){
    this->pertmitSortTransparentPoints = pertmitSortTransparentPoints;
}

bool Points::isPertmitSortTransparentPoints(){
    return pertmitSortTransparentPoints;
}

bool Points::renderDraw(){
    if (!ConcreteObject::renderDraw())
        return false;

    if (!visible)
        return false;
    
    render->prepareDraw();
    render->draw();
    render->finishDraw();
    
    return true;
}

bool Points::textureLoad(){
    if (!ConcreteObject::textureLoad())
        return false;
    
    if (render){
        material.getTexture()->load();
        render->addTexture(S_TEXTURESAMPLER_DIFFUSE, material.getTexture());
    }
    
    return true;
}

bool Points::load(){

    if (render == NULL)
        render = ObjectRender::newInstance();
    
    render->setPrimitiveType(S_PRIMITIVE_POINTS);
    render->setProgramShader(S_SHADER_POINTS);

    render->addTexture(S_TEXTURESAMPLER_DIFFUSE, material.getTexture());

    if (scene){
        render->setNumLights((int)scene->getLights()->size());
        render->setNumShadows2D((int)scene->getLightData()->shadowsMap2D.size());
        render->setNumShadowsCube((int)scene->getLightData()->shadowsMapCube.size());

        render->setSceneRender(scene->getSceneRender());
        render->setLightRender(scene->getLightRender());
        render->setFogRender(scene->getFogRender());

        render->addTextureVector(S_TEXTURESAMPLER_SHADOWMAP2D, scene->getLightData()->shadowsMap2D);
        render->addProperty(S_PROPERTY_NUMSHADOWS2D, S_PROPERTYDATA_INT1, 1, &scene->getLightData()->numShadows2D);
        render->addProperty(S_PROPERTY_DEPTHVPMATRIX, S_PROPERTYDATA_MATRIX4, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowsVPMatrix.front());
        render->addProperty(S_PROPERTY_SHADOWBIAS2D, S_PROPERTYDATA_FLOAT1, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowsBias2D.front());
        render->addProperty(S_PROPERTY_SHADOWCAMERA_NEARFAR2D, S_PROPERTYDATA_FLOAT2, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowsCameraNearFar2D.front());
        render->addProperty(S_PROPERTY_NUMCASCADES2D, S_PROPERTYDATA_INT1, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowNumCascades2D.front());

        render->addTextureVector(S_TEXTURESAMPLER_SHADOWMAPCUBE, scene->getLightData()->shadowsMapCube);
        render->addProperty(S_PROPERTY_SHADOWBIASCUBE, S_PROPERTYDATA_FLOAT1, scene->getLightData()->numShadowsCube, &scene->getLightData()->shadowsBiasCube.front());
        render->addProperty(S_PROPERTY_SHADOWCAMERA_NEARFARCUBE, S_PROPERTYDATA_FLOAT2, scene->getLightData()->numShadowsCube, &scene->getLightData()->shadowsCameraNearFarCube.front());
    }

    if ((material.getTexture()) && (useTextureRects)){
        material.getTexture()->load();
        texWidth = material.getTexture()->getWidth();
        texHeight = material.getTexture()->getHeight();
        normalizeTextureRects();
    }
/*
    render->setVertexSize(points.size());

    render->addVertexBuffer("points", points.size() * sizeof(Point), &points.front(), true);

    render->addVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, "points", 3, sizeof(Point), offsetof(Point, position));
    render->addVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, "points", 3, sizeof(Point), offsetof(Point, normal));
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTSIZES, "points", 1, sizeof(Point), offsetof(Point, size));
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, "points", 4, sizeof(Point), offsetof(Point, color));
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTROTATIONS, "points", 1, sizeof(Point), offsetof(Point, rotation));
    if (useTextureRects) {
        render->addVertexAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, "points", 4, sizeof(Point), offsetof(Point, textureRect));
    }
*/
    copyBuffer();

    for (int b = 0; b < buffers.size(); b++) {
        if (b == 0) {
            render->setVertexSize(buffers[b].getCount());
        }
        render->addVertexBuffer(buffers[b].getName(), buffers[b].getSize() * sizeof(float), buffers[b].getBuffer(), true);
        for (auto const &x : buffers[b].getAttributes()) {
            render->addVertexAttribute(x.first, buffers[b].getName(), x.second.elements, buffers[b].getItemSize() * sizeof(float), x.second.offset * sizeof(float));
        }
    }

    render->addProperty(S_PROPERTY_MODELMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelMatrix);
    render->addProperty(S_PROPERTY_NORMALMATRIX, S_PROPERTYDATA_MATRIX4, 1, &normalMatrix);
    render->addProperty(S_PROPERTY_MVPMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelViewProjectionMatrix);
    render->addProperty(S_PROPERTY_CAMERAPOS, S_PROPERTYDATA_FLOAT3, 1, &cameraPosition);
    
    bool renderloaded = render->load();
    
    if (!ConcreteObject::load())
        return false;

    return renderloaded;
}

void Points::destroy(){
    
    ConcreteObject::destroy();

    if (render)
        render->destroy();
    
}
