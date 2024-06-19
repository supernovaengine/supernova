//
// (c) 2024 Eduardo Doria.
//

#ifndef sokolscene_h
#define sokolscene_h

#include "math/Rect.h"
#include "math/Vector4.h"
#include "render/FramebufferRender.h"

#include "sokol_gfx.h"

namespace Supernova{
    class SokolScene{

    private:
        sg_pass_action pass_action;

    public:
        SokolScene();
        SokolScene(const SokolScene& rhs);
        SokolScene& operator=(const SokolScene& rhs);
        virtual ~SokolScene();

        void setClearColor(Vector4 clearColor);

        void startFrameBuffer(FramebufferRender* framebuffer, size_t face);
        void startFrameBuffer();

        void applyViewport(Rect rect);
        void applyScissor(Rect rect);

        void endFrameBuffer();
    };
}

#endif //sokolscene_h
