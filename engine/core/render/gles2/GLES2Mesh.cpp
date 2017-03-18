#include "GLES2Mesh.h"

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

GLES2Mesh::GLES2Mesh(): MeshRender() {
    lighting = false;
}

GLES2Mesh::~GLES2Mesh() {
    destroy();
}


void GLES2Mesh::updatePositions(){
    if (loaded)
        GLES2Util::updateVBO(vertexBuffer, GL_ARRAY_BUFFER, positions->size() * 3 * sizeof(GLfloat), &positions->front());
}

void GLES2Mesh::updateTexcoords(){
    if (loaded)
        if (texcoords)
            GLES2Util::updateVBO(uvBuffer, GL_ARRAY_BUFFER, texcoords->size() * 2 * sizeof(GLfloat), &texcoords->front());
}

void GLES2Mesh::updateNormals(){
    if (loaded)
        if (normals)
            GLES2Util::updateVBO(normalBuffer, GL_ARRAY_BUFFER, normals->size() * 3 * sizeof(GLfloat), &normals->front());
}

bool GLES2Mesh::load() {

    if (!MeshRender::load()){
        return false;
    }

    std::string programName = "mesh_perfragment";
    std::string programDefs = "";
    if (submeshes){
        if ((*submeshes)[0]->getMaterial()->getTextureType() == S_TEXTURE_CUBE){
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
    if (isSpriteSheet){
        programDefs += "#define IS_SPRITESHEET\n";
    }
    if (texcoords){
        programDefs += "#define USE_TEXTURECOORDS\n";
    }

    gProgram = ProgramManager::useProgram(programName, programDefs);

    light.setProgram((GLES2Program*)gProgram.get());
    fog.setProgram((GLES2Program*)gProgram.get());

    useTexture = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "uUseTexture");

    vertexBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, positions->size() * 3 * sizeof(GLfloat), &positions->front(), GL_STATIC_DRAW);
    aPositionHandle = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Position");
    
    if (texcoords){
        uvBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, texcoords->size() * 2 * sizeof(GLfloat), &texcoords->front(), GL_STATIC_DRAW);
        aTextureCoordinatesLocation = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_TextureCoordinates");
    }
    
    if (normals && lighting){
        normalBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, normals->size() * 3 * sizeof(GLfloat), &normals->front(), GL_STATIC_DRAW);
        aNormal = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Normal");
    }
    
    if (isSpriteSheet){

    }

    for (unsigned int i = 0; i < submeshes->size(); i++){
        if (submeshesGles[(*submeshes)[i]].indicesSizes > 0){
            std::vector<unsigned int> gIndices = *(*submeshes)[i]->getIndices();
            submeshesIndices[(*submeshes)[i]].indiceBuffer = GLES2Util::createVBO(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(unsigned int), &gIndices.front(), GL_STATIC_DRAW);
        }

        if (submeshesGles[(*submeshes)[i]].textured){
            if ((*submeshes)[i]->getMaterial()->getTextureType() == S_TEXTURE_CUBE){
                std::vector<std::string> textures;
                std::string id = "cube|";
                for (int t = 0; t < (*submeshes)[i]->getMaterial()->getTextures().size(); t++){
                    textures.push_back((*submeshes)[i]->getMaterial()->getTextures()[t]);
                    id = id + "|" + textures.back();
                }
                submeshesGles[(*submeshes)[i]].texture = TextureManager::loadTextureCube(textures, id);
            }else{
                submeshesGles[(*submeshes)[i]].texture = TextureManager::loadTexture((*submeshes)[i]->getMaterial()->getTextures()[0]);
            }
            uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
        }else{
            //Fix Chrome warnings of no texture bound with an empty texture
            if (Supernova::getPlatform() == S_WEB){
                GLES2Util::generateEmptyTexture();
                uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
            }
            submeshesGles[(*submeshes)[i]].texture = NULL;
            uColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_Color");
        }
    }

    u_mvpMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mvpMatrix");
    if (lighting){
        light.getUniformLocations();
        uEyePos = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_EyePos");
        u_mMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mMatrix");
        u_nMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_nMatrix");
    }
    
    if (isSpriteSheet) {
        u_spriteSize = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_spriteSize");
        u_textureSize = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_textureSize");
        u_spritePos = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_spritePos");
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

    if (!loaded){
        return false;
    }

    if (isSky) {
       glDepthFunc(GL_LEQUAL);
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
    
    if (isSpriteSheet) {
        glUniform2f(u_spriteSize, spriteSizeWidth, spriteSizeHeight);
        glUniform2f(u_textureSize, textureSizeWidth, textureSizeHeight);
        glUniform2f(u_spritePos, spritePos->first, spritePos->second);
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

        glUniform1i(useTexture, submeshesGles[(*submeshes)[i]].textured);

        if (submeshesGles[(*submeshes)[i]].textured){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(((GLES2Texture*)(submeshesGles[(*submeshes)[i]].texture.get()))->getTextureType(), ((GLES2Texture*)(submeshesGles[(*submeshes)[i]].texture.get()))->getTexture());
            glUniform1i(uTextureUnitLocation, 0);
        }else{
            glUniform4fv(uColor, 1, (*submeshes)[i]->getMaterial()->getColor()->ptr());
            if (Supernova::getPlatform() == S_WEB){
                //Fix Chrome warnings of no texture bound
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, GLES2Util::emptyTexture);
                glUniform1i(uTextureUnitLocation, 0);
            }
        }

        if (submeshesGles[(*submeshes)[i]].indicesSizes > 0){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submeshesIndices[(*submeshes)[i]].indiceBuffer);
            glDrawElements(modeGles, submeshesGles[(*submeshes)[i]].indicesSizes, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
        }else{
            glDrawArrays(modeGles, 0, (int)(*positions).size());
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
    if (loaded){
        glDeleteBuffers(1, &vertexBuffer);
        if (texcoords){
            glDeleteBuffers(1, &uvBuffer);
        }
        if (lighting && normals){
            glDeleteBuffers(1, &normalBuffer);
        }
        if (submeshes){
            for (unsigned int i = 0; i < submeshes->size(); i++){
                if (submeshesGles[(*submeshes)[i]].indicesSizes > 0)
                    glDeleteBuffers(1, &submeshesIndices[(*submeshes)[i]].indiceBuffer);

                if (submeshesGles[(*submeshes)[i]].textured){
                    submeshesGles[(*submeshes)[i]].texture.reset();
                    TextureManager::deleteUnused();
                }
            }
        }
        gProgram.reset();
        ProgramManager::deleteUnused();
    }
    loaded = false;
}
