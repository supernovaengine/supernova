#ifndef quaternion_h
#define quaternion_h

#include "Vector3.h"
#include "Matrix4.h"

namespace Supernova { class Quaternion; }
Supernova::Quaternion operator * (float fScalar, const Supernova::Quaternion& rkQ);

namespace Supernova {

    class Quaternion {
    public:

        float w, x, y, z;

        static const Quaternion IDENTITY;

        Quaternion();
        Quaternion( const float fW, const float fX, const float fY, const float fZ );
        explicit Quaternion( float* const r );
        Quaternion(const float xAngle, const float yAngle, const float zAngle);
        Quaternion(const Vector3* akAxis);
        Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);
        Quaternion(const float angle, const Vector3& rkAxis);

        std::string toString() const;

        float operator [] ( const size_t i ) const;
        float& operator [] ( const size_t i );
        float* ptr();
        const float* ptr() const;

        Quaternion& operator = ( const Quaternion& rhs );
        bool operator == ( const Quaternion& rhs ) const;
        bool operator != ( const Quaternion& rhs ) const;
        bool equals(const Quaternion& rhs) const;

        Quaternion operator + ( const Quaternion& rhs ) const;
        Quaternion operator - ( const Quaternion& rhs ) const;
        Quaternion operator * ( float fScalar ) const;

        friend Quaternion (::operator *) (float fScalar, const Quaternion& rkQ);

        Quaternion operator * ( const Quaternion& rhs) const;
        Vector3 operator * (const Vector3& v) const;
        const Quaternion& operator + () const;
        Quaternion operator - () const;

        void swap(Quaternion& other);

        void fromEulerAngles(const float xAngle, const float yAngle, const float zAngle);
        void fromAxes (const Vector3* akAxis);
        void fromAxes (const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);
        Quaternion& fromRotationMatrix (const Matrix4& kRot);
        Matrix4 getRotationMatrix();
        void fromAngle (const float angle);
        void fromAngleAxis (const float angle, const Vector3& rkAxis);

        Vector3 xAxis(void) const;
        Vector3 yAxis(void) const;
        Vector3 zAxis(void) const;

        float dot (const Quaternion& rkQ) const;
        float norm () const;
        Quaternion inverse () const;
        Quaternion unitInverse () const;
        Quaternion exp () const;
        Quaternion log () const;

        Quaternion slerp(float t, Quaternion q1, Quaternion q2);
        Quaternion slerpExtraSpins (float fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins);
        Quaternion squad (float fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ);
        Quaternion nlerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath = false);
        Quaternion& normalize(void);
        float normalizeL(void);
        float getRoll() const;
        float getPitch() const;
        float getYaw() const;

    };
    
}

#endif /* quaternion_h */
