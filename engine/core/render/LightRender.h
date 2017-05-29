#ifndef LightRender_h
#define LightRender_h

#include <vector>
#include <Vector3.h>
#include <Light.h>

class LightRender {
    
public:
    
    Vector3* ambientLight;
    
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
    
    
    LightRender();
    virtual ~LightRender();
    
    bool updateLights(std::vector<Light*>* lights, Vector3* ambientLight);
};

#endif /* LightRender_h */
