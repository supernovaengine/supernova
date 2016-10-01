
#include "GLES2Scene.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "render/ShaderManager.h"
#include "render/TextureManager.h"
#include "math/Angle.h"
#include "GLES2Lights.h"

bool GLES2Scene::lighting = false;


GLES2Scene::GLES2Scene() {
}

GLES2Scene::~GLES2Scene() {
}


void GLES2Scene::updateLights(std::vector<Light*> lights, Vector3 ambientLight){
    GLES2Lights::ambientLight = ambientLight;
    
    if ((int)lights.size() > 0){
        lighting = true;
    }
    
    GLES2Lights::numPointLight = 0;
    GLES2Lights::pointLightPos.clear();
    GLES2Lights::pointLightColor.clear();
    GLES2Lights::pointLightPower.clear();
    GLES2Lights::numSpotLight = 0;
    GLES2Lights::spotLightPos.clear();
    GLES2Lights::spotLightColor.clear();
    GLES2Lights::spotLightTarget.clear();
    GLES2Lights::spotLightPower.clear();
    GLES2Lights::spotLightCutOff.clear();
    GLES2Lights::numDirectionalLight = 0;
    GLES2Lights::directionalLightDir.clear();
    GLES2Lights::directionalLightColor.clear();
    GLES2Lights::directionalLightPower.clear();
    for ( int i = 0; i < (int)lights.size(); i++){
        if (lights[i]->getType() == S_POINT_LIGHT){
            GLES2Lights::numPointLight++;
            
            GLES2Lights::pointLightPos.push_back(lights[i]->getWorldPosition().x);
            GLES2Lights::pointLightPos.push_back(lights[i]->getWorldPosition().y);
            GLES2Lights::pointLightPos.push_back(lights[i]->getWorldPosition().z);
            
            GLES2Lights::pointLightColor.push_back(lights[i]->getColor().x);
            GLES2Lights::pointLightColor.push_back(lights[i]->getColor().y);
            GLES2Lights::pointLightColor.push_back(lights[i]->getColor().z);
            
            GLES2Lights::pointLightPower.push_back(lights[i]->getPower());
        }
        if (lights[i]->getType() == S_SPOT_LIGHT){
            GLES2Lights::numSpotLight++;
            
            GLES2Lights::spotLightPos.push_back(lights[i]->getWorldPosition().x);
            GLES2Lights::spotLightPos.push_back(lights[i]->getWorldPosition().y);
            GLES2Lights::spotLightPos.push_back(lights[i]->getWorldPosition().z);
            
            GLES2Lights::spotLightColor.push_back(lights[i]->getColor().x);
            GLES2Lights::spotLightColor.push_back(lights[i]->getColor().y);
            GLES2Lights::spotLightColor.push_back(lights[i]->getColor().z);
            
            GLES2Lights::spotLightTarget.push_back(lights[i]->getWorldTarget().x);
            GLES2Lights::spotLightTarget.push_back(lights[i]->getWorldTarget().y);
            GLES2Lights::spotLightTarget.push_back(lights[i]->getWorldTarget().z);
            
            GLES2Lights::spotLightPower.push_back(lights[i]->getPower());
            
            GLES2Lights::spotLightCutOff.push_back(cos(Angle::defaultToRad(lights[i]->getSpotAngle() / 2.0)));
        }
        if (lights[i]->getType() == S_DIRECTIONAL_LIGHT){
            GLES2Lights::numDirectionalLight++;
            
            GLES2Lights::directionalLightDir.push_back(lights[i]->getDirection().x);
            GLES2Lights::directionalLightDir.push_back(lights[i]->getDirection().y);
            GLES2Lights::directionalLightDir.push_back(lights[i]->getDirection().z);
            
            GLES2Lights::directionalLightColor.push_back(lights[i]->getColor().x);
            GLES2Lights::directionalLightColor.push_back(lights[i]->getColor().y);
            GLES2Lights::directionalLightColor.push_back(lights[i]->getColor().z);
            
            GLES2Lights::directionalLightPower.push_back(lights[i]->getPower());
        }
    }
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

    
    return true;
}

bool GLES2Scene::viewSize(int x, int y, int width, int height){
    
    glViewport(x, y, width, height);
    //glScissor(x, y, width, height);
    //glEnable(GL_SCISSOR_TEST);
    //checkGlError("glViewport");
    
    return true;
}
