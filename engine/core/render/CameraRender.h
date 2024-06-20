//
// (c) 2024 Eduardo Doria.
//

#ifndef CameraRender_h
#define CameraRender_h

#include "math/Rect.h"
#include "sokol/SokolCamera.h"

namespace Supernova{
    class CameraRender{        
    public:
        //***Backend***
        SokolCamera backend;
        //***

        CameraRender();
        CameraRender(const CameraRender& rhs);
        CameraRender& operator=(const CameraRender& rhs);
        virtual ~CameraRender();

        void setClearColor(Vector4 clearColor);

        void startFrameBuffer(FramebufferRender* framebuffer, size_t face = 0);
        void startFrameBuffer();

        void applyViewport(Rect rect);
        void applyScissor(Rect rect);

        void endFrameBuffer();
    };
}


#endif /* CameraRender_h */