
#ifndef scenemanager_h
#define scenemanager_h

#include "Light.h"
#include <vector>
#include "render/SceneRender.h"

class SceneManager {
private:
    SceneRender* scene;

    void instanciateRender();

public:
    SceneManager();
    virtual ~SceneManager();

    SceneRender* getRender();

    bool load();
    bool draw();
    bool viewSize(int x, int y, int width, int height);
};

#endif /* scenemanager_h */
