
#ifndef GLES2Lights_h
#define GLES2Lights_h

#include "GLES2Header.h"
#include "math/Vector3.h"
#include <vector>

class GLES2Lights {
public:
    static Vector3 ambientLight;
    
    static int numPointLight;
    static std::vector<GLfloat> pointLightPos;
    static std::vector<GLfloat> pointLightPower;
    static std::vector<GLfloat> pointLightColor;
    
    static int numSpotLight;
    static std::vector<GLfloat> spotLightPos;
    static std::vector<GLfloat> spotLightPower;
    static std::vector<GLfloat> spotLightColor;
    static std::vector<GLfloat> spotLightCutOff;
    static std::vector<GLfloat> spotLightTarget;
    
    static int numDirectionalLight;
    static std::vector<GLfloat> directionalLightDir;
    static std::vector<GLfloat> directionalLightPower;
    static std::vector<GLfloat> directionalLightColor;
    
};

#endif /* GLES2Lights_h */
