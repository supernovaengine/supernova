#include "DirectionalLight.h"
#include "math/Angle.h"
#include "Scene.h"
#include <algorithm>

using namespace Supernova;

DirectionalLight::DirectionalLight(): Light(){

    this->type = S_DIRECTIONAL_LIGHT;
    this->power = 1;

    this->numShadowCascades = 3;
    this->shadowSplitLogFactor = .7f;
    this->cascadeCameraNearFar.clear();

}

DirectionalLight::~DirectionalLight(){

}

void DirectionalLight::configLightOrthoCamera(Camera* lightCamera, Matrix4 sceneCameraInv) {

    Matrix4 t = *lightCamera->getViewMatrix() * sceneCameraInv;

    std::vector<Vector4> v = {
            t * Vector4(-1.f, 1.f, -1.f, 1.f),
            t * Vector4(1.f, 1.f, -1.f, 1.f),
            t * Vector4(1.f, -1.f, -1.f, 1.f),
            t * Vector4(-1.f, -1.f, -1.f, 1.f),
            t * Vector4(-1.f, 1.f, 1.f, 1.f),
            t * Vector4(1.f, 1.f, 1.f, 1.f),
            t * Vector4(1.f, -1.f, 1.f, 1.f),
            t * Vector4(-1.f, -1.f, 1.f, 1.f)
    };

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();

    for (auto& p : v)
    {
        p = p / p.w;

        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
        if (p.z < minZ) minZ = p.z;
        if (p.z > maxZ) maxZ = p.z;
    }

    lightCamera->setOrtho(minX, maxX, minY, maxY, -maxZ, -minZ);

}

float DirectionalLight::lerp(float a, float b, float fraction) {
    return (a * (1.0f - fraction)) + (b * fraction);
}

void DirectionalLight::updateLightCamera(){

    if (lightCameras.size() > 0) {

        if (scene && !scene->isDrawingShadow()) {

            float zFar = 5000;
            float zNear = 1;
            float fov = 0;
            float ratio = 1;

            std::vector<float> splitFar;
            std::vector<float> splitNear;

            if (scene->getCamera()->getType() == S_CAMERA_PERSPECTIVE) {

                Matrix4 projection = *scene->getCamera()->getProjectionMatrix();
                Matrix4 invProjection = projection.inverse();
                std::vector<Vector4> v1 = {
                    invProjection * Vector4(-1.f, 1.f, -1.f, 1.f),
                    invProjection * Vector4(1.f, 1.f, -1.f, 1.f),
                    invProjection * Vector4(1.f, -1.f, -1.f, 1.f),
                    invProjection * Vector4(-1.f, -1.f, -1.f, 1.f),
                    invProjection * Vector4(-1.f, 1.f, 1.f, 1.f),
                    invProjection * Vector4(1.f, 1.f, 1.f, 1.f),
                    invProjection * Vector4(1.f, -1.f, 1.f, 1.f),
                    invProjection * Vector4(-1.f, -1.f, 1.f, 1.f)
                };

                zFar = std::min(zFar, -(v1[4] / v1[4].w).z);
                zNear = -(v1[0] / v1[0].w).z;
                fov = atanf(1.f / projection[1][1]) * 2.f;
                ratio = projection[1][1] / projection[0][0];

                splitFar = std::vector<float> { zFar, zFar, zFar };
                splitNear = std::vector<float> { zNear, zNear, zNear };
                float j = 1.f;
                for (auto i = 0u; i < numShadowCascades - 1; ++i, j+= 1.f)
                {
                    splitFar[i] = lerp(
                            zNear + (j / (float)numShadowCascades) * (zFar - zNear),
                            zNear * powf(zFar / zNear, j / (float)numShadowCascades),
                            shadowSplitLogFactor
                    );
                    splitNear[i + 1] = splitFar[i];
                }

            }

            for (int ca = 0; ca < numShadowCascades; ca++) {

                lightCameras[ca]->setPosition(scene->getCamera()->getWorldPosition());
                lightCameras[ca]->setView(getDirection() + scene->getCamera()->getWorldPosition());

                //TODO: Check this
                Vector3 cameraDirection = (lightCameras[ca]->getPosition() - lightCameras[ca]->getView()).normalize();
                if (cameraDirection == Vector3(0, 1, 0)) {
                    lightCameras[ca]->setUp(0, 0, 1);
                } else {
                    lightCameras[ca]->setUp(0, 1, 0);
                }

                Matrix4 sceneCameraInv;

                if (scene->getCamera()->getType() == S_CAMERA_PERSPECTIVE) {

                    Matrix4 cameraProjection = Matrix4::perspectiveMatrix(fov, ratio, splitNear[ca], splitFar[ca]);
                    sceneCameraInv = (cameraProjection * *scene->getCamera()->getViewMatrix()).inverse();

                    cascadeCameraNearFar[ca] = Vector2(splitNear[ca], splitFar[ca]);

                }else{

                    sceneCameraInv = scene->getCamera()->getViewProjectionMatrix()->inverse();

                    cascadeCameraNearFar[ca] = scene->getCamera()->getNearFarPlane();

                }

                lightCameras[ca]->updateModelMatrix();

                configLightOrthoCamera(lightCameras[ca], sceneCameraInv);

                lightCameras[ca]->updateProjectionMatrix();
                lightCameras[ca]->updateViewProjectionMatrix();

                depthVPMatrix[ca] = (*lightCameras[ca]->getViewProjectionMatrix());

            }

        }
    }

    Light::updateLightCamera();
}

void DirectionalLight::setDirection(Vector3 direction){
    if (this->direction != direction) {
        this->direction = direction;
        needUpdate();
    }
}

void DirectionalLight::setDirection(float x, float y, float z){
    setDirection(Vector3(x, y, z));
}

Vector2 DirectionalLight::getCascadeCameraNearFar(int index){
    return cascadeCameraNearFar[index];
}

int DirectionalLight::getNumShadowCasdades(){
    return numShadowCascades;
}

float DirectionalLight::getShadowSplitLogFactor(){
    return shadowSplitLogFactor;
}

void DirectionalLight::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    Light::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    if (scene && !scene->isDrawingShadow()) {
        if (sceneCameraViewProjection != *viewProjectionMatrix) {
            sceneCameraViewProjection = *viewProjectionMatrix;
            updateLightCamera();
        }
    }
}

bool DirectionalLight::loadShadow(){
    if (useShadow) {

        if (scene && scene->getCamera()->getType() != S_CAMERA_PERSPECTIVE) {
            numShadowCascades = 1;
        }

        for (int ca = 0; ca < numShadowCascades; ca++) {

            if (lightCameras.size() < (ca+1))
                lightCameras.push_back(new Camera());

            if (depthVPMatrix.size() < (ca+1))
                depthVPMatrix.push_back(Matrix4());

            if (cascadeCameraNearFar.size() < (ca+1))
                cascadeCameraNearFar.push_back(Vector2());

            if (shadowMap.size() < (ca+1)) {
                shadowMap.push_back(new Texture(shadowMapWidth, shadowMapHeight));

                char rand_id[10];
                static const char alphanum[] =
                        "0123456789"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwxyz";
                for (int i = 0; i < 10; ++i) {
                    rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
                }

                shadowMap[ca]->setId("shadowMap|" + std::string(rand_id));
                shadowMap[ca]->setType(S_TEXTURE_DEPTH_FRAME);
            }

        }

        updateLightCamera();

    }

    return Light::loadShadow();
}
