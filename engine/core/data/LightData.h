#ifndef LightData_h
#define LightData_h

#include <vector>
#include "math/Vector3.h"
#include <Light.h>

namespace Supernova {

    class LightData {
        
    public:
        
        Vector3* ambientLight;
        
        int numPointLight;
        std::vector<float> pointLightPos;
        std::vector<float> pointLightPower;
        std::vector<float> pointLightColor;
        std::vector<int> pointLightShadowIdx;
        
        int numSpotLight;
        std::vector<float> spotLightPos;
        std::vector<float> spotLightPower;
        std::vector<float> spotLightColor;
        std::vector<float> spotLightCutOff;
        std::vector<float> spotLightTarget;
        std::vector<int> spotLightShadowIdx;
        
        int numDirectionalLight;
        std::vector<float> directionalLightDir;
        std::vector<float> directionalLightPower;
        std::vector<float> directionalLightColor;
        std::vector<int> directionalLightShadowIdx;

        int numShadows;
        int numShadowsCube;
        std::vector<Texture*> shadowsMap;
        std::vector<Texture*> shadowsMapCube;
        std::vector<Matrix4> shadowsVPMatrix;


        LightData();
        virtual ~LightData();
        
        bool updateLights(std::vector<Light*>* lights, Vector3* ambientLight);
    };
    
}

#endif /* LightData_h */
