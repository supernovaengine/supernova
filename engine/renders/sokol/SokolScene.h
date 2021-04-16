#ifndef sokolscene_h
#define sokolscene_h

#include "math/Rect.h"

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

        void startFrameBuffer();
        void applyViewport(Rect rect);
        void endFrameBuffer();
    };
}

#endif //sokolscene_h
