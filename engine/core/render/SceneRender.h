#ifndef SceneRender_h
#define SceneRender_h

#include "Light.h"

class SceneRender {
    
protected:
    
public:

    Vector3 ambientLight;

    int numPointLight;
    std::vector<float> pointLightPos;
    std::vector<float> pointLightPower;
    std::vector<float> pointLightColor;

    int numSpotLight;
    std::vector<float> spotLightPos;
    std::vector<float> spotLightPower;
    std::vector<float> spotLightColor;
    std::vector<float> spotLightCutOff;
    std::vector<float> spotLightTarget;

    int numDirectionalLight;
    std::vector<float> directionalLightDir;
    std::vector<float> directionalLightPower;
    std::vector<float> directionalLightColor;
    
    bool lighting;

    SceneRender();
    virtual ~SceneRender();
    
    void setLights(std::vector<Light*> lights);
    void setAmbientLight(Vector3 ambientLight);

    virtual bool load(bool childScene) = 0;
    virtual bool draw(bool childScene, bool useDepth, bool useTransparency) = 0;
    virtual bool viewSize(int x, int y, int width, int height) = 0;
    
};

#endif /* SceneRender_h */
