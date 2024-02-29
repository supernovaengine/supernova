#ifndef ray_h
#define ray_h

#include "math/Vector3.h"
#include "math/Plane.h"
#include "math/AlignedBox.h"
#include "object/physics/Body2D.h"
#include "object/physics/Body3D.h"

namespace Supernova {

    enum class RayTestType{
        STATIC_2D_BODIES,
        ALL_2D_BODIES,
        STATIC_3D_BODIES,
        ALL_3D_BODIES
    };

    struct RayReturn{
        bool hit;
        float distance;
        Vector3 point;
        Vector3 normal;
        Entity body;
        size_t shapeIndex;
    };

    class Ray{

    private:

        Vector3 origin;
        Vector3 direction; // direction and length of the ray (anything beyond this length will not be a hit)

    public:

        static const RayReturn NO_HIT;

        Ray();
        Ray(const Ray &ray);
        Ray(Vector3 origin, Vector3 direction);

        Ray &operator=(const Ray &);
        Vector3 operator*(float t);

        void setOrigin(Vector3 point);
        Vector3 getOrigin() const;

        void setDirection(Vector3 direction);
        Vector3 getDirection() const;

        Vector3 getPoint(float distance);

        RayReturn intersects(Plane plane);
        Vector3 intersectionPoint(Plane plane);

        RayReturn intersects(AlignedBox box);
        Vector3 intersectionPoint(AlignedBox box);

        RayReturn intersects(Body2D body);
        Vector3 intersectionPoint(Body2D body);

        RayReturn intersects(Body2D body, size_t shape);
        Vector3 intersectionPoint(Body2D body, size_t shape);

        RayReturn intersects(Body3D body);
        Vector3 intersectionPoint(Body3D body);

        RayReturn intersects(Body3D body, size_t shape);
        Vector3 intersectionPoint(Body3D body, size_t shape);

        RayReturn intersects(Scene* scene, RayTestType raytest);
        Vector3 intersectionPoint(Scene* scene, RayTestType raytest);
    };
    
}


#endif /* ray_h */
