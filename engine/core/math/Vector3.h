#ifndef vector3_h
#define vector3_h

#include <math.h>
#include <string>

namespace Supernova { class Vector3; }
Supernova::Vector3 operator * ( float f, const Supernova::Vector3& v );


namespace Supernova {

    class Vector3
    {
    public:
        float x, y, z;

        static const Vector3 ZERO;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
        static const Vector3 UNIT_SCALE;


        Vector3();

        Vector3( float nx, float ny, float nz );

        Vector3( float v[3] );

        Vector3( int v[3] );

        Vector3( const float* const v );

        Vector3( const Vector3& v );

        std::string toString() const;

        float operator [] ( unsigned i ) const;

        float& operator [] ( unsigned i );

        Vector3& operator = ( const Vector3& v );

        bool operator == ( const Vector3& v ) const;

        bool operator != ( const Vector3& v ) const;


        Vector3 operator + ( const Vector3& v ) const;

        Vector3 operator - ( const Vector3& v ) const;

        Vector3 operator * ( float f ) const;

        Vector3 operator * ( const Vector3& v) const;

        Vector3 operator / ( float f ) const;

        Vector3 operator - () const;

        friend Vector3 (::operator *) ( float f, const Vector3& v );

        Vector3& operator += ( const Vector3& v );

        Vector3& operator -= ( const Vector3& v );

        Vector3& operator *= ( float f );

        Vector3& operator /= ( float f );

        bool operator < ( const Vector3& v ) const;

        bool operator > ( const Vector3& v ) const;

        float length () const;

        float squaredLength () const;

        float dotProduct(const Vector3& v) const;

        float absDotProduct(const Vector3& v) const;

        float distance(const Vector3& rhs) const;

        float squaredDistance(const Vector3& rhs) const;

        Vector3& normalize();
        float normalizeL();

        Vector3 crossProduct( const Vector3& v ) const;

        Vector3 midPoint( const Vector3& v ) const;

        void makeFloor( const Vector3& v );

        void makeCeil( const Vector3& v );

        Vector3 perpendicular(void);

        Vector3 reflect(const Vector3& normal) const;

    };
    
}

#endif
