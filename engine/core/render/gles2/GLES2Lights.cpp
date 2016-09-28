
#include "GLES2Lights.h"

Vector3 GLES2Lights::ambientLight;

int GLES2Lights::numPointLight;
std::vector<GLfloat> GLES2Lights::pointLightPos;
std::vector<GLfloat> GLES2Lights::pointLightPower;
std::vector<GLfloat> GLES2Lights::pointLightColor;

int GLES2Lights::numSpotLight;
std::vector<GLfloat> GLES2Lights::spotLightPos;
std::vector<GLfloat> GLES2Lights::spotLightPower;
std::vector<GLfloat> GLES2Lights::spotLightColor;
std::vector<GLfloat> GLES2Lights::spotLightCutOff;
std::vector<GLfloat> GLES2Lights::spotLightTarget;

int GLES2Lights::numDirectionalLight;
std::vector<GLfloat> GLES2Lights::directionalLightDir;
std::vector<GLfloat> GLES2Lights::directionalLightPower;
std::vector<GLfloat> GLES2Lights::directionalLightColor;
