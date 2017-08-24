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
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, points.size(), positionsData);
}

void Points::updateNormals(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_NORMALS,points.size(), normalsData);
}

void Points::updatePointColors(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, points.size(), colorsData);
}

void Points::updatePointSizes(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_POINTSIZES, points.size(), sizesData);
}

void Points::updateTextureRects(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, points.size(), textureRectsData);
}

void Points::addPoint(){

    points.push_back({Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), NULL, 1, *material.getColor(), -1});

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

        points[point].position = position;

        if (loaded){
            fillPointsData();
            updatePositions();
            updateNormals();
            deletePointsData();
        }
    }
}

void Points::setPointPosition(int point, float x, float y, float z){
    setPointPosition(point, Vector3(x, y, z));
}

void Points::setPointSize(int point, float size){
    if (point >= 0 && point < points.size()){

        points[point].size = size;

        if (loaded){
            fillPointsData();
            updatePointSizes();
            deletePointsData();
        }
    }
}

void Points::setPointColor(int point, Vector4 color){
    if (point >= 0 && point < points.size()) {

        points[point].color = color;

        if (loaded){
            fillPointsData();
            updatePointColors();
            deletePointsData();
        }
    }
}

void Points::setPointColor(int point, float red, float green, float blue, float alpha){
    setPointColor(point, Vector4(red, green, blue, alpha));
}

void Points::setPointSprite(int point, int index){
    if (points[point].textureRect){
        points[point].textureRect->setRect(&framesRect[index].rect);
    }else {
        points[point].textureRect = new Rect(framesRect[index].rect);
    }

    if (!useTextureRects){
        useTextureRects = true;

        if (loaded)
            reload();
    }else{
        useTextureRects = true;

        if (loaded){
            fillPointsData();
            updateTextureRects();
            deletePointsData();
        }
    }
}

void Points::setPointSprite(int point, std::string id){
    std::vector<int> frameslist = findFramesByString(id);
    if (frameslist.size() > 0){
        setPointSprite(point, frameslist[0]);
    }
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

    if (loaded){
        fillPointsData();
        updatePointSizes();
        deletePointsData();
    }
}

void Points::sortTransparentPoints(){
    if (transparent && scene && scene->isUseDepth()){
        
        bool needSort = false;
        for (int i=0; i < points.size(); i++){
            if (this->cameraPosition != NULL){
                points[i].distanceToCamera = ((*this->cameraPosition) - (modelMatrix * points[i].position)).length();
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
                fillPointsData();
                updatePositions();
                updatePointColors();
                updatePointSizes();
                updateTextureRects();
                deletePointsData();
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

void Points::fillPointsData(){
    positionsData = new float[points.size() * 3];
    normalsData = new float[points.size() * 3];
    textureRectsData = new float[points.size() * 4];
    sizesData = new float[points.size() * 1];
    colorsData = new float[points.size() * 4];

    for (int i=0; i < points.size(); i++){

        positionsData[3*i + 0] = points[i].position.x;
        positionsData[3*i + 1] = points[i].position.y;
        positionsData[3*i + 2] = points[i].position.z;

        normalsData[3*i + 0] = points[i].normal.x;
        normalsData[3*i + 1] = points[i].normal.y;
        normalsData[3*i + 2] = points[i].normal.z;

        if (points[i].textureRect && this->texWidth != 0 && this->texHeight != 0) {

            if (!points[i].textureRect->isNormalized()) {
                textureRectsData[4 * i + 0] = points[i].textureRect->getX() / (float) texWidth;
                textureRectsData[4 * i + 1] = points[i].textureRect->getY() / (float) texHeight;
                textureRectsData[4 * i + 2] = points[i].textureRect->getWidth() / (float) texWidth;
                textureRectsData[4 * i + 3] = points[i].textureRect->getHeight() / (float) texHeight;
            }else{
                textureRectsData[4 * i + 0] = points[i].textureRect->getX();
                textureRectsData[4 * i + 1] = points[i].textureRect->getY();
                textureRectsData[4 * i + 2] = points[i].textureRect->getWidth();
                textureRectsData[4 * i + 3] = points[i].textureRect->getHeight();
            }

        }

        float pointSizeScaledVal = points[i].size * pointScale;
        if (pointSizeScaledVal < minPointSize)
            pointSizeScaledVal = minPointSize;
        if (pointSizeScaledVal > maxPointSize)
            pointSizeScaledVal = maxPointSize;
        sizesData[i] = pointSizeScaledVal;

        colorsData[4*i + 0] = points[i].color.x;
        colorsData[4*i + 1] = points[i].color.y;
        colorsData[4*i + 2] = points[i].color.z;
        colorsData[4*i + 3] = points[i].color.w;
    }
}

void Points::deletePointsData(){
    if (points.size() > 0) {
        delete[] positionsData;
        delete[] normalsData;
        delete[] textureRectsData;
        delete[] sizesData;
        delete[] colorsData;
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
        render->setSceneRender(scene->getSceneRender());
        render->setLightRender(scene->getLightRender());
        render->setFogRender(scene->getFogRender());
    }

    if ((material.getTexture()) && (useTextureRects)){
        material.getTexture()->load();
        texWidth = material.getTexture()->getWidth();
        texHeight = material.getTexture()->getHeight();
    }

    fillPointsData();
    
    render->addVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, 3, points.size(), positionsData);
    render->addVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, 3, points.size(), normalsData);
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTSIZES, 1, points.size(), sizesData);
    render->addVertexAttribute(S_VERTEXATTRIBUTE_POINTCOLORS, 4, points.size(), colorsData);
    if (useTextureRects)
        render->addVertexAttribute(S_VERTEXATTRIBUTE_TEXTURERECTS, 4, points.size(), textureRectsData);
    
    render->addProperty(S_PROPERTY_MODELMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelMatrix);
    render->addProperty(S_PROPERTY_NORMALMATRIX, S_PROPERTYDATA_MATRIX4, 1, &normalMatrix);
    render->addProperty(S_PROPERTY_MVPMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelViewProjectionMatrix);
    render->addProperty(S_PROPERTY_CAMERAPOS, S_PROPERTYDATA_FLOAT3, 1, &cameraPosition);
    
    bool renderloaded = render->load();
    
    deletePointsData();
    
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
