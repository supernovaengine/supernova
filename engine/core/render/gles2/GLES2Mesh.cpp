#include "GLES2Mesh.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "GLES2Scene.h"
#include "platform/Log.h"
#include "GLES2Texture.h"
#include "math/Vector2.h"
#include "math/Angle.h"
#include "PrimitiveMode.h"
#include "Engine.h"
#include "GLES2Submesh.h"
#include <algorithm>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define BUFFER_OFFSET(i) ((void*)(i))

using namespace Supernova;

GLES2Mesh::GLES2Mesh(): MeshRender() {
    lighting = false;
    usageBuffer = GL_STATIC_DRAW;
}

GLES2Mesh::~GLES2Mesh() {
    destroy();
}

void GLES2Mesh::useVerticesBuffer(){
    if (vertexBufferSize == 0){
        vertexBuffer = GLES2Util::createVBO();
    }
    if (vertexBufferSize >= vertices->size()){
        GLES2Util::updateVBO(vertexBuffer, GL_ARRAY_BUFFER, vertices->size() * 3 * sizeof(GLfloat), &vertices->front());
    }else{
        vertexBufferSize = std::max((unsigned int)vertices->size(), minBufferSize);
        GLES2Util::dataVBO(vertexBuffer, GL_ARRAY_BUFFER, vertexBufferSize * 3 * sizeof(GLfloat), &vertices->front(), usageBuffer);
    }
}

void GLES2Mesh::useTexcoordsBuffer(){
    if (uvBufferSize == 0){
        uvBuffer = GLES2Util::createVBO();
    }
    if (uvBufferSize >= texcoords->size()){
        GLES2Util::updateVBO(uvBuffer, GL_ARRAY_BUFFER, texcoords->size() * 2 * sizeof(GLfloat), &texcoords->front());
    }else{
        uvBufferSize = std::max((unsigned int)texcoords->size(), minBufferSize);
        GLES2Util::dataVBO(uvBuffer, GL_ARRAY_BUFFER, uvBufferSize * 2 * sizeof(GLfloat), &texcoords->front(), usageBuffer);
    }
}

void GLES2Mesh::useNormalsBuffer(){
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

void GLES2Mesh::updateVertices(){
    MeshRender::updateVertices();
    useVerticesBuffer();
}

void GLES2Mesh::updateTexcoords(){
    MeshRender::updateTexcoords();
    if (hasTexture)
        useTexcoordsBuffer();
}

void GLES2Mesh::updateNormals(){
    MeshRender::updateNormals();
    if (lighting)
        useNormalsBuffer();
}

void GLES2Mesh::updateIndices(){
    MeshRender::updateIndices();
    for (unsigned int i = 0; i < submeshes->size(); i++){
        GLES2Submesh* submeshRender = (GLES2Submesh*)submeshes->at(i)->getSubmeshRender();
        submeshRender->useIndicesBuffer();
    }
}

bool GLES2Mesh::load() {

    if (!MeshRender::load()){
        return false;
    }

    std::string programName = "mesh_perfragment";
    std::string programDefs = "";
    if (hasTextureCube){
        programDefs += "#define USE_TEXTURECUBE\n";
    }
    if (isSky){
        programDefs += "#define IS_SKY\n";
    }
    if (isText){
        programDefs += "#define IS_TEXT\n";
    }
    if (lighting){
        programDefs += "#define USE_LIGHTING\n";
    }
    if (hasfog){
        programDefs += "#define HAS_FOG\n";
    }
    if (hasTexture){
        programDefs += "#define USE_TEXTURECOORDS\n";
    }
    if (hasTextureRect){
        programDefs += "#define HAS_TEXTURERECT\n";
    }

    gProgram = ProgramManager::useProgram(programName, programDefs);

    light.setProgram((GLES2Program*)gProgram.get());
    fog.setProgram((GLES2Program*)gProgram.get());

    useTexture = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "uUseTexture");
    
    if (isDynamic){
        usageBuffer = GL_DYNAMIC_DRAW;
    }

    vertexBufferSize = 0;
    uvBufferSize = 0;
    normalBufferSize = 0;
    
    useVerticesBuffer();
    aPositionHandle = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Position");

    if (hasTexture) {
        useTexcoordsBuffer();
        aTextureCoordinates = glGetAttribLocation(((GLES2Program *) gProgram.get())->getProgram(), "a_TextureCoordinates");
    }
    
    if (lighting){
        useNormalsBuffer();
        aNormal = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Normal");
    }
    
    if (hasTextureRect){
        u_textureRect = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_textureRect");
    }
    uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
    uColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_Color");

    u_mvpMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mvpMatrix");
    if (lighting){
        light.getUniformLocations();
        uEyePos = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_EyePos");
        u_mMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mMatrix");
        u_nMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_nMatrix");
    }
    
    if (hasfog){
        fog.getUniformLocations();
    }

    GLES2Util::checkGlError("Error on load GLES2");

    return true;
}

bool GLES2Mesh::draw() {

    if (!MeshRender::draw()){
        return false;
    }

    if (isSky) {
       glDepthFunc(GL_LEQUAL);
    }

    glUseProgram(((GLES2Program*)gProgram.get())->getProgram());
    GLES2Util::checkGlError("glUseProgram");

    glUniformMatrix4fv(u_mvpMatrix, 1, GL_FALSE, (GLfloat*)modelViewProjectMatrix);
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

    if (hasTexture){
        attributePos++;
        glEnableVertexAttribArray(attributePos);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        if (aTextureCoordinates == -1) aTextureCoordinates = attributePos;
        glVertexAttribPointer(aTextureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }

    if (lighting) {
        attributePos++;
        glEnableVertexAttribArray(attributePos);
        if (normals) {
            glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
            if (aNormal == -1) aNormal = attributePos;
            glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        }
    }

    GLenum modeGles = GL_TRIANGLES;
    if (primitiveMode == S_TRIANGLES_STRIP){
        modeGles = GL_TRIANGLE_STRIP;
    }
    if (primitiveMode == S_POINTS){
        modeGles = GL_POINTS;
    }

    for (int i = 0; i < submeshes->size(); i++){
        
        GLES2Submesh* submeshRender = (GLES2Submesh*)submeshes->at(i)->getSubmeshRender();
        
        if (hasTextureRect){
            Rect textureRect;
            if (submeshRender->material->getTextureRect())
                textureRect = *submeshRender->material->getTextureRect();
            
            glUniform4f(u_textureRect, textureRect.getX(), textureRect.getY(), textureRect.getWidth(), textureRect.getHeight());
        }
        
        glUniform1i(useTexture, submeshRender->textured);
        
        if (submeshRender->textured){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(((GLES2Texture*)(submeshRender->texture.get()))->getTextureType(), ((GLES2Texture*)(submeshRender->texture.get()))->getTexture());
            glUniform1i(uTextureUnitLocation, 0);
        }else{
            if (Engine::getPlatform() == S_WEB){
                //Fix Chrome warnings of no texture bound
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, GLES2Util::emptyTexture);
                glUniform1i(uTextureUnitLocation, 0);
            }
        }
        glUniform4fv(uColor, 1, submeshRender->material->getColor()->ptr());
        
        
        if (submeshRender->indicesSizes > 0){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submeshRender->indexBuffer);
            glDrawElements(modeGles, submeshRender->indicesSizes, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
        }else{
            glDrawArrays(modeGles, 0, (int)vertices->size());
        }
    }
    
    
    for (int i = 0; i <= attributePos; i++)
        glDisableVertexAttribArray(i);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLES2Util::checkGlError("Error on draw GLES2");

    return true;
}

void GLES2Mesh::destroy(){

    glDeleteBuffers(1, &vertexBuffer);
    if (texcoords){
        glDeleteBuffers(1, &uvBuffer);
    }
    if (lighting && normals){
        glDeleteBuffers(1, &normalBuffer);
    }
    gProgram.reset();
    ProgramManager::deleteUnused();
    
    MeshRender::destroy();
}
