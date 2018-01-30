#ifndef ray_h
#define ray_h

#include "math/Vector3.h"
#include "math/Plane.h"
#include "math/AlignedBox.h"

namespace Supernova {

    class Ray{

    private:

        Vector3 origin;
        Vector3 direction;

    public:

        Ray();
        Ray(const Ray &ray);
        Ray(Vector3 origin, Vector3 direction);

        Ray &operator=(const Ray &);
        Vector3 operator*(float t);

        void setOrigin(Vector3 point);
        Vector3 getOrigin();

        void setDirection(Vector3 direction);
        Vector3 getDirection();

        Vector3 getPoint(float distance);

        float intersects(Plane plane);
        float intersects(AlignedBox box);

        Vector3 intersectionPoint(Plane plane);
        Vector3 intersectionPoint(AlignedBox box);
    };
    
}


#endif /* ray_h */
