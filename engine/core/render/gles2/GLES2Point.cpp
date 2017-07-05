
#include "GLES2Point.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "GLES2Scene.h"
#include "platform/Log.h"
#include "GLES2Texture.h"
#include "math/Vector2.h"
#include "math/Angle.h"
#include "PrimitiveMode.h"
#include "Engine.h"
#include <algorithm>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define BUFFER_OFFSET(i) ((void*)(i))

using namespace Supernova;

GLES2Point::GLES2Point(): PointRender() {
    usageBuffer = GL_DYNAMIC_DRAW;
}

GLES2Point::~GLES2Point() {
    destroy();
}

std::vector<float> GLES2Point::rectsData(){
    std::vector<float> rects;
    for (int i = 0; i < textureRects->size(); i++){

        Rect textureRect;
        if (textureRects->at(i))
            textureRect = *textureRects->at(i);

        rects.push_back(textureRect.getX());
        rects.push_back(textureRect.getY());
        rects.push_back(textureRect.getWidth());
        rects.push_back(textureRect.getHeight());
    }
    return rects;
}

void GLES2Point::useVerticesBuffer(){
    if (vertexBufferSize == 0){
        vertexBuffer = GLES2Util::createVBO();
    }
    if (vertexBufferSize >= positions->size()){
        GLES2Util::updateVBO(vertexBuffer, GL_ARRAY_BUFFER, positions->size() * 3 * sizeof(GLfloat), &positions->front());
    }else{
        vertexBufferSize = std::max((unsigned int)positions->size(), minBufferSize);
        GLES2Util::dataVBO(vertexBuffer, GL_ARRAY_BUFFER, vertexBufferSize * 3 * sizeof(GLfloat), &positions->front(), usageBuffer);
    }
}

void GLES2Point::useNormalsBuffer(){
    if (normalBufferSize == 0){
        normalBuffer = GLES2Util::createVBO();
    }
    if (normalBufferSize >= normals->size()){
        GLES2Util::updateVBO(normalBuffer, GL_ARRAY_BUFFER, normals->size() * 3 * sizeof(GLfloat), &normals->front());
    }else{
        normalBufferSize = std::max((unsigned int)normals->size(), minBufferSize);
        GLES2Util::dataVBO(normalBuffer, GL_ARRAY_BUFFER, normalBufferSize * 3 * sizeof(GLfloat), &normals->front(), usageBuffer);
    }
}

void GLES2Point::usePointSizesBuffer(){
    if (pointSizeBufferSize == 0){
        pointSizeBuffer = GLES2Util::createVBO();
    }
    if (pointSizeBufferSize >= pointSizes->size()){
        GLES2Util::updateVBO(pointSizeBuffer, GL_ARRAY_BUFFER, pointSizes->size() * sizeof(GLfloat), &pointSizes->front());
    }else{
        pointSizeBufferSize = std::max((unsigned int)pointSizes->size(), minBufferSize);
        GLES2Util::dataVBO(pointSizeBuffer, GL_ARRAY_BUFFER, pointSizeBufferSize * sizeof(GLfloat), &pointSizes->front(), usageBuffer);
    }
}

void GLES2Point::useTextureRectsBuffer(){
    if (textureRectBufferSize == 0){
        textureRectBuffer = GLES2Util::createVBO();
    }
    if (textureRectBufferSize >= textureRects->size()){
        GLES2Util::updateVBO(textureRectBuffer, GL_ARRAY_BUFFER, textureRects->size() * 4 * sizeof(GLfloat), &rectsData().front());
    }else{
        textureRectBufferSize = std::max((unsigned int)textureRects->size(), minBufferSize);
        GLES2Util::dataVBO(textureRectBuffer, GL_ARRAY_BUFFER, textureRectBufferSize * 4 * sizeof(GLfloat), &rectsData().front(), usageBuffer);
    }
}

void GLES2Point::usePointColorsBuffer(){
    if (pointColorBufferSize == 0){
        pointColorBuffer = GLES2Util::createVBO();
    }
    if (pointColorBufferSize >= pointColors->size()){
        GLES2Util::updateVBO(pointColorBuffer, GL_ARRAY_BUFFER, pointColors->size() * 4 * sizeof(GLfloat), &pointColors->front());
    }else{
        pointColorBufferSize = std::max((unsigned int)pointColors->size(), minBufferSize);
        GLES2Util::dataVBO(pointColorBuffer, GL_ARRAY_BUFFER, pointColorBufferSize * 4 * sizeof(GLfloat), &pointColors->front(), usageBuffer);
    }
}

void GLES2Point::updatePositions(){
    PointRender::updatePositions();
    useVerticesBuffer();
}

void GLES2Point::updateNormals(){
    PointRender::updateNormals();
    if (lighting)
        useNormalsBuffer();
}

void GLES2Point::updatePointSizes(){
    PointRender::updatePointSizes();
    usePointSizesBuffer();
}

void GLES2Point::updateTextureRects(){
    PointRender::updateTextureRects();
    if (hasTextureRect)
        useTextureRectsBuffer();
}

void GLES2Point::updatePointColors(){
    PointRender::updatePointColors();
    usePointColorsBuffer();
}

bool GLES2Point::load() {
    
    if (!PointRender::load()){
        return false;
    }
    
    std::string programName = "points_perfragment";
    std::string programDefs = "";
    if (lighting){
        programDefs += "#define USE_LIGHTING\n";
    }
    if (hasfog){
        programDefs += "#define HAS_FOG\n";
    }
    if (hasTextureRect){
        programDefs += "#define HAS_TEXTURERECT\n";
    }
    
    gProgram = ProgramManager::useProgram(programName, programDefs);
    GLuint glesProgram = ((GLES2Program*)gProgram.getProgramRender().get())->getProgram();

    light.setProgram((GLES2Program*)gProgram.getProgramRender().get());
    fog.setProgram((GLES2Program*)gProgram.getProgramRender().get());
    
    useTexture = glGetUniformLocation(glesProgram, "uUseTexture");
    
    vertexBufferSize = 0;
    normalBufferSize = 0;
    pointSizeBufferSize = 0;
    textureRectBufferSize = 0;
    pointColorBufferSize = 0;
    
    useVerticesBuffer();
    aPositionHandle = glGetAttribLocation(glesProgram, "a_Position");
    
    if (lighting){
        useNormalsBuffer();
        aNormal = glGetAttribLocation(glesProgram, "a_Normal");
    }
    
    usePointSizesBuffer();
    a_PointSize = glGetAttribLocation(glesProgram, "a_PointSize");
    
    if (hasTextureRect){
        useTextureRectsBuffer();
        a_textureRect = glGetAttribLocation(glesProgram, "a_textureRect");
    }

    usePointColorsBuffer();
    a_pointColor = glGetAttribLocation(glesProgram, "a_pointColor");

    if (textured){
        texture = TextureManager::loadTexture(materialTexture);
        uTextureUnitLocation = glGetUniformLocation(glesProgram, "u_TextureUnit");
    }else{
        if (Engine::getPlatform() == S_WEB){
            GLES2Util::generateEmptyTexture();
            uTextureUnitLocation = glGetUniformLocation(glesProgram, "u_TextureUnit");
        }
        texture.setTextureRender(NULL);
    }
    
    u_mvpMatrix = glGetUniformLocation(glesProgram, "u_mvpMatrix");
    if (lighting){
        light.getUniformLocations();
        uEyePos = glGetUniformLocation(glesProgram, "u_EyePos");
        u_mMatrix = glGetUniformLocation(glesProgram, "u_mMatrix");
        u_nMatrix = glGetUniformLocation(glesProgram, "u_nMatrix");
    }
    
    if (hasfog){
        fog.getUniformLocations();
    }

    
    GLES2Util::checkGlError("Error on load GLES2");
    
    return true;
}

bool GLES2Point::draw() {

    if (!PointRender::draw()){
        return false;
    }
    GLuint glesProgram = ((GLES2Program*)gProgram.getProgramRender().get())->getProgram();
    glUseProgram(glesProgram);
    GLES2Util::checkGlError("glUseProgram");
    
    glUniformMatrix4fv(u_mvpMatrix, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix);
    if (lighting){
        light.setUniformValues(sceneRender);
        glUniform3fv(uEyePos, 1, cameraPosition.ptr());
        glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, (GLfloat*)modelMatrix);
        glUniformMatrix4fv(u_nMatrix, 1, GL_FALSE, (GLfloat*)normalMatrix);
    }
    
    if (hasfog){
        fog.setUniformValues(sceneRender);
    }
    
    int attributePos = -1;
    
    attributePos++;
    glEnableVertexAttribArray(attributePos);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    if (aPositionHandle == -1) aPositionHandle = attributePos;
    glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));
    
    if (lighting) {
        attributePos++;
        glEnableVertexAttribArray(attributePos);
        if (normals) {
            glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
            if (aNormal == -1) aNormal = attributePos;
            glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        }
    }
    
    if (hasTextureRect){
        attributePos++;
        glEnableVertexAttribArray(attributePos);
        if (textureRects) {
            glBindBuffer(GL_ARRAY_BUFFER, textureRectBuffer);
            if (a_textureRect == -1) a_textureRect = attributePos;
            glVertexAttribPointer(a_textureRect, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        }
    }

    attributePos++;
    glEnableVertexAttribArray(attributePos);
    if (pointSizes) {
        glBindBuffer(GL_ARRAY_BUFFER, pointSizeBuffer);
        if (a_PointSize == -1) a_PointSize = attributePos;
        glVertexAttribPointer(a_PointSize, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }
        
    attributePos++;
    glEnableVertexAttribArray(attributePos);
    if (pointColors) {
        glBindBuffer(GL_ARRAY_BUFFER, pointColorBuffer);
        if (a_pointColor == -1) a_pointColor = attributePos;
        glVertexAttribPointer(a_pointColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }
        
    glUniform1i(useTexture, textured);
        
    if (textured){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(((GLES2Texture*)(texture.getTextureRender().get()))->getTextureType(),
                      ((GLES2Texture*)(texture.getTextureRender().get()))->getTexture());
        glUniform1i(uTextureUnitLocation, 0);
    }else{
        if (Engine::getPlatform() == S_WEB){
                //Fix Chrome warnings of no texture bound
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, GLES2Util::emptyTexture);
            glUniform1i(uTextureUnitLocation, 0);
        }
    }
        
    glDrawArrays(GL_POINTS, 0, numPoints);

    
    for (int i = 0; i <= attributePos; i++)
        glDisableVertexAttribArray(i);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    GLES2Util::checkGlError("Error on draw GLES2");
    
    return true;
}

void GLES2Point::destroy(){
    glDeleteBuffers(1, &vertexBuffer);
    if (lighting && normals){
        glDeleteBuffers(1, &normalBuffer);
    }
    if (hasTextureRect && textureRects){
        glDeleteBuffers(1, &textureRectBuffer);
    }
    if (pointSizes)
        glDeleteBuffers(1, &pointSizeBuffer);
    if (pointColors)
        glDeleteBuffers(1, &pointColorBuffer);

    if (textured){
        texture.destroy();
    }

    gProgram.destroy();
    
    PointRender::destroy();
}
