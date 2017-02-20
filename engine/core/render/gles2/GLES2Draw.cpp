#include "GLES2Draw.h"

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

GLuint GLES2Draw::emptyTexture;
bool GLES2Draw::emptyTextureLoaded;

GLES2Draw::GLES2Draw(): DrawRender() {
    loaded = false;
    lighting = false;
}

GLES2Draw::~GLES2Draw() {
    destroy();
}

void GLES2Draw::update(){
    if (loaded)
        GLES2Util::updateVBO(vertexBuffer, GL_ARRAY_BUFFER, positions->size() * 3 * sizeof(GLfloat), &positions->front());
}

void GLES2Draw::checkLighting(){
    lighting = false;
    if ((sceneRender != NULL) && (objectType != S_DRAW_SKY)){
        lighting = ((GLES2Scene*)sceneRender)->lighting;
    }
}

bool GLES2Draw::load() {

    if (positions->size() <= 0){
        return false;
    }
    
    loaded = true;
    
    checkLighting();

    primitiveSize = (int)(*positions).size();

    submeshesGles.clear();
    //---> For meshes
    if (submeshes){
        for (unsigned int i = 0; i < submeshes->size(); i++){
            submeshesGles[(*submeshes)[i]].indicesSizes = (int)(*submeshes)[i]->getIndices()->size();

            if ((*submeshes)[i]->getMaterial()->getTextures().size() > 0){
                submeshesGles[(*submeshes)[i]].textured = true;
            }else{
                submeshesGles[(*submeshes)[i]].textured = false;
            }
            if ((*submeshes)[i]->getMaterial()->getTextureType() == S_TEXTURE_2D &&  texcoords->size() == 0){
                submeshesGles[(*submeshes)[i]].textured = false;
            }
        }
    }
    //---> For points
    if (objectType == S_DRAW_POINTS){
        if (material->getTextures().size() > 0){
            textured = true;
        }else{
            textured = false;
        }
    }
    
    if (texcoords){
        while (positions->size() > texcoords->size()){
            texcoords->push_back(Vector2(0,0));
        }
    }    
    
    if (normals){
        while (positions->size() > normals->size()){
            normals->push_back(Vector3(0,0,0));
        }
    }

    std::string programName = "perfragment";
    std::string programDefs = "";
    if (submeshes){
        if ((*submeshes)[0]->getMaterial()->getTextureType() == S_TEXTURE_CUBE){
            programDefs += "#define USE_TEXTURECUBE\n";
        }
    }
    if (objectType == S_DRAW_SKY){
        programDefs += "#define IS_SKY\n";
    }
    if (objectType == S_DRAW_POINTS){
        programDefs += "#define IS_POINTS\n";
    }
    if (this->lighting){
        programDefs += "#define USE_LIGHTING\n";
    }

    gProgram = ProgramManager::useProgram(programName, programDefs);

    useTexture = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "uUseTexture");

    vertexBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, positions->size() * 3 * sizeof(GLfloat), &positions->front(), GL_STATIC_DRAW);
    aPositionHandle = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Position");
    
    if (texcoords){
        uvBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, texcoords->size() * 2 * sizeof(GLfloat), &texcoords->front(), GL_STATIC_DRAW);
        aTextureCoordinatesLocation = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_TextureCoordinates");
    }
    
    if (normals && this->lighting){
        normalBuffer = GLES2Util::createVBO(GL_ARRAY_BUFFER, normals->size() * 3 * sizeof(GLfloat), &normals->front(), GL_STATIC_DRAW);
        aNormal = glGetAttribLocation(((GLES2Program*)gProgram.get())->getProgram(), "a_Normal");
    }
    
    //---> For meshes
    if (submeshes){
        for (unsigned int i = 0; i < submeshes->size(); i++){
            if (submeshesGles[(*submeshes)[i]].indicesSizes > 0){
                std::vector<unsigned int> gIndices = *(*submeshes)[i]->getIndices();
                submeshesGles[(*submeshes)[i]].indiceBuffer = GLES2Util::createVBO(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(unsigned int), &gIndices.front(), GL_STATIC_DRAW);
            }else{
                submeshesGles[(*submeshes)[i]].indiceBuffer = NULL;
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
                
                if (Supernova::getPlatform() == S_WEB){
                    //Fix Chrome warnings of no texture bound with an empty texture
                    if (!GLES2Draw::emptyTextureLoaded){
                        glGenTextures(1, &GLES2Draw::emptyTexture);
                        glBindTexture(GL_TEXTURE_2D, GLES2Draw::emptyTexture);
                        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1, 1, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        GLES2Draw::emptyTextureLoaded = true;
                    }
                    
                    submeshesGles[(*submeshes)[i]].texture = NULL;
                    uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
                }else{
                    submeshesGles[(*submeshes)[i]].texture = NULL;
                }
                uColor = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_Color");
                
            }
        }
    }
    //---> For points
    if (objectType == S_DRAW_POINTS){
        if (textured){
            texture = TextureManager::loadTexture(material->getTextures()[0]);
            uTextureUnitLocation = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_TextureUnit");
        }else{
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
    if (this->lighting){
        u_mMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_mMatrix");
        u_nMatrix = glGetUniformLocation(((GLES2Program*)gProgram.get())->getProgram(), "u_nMatrix");
    }

    GLES2Util::checkGlError("Error on load GLES2");

    return true;
}

bool GLES2Draw::draw() {

    if (!loaded){
        return false;
    }

    if (objectType == S_DRAW_SKY) {
       glDepthFunc(GL_LEQUAL);
    }

    glUseProgram(((GLES2Program*)gProgram.get())->getProgram());
    GLES2Util::checkGlError("glUseProgram");

    glUniformMatrix4fv(u_mvpMatrix, 1, GL_FALSE, (GLfloat*)modelViewProjectionMatrix);
    if (this->lighting){
        glUniformMatrix4fv(u_mMatrix, 1, GL_FALSE, (GLfloat*)modelMatrix);
        glUniformMatrix4fv(u_nMatrix, 1, GL_FALSE, (GLfloat*)normalMatrix);
    }

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
    if (aPositionHandle == -1) aPositionHandle = 0;
    glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));

    if (texcoords){
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        if (aTextureCoordinatesLocation == -1) aTextureCoordinatesLocation = 1;
        glVertexAttribPointer(aTextureCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }
    
    if (normals && this->lighting){
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        if (aNormal == -1) aNormal = 2;
        glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));
    }

    GLenum modeGles = GL_TRIANGLES;
    if (primitiveMode == S_TRIANGLES_STRIP){
        modeGles = GL_TRIANGLE_STRIP;
    }
    if (primitiveMode == S_POINTS){
        modeGles = GL_POINTS;
    }

    if (submeshes){
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
                    glBindTexture(GL_TEXTURE_2D, GLES2Draw::emptyTexture);
                    glUniform1i(uTextureUnitLocation, 0);
                }
            }

            if (submeshesGles[(*submeshes)[i]].indicesSizes > 0){
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submeshesGles[(*submeshes)[i]].indiceBuffer);
                glDrawElements(modeGles, submeshesGles[(*submeshes)[i]].indicesSizes, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
            }else{
                glDrawArrays(modeGles, 0, primitiveSize);
            }
        }
    }
    
    if (objectType == S_DRAW_POINTS){
        glUniform1i(useTexture, textured);
        
        if (textured){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(((GLES2Texture*)(texture.get()))->getTextureType(), ((GLES2Texture*)(texture.get()))->getTexture());
            glUniform1i(uTextureUnitLocation, 0);
        }else{
            glUniform4fv(uColor, 1, material->getColor()->ptr());
            if (Supernova::getPlatform() == S_WEB){
                //Fix Chrome warnings of no texture bound
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, GLES2Draw::emptyTexture);
                glUniform1i(uTextureUnitLocation, 0);
            }
        }
        
        glDrawArrays(modeGles, 0, primitiveSize);
    }

    //Log::Verbose(LOG_TAG, "oi\n");
    //printf("oi\n");

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLES2Util::checkGlError("Error on draw GLES2");

    return true;
}

void GLES2Draw::destroy(){
    if (loaded){
        glDeleteBuffers(1, &vertexBuffer);
        if (texcoords){
            glDeleteBuffers(1, &uvBuffer);
        }
        if (normals && this->lighting){
            glDeleteBuffers(1, &normalBuffer);
        }
        if (submeshes){
            for (unsigned int i = 0; i < submeshes->size(); i++){
                if (submeshesGles[(*submeshes)[i]].indicesSizes > 0)
                    glDeleteBuffers(1, &submeshesGles[(*submeshes)[i]].indiceBuffer);

                if (submeshesGles[(*submeshes)[i]].textured){
                    submeshesGles[(*submeshes)[i]].texture.reset();
                    TextureManager::deleteUnused();
                }
            }
        }
        if (objectType == S_DRAW_POINTS){
            if (textured){
                texture.reset();
                TextureManager::deleteUnused();
            }
        }
        gProgram.reset();
        ProgramManager::deleteUnused();
    }
    loaded = false;
}
