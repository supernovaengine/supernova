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
#include "ui/UIObject.h"
#include "util/LightData.h"
#include "math/Matrix4.h"
#include "physics/PhysicsWorld.h"

namespace Supernova {

    class Scene: public Object {
        friend class Engine;
        friend class Object;
        friend class GraphicObject;
        friend class Mesh;
        friend class Points;
    private:

        SceneRender* render;
        Texture* textureRender;

        Vector3 drawShadowLightPos;
        Vector2 drawShadowCameraNearFar;
        bool drawIsPointShadow;
        
        LightData lightData;

        Matrix4 viewProjectionMatrix;

        Camera* camera;
        bool userCamera;
        
        std::multimap<float, GraphicObject*> transparentQueue;

        std::vector<Light*> lights;
        std::vector<Scene*> subScenes;
        std::vector<UIObject*> guiObjects;

        PhysicsWorld* physicsWorld;
        SkyBox* sky;
        Fog* fog;
        
        Vector3 ambientLight;

        bool loadedShadow;
        bool drawingShadow;
        bool childScene;
        bool useTransparency;
        bool useDepth;
        bool useLight;

        bool ownedPhysicsWorld;

        // S_OPTION
        int userDefinedTransparency;
        int userDefinedDepth;
        
        void addLight (Light* light);
        void removeLight (Light* light);
        
        void addSubScene (Scene* scene);
        void removeSubScene (Scene* scene);
        
        void addGUIObject (UIObject* guiobject);
        void removeGUIObject (UIObject* guiobject);

        void setSky(SkyBox* sky);

        bool addLightProperties(ObjectRender* render);
        bool addFogProperties(ObjectRender* render);

        void resetSceneProperties();
        void drawTransparentMeshes();
        void drawSky();

        void drawChildScenes();
        bool renderDraw(bool shadowMap=false, bool cubeMap=false, int cubeFace=0);

    public:

        Scene();
        virtual ~Scene();
        
        SceneRender* getSceneRender();
        Vector3 getDrawShadowLightPos();

        void setAmbientLight(Vector3 ambientLight);
        void setAmbientLight(const float ambientFactor);

        void setOwnedPhysicsWorld(bool ownedPhysicsWorld);

        PhysicsWorld2D* createPhysicsWorld2D();
        //void createPhysicsWorld3D();

        void setPhysicsWorld(PhysicsWorld* physicsWorld);
        PhysicsWorld* getPhysicsWorld();
        
        Vector3* getAmbientLight();
        std::vector<Light*>* getLights();
        LightData* getLightData();

        bool isLoadedShadow();
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

        void updatePhysics(float time);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
    
}

#endif /* scene_h */
