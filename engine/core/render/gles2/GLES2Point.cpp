
#include "GLES2Point.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "GLES2Scene.h"
#include "platform/Log.h"
#include "GLES2Texture.h"
#include "math/Vector2.h"
#include "math/Angle.h"
#include "PrimitiveMode.h"
#include "Supernova.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

GLES2Point::GLES2Point(): PointRender() {
}

GLES2Point::~GLES2Point() {
    destroy();
}


void GLES2Point::updatePositions(){
    if (loaded)
        GLES2Util::updateVBO(vertexBuffer, GL_ARRAY_BUFFER, positions->size() * 3 * sizeof(GLfloat), &positions->front());
}

void GLES2Point::updateNormals(){
    if (loaded)
        if (normals)
            GLES2Util::updateVBO(normalBuffer, GL_ARRAY_BUFFER, normals->size() * 3 * sizeof(GLfloat), &normals->front());
}

void GLES2Point::updatePointSizes(){
    if (loaded)
        if (pointSizes)
            GLES2Util::updateVBO(pointSizeBuffer, GL_ARRAY_BUFFER, pointSizes->size() * sizeof(GLfloat), &pointSizes->front());
}

void GLES2Point::updateSlicesPos(){
    if (loaded){
        if (isSlicedTexture && slicesPos && textured){
            std::vector<float> slicesPosFloat;
            std::transform(slicesPos->begin(), slicesPos->end(), std::back_inserter(slicesPosFloat),
                           [&slicesPosFloat](const std::pair<int, int> &p)
                           { slicesPosFloat.push_back(p.first); return p.second ;});
            
            GLES2Util::updateVBO(slicePosBuffer, GL_ARRAY_BUFFER, slicesPosFloat.size() * sizeof(GLfloat), &slicesPosFloat.front());
        }
    }
}

void GLES2Point::updatePointColors(){
    if (loaded)
        if (pointColors)
            GLES2Util::updateVBO(pointColorBuffer, GL_ARRAY_BUFFER, pointColors->size() * 4 * sizeof(GLfloat), &pointColors->front());
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
    if (isSlicedTexture){
        programDefs += "#define IS_SLICEDTEXTURE\n";
    }
    
    gProgram = ProgramManager::useProgram(programName, programDefs);

    light.setProgram((GLES2Program*)gProgram.get());
    fog.setProgram((GLES2Program*)gProgram.get());
    
    useTexture = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "uUseTexture");
    
    vertexBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, positions->size() * 3 * sizeof(GLfloat), &positions->front(), GL_DYNAMIC_DRAW);
    aPositionHandle = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Position");
    
    if (normals && lighting){
        normalBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, normals->size() * 3 * sizeof(GLfloat), &normals->front(), GL_STATIC_DRAW);
        aNormal = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Normal");
    }
    
    if (isSlicedTexture){
        std::vector<float> slicesPosFloat;
        std::transform(slicesPos->begin(), slicesPos->end(), std::back_inserter(slicesPosFloat),
                       [&slicesPosFloat](const std::pair<int, int> &p)
                       { slicesPosFloat.push_back(p.first); return p.second ;});
        
        slicePosBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, slicesPosFloat.size() * sizeof(GLfloat), &slicesPosFloat.front(), GL_DYNAMIC_DRAW);
        a_slicePos = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_slicePos");
    }

    pointSizeBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, pointSizes->size() * sizeof(GLfloat), &pointSizes->front(), GL_DYNAMIC_DRAW);
    a_PointSize = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_PointSize");
        
    pointColorBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, pointColors->size() * 4 * sizeof(GLfloat), &pointColors->front(), GL_DYNAMIC_DRAW);
    a_pointColor = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_pointColor");

    if (textured){
        texture = TextureManager::loadTexture(material->getTextures()[0]);
        uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
    }else{
        texture = NULL;
        if (Supernova::getPlatform() == S_WEB){
            GLES2Util::generateEmptyTexture();
            uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
        }
    }
    
    u_mvpMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mvpMatrix");
    if (lighting){
        light.getUniformLocations();
        uEyePos = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_EyePos");
        u_mMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mMatrix");
        u_nMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_nMatrix");
    }
    
    if (isSlicedTexture) {
        u_sliceSize = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_sliceSize");
        u_textureSize = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_textureSize");
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
    
    if (!loaded){
        return false;
    }
    
    glUseProgram(((GLES2Program*)gProgram.get())->getProgram());
    GLES2Util::checkGlError("glUseProgram");
    
    glUniformMatrix4fv(u_mvpMatrix, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix);
    if (lighting){
        light.setUniformValues(sceneRender);
        glUniform3fv(uEyePos, 1, cameraPosition->ptr());
        glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, (GLfloat*)modelMatrix);
        glUniformMatrix4fv(u_nMatrix, 1, GL_FALSE, (GLfloat*)normalMatrix);
    }
    
    if (isSlicedTexture) {
        glUniform2f(u_sliceSize, sliceSizeWidth, sliceSizeHeight);
        glUniform2f(u_textureSize, textureSizeWidth, textureSizeHeight);
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
    
    if (isSlicedTexture) {
        attributePos++;
        glEnableVertexAttribArray(attributePos);
        if (slicesPos) {
            glBindBuffer(GL_ARRAY_BUFFER, slicePosBuffer);
            if (a_slicePos == -1) a_slicePos = attributePos;
            glVertexAttribPointer(a_slicePos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
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
        glBindTexture(((GLES2Texture*)(texture.get()))->getTextureType(), ((GLES2Texture*)(texture.get()))->getTexture());
        glUniform1i(uTextureUnitLocation, 0);
    }else{
        if (Supernova::getPlatform() == S_WEB){
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
    if (loaded){
        glDeleteBuffers(1, &vertexBuffer);
        if (lighting && normals){
            glDeleteBuffers(1, &normalBuffer);
        }
        if (isSlicedTexture && slicesPos){
            glDeleteBuffers(1, &slicePosBuffer);
        }

        if (pointSizes)
            glDeleteBuffers(1, &pointSizeBuffer);
        if (pointColors)
            glDeleteBuffers(1, &pointColorBuffer);

        if (textured){
            texture.reset();
            TextureManager::deleteUnused();
        }

        gProgram.reset();
        ProgramManager::deleteUnused();
    }
    loaded = false;
}
