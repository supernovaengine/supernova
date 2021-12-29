#ifndef SokolShader_h
#define SokolShader_h

#include "render/Render.h"
#include "shader/ShaderData.h"

#include "sokol_gfx.h"

namespace Supernova{
    class SokolShader{

    private:
        sg_shader shader;

        int roundup(int val, int round_to);

    public:
        SokolShader();
        SokolShader(const SokolShader& rhs);
        SokolShader& operator=(const SokolShader& rhs);

        bool createShader(ShaderData& shaderData);
        void destroyShader();
        bool isCreated();

        sg_shader get();
    };
}


#endif /* SokolShader_h */
