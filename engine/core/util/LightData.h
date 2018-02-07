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
        std::vector<float> spotLightColor;
        std::vector<float> spotLightTarget;
        std::vector<float> spotLightPower;
        std::vector<float> spotLightCutOff;
        std::vector<float> spotLightOuterCutOff;
        std::vector<int> spotLightShadowIdx;
        
        int numDirectionalLight;
        std::vector<float> directionalLightDir;
        std::vector<float> directionalLightPower;
        std::vector<float> directionalLightColor;
        std::vector<int> directionalLightShadowIdx;

        int numShadows2D;
        std::vector<Texture*> shadowsMap2D;
        std::vector<Matrix4> shadowsVPMatrix;
        std::vector<float> shadowsBias2D;
        std::vector<Vector2> shadowsCameraNearFar2D;
        std::vector<int> shadowNumCascades2D;

        int numShadowsCube;
        std::vector<Texture*> shadowsMapCube;
        std::vector<float> shadowsBiasCube;
        std::vector<Vector2> shadowsCameraNearFarCube;


        LightData();
        virtual ~LightData();
        
        bool updateLights(std::vector<Light*>* lights, Vector3* ambientLight);
    };
    
}

#endif /* LightData_h */
