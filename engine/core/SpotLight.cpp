#include "SpotLight.h"

#include "math/Angle.h"
#include <stdlib.h>

using namespace Supernova;

SpotLight::SpotLight(): Light(){
    type = S_SPOT_LIGHT;
    this->power = 50;

    this->smooth = Angle::degToRad(2);

    this->spotOuterAngle = this->spotAngle + this->smooth;
}

SpotLight::~SpotLight(){

}

void SpotLight::updateLightCamera(){

    lightCameras[0]->setPosition(getWorldPosition());
    lightCameras[0]->setView(getWorldTarget());
    lightCameras[0]->setPerspective(Angle::radToDefault(spotOuterAngle), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);

    //TODO: Check this
    Vector3 cameraDirection = (lightCameras[0]->getPosition() - lightCameras[0]->getView()).normalize();
    if (cameraDirection == Vector3(0, 1, 0)) {
        lightCameras[0]->setUp(0, 0, 1);
    } else {
        lightCameras[0]->setUp(0, 1, 0);
    }

    lightCameras[0]->updateModelMatrix();

    depthVPMatrix[0] = (*lightCameras[0]->getViewProjectionMatrix());

    Light::updateLightCamera();
}

void SpotLight::setTarget(Vector3 target){
    if (this->target != target){
        this->target = target;
        needUpdate();
    }
}

void SpotLight::setTarget(float x, float y, float z){
    setTarget(Vector3(x, y, z));
}

void SpotLight::setSpotAngle(float angle){
    this->spotAngle = Angle::defaultToRad(angle);
    this->spotOuterAngle = this->spotAngle + this->smooth;
}

void SpotLight::setSmooth(float angle){
    this->smooth = Angle::defaultToRad(angle);
    this->spotOuterAngle = this->spotAngle + this->smooth;
}

bool SpotLight::loadShadow(){
    if (useShadow){
        if (lightCameras.size()==0)
            lightCameras.push_back(new Camera());

        if (depthVPMatrix.size()==0)
            depthVPMatrix.push_back(Matrix4());

        if (shadowMap.size()==0) {
            shadowMap.push_back(new Texture(shadowMapWidth, shadowMapHeight));

            char rand_id[10];
            static const char alphanum[] =
                    "0123456789"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz";
            for (int i = 0; i < 10; ++i) {
                rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
            }

            shadowMap[0]->setId("shadowMap|" + std::string(rand_id));
            shadowMap[0]->setType(S_TEXTURE_DEPTH_FRAME);
        }

        updateLightCamera();

    }

    return Light::loadShadow();
}