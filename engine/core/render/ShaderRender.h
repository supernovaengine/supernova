#ifndef ShaderRender_h
#define ShaderRender_h

#include "Render.h"
#include "sokol/SokolShader.h"

namespace Supernova{
    class ShaderRender{

    public:
        //***Backend***
        SokolShader backend;
        //***
        ShaderData shaderData; //For reflection info

        ShaderRender();
        ShaderRender(const ShaderRender& rhs);
        ShaderRender& operator=(const ShaderRender& rhs);
        virtual ~ShaderRender();

        bool createShader(ShaderData& shaderData);

        void destroyShader();
    };
}


#endif /* ShaderRender_h */