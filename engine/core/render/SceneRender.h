#ifndef SceneRender_h
#define SceneRender_h

class SceneRender {
    
    
public:
    
    inline virtual ~SceneRender(){}
    
    virtual void setLights(std::vector<Light*> lights) = 0;
    virtual void setAmbientLight(Vector3 ambientLight) = 0;

    virtual bool load() = 0;
    virtual bool draw() = 0;
    virtual bool screenSize(int width, int height) = 0;
    
};

#endif /* SceneRender_h */
