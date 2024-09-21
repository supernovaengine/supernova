//
// (c) 2024 Eduardo Doria.
//

#ifndef SokolCamera_h
#define SokolCamera_h

#include "math/Rect.h"
#include "math/Vector4.h"
#include "render/FramebufferRender.h"

#include "sokol_gfx.h"

namespace Supernova{
    class SokolCamera{

    private:
        sg_pass pass;

    public:
        SokolCamera();
        SokolCamera(const SokolCamera& rhs);
        SokolCamera& operator=(const SokolCamera& rhs);
        virtual ~SokolCamera();

        void setClearColor(Vector4 clearColor);

        void startFrameBuffer(FramebufferRender* framebuffer, size_t face);
        void startFrameBuffer(int width, int height);
        void startFrameBuffer();

        void applyViewport(Rect rect);
        void applyScissor(Rect rect);

        void endFrameBuffer();
    };
}

#endif //SokolCamera_h
