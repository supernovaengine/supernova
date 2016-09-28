
#include "GLES2Scene.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "render/ShaderManager.h"
#include "render/TextureManager.h"
#include "math/Angle.h"

std::vector<Light*> GLES2Scene::lights;
Vector3 GLES2Scene::ambientLight;

int GLES2Scene::numPointLight;
std::vector<GLfloat> GLES2Scene::pointLightPos;
std::vector<GLfloat> GLES2Scene::pointLightPower;
std::vector<GLfloat> GLES2Scene::pointLightColor;

int GLES2Scene::numSpotLight;
std::vector<GLfloat> GLES2Scene::spotLightPos;
std::vector<GLfloat> GLES2Scene::spotLightPower;
std::vector<GLfloat> GLES2Scene::spotLightColor;
std::vector<GLfloat> GLES2Scene::spotLightCutOff;
std::vector<GLfloat> GLES2Scene::spotLightTarget;

int GLES2Scene::numDirectionalLight;
std::vector<GLfloat> GLES2Scene::directionalLightDir;
std::vector<GLfloat> GLES2Scene::directionalLightPower;
std::vector<GLfloat> GLES2Scene::directionalLightColor;

GLES2Scene::GLES2Scene() {
}

GLES2Scene::~GLES2Scene() {
}

void GLES2Scene::setLights(std::vector<Light*> lights){
    GLES2Scene::lights = lights;
}

void GLES2Scene::setAmbientLight(Vector3 ambientLight){
    GLES2Scene::ambientLight = ambientLight;
}

std::vector<Light*> GLES2Scene::getLights(){
    return lights;
}

Vector3 GLES2Scene::getAmbientLight(){
    return ambientLight;
}

bool GLES2Scene::load() {

	ShaderManager::clear();
    TextureManager::clear();
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glEnable(GL_DEPTH_TEST);
    
    //glEnable(GL_CULL_FACE);
    
    //Activate transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    GLES2Util::checkGlError("Error on load scene GLES2");
    
    return true;
}

bool GLES2Scene::draw() {
    
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    GLES2Util::checkGlError("glClearColor");
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLES2Util::checkGlError("glClear");
    
    numPointLight = 0;
    pointLightPos.clear();
    pointLightColor.clear();
    pointLightPower.clear();
    numSpotLight = 0;
    spotLightPos.clear();
    spotLightColor.clear();
    spotLightTarget.clear();
    spotLightPower.clear();
    spotLightCutOff.clear();
    numDirectionalLight = 0;
    directionalLightDir.clear();
    directionalLightColor.clear();
    directionalLightPower.clear();
    for ( int i = 0; i < (int)GLES2Scene::getLights().size(); i++){
        if (GLES2Scene::getLights()[i]->getType() == S_POINT_LIGHT){
            numPointLight++;
            
            pointLightPos.push_back(GLES2Scene::getLights()[i]->getWorldPosition().x);
            pointLightPos.push_back(GLES2Scene::getLights()[i]->getWorldPosition().y);
            pointLightPos.push_back(GLES2Scene::getLights()[i]->getWorldPosition().z);
            
            pointLightColor.push_back(GLES2Scene::getLights()[i]->getColor().x);
            pointLightColor.push_back(GLES2Scene::getLights()[i]->getColor().y);
            pointLightColor.push_back(GLES2Scene::getLights()[i]->getColor().z);
            
            pointLightPower.push_back(GLES2Scene::getLights()[i]->getPower());
        }
        if (GLES2Scene::getLights()[i]->getType() == S_SPOT_LIGHT){
            numSpotLight++;
            
            spotLightPos.push_back(GLES2Scene::getLights()[i]->getWorldPosition().x);
            spotLightPos.push_back(GLES2Scene::getLights()[i]->getWorldPosition().y);
            spotLightPos.push_back(GLES2Scene::getLights()[i]->getWorldPosition().z);
            
            spotLightColor.push_back(GLES2Scene::getLights()[i]->getColor().x);
            spotLightColor.push_back(GLES2Scene::getLights()[i]->getColor().y);
            spotLightColor.push_back(GLES2Scene::getLights()[i]->getColor().z);
            
            spotLightTarget.push_back(GLES2Scene::getLights()[i]->getWorldTarget().x);
            spotLightTarget.push_back(GLES2Scene::getLights()[i]->getWorldTarget().y);
            spotLightTarget.push_back(GLES2Scene::getLights()[i]->getWorldTarget().z);
            
            spotLightPower.push_back(GLES2Scene::getLights()[i]->getPower());
            
            spotLightCutOff.push_back(cos(Angle::defaultToRad(GLES2Scene::getLights()[i]->getSpotAngle() / 2.0)));
        }
        if (GLES2Scene::getLights()[i]->getType() == S_DIRECTIONAL_LIGHT){
            numDirectionalLight++;
            
            directionalLightDir.push_back(GLES2Scene::getLights()[i]->getDirection().x);
            directionalLightDir.push_back(GLES2Scene::getLights()[i]->getDirection().y);
            directionalLightDir.push_back(GLES2Scene::getLights()[i]->getDirection().z);
            
            directionalLightColor.push_back(GLES2Scene::getLights()[i]->getColor().x);
            directionalLightColor.push_back(GLES2Scene::getLights()[i]->getColor().y);
            directionalLightColor.push_back(GLES2Scene::getLights()[i]->getColor().z);
            
            directionalLightPower.push_back(GLES2Scene::getLights()[i]->getPower());
        }
    }
    
    return true;
}

bool GLES2Scene::screenSize(int width, int height){
    
    glViewport(0, 0, width, height);
    //glScissor(0,0,width, height);
    //glEnable(GL_SCISSOR_TEST);
    //checkGlError("glViewport");
    
    return true;
}
