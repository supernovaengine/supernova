#ifndef SceneRender_h
#define SceneRender_h

#include "Light.h"
#include "Fog.h"
#include "LightData.h"
#include "math/Rect.h"

namespace Supernova {

    class Scene;

    class SceneRender {
        
    private:
        void fillSceneProperties();
        
        Fog* fog;
        LightData lightData;
        
    protected:

        bool childScene;
        bool useDepth;
        bool useTransparency;
        
        Scene* scene;

        void updateLights();
        
    public:
        bool lighting;

        SceneRender();
        virtual ~SceneRender();

        static void newInstance(SceneRender** render);
        
        void setScene(Scene* scene);

        LightData* getLightData();
        Fog* getFog();

        virtual bool load();
        virtual bool draw();
        virtual bool viewSize(Rect rect) = 0;
        virtual bool enableScissor(Rect rect) = 0;
        virtual bool disableScissor() = 0;

        virtual bool isEnabledScissor() = 0;
        virtual Rect getActiveScissor() = 0;
    };
    
}

#endif /* SceneRender_h */
