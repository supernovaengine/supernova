
#ifndef gles2scene_h
#define gles2scene_h

#include "Light.h"
#include <vector>
#include "render/SceneRender.h"
#include "GLES2Header.h"

namespace Supernova {

    class GLES2Scene : public SceneRender{
        
    friend class GLES2Mesh;
    friend class GLES2Point;
        
    public:
        
        GLES2Scene();
        virtual ~GLES2Scene();

        virtual bool load();
        virtual bool draw();
        virtual bool clear(float value = 0);
        virtual bool viewSize(Rect rect);
        virtual bool enableScissor(Rect rect);
        virtual bool disableScissor();

        virtual bool isEnabledScissor();
        virtual Rect getActiveScissor();
    };
    
}

#endif /* gles2scene_h */
