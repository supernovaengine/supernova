#ifndef scene_h
#define scene_h

#include <vector>
#include "Object.h"
#include "Camera.h"
#include "Render.h"
#include "render/SceneManager.h"
#include "Light.h"
#include <vector>
#include <map>
#include "GUIObject.h"

#include "math/Matrix4.h"

class Scene: public Object {
    friend class Object;
    friend class Mesh;
private:

    SceneManager sceneManager;

    Matrix4 viewProjectionMatrix;

    Camera* camera;
    bool userCamera;
    
    std::multimap<float, Mesh*> transparentMeshQueue;

    std::vector<Light*> lights;
    std::vector<Scene*> subScenes;
    std::vector<GUIObject*> guiObjects;
    
    Vector3 ambientLight;

    bool isChildScene;
	bool useTransparency;
	bool useDepth;
    
    void addLight (Light* light);
    void removeLight (Light* light);
    
    void addSubScene (Scene* scene);
    void removeSubScene (Scene* scene);
    
    void addGUIObject (GUIObject* guiobject);
    void removeGUIObject (GUIObject* guiobject);

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
