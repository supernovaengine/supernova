
#ifndef scenemanager_h
#define scenemanager_h

#include "gles2/GLES2Scene.h"
#include "Light.h"
#include <vector>
#include "render/SceneRender.h"

class SceneManager {
private:
    SceneRender *scene;
    
    std::vector<Light*> lights;
    Vector3 ambientLight;

    void instanciateRender();
    void updateLights();
public:
    SceneManager();
    virtual ~SceneManager();

    void setLights(std::vector<Light*> lights);
    void setAmbientLight(Vector3 ambientLight);

    bool load();
    bool draw();
    bool viewSize(int x, int y, int width, int height);
};

#endif /* scenemanager_h */
