
//
// (c) 2024 Eduardo Doria.
//

#ifndef color_h
#define color_h

#include "Vector3.h"
#include "Vector4.h"

namespace Supernova {

    class Color {
    private:
        static float GAMMA;
        static float INV_GAMMA;

    public:

        static Vector3 linearTosRGB(Vector3 color);
        static Vector4 linearTosRGB(Vector4 color);

        static Vector3 sRGBToLinear(Vector3 srgbIn);
        static Vector4 sRGBToLinear(Vector4 srgbIn);

    };
    
}

#endif //color_h
