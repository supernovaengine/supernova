
#ifndef gles2scene_h
#define gles2scene_h

#include "Light.h"
#include <vector>
#include "render/SceneRender.h"

class GLES2Scene : public SceneRender{

public:

    static bool lighting;

    GLES2Scene();
    virtual ~GLES2Scene();
    
    void setAmbientLight(Vector3 ambientLight);
    void setLights(std::vector<Light*> lights);

    bool load(bool childScene);
    bool draw(bool childScene);
    bool viewSize(int x, int y, int width, int height);

};

#endif /* gles2scene_h */
