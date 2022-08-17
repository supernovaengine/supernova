//
// (c) 2021 Eduardo Doria.
//

#ifndef LIGHT_H
#define LIGHT_H

#include "Object.h"

namespace Supernova{

    class Light: public Object{
    public:
        Light(Scene* scene);
        virtual ~Light();

        void setType(LightType type);
        LightType getType();

        void setDirection(Vector3 direction);
        void setDirection(const float x, const float y, const float z);
        Vector3 getDirection();

        void setColor(Vector3 color);
        void setColor(const float r, const float g, const float b);
        Vector3 getColor();

        void setRange(float range);
        float getRange();

        void setIntensity(float intensity);
        float getIntensity();

        void setConeAngle(float inner, float outer);
        void setInnerConeAngle(float inner);
        float getInnerConeAngle();
        void setOuterConeAngle(float outer);
        float getOuterConeAngle();

        void setShadows(bool shadows);
        bool isShadows();

        void setBias(float bias);
        float getBias();

        void setShadowMapSize(unsigned int size);
        unsigned int getShadowMapSize();

        void setShadowCameraNearFar(float near, float far);
        void setCameraNear(float near);
        float getCameraNear();
        void setCameraFar(float far);
        float getCameraFar();

        void setNumCascades(unsigned int numCascades);
        float getNumCascades();
    };
}

#endif //LIGHT_H