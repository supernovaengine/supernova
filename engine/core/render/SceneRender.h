//
// (c) 2024 Eduardo Doria.
//

#ifndef SceneRender_h
#define SceneRender_h

#include "math/Rect.h"
#include "sokol/SokolScene.h"

namespace Supernova{
    class SceneRender{        
    public:
        //***Backend***
        SokolScene backend;
        //***

        SceneRender();
        SceneRender(const SceneRender& rhs);
        SceneRender& operator=(const SceneRender& rhs);
        virtual ~SceneRender();

        void setClearColor(Vector4 clearColor);

        void startFrameBuffer(FramebufferRender* framebuffer, size_t face = 0);
        void startFrameBuffer();

        void applyViewport(Rect rect);
        void applyScissor(Rect rect);

        void endFrameBuffer();
    };
}


#endif /* SceneRender_h */