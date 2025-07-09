
#ifndef vector4_h
#define vector4_h

#include "Export.h"
#include <algorithm>
#include <string>
#include <assert.h>
#include <math.h>

namespace Supernova { class Vector4; }
Supernova::Vector4 operator * ( const float fScalar, const Supernova::Vector4& rkVector );
Supernova::Vector4 operator / ( const float fScalar, const Supernova::Vector4& rkVector );
Supernova::Vector4 operator + (const Supernova::Vector4& lhs, const float rhs);
Supernova::Vector4 operator + (const float lhs, const Supernova::Vector4& rhs);
Supernova::Vector4 operator - (const Supernova::Vector4& lhs, float rhs);
Supernova::Vector4 operator - (const float lhs, const Supernova::Vector4& rhs);


namespace Supernova {

    class Vector2;
    class Vector3;

    class SUPERNOVA_API Vector4
    {
    public:
        float x, y, z, w;
        
        static const Vector4 ZERO;
        static const Vector4 UNIT_X;
        static const Vector4 UNIT_Y;
        static const Vector4 UNIT_Z;
        static const Vector4 UNIT_W;
        static const Vector4 UNIT_SCALE;

        Vector4();
        Vector4( const float fX, const float fY, const float fZ, const float fW );
        Vector4( const Vector4& rhs );
        explicit Vector4( const float afCoordinate[4] );
        explicit Vector4( const int afCoordinate[4] );
        explicit Vector4( float* const r );
        explicit Vector4( const float scaler );
        explicit Vector4( const Vector2& rhs, const float fZ, const float fW );
        explicit Vector4( const Vector3& rhs, const float fW );

        std::string toString() const;

        float operator [] ( const size_t i ) const;
        float& operator [] ( const size_t i );
        Vector4& operator = ( const Vector4& rkVector );
        Vector4& operator = ( const float fScalar);
        bool operator == ( const Vector4& rkVector ) const;
        bool operator != ( const Vector4& rkVector ) const;
        Vector4& operator = (const Vector3& rhs);

        // arithmetic operations
        Vector4 operator + ( const Vector4& rkVector ) const;
        Vector4 operator - ( const Vector4& rkVector ) const;
        Vector4 operator * ( const float fScalar ) const;
        Vector4 operator * ( const Vector4& rhs) const;
        Vector4 operator / ( const float fScalar ) const;
        Vector4 operator / ( const Vector4& rhs) const;
        const Vector4& operator + () const;
        Vector4 operator - () const;

        friend Vector4 (::operator *) ( const float fScalar, const Vector4& rkVector );
        friend Vector4 (::operator /) ( const float fScalar, const Vector4& rkVector );
        friend Vector4 (::operator +) (const Vector4& lhs, const float rhs);
        friend Vector4 (::operator +) (const float lhs, const Vector4& rhs);
        friend Vector4 (::operator -) (const Vector4& lhs, float rhs);
        friend Vector4 (::operator -) (const float lhs, const Vector4& rhs);

        Vector4& operator += ( const Vector4& rkVector );
        Vector4& operator -= ( const Vector4& rkVector );
        Vector4& operator *= ( const float fScalar );
        Vector4& operator += ( const float fScalar );
        Vector4& operator -= ( const float fScalar );
        Vector4& operator *= ( const Vector4& rkVector );
        Vector4& operator /= ( const float fScalar );
        Vector4& operator /= ( const Vector4& rkVector );

        bool operator < ( const Vector4& v ) const;
        bool operator > ( const Vector4& v ) const;

        bool isValid() const;

        void swap(Vector4& other);

        void divideByW();

        float dotProduct(const Vector4& vec) const;

        bool isNaN() const;

    };
    
}


#endif
