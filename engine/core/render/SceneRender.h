
#ifndef scenerender_h
#define scenerender_h

#include "gles2/GLES2Scene.h"
#include "Light.h"
#include <vector>

class SceneRender {
private:
    GLES2Scene *scene;

    void instanciateRender();
public:
    SceneRender();
    virtual ~SceneRender();

    void setLights(std::vector<Light*> lights);
    void setAmbientLight(Vector3 ambientLight);

    bool load();
    bool draw();
    bool screenSize(int width, int height);
};

#endif /* scenerender_h */
