#include "GLES2Mesh.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "GLES2Scene.h"
#include "platform/Log.h"
#include "GLES2Texture.h"
#include "math/Vector2.h"
#include "math/Angle.h"
#include "PrimitiveMode.h"
#include "GLES2Lights.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

GLES2Mesh::GLES2Mesh() {
    programName = NULL;
    loaded = false;
}

GLES2Mesh::~GLES2Mesh() {
    destroy();
}

bool GLES2Mesh::load(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh> submeshes) {

    loaded = true;

    primitiveSize = (int)vertices.size();

    this->submeshes = submeshes;

    for (unsigned int i = 0; i < submeshes.size(); i++){
        indicesSizes.push_back((int)submeshes[i].getIndices()->size());

        if (submeshes[i].getTexture()->isUsed() && texcoords.size() > 0){
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
    
    gProgram = ((GLES2Program*)shaderManager.useShader(programName))->getShader();

    useLighting = glGetUniformLocation(gProgram, "uUseLighting");
    useTexture = glGetUniformLocation(gProgram, "uUseTexture");

    vertexBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, gPrimitiveVertices.size() * sizeof(GLfloat), &gPrimitiveVertices.front(), GL_STATIC_DRAW);
    aPositionHandle = glGetAttribLocation(gProgram, "a_Position");

    uvBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, guvMapping.size() * sizeof(GLfloat), &guvMapping.front(), GL_STATIC_DRAW);
    aTextureCoordinatesLocation = glGetAttribLocation(gProgram, "a_TextureCoordinates");

    normalBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, gNormals.size() * sizeof(GLfloat), &gNormals.front(), GL_STATIC_DRAW);
    aNormal = glGetAttribLocation(gProgram, "a_Normal");

    for (unsigned int i = 0; i < submeshes.size(); i++){
        if (indicesSizes[i] > 0){
            std::vector<unsigned int> gIndices = *submeshes[i].getIndices();
            indiceBuffer.push_back(GLES2Util::createVBO(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(unsigned int), &gIndices.front(), GL_STATIC_DRAW));
        }else{
            indiceBuffer.push_back(NULL);
        }

        if (textured[i]){
            texture.push_back(((GLES2Texture*)(textureManager.loadTexture(submeshes[i].getTexture()->getFilePath())))->getTexture());
            uTextureUnitLocation = glGetUniformLocation(gProgram, "u_TextureUnit");
        }else{
            texture.push_back(NULL);
            uColor = glGetUniformLocation(gProgram, "u_Color");
        }
    }

    if (GLES2Scene::lighting){
        uEyePos = glGetUniformLocation(gProgram, "u_EyePos");

        u_AmbientLight = glGetUniformLocation(gProgram, "u_AmbientLight");

        u_NumPointLight = glGetUniformLocation(gProgram, "u_NumPointLight");
        u_PointLightPos = glGetUniformLocation(gProgram, "u_PointLightPos");
        u_PointLightPower = glGetUniformLocation(gProgram, "u_PointLightPower");
        u_PointLightColor = glGetUniformLocation(gProgram, "u_PointLightColor");

        u_NumSpotLight = glGetUniformLocation(gProgram, "u_NumSpotLight");
        u_SpotLightPos = glGetUniformLocation(gProgram, "u_SpotLightPos");
        u_SpotLightPower = glGetUniformLocation(gProgram, "u_SpotLightPower");
        u_SpotLightColor = glGetUniformLocation(gProgram, "u_SpotLightColor");
        u_SpotLightTarget = glGetUniformLocation(gProgram, "u_SpotLightTarget");
        u_SpotLightCutOff = glGetUniformLocation(gProgram, "u_SpotLightCutOff");

        u_NumDirectionalLight = glGetUniformLocation(gProgram, "u_NumDirectionalLight");
        u_DirectionalLightDir = glGetUniformLocation(gProgram, "u_DirectionalLightDir");
        u_DirectionalLightPower = glGetUniformLocation(gProgram, "u_DirectionalLightPower");
        u_DirectionalLightColor = glGetUniformLocation(gProgram, "u_DirectionalLightColor");

    }

    u_mvpMatrix = glGetUniformLocation(gProgram, "u_mvpMatrix");
    u_mMatrix = glGetUniformLocation(gProgram, "u_mMatrix");
    u_nMatrix = glGetUniformLocation(gProgram, "u_nMatrix");

    GLES2Util::checkGlError("Error on load GLES2");

    return true;
}

bool GLES2Mesh::draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode) {

    glUseProgram(gProgram);
    GLES2Util::checkGlError("glUseProgram");

    glUniform1i(useLighting, GLES2Scene::lighting);

    glUniformMatrix4fv(u_mvpMatrix, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix);
    glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, (GLfloat*)modelMatrix);
    glUniformMatrix4fv(u_nMatrix, 1, GL_FALSE, (GLfloat*)normalMatrix);

    if (GLES2Scene::lighting){
        glUniform3fv(uEyePos, 1, cameraPosition->ptr());

        glUniform3fv(u_AmbientLight, 1, GLES2Lights::ambientLight.ptr());

        glUniform1i(u_NumPointLight, GLES2Lights::numPointLight);
        if (GLES2Lights::numPointLight > 0){
            glUniform3fv(u_PointLightPos, GLES2Lights::numPointLight, &GLES2Lights::pointLightPos.front());
            glUniform1fv(u_PointLightPower, GLES2Lights::numPointLight, &GLES2Lights::pointLightPower.front());
            glUniform3fv(u_PointLightColor, GLES2Lights::numPointLight, &GLES2Lights::pointLightColor.front());
        }

        glUniform1i(u_NumSpotLight, GLES2Lights::numSpotLight);
        if (GLES2Lights::numSpotLight > 0){
            glUniform3fv(u_SpotLightPos, GLES2Lights::numSpotLight, &GLES2Lights::spotLightPos.front());
            glUniform1fv(u_SpotLightPower, GLES2Lights::numSpotLight, &GLES2Lights::spotLightPower.front());
            glUniform3fv(u_SpotLightColor, GLES2Lights::numSpotLight, &GLES2Lights::spotLightColor.front());
            glUniform3fv(u_SpotLightTarget, GLES2Lights::numSpotLight, &GLES2Lights::spotLightTarget.front());
            glUniform1fv(u_SpotLightCutOff, GLES2Lights::numSpotLight, &GLES2Lights::spotLightCutOff.front());
        }

        glUniform1i(u_NumDirectionalLight, GLES2Lights::numDirectionalLight);
        if (GLES2Lights::numDirectionalLight > 0){
            glUniform3fv(u_DirectionalLightDir, GLES2Lights::numDirectionalLight, &GLES2Lights::directionalLightDir.front());
            glUniform1fv(u_DirectionalLightPower, GLES2Lights::numDirectionalLight, &GLES2Lights::directionalLightPower.front());
            glUniform3fv(u_DirectionalLightColor, GLES2Lights::numDirectionalLight, &GLES2Lights::directionalLightColor.front());
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
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glUniform1i(uTextureUnitLocation, 0);
        }else{
            glUniform4fv(uColor, 1, submeshes[0].getColor()->ptr());
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
                textureManager.deleteTexture(submeshes[i].getTexture()->getFilePath());
        }
        shaderManager.deleteShader(programName);
    }
    loaded = false;
}
