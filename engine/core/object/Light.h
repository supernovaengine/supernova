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

        void setDirection(Vector3 direction);
        void setDirection(const float x, const float y, const float z);

        void setColor(Vector3 color);
        void setColor(const float r, const float g, const float b);

        void setRange(float range);
        void setIntensity(float intensity);

        void setConeAngle(float inner, float outer);
    };
}

#endif //LIGHT_H