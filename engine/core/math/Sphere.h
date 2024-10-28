#ifndef sphere_h
#define sphere_h

#include <cmath>
#include "Vector3.h" // Ensure you have this header for the Vector3 class

namespace Supernova{
    class Sphere {
    public:
        Vector3 center;
        float radius;

        Sphere();
        Sphere(Vector3 c, float r);
        Sphere(const Sphere& s);

        Sphere& operator = (const Sphere& s);
        bool operator == (const Sphere& s);
        bool operator != (const Sphere& s);

        std::string toString() const;

        bool contains(const Vector3& point) const;
        bool intersects(const Sphere& other) const;

        float surfaceArea() const;
        float volume() const;
    };
}

#endif