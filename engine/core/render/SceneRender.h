#ifndef SceneRender_h
#define SceneRender_h

#include "Light.h"

class SceneRender {
    
public:
    
    inline virtual ~SceneRender(){}
    
    virtual void setLights(std::vector<Light*> lights) = 0;
    virtual void setAmbientLight(Vector3 ambientLight) = 0;

    virtual bool load(bool childScene) = 0;
    virtual bool draw(bool childScene, bool useDepth, bool useTransparency) = 0;
    virtual bool viewSize(int x, int y, int width, int height) = 0;
    
};

#endif /* SceneRender_h */
