#include "DirectionalLight.h"
#include "math/Angle.h"
#include "Scene.h"
#include <algorithm>

using namespace Supernova;

DirectionalLight::DirectionalLight(): Light(){

    this->type = S_DIRECTIONAL_LIGHT;
    this->power = 1;

}

DirectionalLight::~DirectionalLight(){

}

void DirectionalLight::configLightOrthoCamera(Camera* lightCamera, Camera* sceneCamera) {

    Matrix4 lvMatrix = *lightCamera->getViewMatrix();
    Matrix4 cameraInv;

    if (sceneCamera->getType() == S_CAMERA_PERSPECTIVE) {

        Matrix4 projection = *sceneCamera->getProjectionMatrix();
        Matrix4 invProjection = projection.getInverse();
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

        float zFar = 100;

        zFar = std::min(zFar, -(v1[4] / v1[4].w).z);

        float zNear = -(v1[0] / v1[0].w).z;
        float fov = atanf(1.f / projection[1][1]) * 2.f;
        float ratio = projection[1][1] / projection[0][0];

        //https://github.com/aerys/minko/blob/master/framework/src/minko/component/DirectionalLight.cpp
        /*
        int numShadowCascades = 4;
        auto splitFar = std::vector<float> { zFar, zFar, zFar, zFar };
        auto splitNear = std::vector<float> { zNear, zNear, zNear, zNear };
        float lambda = .5f;
        float j = 1.f;
        for (auto i = 0u; i < numShadowCascades - 1; ++i, j+= 1.f)
        {
            splitFar[i] = math::mix(
                    zNear + (j / (float)numShadowCascades) * (zFar - zNear),
                    zNear * powf(zFar / zNear, j / (float)numShadowCascades),
                    lambda
            );
            splitNear[i + 1] = splitFar[i];
        }
        */

        Matrix4 cameraProjection = Matrix4::perspectiveMatrix(fov, ratio, zNear, zFar);
        cameraInv = (cameraProjection * *sceneCamera->getViewMatrix()).getInverse();

        shadowCameraNearFar = Vector2(zNear, zFar);

    }else{

        cameraInv = sceneCamera->getViewProjectionMatrix()->getInverse();

        shadowCameraNearFar = sceneCamera->getNearFarPlane();

    }

    Matrix4 t = lvMatrix * cameraInv;
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

    for (auto& p : v)
        p = p / p.w;

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();

    for (const auto& p : v)
    {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
        if (p.z < minZ) minZ = p.z;
        if (p.z > maxZ) maxZ = p.z;
    }

    lightCamera->setOrtho(minX, maxX, minY, maxY, minZ, maxZ);

}

void DirectionalLight::updateLightCamera(){

    if (lightCameras.size() > 0) {
        if (scene && !scene->isDrawingShadow()) {

            lightCameras[0]->setPosition(Vector3(0, 0, 0));
            lightCameras[0]->setView(getDirection());

            //TODO: Check this
            Vector3 cameraDirection = (lightCameras[0]->getPosition() - lightCameras[0]->getView()).normalize();
            if (cameraDirection == Vector3(0, 1, 0)) {
                lightCameras[0]->setUp(0, 0, 1);
            } else {
                lightCameras[0]->setUp(0, 1, 0);
            }

            configLightOrthoCamera(lightCameras[0], scene->getCamera());

        }

        depthVPMatrix = (*lightCameras[0]->getViewProjectionMatrix());
    }

    Light::updateLightCamera();
}

void DirectionalLight::setDirection(Vector3 direction){
    if (this->direction != direction) {
        this->direction = direction;

        updateLightCamera();

    }
}

void DirectionalLight::setDirection(float x, float y, float z){
    setDirection(Vector3(x, y, z));
}

Vector2 DirectionalLight::getShadowCameraNearFar(){
    return shadowCameraNearFar;
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
