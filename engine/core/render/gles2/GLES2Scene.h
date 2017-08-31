
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
        virtual bool clear();
        virtual bool viewSize(Rect rect, bool adjustY=true);
        virtual bool enableScissor(Rect rect);
        virtual bool disableScissor();

        virtual bool isEnabledScissor();
        virtual Rect getActiveScissor();
    };
    
}

#endif /* gles2scene_h */
