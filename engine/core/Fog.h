#ifndef Fog_h
#define Fog_h

#define S_FOG_LINEAR 0
#define S_FOG_EXPONENTIAL 1
#define S_FOG_EXPONENTIALSQUARED 2

#include "math/Vector3.h"

namespace Supernova {

    class Fog {
    private:
        int mode;
        float linearStart;
        float linearEnd;
        float density;
        float visibility;
        Vector3 color;
        
    public:
        Fog();
        virtual ~Fog();
        
        void setMode(int mode);
        void setLinearStart(float linearStart);
        void setLinearEnd(float linearEnd);
        void setDensity(float density);
        void setVisibility(float visibility);
        void setColor(Vector3 color);
        
        int getMode();
        float getLinearStart();
        float getLinearEnd();
        float getDensity();
        float getVisibility();
        Vector3 getColor();
    };
    
}


#endif /* Fog_h */
