#ifndef SceneRender_h
#define SceneRender_h

#include "Light.h"
#include "Fog.h"
#include "math/Rect.h"

namespace Supernova {

    class SceneRender {
        
    protected:

        bool useLight;
        bool childScene;
        bool useDepth;
        bool useTransparency;
        
        Scene* scene;

        void updateLights();

        SceneRender();
        
    public:
        static SceneRender* newInstance();

        virtual ~SceneRender();

        void setUseLight(bool useLight);
        void setChildScene(bool childScene);
        void setUseDepth(bool useDepth);
        void setUseTransparency(bool useTransparency);

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
