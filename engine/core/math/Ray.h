#ifndef ray_h
#define ray_h

#include "math/Vector3.h"

class Ray{

private:

    Vector3 point;
    Vector3 vector;

public:

    Ray();
    Ray(const Ray &ray);
    Ray(Vector3 point, Vector3 vector);

    Ray &operator=(const Ray &);

    void setPoint(Vector3 point);
    Vector3 getPoint();

    void setVector(Vector3 vector);
    Vector3 getVector();

    Vector3 intersectionPoint(Vector3 planePoint, Vector3 planeNormal);
};


#endif /* ray_h */
