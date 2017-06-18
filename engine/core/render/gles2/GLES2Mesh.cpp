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
        vertexBufferSize = (unsigned int)vertices->size();
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
        uvBufferSize = (unsigned int)texcoords->size();
        GLES2Util::dataVBO(uvBuffer, GL_ARRAY_BUFFER, uvBufferSize * 2 * sizeof(GLfloat), &texcoords->front(), usageBuffer);
    }
}

void GLES2Mesh::useNormalsBuffer(){
    if (normals){
    if (normalBufferSize == 0){
        normalBuffer = GLES2Util::createVBO();
    }
    if (normalBufferSize >= normals->size()){
        GLES2Util::updateVBO(normalBuffer, GL_ARRAY_BUFFER, normals->size() * 3 * sizeof(GLfloat), &normals->front());
    }else{
        normalBufferSize = (unsigned int)normals->size();
        GLES2Util::dataVBO(normalBuffer, GL_ARRAY_BUFFER, normalBufferSize * 3 * sizeof(GLfloat), &normals->front(), usageBuffer);
    }
    }
}

void GLES2Mesh::useIndicesBuffer(){
    for (unsigned int i = 0; i < submeshes->size(); i++){
        if (submeshesRender[submeshes->at(i)].indicesSizes > 0){
            
            std::vector<unsigned int>* gIndices = submeshes->at(i)->getIndices();
            
            if (submeshesIndices[submeshes->at(i)].indexBufferSize == 0){
                submeshesIndices[submeshes->at(i)].indexBuffer = GLES2Util::createVBO();
            }
            if (submeshesIndices[submeshes->at(i)].indexBufferSize >= gIndices->size()){
                GLES2Util::updateVBO(submeshesIndices[submeshes->at(i)].indexBuffer,GL_ELEMENT_ARRAY_BUFFER,
                                     gIndices->size() * sizeof(unsigned int), &gIndices->front());
            }else{
                submeshesIndices[submeshes->at(i)].indexBufferSize = (unsigned int)gIndices->size();
                GLES2Util::dataVBO(submeshesIndices[submeshes->at(i)].indexBuffer, GL_ELEMENT_ARRAY_BUFFER,
                                   submeshesIndices[submeshes->at(i)].indexBufferSize * sizeof(unsigned int), &gIndices->front(), usageBuffer);
                
            }
        }
    }
}

void GLES2Mesh::updateVertices(){
    MeshRender::updateVertices();
    if (isLoaded)
        useVerticesBuffer();
}

void GLES2Mesh::updateTexcoords(){
    MeshRender::updateTexcoords();
    if (isLoaded)
        useTexcoordsBuffer();
}

void GLES2Mesh::updateNormals(){
    MeshRender::updateNormals();
    if (isLoaded)
        if (lighting)
            useNormalsBuffer();
}

void GLES2Mesh::updateIndices(){
    MeshRender::updateIndices();
    if (isLoaded)
        useIndicesBuffer();
}

bool GLES2Mesh::load() {

    if (!MeshRender::load()){
        return false;
    }

    std::string programName = "mesh_perfragment";
    std::string programDefs = "";
    if (submeshes){
        if (submeshes->at(0)->getMaterial()->getTextureType() == S_TEXTURE_CUBE){
            programDefs += "#define USE_TEXTURECUBE\n";
        }
    }
    if (isSky){
        programDefs += "#define IS_SKY\n";
    }
    if (lighting){
        programDefs += "#define USE_LIGHTING\n";
    }
    if (hasfog){
        programDefs += "#define HAS_FOG\n";
    }
    if (texcoords){
        programDefs += "#define USE_TEXTURECOORDS\n"; //TODO!
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
    
    useTexcoordsBuffer();
    aTextureCoordinatesLocation = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_TextureCoordinates");
    
    if (lighting){
        useNormalsBuffer();
        aNormal = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Normal");
    }
    
    if (hasTextureRect){
        u_textureRect = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_textureRect");
    }
    uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
    uColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_Color");
    
    submeshesIndices.clear();
    for (unsigned int i = 0; i < submeshes->size(); i++){
        if (submeshesRender[submeshes->at(i)].indicesSizes > 0){
            submeshesIndices[submeshes->at(i)].indexBufferSize = 0;
        }
        
        if (submeshesRender[submeshes->at(i)].textured){
            if (submeshes->at(i)->getMaterial()->getTextureType() == S_TEXTURE_CUBE){
                std::vector<std::string> textures;
                std::string id = "cube|";
                for (int t = 0; t < submeshes->at(i)->getMaterial()->getTextures().size(); t++){
                    textures.push_back(submeshes->at(i)->getMaterial()->getTextures()[t]);
                    id = id + "|" + textures.back();
                }
                submeshesRender[submeshes->at(i)].texture = TextureManager::loadTextureCube(textures, id);
            }else{
                submeshesRender[submeshes->at(i)].texture = TextureManager::loadTexture(submeshes->at(i)->getMaterial()->getTextures()[0]);
            }
        }else{
            //Fix Chrome warnings of no texture bound with an empty texture
            if (Engine::getPlatform() == S_WEB){
                GLES2Util::generateEmptyTexture();
            }
            submeshesRender[submeshes->at(i)].texture = NULL;
        }
    }
    useIndicesBuffer();

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

    if (!isLoaded){
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

    if (texcoords){
        attributePos++;
        glEnableVertexAttribArray(attributePos);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        if (aTextureCoordinatesLocation == -1) aTextureCoordinatesLocation = attributePos;
        glVertexAttribPointer(aTextureCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
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
        
        if (hasTextureRect){
            Rect textureRect;
            if (submeshes->at(i)->getMaterial()->getTextureRect())
                textureRect = *submeshes->at(i)->getMaterial()->getTextureRect();
        
            glUniform4f(u_textureRect, textureRect.getX(), textureRect.getY(), textureRect.getWidth(), textureRect.getHeight());
        }

        glUniform1i(useTexture, submeshesRender[submeshes->at(i)].textured);

        if (submeshesRender[submeshes->at(i)].textured){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(((GLES2Texture*)(submeshesRender[submeshes->at(i)].texture.get()))->getTextureType(), ((GLES2Texture*)(submeshesRender[submeshes->at(i)].texture.get()))->getTexture());
            glUniform1i(uTextureUnitLocation, 0);
        }else{
            if (Engine::getPlatform() == S_WEB){
                //Fix Chrome warnings of no texture bound
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, GLES2Util::emptyTexture);
                glUniform1i(uTextureUnitLocation, 0);
            }
        }
        glUniform4fv(uColor, 1, submeshes->at(i)->getMaterial()->getColor()->ptr());


        if (submeshesRender[submeshes->at(i)].indicesSizes > 0){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submeshesIndices[submeshes->at(i)].indexBuffer);
            glDrawElements(modeGles, submeshesRender[submeshes->at(i)].indicesSizes, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
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
    
    if (isLoaded){
        glDeleteBuffers(1, &vertexBuffer);
        if (texcoords){
            glDeleteBuffers(1, &uvBuffer);
        }
        if (lighting && normals){
            glDeleteBuffers(1, &normalBuffer);
        }
        if (submeshes){
            for (unsigned int i = 0; i < submeshes->size(); i++){
                if (submeshesRender[submeshes->at(i)].indicesSizes > 0)
                    glDeleteBuffers(1, &submeshesIndices[submeshes->at(i)].indexBuffer);

                if (submeshesRender[submeshes->at(i)].textured){
                    submeshesRender[submeshes->at(i)].texture.reset();
                    TextureManager::deleteUnused();
                }
            }
        }
        gProgram.reset();
        ProgramManager::deleteUnused();
    }
    
    MeshRender::destroy();
}
