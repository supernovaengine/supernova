
#ifndef vector4_h
#define vector4_h

#include "Vector3.h"
#include <algorithm>
#include <assert.h>
#include <math.h>

    class Vector4
    {
    public:
        float x, y, z, w;
        
        static const Vector4 ZERO;

        Vector4();

        Vector4( const float fX, const float fY, const float fZ, const float fW );

        explicit Vector4( const float afCoordinate[4] );

        explicit Vector4( const int afCoordinate[4] );

        explicit Vector4( float* const r );

        explicit Vector4( const float scaler );

        explicit Vector4(const Vector3& rhs);


        void swap(Vector4& other);

        float operator [] ( const size_t i ) const;

        float& operator [] ( const size_t i );


        float* ptr();

        const float* ptr() const;

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

        friend Vector4 operator * ( const float fScalar, const Vector4& rkVector );

        friend Vector4 operator / ( const float fScalar, const Vector4& rkVector );

        friend Vector4 operator + (const Vector4& lhs, const float rhs);

        friend Vector4 operator + (const float lhs, const Vector4& rhs);

        friend Vector4 operator - (const Vector4& lhs, float rhs);

        friend Vector4 operator - (const float lhs, const Vector4& rhs);


        Vector4& operator += ( const Vector4& rkVector );

        Vector4& operator -= ( const Vector4& rkVector );

        Vector4& operator *= ( const float fScalar );

        Vector4& operator += ( const float fScalar );

        Vector4& operator -= ( const float fScalar );

        Vector4& operator *= ( const Vector4& rkVector );

        Vector4& operator /= ( const float fScalar );

        Vector4& operator /= ( const Vector4& rkVector );

        void divideByW();


        float dotProduct(const Vector4& vec) const;

        bool isNaN() const;

    };


#endif
