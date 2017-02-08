
#include "GLES2Scene.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "render/ProgramManager.h"
#include "render/TextureManager.h"
#include "math/Angle.h"


GLES2Scene::GLES2Scene() {
    lighting = false;
}

GLES2Scene::~GLES2Scene() {
}

void GLES2Scene::setAmbientLight(Vector3 ambientLight){
    
    this->ambientLight = ambientLight;
    
}

void GLES2Scene::setLights(std::vector<Light*> lights){
    
    if ((int)lights.size() > 0){
        lighting = true;
    }else{
        lighting = false;
    }
    
    this->numPointLight = 0;
    this->pointLightPos.clear();
    this->pointLightColor.clear();
    this->pointLightPower.clear();
    this->numSpotLight = 0;
    this->spotLightPos.clear();
    this->spotLightColor.clear();
    this->spotLightTarget.clear();
    this->spotLightPower.clear();
    this->spotLightCutOff.clear();
    this->numDirectionalLight = 0;
    this->directionalLightDir.clear();
    this->directionalLightColor.clear();
    this->directionalLightPower.clear();
    for ( int i = 0; i < (int)lights.size(); i++){
        if (lights[i]->getType() == S_POINT_LIGHT){
            this->numPointLight++;
            
            this->pointLightPos.push_back(lights[i]->getWorldPosition().x);
            this->pointLightPos.push_back(lights[i]->getWorldPosition().y);
            this->pointLightPos.push_back(lights[i]->getWorldPosition().z);
            
            this->pointLightColor.push_back(lights[i]->getColor().x);
            this->pointLightColor.push_back(lights[i]->getColor().y);
            this->pointLightColor.push_back(lights[i]->getColor().z);
            
            this->pointLightPower.push_back(lights[i]->getPower());
        }
        if (lights[i]->getType() == S_SPOT_LIGHT){
            this->numSpotLight++;
            
            this->spotLightPos.push_back(lights[i]->getWorldPosition().x);
            this->spotLightPos.push_back(lights[i]->getWorldPosition().y);
            this->spotLightPos.push_back(lights[i]->getWorldPosition().z);
            
            this->spotLightColor.push_back(lights[i]->getColor().x);
            this->spotLightColor.push_back(lights[i]->getColor().y);
            this->spotLightColor.push_back(lights[i]->getColor().z);
            
            this->spotLightTarget.push_back(lights[i]->getWorldTarget().x);
            this->spotLightTarget.push_back(lights[i]->getWorldTarget().y);
            this->spotLightTarget.push_back(lights[i]->getWorldTarget().z);
            
            this->spotLightPower.push_back(lights[i]->getPower());
            
            this->spotLightCutOff.push_back(cos(Angle::defaultToRad(lights[i]->getSpotAngle() / 2.0)));
        }
        if (lights[i]->getType() == S_DIRECTIONAL_LIGHT){
            this->numDirectionalLight++;
            
            this->directionalLightDir.push_back(lights[i]->getDirection().x);
            this->directionalLightDir.push_back(lights[i]->getDirection().y);
            this->directionalLightDir.push_back(lights[i]->getDirection().z);
            
            this->directionalLightColor.push_back(lights[i]->getColor().x);
            this->directionalLightColor.push_back(lights[i]->getColor().y);
            this->directionalLightColor.push_back(lights[i]->getColor().z);
            
            this->directionalLightPower.push_back(lights[i]->getPower());
        }
    }
}


bool GLES2Scene::load(bool childScene) {
    if (!childScene) {
        //ProgramManager::clear();
        //TextureManager::clear();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        GLES2Util::checkGlError("Error on load scene GLES2");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    return true;
}

bool GLES2Scene::draw(bool childScene, bool useDepth, bool useTransparency) {
    if (!childScene) {
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        GLES2Util::checkGlError("glClearColor");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLES2Util::checkGlError("glClear");
    }

    if (useDepth){
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }else{
        glDisable(GL_DEPTH_TEST);
    }

    if (useTransparency){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }else{
        glDisable(GL_BLEND);
    }

    return true;
}

bool GLES2Scene::viewSize(int x, int y, int width, int height){
    
    glViewport(x, y, width, height);
    //glScissor(x, y, width, height);
    //glEnable(GL_SCISSOR_TEST);
    //checkGlError("glViewport");
    
    return true;
}
