#ifndef scene_h
#define scene_h

#define S_ORIENTATION_Y_DOWN 1
#define S_ORIENTATION_Y_UP 2

#include "Object.h"
#include "Camera.h"
#include "Render.h"
#include "render/SceneRender.h"
#include "Light.h"
#include "Fog.h"
#include "SkyBox.h"
#include <vector>
#include <map>
#include "GUIObject.h"

#include "math/Matrix4.h"

namespace Supernova {

    class Scene: public Object {
        friend class Object;
        friend class ConcreteObject;
        friend class Mesh;
    private:

        SceneRender* render;

        Matrix4 viewProjectionMatrix;

        Camera* camera;
        bool userCamera;
        
        std::multimap<float, ConcreteObject*> transparentQueue;

        std::vector<Light*> lights;
        std::vector<Scene*> subScenes;
        std::vector<GUIObject*> guiObjects;
        SkyBox* sky;
        Fog* fog;
        
        Vector3 ambientLight;

        bool childScene;
        bool useTransparency;
        bool useDepth;
        
        void addLight (Light* light);
        void removeLight (Light* light);
        
        void addSubScene (Scene* scene);
        void removeSubScene (Scene* scene);
        
        void addGUIObject (GUIObject* guiobject);
        void removeGUIObject (GUIObject* guiobject);

        void setSky(SkyBox* sky);

        void resetSceneProperties();
        void drawTransparentMeshes();
        void drawSky();

        void updateChildScenes();

    public:

        Scene();
        virtual ~Scene();
        
        SceneRender* getSceneRender();

        void setAmbientLight(Vector3 ambientLight);
        void setAmbientLight(const float ambientFactor);
        
        Vector3* getAmbientLight();
        std::vector<Light*>* getLights();
        
        bool isChildScene();
        bool isUseDepth();
        bool isUseTransparency();
        
        void setFog(Fog* fog);

        int getOrientation();

        void setCamera(Camera* camera);
        Camera* getCamera();

        void doCamera();

        bool updateViewSize();
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

        virtual void update();
        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
    
}

#endif /* scene_h */
