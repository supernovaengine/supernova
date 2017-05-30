#ifndef SceneRender_h
#define SceneRender_h

#include "Light.h"
#include "Fog.h"
#include "LightRender.h"
#include "Rect.h"

class Scene;

class SceneRender {
    
private:
    void fillSceneProperties();
    
    Vector3* ambientLight;
    std::vector<Light*>* lights;
    Fog* fog;
    
protected:

    bool childScene;
    bool useDepth;
    bool useTransparency;
    
    Scene* scene;
    
    LightRender lightRender;

    void updateLights();
    
public:
    bool lighting;

    SceneRender();
    virtual ~SceneRender();
    
    void setScene(Scene* scene);
    
    LightRender* getLightRender();
    Fog* getFog();

    virtual bool load();
    virtual bool draw();
    virtual bool viewSize(Rect rect) = 0;
    
};

#endif /* SceneRender_h */
