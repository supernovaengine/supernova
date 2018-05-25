#ifndef scene_h
#define scene_h

#define S_OPTION_NO 0
#define S_OPTION_YES 1
#define S_OPTION_AUTOMATIC 2

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
#include "util/LightData.h"
#include "math/Matrix4.h"
#include "physics/PhysicsWorld.h"

namespace Supernova {

    class Scene: public Object {
        friend class Engine;
        friend class Object;
        friend class ConcreteObject;
        friend class Mesh;
        friend class Points;
    private:

        SceneRender* render;
        ObjectRender* lightRender;
        ObjectRender* fogRender;
        Texture* textureRender;

        Vector3 drawShadowLightPos;
        Vector2 drawShadowCameraNearFar;
        bool drawIsPointShadow;
        
        LightData lightData;

        Matrix4 viewProjectionMatrix;

        Camera* camera;
        bool userCamera;
        
        std::multimap<float, ConcreteObject*> transparentQueue;

        std::vector<Light*> lights;
        std::vector<Scene*> subScenes;
        std::vector<GUIObject*> guiObjects;
        std::vector<PhysicsWorld*> physicsWorlds;
        SkyBox* sky;
        Fog* fog;
        
        Vector3 ambientLight;

        bool drawingShadow;
        bool childScene;
        bool useTransparency;
        bool useDepth;
        bool useLight;

        bool ownedPhysics;

        // S_OPTION
        int userDefinedTransparency;
        int userDefinedDepth;
        
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

        void drawChildScenes();
        bool renderDraw(bool shadowMap=false, bool cubeMap=false, int cubeFace=0);

    public:

        Scene();
        virtual ~Scene();
        
        SceneRender* getSceneRender();
        ObjectRender* getLightRender();
        ObjectRender* getFogRender();
        Vector3 getDrawShadowLightPos();

        void setAmbientLight(Vector3 ambientLight);
        void setAmbientLight(const float ambientFactor);

        void addPhysics (PhysicsWorld* physics);
        void removePhysics (PhysicsWorld* physics);
        
        Vector3* getAmbientLight();
        std::vector<Light*>* getLights();
        LightData* getLightData();

        bool isDrawingShadow();
        bool isChildScene();
        bool isUseDepth();
        bool isUseLight();
        bool isUseTransparency();

        void setTransparency(bool transparency);
        void setDepth(bool depth);

        int getUserDefinedTransparency();
        int getUserDefinedDepth();
        
        void setFog(Fog* fog);

        bool is3D();

        void setCamera(Camera* camera);
        Camera* getCamera();

        void doCamera();

        void setTextureRender(Texture* textureRender);
        Texture* getTextureRender();

        bool updateCameraSize();

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

        bool isOwnedPhysics();
        void setOwnedPhysics(bool ownedPhysics);

        void updatePhysics(float time);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
    
}

#endif /* scene_h */
