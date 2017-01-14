#ifndef scene_h
#define scene_h

#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Render.h"
#include "render/SceneManager.h"
#include "Light.h"

#include "math/Matrix4.h"

class Scene: public Object {
    friend class Object;
private:

    SceneManager sceneManager;

    Matrix4 viewProjectionMatrix;

    Camera* camera;
    bool userCamera;

    std::vector<Light*> lights;
    Vector3 ambientLight;

	Scene* childScene;
    
    void addLight (Light* light);
    void removeLight (Light* light);

public:

	Scene();
	virtual ~Scene();

    void setAmbientLight(Vector3 ambientLight);
    void setAmbientLight(const float ambientFactor);
    Vector3 getAmbientLight();

    void setCamera(Camera* camera);
    Camera* getCamera();

	void setChildScene(Scene* childScene);
	Scene* getChildScene();

    void doCamera();

    bool updateViewSize();

	bool load();
	bool draw();
    void destroy();
};

#endif /* scene_h */
