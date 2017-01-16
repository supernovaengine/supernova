#ifndef scene_h
#define scene_h

#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Render.h"
#include "render/SceneManager.h"
#include "Light.h"
#include <vector>

#include "math/Matrix4.h"

class Scene: public Object {
    friend class Object;
private:

    SceneManager sceneManager;

    Matrix4 viewProjectionMatrix;

    Camera* camera;
    bool userCamera;

    std::vector<Light*> lights;
    
    std::vector<Scene*> subScenes;
    
    Vector3 ambientLight;

    bool isChildScene;
    
    void addLight (Light* light);
    void removeLight (Light* light);
    
    void addSubScene (Scene* scene);
    void removeSubScene (Scene* scene);

public:

	Scene();
	virtual ~Scene();

    void setAmbientLight(Vector3 ambientLight);
    void setAmbientLight(const float ambientFactor);
    Vector3 getAmbientLight();
    
    void transform(Matrix4* viewMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

    void setCamera(Camera* camera);
    Camera* getCamera();

    void doCamera();

    bool updateViewSize();

	bool load();
	bool draw();
    void destroy();
};

#endif /* scene_h */
