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

GLuint GLES2Mesh::emptyTexture;
bool GLES2Mesh::emptyTextureLoaded;

GLES2Mesh::GLES2Mesh() {
    programName = NULL;
    loaded = false;
    lighting = false;
}

GLES2Mesh::~GLES2Mesh() {
    destroy();
}

void GLES2Mesh::checkLighting(){
    lighting = false;
    if (sceneRender != NULL){
        lighting = ((GLES2Scene*)sceneRender)->lighting;
    }
}

bool GLES2Mesh::load(SceneRender* sceneRender, std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh> submeshes) {

    loaded = true;
    
    if (vertices.size() <= 0){
        return false;
    }
    
    this->sceneRender = sceneRender;
    
    checkLighting();

    primitiveSize = (int)vertices.size();

    this->submeshes = submeshes;

    textured.clear();
    for (unsigned int i = 0; i < submeshes.size(); i++){
        indicesSizes.push_back((int)submeshes[i].getIndices()->size());

        if (submeshes[i].getTexture()!="" && texcoords.size() > 0){
            textured.push_back(true);
        }else{
            textured.push_back(false);
        }
    }

    gPrimitiveVertices.clear();
    guvMapping.clear();
    gNormals.clear();
    for ( int i = 0; i < (int)vertices.size(); i++){
        gPrimitiveVertices.push_back(vertices[i].x);
        gPrimitiveVertices.push_back(vertices[i].y);
        gPrimitiveVertices.push_back(vertices[i].z);

        if (i < (int)texcoords.size()){
            guvMapping.push_back(texcoords[i].x);
            guvMapping.push_back(texcoords[i].y);
        }else{
            guvMapping.push_back(0);
            guvMapping.push_back(0);
        }

        if (i < (int)normals.size()){
            gNormals.push_back(normals[i].x);
            gNormals.push_back(normals[i].y);
            gNormals.push_back(normals[i].z);
        }else{
            gNormals.push_back(0);
            gNormals.push_back(0);
            gNormals.push_back(0);
        }
    }

    programName = "perfragment";

    gProgram = ProgramManager::useProgram(programName, "");

    useLighting = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "uUseLighting");
    useTexture = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "uUseTexture");

    vertexBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, gPrimitiveVertices.size() * sizeof(GLfloat), &gPrimitiveVertices.front(), GL_STATIC_DRAW);
    aPositionHandle = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Position");

    uvBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, guvMapping.size() * sizeof(GLfloat), &guvMapping.front(), GL_STATIC_DRAW);
    aTextureCoordinatesLocation = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_TextureCoordinates");

    normalBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, gNormals.size() * sizeof(GLfloat), &gNormals.front(), GL_STATIC_DRAW);
    aNormal = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Normal");

    indiceBuffer.clear();
    texture.clear();
    for (unsigned int i = 0; i < submeshes.size(); i++){
        if (indicesSizes[i] > 0){
            std::vector<unsigned int> gIndices = *submeshes[i].getIndices();
            indiceBuffer.push_back(GLES2Util::createVBO(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(unsigned int), &gIndices.front(), GL_STATIC_DRAW));
        }else{
            indiceBuffer.push_back(NULL);
        }

        if (textured[i]){
            texture.push_back(TextureManager::loadTexture(submeshes[i].getTexture()));
            uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
        }else{
            
            if (Supernova::getPlatform() == S_WEB){
                //Fix Chrome warnings of no texture bound with an empty texture
                if (!GLES2Mesh::emptyTextureLoaded){
                    glGenTextures(1, &GLES2Mesh::emptyTexture);
                    glBindTexture(GL_TEXTURE_2D, GLES2Mesh::emptyTexture);
                    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1, 1, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    GLES2Mesh::emptyTextureLoaded = true;
                }
                
                texture.push_back(NULL);
                uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
            }else{
                texture.push_back(NULL);
            }
            uColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_Color");
            
        }
    }

    if (this->lighting){
        uEyePos = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_EyePos");

        u_AmbientLight = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_AmbientLight");

        u_NumPointLight = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_NumPointLight");
        u_PointLightPos = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_PointLightPos");
        u_PointLightPower = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_PointLightPower");
        u_PointLightColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_PointLightColor");

        u_NumSpotLight = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_NumSpotLight");
        u_SpotLightPos = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_SpotLightPos");
        u_SpotLightPower = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_SpotLightPower");
        u_SpotLightColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_SpotLightColor");
        u_SpotLightTarget = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_SpotLightTarget");
        u_SpotLightCutOff = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_SpotLightCutOff");

        u_NumDirectionalLight = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_NumDirectionalLight");
        u_DirectionalLightDir = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_DirectionalLightDir");
        u_DirectionalLightPower = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_DirectionalLightPower");
        u_DirectionalLightColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_DirectionalLightColor");

    }

    u_mvpMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mvpMatrix");
    u_mMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mMatrix");
    u_nMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_nMatrix");

    GLES2Util::checkGlError("Error on load GLES2");

    return true;
}

bool GLES2Mesh::draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode) {
    
    if (gPrimitiveVertices.size() <= 0){
        return false;
    }
    
    //Fix IOS lighting set true in subscenes
    checkLighting();

    glUseProgram(((GLES2Program*)gProgram.get())->getProgram());
    GLES2Util::checkGlError("glUseProgram");

    glUniform1i(useLighting, this->lighting);

    glUniformMatrix4fv(u_mvpMatrix, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix);
    glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, (GLfloat*)modelMatrix);
    glUniformMatrix4fv(u_nMatrix, 1, GL_FALSE, (GLfloat*)normalMatrix);

    if (this->lighting){
        glUniform3fv(uEyePos, 1, cameraPosition->ptr());

        glUniform3fv(u_AmbientLight, 1, ((GLES2Scene*)sceneRender)->ambientLight.ptr());

        glUniform1i(u_NumPointLight, ((GLES2Scene*)sceneRender)->numPointLight);
        if (((GLES2Scene*)sceneRender)->numPointLight > 0){
            glUniform3fv(u_PointLightPos, ((GLES2Scene*)sceneRender)->numPointLight, &((GLES2Scene*)sceneRender)->pointLightPos.front());
            glUniform1fv(u_PointLightPower, ((GLES2Scene*)sceneRender)->numPointLight, &((GLES2Scene*)sceneRender)->pointLightPower.front());
            glUniform3fv(u_PointLightColor, ((GLES2Scene*)sceneRender)->numPointLight, &((GLES2Scene*)sceneRender)->pointLightColor.front());
        }

        glUniform1i(u_NumSpotLight, ((GLES2Scene*)sceneRender)->numSpotLight);
        if (((GLES2Scene*)sceneRender)->numSpotLight > 0){
            glUniform3fv(u_SpotLightPos, ((GLES2Scene*)sceneRender)->numSpotLight, &((GLES2Scene*)sceneRender)->spotLightPos.front());
            glUniform1fv(u_SpotLightPower, ((GLES2Scene*)sceneRender)->numSpotLight, &((GLES2Scene*)sceneRender)->spotLightPower.front());
            glUniform3fv(u_SpotLightColor, ((GLES2Scene*)sceneRender)->numSpotLight, &((GLES2Scene*)sceneRender)->spotLightColor.front());
            glUniform3fv(u_SpotLightTarget, ((GLES2Scene*)sceneRender)->numSpotLight, &((GLES2Scene*)sceneRender)->spotLightTarget.front());
            glUniform1fv(u_SpotLightCutOff, ((GLES2Scene*)sceneRender)->numSpotLight, &((GLES2Scene*)sceneRender)->spotLightCutOff.front());
        }

        glUniform1i(u_NumDirectionalLight, ((GLES2Scene*)sceneRender)->numDirectionalLight);
        if (((GLES2Scene*)sceneRender)->numDirectionalLight > 0){
            glUniform3fv(u_DirectionalLightDir, ((GLES2Scene*)sceneRender)->numDirectionalLight, &((GLES2Scene*)sceneRender)->directionalLightDir.front());
            glUniform1fv(u_DirectionalLightPower, ((GLES2Scene*)sceneRender)->numDirectionalLight, &((GLES2Scene*)sceneRender)->directionalLightPower.front());
            glUniform3fv(u_DirectionalLightColor, ((GLES2Scene*)sceneRender)->numDirectionalLight, &((GLES2Scene*)sceneRender)->directionalLightColor.front());
        }

    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(aTextureCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));

    GLenum modeGles = GL_TRIANGLES;

    if (mode == S_TRIANGLES_STRIP){
        modeGles = GL_TRIANGLE_STRIP;
    }

    for (int i = ((int)submeshes.size()-1); i >= 0; i--){

        glUniform1i(useTexture, textured[i]);

        if (textured[i]){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ((GLES2Texture*)(texture[i].get()))->getTexture());
            glUniform1i(uTextureUnitLocation, 0);
        }else{
            glUniform4fv(uColor, 1, submeshes[i].getColor()->ptr());
            if (Supernova::getPlatform() == S_WEB){
                //Fix Chrome warnings of no texture bound
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, GLES2Mesh::emptyTexture);
                glUniform1i(uTextureUnitLocation, 0);
            }
        }

        if (indicesSizes[i] > 0){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer[i]);
            glDrawElements(modeGles, indicesSizes[i], GL_UNSIGNED_INT, BUFFER_OFFSET(0));
        }else{
            glDrawArrays(modeGles, 0, primitiveSize);
        }

    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLES2Util::checkGlError("Error on draw GLES2");

    return true;
}

void GLES2Mesh::destroy(){
    if (loaded){
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &uvBuffer);
        glDeleteBuffers(1, &normalBuffer);
        for (unsigned int i = 0; i < submeshes.size(); i++){
            if (indicesSizes[i] > 0)
                glDeleteBuffers(1, &indiceBuffer[i]);

            if (textured[i])
                texture[i].reset();
            TextureManager::deleteUnused();
        }
        gProgram.reset();
        ProgramManager::deleteUnused();
    }
    loaded = false;
}
