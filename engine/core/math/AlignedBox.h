#ifndef AlignedBox_h
#define AlignedBox_h

#include "Vector3.h"
#include "Matrix4.h"

namespace Supernova{

    class Plane;
    
    class AlignedBox{

    public:

        enum BoxType {
            BOXTYPE_NULL,
            BOXTYPE_FINITE,
            BOXTYPE_INFINITE
        };

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

    private:

        Vector3 mMinimum;
        Vector3 mMaximum;
        BoxType mBoxType;
        mutable Vector3* mCorners;

    public:

        AlignedBox();
        AlignedBox(BoxType e);
        AlignedBox(const AlignedBox & rkBox);
        AlignedBox(const Vector3& min, const Vector3& max);
        AlignedBox(float mx, float my, float mz, float Mx, float My, float Mz);

        ~AlignedBox();

        AlignedBox& operator=(const AlignedBox& rhs);
        bool operator== (const AlignedBox& rhs) const;
        bool operator!= (const AlignedBox& rhs) const;

        const Vector3& getMinimum() const;
        Vector3& getMinimum();

        const Vector3& getMaximum() const;
        Vector3& getMaximum();

        void setMinimum( const Vector3& vec );
        void setMinimum( float x, float y, float z );
        void setMinimumX(float x);
        void setMinimumY(float y);
        void setMinimumZ(float z);

        void setMaximum( const Vector3& vec );
        void setMaximum( float x, float y, float z );
        void setMaximumX( float x );
        void setMaximumY( float y );
        void setMaximumZ( float z );

        void setExtents( const Vector3& min, const Vector3& max );

        void setExtents(float mx, float my, float mz, float Mx, float My, float Mz );

        const Vector3* getAllCorners(void) const;

        Vector3 getCorner(CornerEnum cornerToGet) const;

        void merge( const AlignedBox& rhs );
        void merge( const Vector3& point );

        void transform( const Matrix4& matrix );

        void setNull();
        bool isNull(void) const;
        bool isFinite(void) const;
        void setInfinite();
        bool isInfinite(void) const;

        bool intersects(const AlignedBox& b2) const;
        bool intersects(const Plane& p) const;
        bool intersects(const Vector3& v) const;

        AlignedBox intersection(const AlignedBox& b2) const;

        float volume(void) const;

        void scale(const Vector3& s);

        Vector3 getCenter(void) const;
        Vector3 getSize(void) const;
        Vector3 getHalfSize(void) const;

        bool contains(const Vector3& v) const;
        bool contains(const AlignedBox& other) const;

        float squaredDistance(const Vector3& v) const;
        float distance (const Vector3& v) const;
        
    };
    
}

#endif /* AlignedBox_h */
