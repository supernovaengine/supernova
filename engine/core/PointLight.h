#ifndef pointlight_h
#define pointlight_h

#include "Light.h"

namespace Supernova {

    class PointLight: public Light {

    protected:

        virtual void updateLightCamera();

    public:

        PointLight();
        virtual ~PointLight();

        virtual bool loadShadow();

    };
    
}

#endif /* PointLight_hpp */
