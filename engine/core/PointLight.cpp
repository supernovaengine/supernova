#include "PointLight.h"
#include "math/Angle.h"

using namespace Supernova;

PointLight::PointLight(): Light(){
    type = S_POINT_LIGHT;
    this->power = 50;
}

PointLight::~PointLight(){

}

void PointLight::updateLightCamera(){

    lightCameras[0]->setPosition(getWorldPosition());
    lightCameras[0]->setView(getWorldPosition() + Vector3(1,0,0));
    lightCameras[0]->setPerspective(Angle::degToDefault(90), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);
    lightCameras[0]->setUp(0, -1, 0);

    lightCameras[1]->setPosition(getWorldPosition());
    lightCameras[1]->setView(getWorldPosition() + Vector3(-1,0,0));
    lightCameras[1]->setPerspective(Angle::degToDefault(90), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);
    lightCameras[1]->setUp(0, -1, 0);

    lightCameras[2]->setPosition(getWorldPosition());
    lightCameras[2]->setView(getWorldPosition() + Vector3(0,1,0));
    lightCameras[2]->setPerspective(Angle::degToDefault(90), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);
    lightCameras[2]->setUp(0, 0, 1);

    lightCameras[3]->setPosition(getWorldPosition());
    lightCameras[3]->setView(getWorldPosition() + Vector3(0,-1,0));
    lightCameras[3]->setPerspective(Angle::degToDefault(90), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);
    lightCameras[3]->setUp(0, 0, -1);

    lightCameras[4]->setPosition(getWorldPosition());
    lightCameras[4]->setView(getWorldPosition() + Vector3(0,0,1));
    lightCameras[4]->setPerspective(Angle::degToDefault(90), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);
    lightCameras[4]->setUp(0, -1, 0);

    lightCameras[5]->setPosition(getWorldPosition());
    lightCameras[5]->setView(getWorldPosition() + Vector3(0,0,-1));
    lightCameras[5]->setPerspective(Angle::degToDefault(90), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);
    lightCameras[5]->setUp(0, -1, 0);

    Light::updateLightCamera();
}

bool PointLight::loadShadow(){
    if (useShadow){
        if (lightCameras.size()==0) {
            lightCameras.push_back(new Camera());
            lightCameras.push_back(new Camera());
            lightCameras.push_back(new Camera());
            lightCameras.push_back(new Camera());
            lightCameras.push_back(new Camera());
            lightCameras.push_back(new Camera());
        }
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

            shadowMap->setId("shadowCubeMap|" + std::string(rand_id));
            shadowMap->setType(S_TEXTURE_FRAME_CUBE);
        }
    }

    return Light::loadShadow();
}
