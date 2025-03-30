#ifndef OBB_h
#define OBB_h

#include "Vector3.h"
#include "Matrix4.h"
#include "AABB.h"
#include "Quaternion.h"

namespace Supernova {

    class Plane;
    class Sphere;
    class Ray;

    class SUPERNOVA_API OBB {
    private:
        Vector3 mCenter;         // Center of the OBB
        Vector3 mHalfExtents;    // Half-lengths of the OBB along each axis
        Vector3 mAxisX;          // Local X axis direction (normalized)
        Vector3 mAxisY;          // Local Y axis direction (normalized)
        Vector3 mAxisZ;          // Local Z axis direction (normalized)
        mutable Vector3* mCorners;

    public:
        enum CornerEnum {
            FAR_LEFT_BOTTOM = 0,
            FAR_LEFT_TOP = 1,
            FAR_RIGHT_TOP = 2,
            FAR_RIGHT_BOTTOM = 3,
            NEAR_RIGHT_BOTTOM = 7,
            NEAR_LEFT_BOTTOM = 6,
            NEAR_LEFT_TOP = 5,
            NEAR_RIGHT_TOP = 4
        };

        static const OBB ZERO;

        OBB();
        OBB(const OBB& obb);
        OBB(const Vector3& center, const Vector3& halfExtents);
        OBB(const Vector3& center, const Vector3& halfExtents, 
            const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ);
        OBB(const AABB& aabb);
        OBB(const AABB& aabb, const Matrix4& transform);

        ~OBB();

        OBB& operator=(const OBB& rhs);
        bool operator==(const OBB& rhs) const;
        bool operator!=(const OBB& rhs) const;

        std::string toString() const;

        // Getters and setters
        const Vector3& getCenter() const;
        void setCenter(const Vector3& center);

        const Vector3& getHalfExtents() const;
        void setHalfExtents(const Vector3& halfExtents);

        const Vector3& getAxisX() const;
        const Vector3& getAxisY() const;
        const Vector3& getAxisZ() const;
        void setAxes(const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ);
        void setAxes(const Quaternion& orientation);

        // Transforms
        void setOrientation(const Quaternion& orientation);
        Quaternion getOrientation() const;

        void transform(const Matrix4& matrix);
        void transform(const Vector3& translate, const Quaternion& rotate, const Vector3& scale);

        // Conversion
        AABB toAABB() const;
        Matrix4 toMatrix() const;

        // Get corners of the OBB
        Vector3 getCorner(CornerEnum cornerToGet) const;
        const Vector3* getCorners() const;

        void enclose(const OBB& other);
        void enclose(const Vector3& point);

        // Testing
        bool intersects(const OBB& other) const;
        bool intersects(const AABB& aabb) const;
        bool intersects(const Sphere& sphere) const;
        bool intersects(const Plane& plane) const;
        bool intersects(const Vector3& point) const;

        bool contains(const Vector3& point) const;
        bool contains(const OBB& other) const;

        // Distance calculations
        float squaredDistance(const Vector3& point) const;
        float distance(const Vector3& point) const;

        // Volume
        float volume() const;

        // Closest point
        Vector3 closestPoint(const Vector3& point) const;
    };

}

#endif /* OBB_h */