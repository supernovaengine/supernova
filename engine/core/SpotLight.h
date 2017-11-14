#ifndef spotlight_h
#define spotlight_h

#include "Light.h"
#include "math/Vector3.h"

namespace Supernova {

    class SpotLight: public Light {

    protected:

        virtual void updateLightCamera();

    public:

        SpotLight();
        virtual ~SpotLight();

        void setTarget(Vector3 target);
        void setTarget(float x, float y, float z);
        void setSpotAngle(float angle);

        virtual bool loadShadow();

    };
    
}

#endif /* SpotLight_h */
