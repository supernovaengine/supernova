#include "Quaternion.h"

#include "util/Angle.h"
#include <algorithm>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <float.h>
#include "Log.h"

using namespace Supernova;

const Quaternion Quaternion::IDENTITY( 1, 0, 0, 0 );

Quaternion::Quaternion()
    : w(1), x(0), y(0), z(0){
}

Quaternion::Quaternion( const float fW, const float fX, const float fY, const float fZ)
    : w( fW), x( fX ), y( fY ), z( fZ ){
}

Quaternion::Quaternion( float* const r )
    : w( r[0] ), x( r[1] ), y( r[2] ), z( r[3] ){
}

Quaternion::Quaternion(const Vector3* akAxis){
    this->fromAxes(akAxis);
}


Quaternion::Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis){
    this->fromAxes(xaxis, yaxis, zaxis);
}

std::string Quaternion::toString() const{
    return "Quaternion(" + std::to_string(w) + ", " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; 
}

float Quaternion::operator [] ( const size_t i ) const
{
    assert( i < 4 );

    return *(&w+i);
}

float& Quaternion::operator [] ( const size_t i )
{
    assert( i < 4 );

    return *(&w+i);
}


float* Quaternion::ptr()
{
    return &w;
}


const float* Quaternion::ptr() const
{
    return &w;
}

Quaternion& Quaternion::operator = ( const Quaternion& rkQ )
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;

    return *this;
}

bool Quaternion::operator == ( const Quaternion& rhs ) const
{
    return ( w == rhs.w &&
            x == rhs.x &&
            y == rhs.y &&
            z == rhs.z );
}

bool Quaternion::operator != ( const Quaternion& rhs ) const
{
    return ( w != rhs.w ||
            x != rhs.x ||
            y != rhs.y ||
            z != rhs.z );
}


Quaternion Quaternion::operator + ( const Quaternion& rhs ) const
{
    return Quaternion(
                      w + rhs.w,
                      x + rhs.x,
                      y + rhs.y,
                      z + rhs.z);
}

Quaternion Quaternion::operator - ( const Quaternion& rhs ) const
{
    return Quaternion(
                      w - rhs.w,
                      x - rhs.x,
                      y - rhs.y,
                      z - rhs.z);
}


Quaternion Quaternion::operator * ( float fScalar ) const
{
    return Quaternion(w * fScalar, x * fScalar, y * fScalar, z * fScalar);
}

Quaternion operator * (float fScalar, const Quaternion& rkQ){
    return Quaternion(fScalar * rkQ.w, fScalar * rkQ.x, fScalar * rkQ.y, fScalar * rkQ.z);
}


Quaternion Quaternion::operator * ( const Quaternion& rhs) const
{
    return Quaternion
    (
     w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
     w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
     w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
     w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x
     );
}

Vector3 Quaternion::operator* (const Vector3& v) const
{
    // nVidia SDK implementation
    Vector3 uv, uuv;
    Vector3 qvec(x, y, z);
    uv = qvec.crossProduct(v);
    uuv = qvec.crossProduct(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;

}
const Quaternion& Quaternion::operator + () const{
    return *this;
}

Quaternion Quaternion::operator - () const{
    return Quaternion(-w, -x, -y, -z);
}

void Quaternion::swap(Quaternion& other){
    std::swap(w, other.w);
    std::swap(x, other.x);
    std::swap(y, other.y);
    std::swap(z, other.z);
}

void Quaternion::fromAxes (const Vector3* akAxis){
    Matrix4 kRot;

    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot.set(iCol, 0, akAxis[iCol].x);
        kRot.set(iCol, 1, akAxis[iCol].y);
        kRot.set(iCol, 2, akAxis[iCol].z);
        kRot.set(iCol, 3, 0.0f);
    }

    kRot.set(3,0, 0.0f);
    kRot.set(3,1, 0.0f);
    kRot.set(3,2, 0.0f);
    kRot.set(3,3, 1.0f);

    fromRotationMatrix(kRot);
}

void Quaternion::fromAxes (const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis){
    Matrix4 kRot;

    kRot.set(0,0, xaxis.x);
    kRot.set(0,1, xaxis.y);
    kRot.set(0,2, xaxis.z);
    kRot.set(0,3, 0.0f);

    kRot.set(1,0, yaxis.x);
    kRot.set(1,1, yaxis.y);
    kRot.set(1,2, yaxis.z);
    kRot.set(1,3, 0.0f);

    kRot.set(2,0, zaxis.x);
    kRot.set(2,1, zaxis.y);
    kRot.set(2,2, zaxis.z);
    kRot.set(2,3, 0.0f);

    kRot.set(3,0, 0.0f);
    kRot.set(3,1, 0.0f);
    kRot.set(3,2, 0.0f);
    kRot.set(3,3, 1.0f);

    fromRotationMatrix(kRot);

}

void Quaternion::fromRotationMatrix (const Matrix4& kRot){

    float trace = kRot[0][0]+kRot[1][1]+kRot[2][2];
    float root;

    if ( trace > 0.0 ) {
        root = sqrt(trace + 1.0f);
        w = 0.5f*root;
        root = 0.5f/root;
        x = (kRot[1][2]-kRot[2][1])*root;
        y = (kRot[2][0]-kRot[0][2])*root;
        z = (kRot[0][1]-kRot[1][0])*root;
    } else {
        static size_t s_iNext[3] = { 1, 2, 0 };
        size_t i = 0;
        if ( kRot[1][1] > kRot[0][0] )
            i = 1;
        if ( kRot[2][2] > kRot[i][i] )
            i = 2;
        size_t j = s_iNext[i];
        size_t k = s_iNext[j];

        root = sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
        float* apkQuat[3] = { &x, &y, &z };
        *apkQuat[i] = 0.5f*root;
        root = 0.5f/root;
        w = (kRot[j][k]-kRot[k][j])*root;
        *apkQuat[j] = (kRot[i][j]+kRot[j][i])*root;
        *apkQuat[k] = (kRot[i][k]+kRot[k][i])*root;
    }
}

Matrix4 Quaternion::getRotationMatrix(){

    float xx      = x * x;
    float xy      = x * y;
    float xz      = x * z;
    float xw      = x * w;

    float yy      = y * y;
    float yz      = y * z;
    float yw      = y * w;

    float zz      = z * z;
    float zw      = z * w;

    Matrix4 mat;

    mat[0][0] = 1 - 2 * ( yy + zz );
    mat[0][1] =     2 * ( xy + zw );
    mat[0][2] =     2 * ( xz - yw );
    mat[0][3] = 0.0f;

    mat[1][0] =     2 * ( xy - zw );
    mat[1][1] = 1 - 2 * ( xx + zz );
    mat[1][2] =     2 * ( yz + xw );
    mat[1][3] = 0.0f;

    mat[2][0] =     2 * ( xz + yw );
    mat[2][1] =     2 * ( yz - xw );
    mat[2][2] = 1 - 2 * ( xx + yy );
    mat[2][3] = 0.0f;

    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;

    return mat;
}

void Quaternion::fromAngle (const float angle) {

    fromAngleAxis(angle, Vector3(0, 0, 1));
}

void Quaternion::fromAngleAxis (const float angle, const Vector3& rkAxis) {

    float defAngle = Angle::defaultToRad(angle);

    float fHalfAngle = 0.5*defAngle;
    float fSin = sin(fHalfAngle);
    w = cos(fHalfAngle);
    x = fSin*rkAxis.x;
    y = fSin*rkAxis.y;
    z = fSin*rkAxis.z;
}

Vector3 Quaternion::xAxis(void) const {
    float fTy  = 2.0f*y;
    float fTz  = 2.0f*z;
    float fTwy = fTy*w;
    float fTwz = fTz*w;
    float fTxy = fTy*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTzz = fTz*z;

    return Vector3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
}

Vector3 Quaternion::yAxis(void) const
{
    float fTx  = 2.0f*x;
    float fTy  = 2.0f*y;
    float fTz  = 2.0f*z;
    float fTwx = fTx*w;
    float fTwz = fTz*w;
    float fTxx = fTx*x;
    float fTxy = fTy*x;
    float fTyz = fTz*y;
    float fTzz = fTz*z;

    return Vector3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
}

Vector3 Quaternion::zAxis(void) const
{
    float fTx  = 2.0f*x;
    float fTy  = 2.0f*y;
    float fTz  = 2.0f*z;
    float fTwx = fTx*w;
    float fTwy = fTy*w;
    float fTxx = fTx*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTyz = fTz*y;

    return Vector3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
}

float Quaternion::dot (const Quaternion& rkQ) const
{
    return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}

float Quaternion::norm () const
{
    return w*w+x*x+y*y+z*z;
}

Quaternion Quaternion::inverse () const
{
    float fNorm = w*w+x*x+y*y+z*z;
    if ( fNorm > 0.0 )
    {
        float fInvNorm = 1.0f/fNorm;
        return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
    }
    else
    {
        return NULL;
    }
}

Quaternion Quaternion::unitInverse () const
{
    return Quaternion(w,-x,-y,-z);
}

Quaternion Quaternion::exp () const
{

    float fAngle = sqrt(x*x+y*y+z*z);
    float fSin = sin(fAngle);

    Quaternion kResult;
    kResult.w = cos(fAngle);

    if ( fabs(fSin) >= FLT_EPSILON )
    {
        float fCoeff = fSin/(fAngle);
        kResult.x = fCoeff*x;
        kResult.y = fCoeff*y;
        kResult.z = fCoeff*z;
    }
    else
    {
        kResult.x = x;
        kResult.y = y;
        kResult.z = z;
    }

    return kResult;
}

Quaternion Quaternion::log () const
{

    Quaternion kResult;
    kResult.w = 0.0;

    if ( fabs(w) < 1.0 )
    {
        float fAngle = acos(w);
        float fSin = sin(fAngle);
        if ( fabs(fSin) >= FLT_EPSILON )
        {
            float fCoeff = fAngle/fSin;
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
            return kResult;
        }
    }

    kResult.x = x;
    kResult.y = y;
    kResult.z = z;

    return kResult;
}

bool Quaternion::equals(const Quaternion& rhs) const
{
    float matching = dot(rhs);

    return ( fabs(matching-1.0) < 0.001 );
}

Quaternion Quaternion::slerp(float t, Quaternion q1, Quaternion q2)
{
    float w1, x1, y1, z1, w2, x2, y2, z2, w3, x3, y3, z3;

    float theta, mult1, mult2;

    w1 = q1.w; x1 = q1.x; y1 = q1.y; z1 = q1.z;
    w2 = q2.w; x2 = q2.x; y2 = q2.y; z2 = q2.z;

    if (w1*w2 + x1*x2 + y1*y2 + z1*z2 < 0) {
        w2 = -w2; x2 = -x2; y2 = -y2; z2 = -z2;
    }

    theta = acos(w1*w2 + x1*x2 + y1*y2 + z1*z2);

    if (theta > FLT_EPSILON) {
        mult1 = sin( (1-t)*theta ) / sin( theta );
        mult2 = sin( t*theta ) / sin( theta );
    } else {
        mult1 = 1 - t;
        mult2 = t;
    }

    w3 =  mult1*w1 + mult2*w2;
    x3 =  mult1*x1 + mult2*x2;
    y3 =  mult1*y1 + mult2*y2;
    z3 =  mult1*z1 + mult2*z2;

    return Quaternion(w3, x3, y3, z3);
}

Quaternion Quaternion::slerpExtraSpins (float fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
    float fCos = rkP.dot(rkQ);
    float fAngle = acos(fCos);

    if ( fabs(fAngle) < FLT_EPSILON )
        return rkP;

    float fSin = sin(fAngle);
    float fPhase ( M_PI*iExtraSpins*fT );
    float fInvSin = 1.0f/fSin;
    float fCoeff0 = sin((1.0f-fT)*fAngle - fPhase)*fInvSin;
    float fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
    return fCoeff0*rkP + fCoeff1*rkQ;
}

Quaternion Quaternion::squad (float fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ)
{
    float fSlerpT = 2.0f*fT*(1.0f-fT);
    Quaternion kSlerpP = slerp(fT, rkP, rkQ);
    Quaternion kSlerpQ = slerp(fT, rkA, rkB);
    return slerp(fSlerpT, kSlerpP ,kSlerpQ);
}

float Quaternion::normalise(void)
{
    float len = norm();
    float factor = 1.0f / sqrt(len);
    *this = *this * factor;
    return len;
}

float Quaternion::getRoll() const
{
        //float fTx  = 2.0*x;
        float fTy  = 2.0f*y;
        float fTz  = 2.0f*z;
        float fTwz = fTz*w;
        float fTxy = fTy*x;
        float fTyy = fTy*y;
        float fTzz = fTz*z;

        // Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

        return Angle::radToDefault(atan2(fTxy+fTwz, 1.0f-(fTyy+fTzz)));
}

float Quaternion::getPitch() const
{
        float fTx  = 2.0f*x;
        //float fTy  = 2.0f*y;
        float fTz  = 2.0f*z;
        float fTwx = fTx*w;
        float fTxx = fTx*x;
        float fTyz = fTz*y;
        float fTzz = fTz*z;

        // Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
        return Angle::radToDefault(atan2(fTyz+fTwx, 1.0f-(fTxx+fTzz)));
}

float Quaternion::getYaw() const
{
        float fTx  = 2.0f*x;
        float fTy  = 2.0f*y;
        float fTz  = 2.0f*z;
        float fTwy = fTy*w;
        float fTxx = fTx*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;

        // Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));
        return Angle::radToDefault(atan2(fTxz+fTwy, 1.0f-(fTxx+fTyy)));
}

Quaternion Quaternion::nlerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath)
{
    Quaternion result;
    float fCos = rkP.dot(rkQ);
    if (fCos < 0.0f && shortestPath)
    {
        result = rkP + fT * ((-rkQ) - rkP);
    }
    else
    {
        result = rkP + fT * (rkQ - rkP);
    }
    result.normalise();
    return result;
}
