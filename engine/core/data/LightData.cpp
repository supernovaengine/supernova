#include "LightData.h"
#include "DirectionalLight.h"
#include "math/Angle.h"


using namespace Supernova;

LightData::LightData(){
    ambientLight = NULL;
}

LightData::~LightData(){
    
}

bool LightData::updateLights(std::vector<Light*>* lights, Vector3* ambientLight){
    
    this->ambientLight = ambientLight;
    
    this->numPointLight = 0;
    this->pointLightPos.clear();
    this->pointLightColor.clear();
    this->pointLightPower.clear();
    this->pointLightShadowIdx.clear();
    
    this->numSpotLight = 0;
    this->spotLightPos.clear();
    this->spotLightColor.clear();
    this->spotLightTarget.clear();
    this->spotLightPower.clear();
    this->spotLightCutOff.clear();
    this->spotLightOuterCutOff.clear();
    this->spotLightShadowIdx.clear();
    
    this->numDirectionalLight = 0;
    this->directionalLightDir.clear();
    this->directionalLightColor.clear();
    this->directionalLightPower.clear();
    this->directionalLightShadowIdx.clear();

    this->numShadows2D = 0;
    this->numShadowsCube = 0;
    this->shadowsMap2D.clear();
    this->shadowsMapCube.clear();
    this->shadowsVPMatrix.clear();
    this->shadowsBias2D.clear();
    this->shadowsBiasCube.clear();
    this->shadowsCameraNearFar2D.clear();
    this->shadowsCameraNearFarCube.clear();
    this->shadowNumCascades2D.clear();

    for ( int i = 0; i < (int)lights->size(); i++){
        if (lights->at(i)->getType() == S_POINT_LIGHT){
            this->numPointLight++;
            
            this->pointLightPos.push_back(lights->at(i)->getWorldPosition().x);
            this->pointLightPos.push_back(lights->at(i)->getWorldPosition().y);
            this->pointLightPos.push_back(lights->at(i)->getWorldPosition().z);
            
            this->pointLightColor.push_back(lights->at(i)->getColor().x);
            this->pointLightColor.push_back(lights->at(i)->getColor().y);
            this->pointLightColor.push_back(lights->at(i)->getColor().z);
            
            this->pointLightPower.push_back(lights->at(i)->getPower());
            
            if (lights->at(i)->isUseShadow()){
                this->pointLightShadowIdx.push_back(this->numShadowsCube);
            }else{
                this->pointLightShadowIdx.push_back(-1);
            }
        }
        if (lights->at(i)->getType() == S_SPOT_LIGHT){
            this->numSpotLight++;
            
            this->spotLightPos.push_back(lights->at(i)->getWorldPosition().x);
            this->spotLightPos.push_back(lights->at(i)->getWorldPosition().y);
            this->spotLightPos.push_back(lights->at(i)->getWorldPosition().z);
            
            this->spotLightColor.push_back(lights->at(i)->getColor().x);
            this->spotLightColor.push_back(lights->at(i)->getColor().y);
            this->spotLightColor.push_back(lights->at(i)->getColor().z);
            
            this->spotLightTarget.push_back(lights->at(i)->getWorldTarget().x);
            this->spotLightTarget.push_back(lights->at(i)->getWorldTarget().y);
            this->spotLightTarget.push_back(lights->at(i)->getWorldTarget().z);
            
            this->spotLightPower.push_back(lights->at(i)->getPower());
            
            this->spotLightCutOff.push_back(cos(lights->at(i)->getSpotAngle() / 2.0));
            this->spotLightOuterCutOff.push_back(cos(lights->at(i)->getSpotOuterAngle() / 2.0));
            
            if (lights->at(i)->isUseShadow()){
                this->spotLightShadowIdx.push_back(this->numShadows2D);
            }else{
                this->spotLightShadowIdx.push_back(-1);
            }
        }
        if (lights->at(i)->getType() == S_DIRECTIONAL_LIGHT){
            this->numDirectionalLight++;
            
            this->directionalLightDir.push_back(lights->at(i)->getDirection().x);
            this->directionalLightDir.push_back(lights->at(i)->getDirection().y);
            this->directionalLightDir.push_back(lights->at(i)->getDirection().z);
            
            this->directionalLightColor.push_back(lights->at(i)->getColor().x);
            this->directionalLightColor.push_back(lights->at(i)->getColor().y);
            this->directionalLightColor.push_back(lights->at(i)->getColor().z);
            
            this->directionalLightPower.push_back(lights->at(i)->getPower());
            
            if (lights->at(i)->isUseShadow()){
                this->directionalLightShadowIdx.push_back(this->numShadows2D);
            }else{
                this->directionalLightShadowIdx.push_back(-1);
            }
        }

        if (lights->at(i)->isUseShadow()){

            if (lights->at(i)->getType() == S_SPOT_LIGHT) {
                this->numShadows2D++;
                this->shadowsMap2D.push_back(lights->at(i)->getShadowMap());
                this->shadowsVPMatrix.push_back(lights->at(i)->getDepthVPMatrix());
                this->shadowsBias2D.push_back(lights->at(i)->getShadowBias());
                this->shadowsCameraNearFar2D.push_back(lights->at(i)->getLightCamera()->getNearFarPlane());
                this->shadowNumCascades2D.push_back(0);

            }else if (lights->at(i)->getType() == S_DIRECTIONAL_LIGHT) {
                for (int ca = 0; ca < ((DirectionalLight*)lights->at(i))->getNumShadowCasdades(); ca++) {
                    this->numShadows2D++;
                    this->shadowsMap2D.push_back(lights->at(i)->getShadowMap(ca));
                    this->shadowsVPMatrix.push_back(lights->at(i)->getDepthVPMatrix(ca));
                    this->shadowsBias2D.push_back(lights->at(i)->getShadowBias());
                    this->shadowsCameraNearFar2D.push_back(((DirectionalLight *) lights->at(i))->getCascadeCameraNearFar(ca));
                    this->shadowNumCascades2D.push_back(((DirectionalLight *) lights->at(i))->getNumShadowCasdades());
                }
            }else if (lights->at(i)->getType() == S_POINT_LIGHT) {
                this->numShadowsCube++;
                this->shadowsMapCube.push_back(lights->at(i)->getShadowMap());
                this->shadowsBiasCube.push_back(lights->at(i)->getShadowBias());
                this->shadowsCameraNearFarCube.push_back(lights->at(i)->getLightCamera()->getNearFarPlane());
            }

        }
    }
    
    if ((int)lights->size() > 0){
        return true;
    }
    return false;
}
