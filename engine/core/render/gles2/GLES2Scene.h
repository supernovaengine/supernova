
#ifndef gles2scene_h
#define gles2scene_h

#include "Light.h"
#include <vector>
#include "render/SceneRender.h"
#include "GLES2Header.h"

class GLES2Scene : public SceneRender{

public:
    
    Vector3 ambientLight;
    
    int numPointLight;
    std::vector<GLfloat> pointLightPos;
    std::vector<GLfloat> pointLightPower;
    std::vector<GLfloat> pointLightColor;
    
    int numSpotLight;
    std::vector<GLfloat> spotLightPos;
    std::vector<GLfloat> spotLightPower;
    std::vector<GLfloat> spotLightColor;
    std::vector<GLfloat> spotLightCutOff;
    std::vector<GLfloat> spotLightTarget;
    
    int numDirectionalLight;
    std::vector<GLfloat> directionalLightDir;
    std::vector<GLfloat> directionalLightPower;
    std::vector<GLfloat> directionalLightColor;

    bool lighting;

    GLES2Scene();
    virtual ~GLES2Scene();
    
    void setAmbientLight(Vector3 ambientLight);
    void setLights(std::vector<Light*> lights);

    bool load(bool childScene);
    bool draw(bool childScene);
    bool viewSize(int x, int y, int width, int height);

};

#endif /* gles2scene_h */
