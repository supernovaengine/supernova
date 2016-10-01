
#ifndef gles2scene_h
#define gles2scene_h

#include "GLES2Header.h"
#include "Light.h"
#include <vector>
#include "render/SceneRender.h"

class GLES2Scene : public SceneRender{

public:

    static bool lighting;

    GLES2Scene();
    virtual ~GLES2Scene();
    
    void updateLights(std::vector<Light*> lights, Vector3 ambientLight);

    bool load();
    bool draw();
    bool viewSize(int x, int y, int width, int height);

};

#endif /* gles2scene_h */
