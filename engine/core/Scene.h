#ifndef scene_h
#define scene_h

#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Render.h"
#include "render/SceneRender.h"
#include "Light.h"

#include "math/Matrix4.h"

class Scene: public Render {
private:

    SceneRender sceneRender;

    Matrix4 viewProjectionMatrix;

    Camera* camera;
    bool userCamera;

    std::vector<Light*> lights;
    Vector3 ambientLight;

    Object baseObject;

public:

	Scene();
	virtual ~Scene();

    void addObject (Object* obj);

    void addLight (Light* light);
    void removeLight (Light* light);

    void setAmbientLight(Vector3 ambientLight);
    void setAmbientLight(const float ambientFactor);
    Vector3 getAmbientLight();

    void setCamera(Camera* camera);
    Camera* getCamera();

    Object* getBaseObject();

    void doCamera();

    bool screenSize(int width, int height);

	bool load();
	bool draw();
    void destroy();
};

#endif /* scene_h */
