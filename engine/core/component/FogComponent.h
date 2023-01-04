#ifndef FOG_COMPONENT_H
#define FOG_COMPONENT_H

#include "util/Color.h"

namespace Supernova{

    enum class FogType{
        LINEAR,
		EXPONENTIAL,
		EXPONENTIALSQUARED
    };

    struct FogComponent{
        FogType type = FogType::LINEAR;
		Vector3 color = Color::sRGBToLinear(Vector3(0.8, 0.8, 0.8)); //linear
		float density = 0.01; //for  exponential fog
		float linearStart = 10;
		float linearEnd = 60;
    };
    
}

#endif //FOG_COMPONENT_H