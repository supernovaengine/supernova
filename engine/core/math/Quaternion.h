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

        Quaternion();
        Quaternion( const float fW, const float fX, const float fY, const float fZ );
        explicit Quaternion( float* const r );
        Quaternion(const Vector3* akAxis);
        Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);

        void swap(Quaternion& other);

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

        void fromAxes (const Vector3* akAxis);
        void fromAxes (const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);
        void fromRotationMatrix (const Matrix4& kRot);
        Matrix4 getRotationMatrix();
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

        Quaternion slerp (float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath);
        Quaternion slerpExtraSpins (float fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins);
        Quaternion squad (float fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath);
        float normalise(void);
        float getRoll() const;
        float getPitch() const;
        float getYaw() const;
        Quaternion nlerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath);

    };
    
}

#endif /* quaternion_h */
