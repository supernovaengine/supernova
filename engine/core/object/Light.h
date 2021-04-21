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
        void setColor(Vector3 color);

        void setRange(float range);
        void setIntensity(float intensity);

        void setConeAngle(float inner, float outer);
    };
}

#endif //LIGHT_H