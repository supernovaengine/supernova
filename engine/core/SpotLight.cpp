#include "SpotLight.h"

#include "math/Angle.h"
#include <stdlib.h>

using namespace Supernova;

SpotLight::SpotLight(): Light(){
    type = S_SPOT_LIGHT;
    this->power = 50;
}

SpotLight::~SpotLight(){

}

void SpotLight::updateLightCamera(){

    lightCameras[0]->setPosition(getWorldPosition());
    lightCameras[0]->setView(getWorldTarget());
    lightCameras[0]->setPerspective(Angle::radToDefault(spotAngle), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);

    //TODO: Check this
    Vector3 cameraDirection = (lightCameras[0]->getPosition() - lightCameras[0]->getView()).normalize();
    if (cameraDirection == Vector3(0, 1, 0)) {
        lightCameras[0]->setUp(0, 0, 1);
    } else {
        lightCameras[0]->setUp(0, 1, 0);
    }

    depthVPMatrix = (*lightCameras[0]->getViewProjectionMatrix());

    Light::updateLightCamera();
}

void SpotLight::setTarget(Vector3 target){
    if (this->target != target){
        this->target = target;
        updateMatrix();
    }
}

void SpotLight::setTarget(float x, float y, float z){
    setTarget(Vector3(x, y, z));
}

void SpotLight::setSpotAngle(float angle){
    this->spotAngle = Angle::defaultToRad(angle);
}

bool SpotLight::loadShadow(){
    if (useShadow){
        if (lightCameras.size()==0)
            lightCameras.push_back(new Camera());
        updateLightCamera();

        if (!shadowMap) {
            shadowMap = new Texture(shadowMapWidth, shadowMapHeight);

            char rand_id[10];
            static const char alphanum[] =
                    "0123456789"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz";
            for (int i = 0; i < 10; ++i) {
                rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
            }

            shadowMap->setId("shadowMap|" + std::string(rand_id));
            shadowMap->setType(S_TEXTURE_DEPTH_FRAME);
        }
    }

    return Light::loadShadow();
}