//
// (c) 2022 Eduardo Doria.
//

#ifndef FOG_H
#define FOG_H

#include "math/Vector3.h"

namespace Supernova{
    enum class FogType{
        LINEAR,
		    EXPONENTIAL,
		    EXPONENTIALSQUARED
    };

    class Fog{

    private:
		FogType type;
		Vector3 color; //linear
		float density; //for  exponential fog
		float linearStart;
		float linearEnd;

    public:
        Fog();

        FogType getType() const;
        Vector3 getColor() const;
        float getDensity() const;
        float getLinearStart() const;
        float getLinearEnd() const;

        void setType(FogType type);
        void setColor(Vector3 color);
        void setDensity(float density);
        void setLinearStart(float start);
        void setLinearEnd(float end);
        void setLinearStartEnd(float start, float end);

    };
}

#endif //FOG_H