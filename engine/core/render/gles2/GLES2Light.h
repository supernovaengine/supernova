#ifndef GLES2LIGHT_H
#define GLES2LIGHT_H

#include "GLES2Header.h"
#include "GLES2Program.h"
#include "render/SceneRender.h"

namespace Supernova {

    class GLES2Light {

    private:

        GLES2Program* program;

    public:

        GLES2Light();
        virtual ~GLES2Light();

        GLuint u_NumPointLight;
        GLuint u_PointLightPos;
        GLuint u_PointLightPower;
        GLuint u_PointLightColor;

        GLuint u_NumSpotLight;
        GLuint u_SpotLightPos;
        GLuint u_SpotLightPower;
        GLuint u_SpotLightColor;
        GLuint u_SpotLightCutOff;
        GLuint u_SpotLightTarget;

        GLuint u_NumDirectionalLight;
        GLuint u_DirectionalLightDir;
        GLuint u_DirectionalLightPower;
        GLuint u_DirectionalLightColor;

        GLuint u_AmbientLight;

        void setProgram(GLES2Program* program);
        void getUniformLocations();
        void setUniformValues(SceneRender* sceneRender);

    };
        
}


#endif //GLES2LIGHT_H
