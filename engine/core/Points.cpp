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
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, points.size(), &pointData.positions.front());
}

void Points::updateNormals(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_NORMALS,points.size(), &pointData.normals.front());
}

void Points::updatePointColors(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, points.size(), &pointData.colors.front());
}

void Points::updatePointSizes(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_POINTSIZES, points.size(), &pointData.sizes.front());
}

void Points::updateTextureRects(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, points.size(), &pointData.textureRects.front());
}

void Points::addPoint(){
    points.push_back({Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), NULL, 1, *material.getColor(), -1, true});
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

        if (loaded && changed){
            updatePointsData();
            
            updatePositions();
            updateNormals();
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

        if (loaded && changed){
            updatePointsData();
            
            updatePointSizes();
        }
    }
}

void Points::setPointColor(int point, Vector4 color){
    if (point >= 0 && point < points.size()) {
        
        bool changed = false;
        if (points[point].color != color)
            changed = true;

        points[point].color = color;

        if (loaded && changed){
            updatePointsData();
            
            updatePointColors();
        }
    }
}

void Points::setPointColor(int point, float red, float green, float blue, float alpha){
    setPointColor(point, Vector4(red, green, blue, alpha));
}

void Points::setPointSprite(int point, int index){
    if (point >= 0 && point < points.size()) {
        
        bool changed = false;
        
        if (points[point].textureRect){
            
            if ((*points[point].textureRect) != framesRect[index].rect)
                changed = true;
            
            points[point].textureRect->setRect(&framesRect[index].rect);
        }else {
            changed = true;
            points[point].textureRect = new Rect(framesRect[index].rect);
        }

        if (!useTextureRects){
            useTextureRects = true;

            if (loaded)
                reload();
        }else{
            useTextureRects = true;

            if (loaded && changed){
                updatePointsData();
                
                updateTextureRects();
            }
        }
        
    }
}

void Points::setPointSprite(int point, std::string id){
    std::vector<int> frameslist = findFramesByString(id);
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
        
        if (loaded && changed){
            updatePointsData();
            
            updatePositions();
            updateNormals();
            updatePointColors();
            updatePointSizes();
            updateTextureRects();
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

void Points::updatePointScale(){
    float newPointScale;
    
    if (sizeAttenuation) {
        newPointScale = pointScaleFactor / (modelViewProjectionMatrix * Vector4(0, 0, 0, 1.0)).w;
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
            updatePointsData();
        
            updatePointSizes();
        }
    }
}

void Points::sortTransparentPoints(){
    if (transparent && scene && scene->isUseDepth()){
        
        bool needSort = false;
        for (int i=0; i < points.size(); i++){
            points[i].distanceToCamera = (this->cameraPosition - (modelMatrix * points[i].position)).length();
                
            if (i > 0){
                if (points[i-1].distanceToCamera < points[i].distanceToCamera)
                    needSort = true;
            }
        }
        
        if (needSort){
            std::sort(points.begin(), points.end(),
                      [](const Point a, const Point b) -> bool
                      {
                          if (a.distanceToCamera == -1)
                              return true;
                          if (b.distanceToCamera == -1)
                              return false;
                          return a.distanceToCamera > b.distanceToCamera;
                      });
            
            if (loaded){
                updatePointsData();
                
                updatePositions();
                updateNormals();
                updatePointColors();
                updatePointSizes();
                updateTextureRects();
            }
        }
        
    }
}

void Points::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updatePointScale();
    sortTransparentPoints();
}

void Points::updateMatrix(){
    ConcreteObject::updateMatrix();

    if (this->viewMatrix){
       this->normalMatrix = viewMatrix->getTranspose();
    }

    updatePointScale();
    sortTransparentPoints();
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

void Points::addSpriteFrame(std::string id, float x, float y, float width, float height){
    framesRect.push_back({id, Rect(x, y, width, height)});
}

void Points::addSpriteFrame(float x, float y, float width, float height){
    addSpriteFrame("", x, y,  width, height);
}

void Points::addSpriteFrame(Rect rect){
    addSpriteFrame(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void Points::removeSpriteFrame(int index){
    framesRect.erase(framesRect.begin() + index);
}

void Points::removeSpriteFrame(std::string id){
    std::vector<int> frameslist = findFramesByString(id);

    while (frameslist.size() > 0) {
        framesRect.erase(framesRect.begin() + frameslist[0]);
        frameslist.clear();
        frameslist = findFramesByString(id);
    }
}

void Points::updatePointsData(){
    pointData.clearPoints();

    for (int i=0; i < points.size(); i++){
            
        pointData.positions.push_back(points[i].position.x);
        pointData.positions.push_back(points[i].position.y);
        pointData.positions.push_back(points[i].position.z);

        pointData.normals.push_back(points[i].normal.x);
        pointData.normals.push_back(points[i].normal.y);
        pointData.normals.push_back(points[i].normal.z);

        if (points[i].textureRect && this->texWidth != 0 && this->texHeight != 0) {

            if (!points[i].textureRect->isNormalized()) {
                pointData.textureRects.push_back(points[i].textureRect->getX() / (float) texWidth);
                pointData.textureRects.push_back(points[i].textureRect->getY() / (float) texHeight);
                pointData.textureRects.push_back(points[i].textureRect->getWidth() / (float) texWidth);
                pointData.textureRects.push_back(points[i].textureRect->getHeight() / (float) texHeight);
            }else{
                pointData.textureRects.push_back(points[i].textureRect->getX());
                pointData.textureRects.push_back(points[i].textureRect->getY());
                pointData.textureRects.push_back(points[i].textureRect->getWidth());
                pointData.textureRects.push_back(points[i].textureRect->getHeight());
            }

        }

        if (points[i].visible) {
            float pointSizeScaledVal = points[i].size * pointScale;
            if (pointSizeScaledVal < minPointSize)
                pointSizeScaledVal = minPointSize;
            if (pointSizeScaledVal > maxPointSize)
                pointSizeScaledVal = maxPointSize;
            pointData.sizes.push_back(pointSizeScaledVal);
        }else{
            pointData.sizes.push_back(0);
        }

        pointData.colors.push_back(points[i].color.x);
        pointData.colors.push_back(points[i].color.y);
        pointData.colors.push_back(points[i].color.z);
        pointData.colors.push_back(points[i].color.w);
            

    }
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

    if (render == NULL)
        render = ObjectRender::newInstance();
    
    render->setPrimitiveType(S_PRIMITIVE_POINTS);
    render->setProgramShader(S_SHADER_POINTS);
    render->setDynamicBuffer(true);
    
    render->setTexture(material.getTexture());
    if (scene){
        render->setNumLights((int)scene->getLights()->size());
        render->setNumShadows2D((int)scene->getLightData()->shadowsMap2D.size());
        render->setNumShadowsCube((int)scene->getLightData()->shadowsMapCube.size());

        render->setSceneRender(scene->getSceneRender());
        render->setLightRender(scene->getLightRender());
        render->setFogRender(scene->getFogRender());
        
        render->setShadowsMap2D(scene->getLightData()->shadowsMap2D);
        render->addProperty(S_PROPERTY_NUMSHADOWS2D, S_PROPERTYDATA_INT1, 1, &scene->getLightData()->numShadows2D);
        render->addProperty(S_PROPERTY_DEPTHVPMATRIX, S_PROPERTYDATA_MATRIX4, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowsVPMatrix.front());
        render->addProperty(S_PROPERTY_SHADOWBIAS2D, S_PROPERTYDATA_FLOAT1, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowsBias2D.front());
        render->addProperty(S_PROPERTY_SHADOWCAMERA_NEARFAR2D, S_PROPERTYDATA_FLOAT2, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowsCameraNearFar2D.front());
        render->addProperty(S_PROPERTY_NUMCASCADES2D, S_PROPERTYDATA_INT1, scene->getLightData()->numShadows2D, &scene->getLightData()->shadowNumCascades2D.front());

        render->setShadowsMapCube(scene->getLightData()->shadowsMapCube);
        render->addProperty(S_PROPERTY_SHADOWBIASCUBE, S_PROPERTYDATA_FLOAT1, scene->getLightData()->numShadowsCube, &scene->getLightData()->shadowsBiasCube.front());
        render->addProperty(S_PROPERTY_SHADOWCAMERA_NEARFARCUBE, S_PROPERTYDATA_FLOAT2, scene->getLightData()->numShadowsCube, &scene->getLightData()->shadowsCameraNearFarCube.front());
    }

    if ((material.getTexture()) && (useTextureRects)){
        material.getTexture()->load();
        texWidth = material.getTexture()->getWidth();
        texHeight = material.getTexture()->getHeight();
    }

    updatePointsData();
    
    render->addVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, 3, points.size(), &pointData.positions.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, 3, points.size(), &pointData.normals.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTSIZES, 1, points.size(), &pointData.sizes.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, 4, points.size(), &pointData.colors.front());
    if (useTextureRects)
        render->addVertexAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, 4, points.size(), &pointData.textureRects.front());
    
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
    
    for (int i=0; i < points.size(); i++){
        delete points[i].textureRect;
    }
    
}
