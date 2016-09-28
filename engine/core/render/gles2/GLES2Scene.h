
#ifndef gles2scene_h
#define gles2scene_h

#include "GLES2Header.h"
#include "Light.h"
#include <vector>
#include "render/SceneRender.h"

class GLES2Scene : public SceneRender{
private:
    static std::vector<Light*> lights;
    static Vector3 ambientLight;

public:

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


    GLES2Scene();
    virtual ~GLES2Scene();

    void setLights(std::vector<Light*> lights);
    void setAmbientLight(Vector3 ambientLight);

    static std::vector<Light*> getLights();
    static Vector3 getAmbientLight();

    bool load();
    bool draw();
    bool screenSize(int width, int height);

};

#endif /* gles2scene_h */
