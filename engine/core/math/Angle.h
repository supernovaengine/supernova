
#ifndef angle_h
#define angle_h

namespace Supernova {

    class Angle {
    private:
        static bool useDegrees;

    public:

        static float radToDefault(float radians);
        static float degToDefault(float degrees);
        static float defaultToRad(float angle);
        static float defaultToDeg(float angle);
        static float radToDeg(float radians);
        static float degToRad(float degrees);
    };
    
}

#endif
