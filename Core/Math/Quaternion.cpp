#include "Quaternion.hpp"
#include "Matrix3x3.hpp"
#include "Vector3.hpp"
#include "Mathf.hpp"

// ========================================================================
// STATIC PROPERTIES
// ========================================================================

Quaternion Quaternion::zero( 0.0f, 0.0f, 0.0f, 0.0f );
Quaternion Quaternion::identity( 1.0f, 0.0f, 0.0f, 0.0f );

// ========================================================================
// CONSTRUCTORS
// ========================================================================

Quaternion::Quaternion ( float zRot, float yRot, float xRot )
{
    Set ( zRot, yRot, xRot );
}

Quaternion::Quaternion( const Vector3& axis, float angle )
{
    Set( axis, angle );
}

Quaternion::Quaternion( const Vector3& from, const Vector3& to )
{
    Set( from, to );
}

Quaternion::Quaternion( const Vector3& vector )
{
    Set( 0.0f, vector.x, vector.y, vector.z );
}

Quaternion::Quaternion( const Matrix3x3& rotation )
{
    Set( rotation );
}

// ========================================================================
// PUBLIC METHODS
// ========================================================================

void Quaternion::Clean()
{
    if (Mathf::IsZero(w))
        w = 0.0f;

    if (Mathf::IsZero(x))
        x = 0.0f;
    
    if (Mathf::IsZero(y))
        y = 0.0f;
    
    if (Mathf::IsZero(z))
        z = 0.0f;
}

void Quaternion::GetAxisAngle( Vector3& axis, float& angle )
{
    angle = 2.0f*acosf( w );
    float length = sqrtf( 1.0f - w*w );
    
    if ( Mathf::IsZero(length) )
    {
        axis.Zero();
    }
    else
    {
        float lengthRecip = 1.0f / length;
        axis.Set( x*lengthRecip, y*lengthRecip, z*lengthRecip );
    }
}

Vector3 Quaternion::GetEulerAngles() const
{
    return Matrix3x3 (*this).GetEulerAngles();
}

void Quaternion::Invert()
{
    float lengthsq = w*w + x*x + y*y + z*z;
    
    if (Mathf::IsZero(lengthsq))
        return;
    
    float lengthsqRecip = 1.0f / lengthsq;
    w = lengthsqRecip*w;
    x = -lengthsqRecip*x;
    y = -lengthsqRecip*y;
    z = -lengthsqRecip*z;
}

bool Quaternion::IsIdentity() const
{
    return Mathf::IsZero(1.0f - w) && Mathf::IsZero( x ) && Mathf::IsZero( y ) && Mathf::IsZero( z );
}

bool Quaternion::IsUnit() const
{
    return Mathf::IsZero(1.0f - w*w - x*x - y*y - z*z);
}

bool Quaternion::IsZero() const
{
    return Mathf::IsZero(w*w + x*x + y*y + z*z);
}

float Quaternion::Magnitude() const
{
    return sqrtf( w*w + x*x + y*y + z*z );
}

float Quaternion::MagnitudeSquared() const
{
	return (w * w + x * x + y * y + z * z);
}

float Quaternion::Norm() const
{
    return ( w*w + x*x + y*y + z*z );
}

void Quaternion::Normalize()
{
    float lengthsq = w*w + x*x + y*y + z*z;
    
    if ( Mathf::IsZero( lengthsq ) )
    {
        Zero();
    }
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        w *= factor;
        x *= factor;
        y *= factor;
        z *= factor;
    }
}

Quaternion Quaternion::Normalized() const
{
    float lengthsq = w*w + x*x + y*y + z*z;
    
    if ( Mathf::IsZero( lengthsq ) )
    {
        return Quaternion::zero;
    }
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        return Quaternion (w,x,y,z) * factor;
    }
}

void Quaternion::Set( float zRot, float yRot, float xRot )
{    
    zRot *= 0.5f;
    yRot *= 0.5f;
    xRot *= 0.5f;
    
    float Cx = cosf(xRot);
    float Sx = sinf(xRot);
    
    float Cy = cosf(yRot);
    float Sy = sinf(yRot);
    
    float Cz = cosf(zRot);
    float Sz = sinf(zRot);

    w = Cx*Cy*Cz - Sx*Sy*Sz;
    x = Sx*Cy*Cz + Cx*Sy*Sz;
    y = Cx*Sy*Cz - Sx*Cy*Sz;
    z = Cx*Cy*Sz + Sx*Sy*Cx;
}

void Quaternion::Set( const Vector3& axis, float angle )
{
    float length = axis.LengthSquared();
    
    if ( Mathf::IsZero( length ) )
    {
        SetIdentity();
        return;
    }
    
    angle *= 0.5f;
    
    float sintheta = sinf(angle);
    float costheta = cosf(angle);
    
    float scaleFactor = sintheta/sqrtf( length );
    
    w = costheta;
    x = scaleFactor * axis.x;
    y = scaleFactor * axis.y;
    z = scaleFactor * axis.z;
}

void Quaternion::Set( const Vector3& from, const Vector3& to )
{
    // Get axis of rotation
    Vector3 axis = Vector3::Cross(from, to);
    
    // Set quaternion to ||from|| * ||to|| * ( cos(theta), r*sin(theta) )
    Set( Vector3::Dot( from, to ), axis.x, axis.y, axis.z );
    
    // Set quaternion to ( cos(theta), r*sin(theta) )
    Normalize();
    
    // Add identity quaternion to this quaternion to get half theta quaternion
    w += 1.0f;
    
    // If w is zero, vectors are opposite...
    if ( w <= kEpsilon )
    {
        // Set quaternion to 180 degree rotation on a perpindicular axis
        if ( from.z*from.z > from.x*from.x )
            Set( 0.0f, 0.0f, from.z, -from.y );
        else
            Set( 0.0f, from.y, -from.x, 0.0f );
    }
    
    // Final normalization
    Normalize();
}

void Quaternion::Set( const Matrix3x3& matrix )
{
    float trace = matrix.Trace();

    if ( trace > 0.0f )
    {
        float s = sqrtf( trace + 1.0f );
        float recip = 0.5f / s;
        
        w = s * 0.5f;
        x = (matrix(2,1) - matrix(1,2)) * recip;
        y = (matrix(0,2) - matrix(2,0)) * recip;
        z = (matrix(1,0) - matrix(0,1)) * recip;
    }
    else
    {
        unsigned int i = 0;
        if ( matrix(1,1) > matrix(0,0) )
            i = 1;
        if ( matrix(2,2) > matrix(i,i) )
            i = 2;
        
        unsigned int j = (i+1)%3;
        unsigned int k = (j+1)%3;
        float s = sqrtf( matrix(i,i) - matrix(j,j) - matrix(k,k) + 1.0f );
        
        (*this)[i] = 0.5f*s;
        float recip = 0.5f/s;
        
        w = (matrix(k,j) - matrix(j,k))*recip;
        
        (*this)[j] = (matrix(j,i) + matrix(i,j))*recip;
        (*this)[k] = (matrix(k,i) + matrix(i,k))*recip;
    }
}

void Quaternion::SetConjugate()
{
    x = -x;
    y = -y;
    z = -z;
}

Vector3 Quaternion::Rotate( const Vector3& vector ) const
{
    float vMult = 2.0f*(x*vector.x + y*vector.y + z*vector.z);
    float crossMult = 2.0f*w;
    float pMult = crossMult*w - 1.0f;
    
    return Vector3( pMult*vector.x + vMult*x + crossMult*(y*vector.z - z*vector.y),
                     pMult*vector.y + vMult*y + crossMult*(z*vector.x - x*vector.z),
                     pMult*vector.z + vMult*z + crossMult*(x*vector.y - y*vector.x) );
}

// ========================================================================
// STATIC METHODS
// ========================================================================

float Quaternion::Angle(const Quaternion& from, const Quaternion& to)
{
    float angle = 2.0f * acosf(Quaternion::Dot(from, to));
    
    // If angle is greater than 180 degrees, take the other way around.
    if (angle > kPI)
        angle = 2*kPI - angle;
    
    return angle;
}

Quaternion Quaternion::Conjugate( const Quaternion& quat )
{
    return Quaternion( quat.w, -quat.x, -quat.y, -quat.z );
}

float Quaternion::Dot( const Quaternion& a, const Quaternion& b )
{
    return ( a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z);
}

Quaternion Quaternion::Inverse( const Quaternion& quat )
{
    float lengthsq = quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z;
    
    if ( Mathf::IsZero( lengthsq ) )
        return Quaternion();

    float lengthsqRecip = 1.0f / lengthsq;
    return Quaternion( lengthsqRecip*quat.w, -lengthsqRecip*quat.x, -lengthsqRecip*quat.y, -lengthsqRecip*quat.z );
}

Quaternion Quaternion::Lerp( const Quaternion& start, const Quaternion& end, float t )
{
    float cosTheta = Quaternion::Dot(start, end);

    if ( cosTheta >= kEpsilon )
    {
        return (t*end + (1.0f-t)*start).Normalized();
    }
    else
    {
        return (t*end + (t-1.0f)*start).Normalized();
    }
}

Quaternion Quaternion::Slerp( const Quaternion& start, const Quaternion& end, float t )
{
    // Get cos(theta)
    float cosTheta = Quaternion::Dot(start, end);
    float startInterp, endInterp;
    
    // If cos(theta) < 90
    if ( cosTheta >= kEpsilon )
    {
        // If theta > 0
        if ( (1.0f - cosTheta) > kEpsilon )
        {
            float theta = acosf( cosTheta );
            float recipSinTheta = 1.0f/sinf( theta );
            
            startInterp = sinf( (1.0f - t)*theta )*recipSinTheta;
            endInterp = sinf( t*theta )*recipSinTheta;
        }
        // If theta = 0
        else
        {
            startInterp = 1.0f - t;
            endInterp = t;
        }
    }
    else
    {
        // If theta < 180
        if ( (1.0f + cosTheta) > kEpsilon )
        {
            float theta = acosf( -cosTheta );
            float recipSinTheta = 1.0f/sinf( theta );
            
            startInterp = sinf( (t-1.0f)*theta )*recipSinTheta;
            endInterp = sinf( t*theta )*recipSinTheta;
        }
        // If theta = 180
        else
        {
            startInterp = t - 1.0f;
            endInterp = t;
        }
    }
    
    return (startInterp*start + endInterp*end);
}

// ========================================================================
// FRIEND OPERATORS
// ========================================================================

std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
{
    os << "<" << quat.w << "," << quat.x << "," << quat.y << "," << quat.z << ">";
    return os;
}

// ========================================================================
// MEMBER OPERATORS
// ========================================================================

Quaternion& Quaternion::operator+=( const Quaternion& other )
{
    w += other.w;
    x += other.x;
    y += other.y;
    z += other.z;
    
    return *this;
}

Quaternion& Quaternion::operator-=( const Quaternion& other )
{
    w -= other.w;
    x -= other.x;
    y -= other.y;
    z -= other.z;
    
    return *this;
}

Quaternion& Quaternion::operator*=( const Quaternion& other )
{
    Set( w*other.w - x*other.x - y*other.y - z*other.z,
        w*other.x + x*other.w + y*other.z - z*other.y,
        w*other.y + y*other.w + z*other.x - x*other.z,
        w*other.z + z*other.w + x*other.y - y*other.x );
    
    return *this;
}

Quaternion& Quaternion::operator*=( float scalar )
{
    w *= scalar;
    x *= scalar;
    y *= scalar;
    z *= scalar;
    
    return *this;
}

Quaternion Quaternion::operator-() const
{
    return Quaternion(-w, -x, -y, -z);
}

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Quaternion& a, const Quaternion& b)
{
    if ( Mathf::IsZero( b.w - a.w )
        && Mathf::IsZero( b.x - a.x )
        && Mathf::IsZero( b.y - a.y )
        && Mathf::IsZero( b.z - a.z ) )
        return true;
    
    return false;
}

bool operator != (const Quaternion& a, const Quaternion& b)
{
    if ( Mathf::IsZero( b.w - a.w )
        && Mathf::IsZero( b.x - a.x )
        && Mathf::IsZero( b.y - a.y )
        && Mathf::IsZero( b.z - a.z ) )
        return false;
    
    return true;
}

Quaternion operator + (const Quaternion& a, const Quaternion& b)
{
    return Quaternion( a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z );
}

Quaternion operator - (const Quaternion& a, const Quaternion& b)
{
    return Quaternion( a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z );
}

Quaternion operator * ( float scalar, const Quaternion& quat )
{
    return Quaternion( scalar*quat.w, scalar*quat.x, scalar*quat.y, scalar*quat.z );
}

Quaternion operator * ( const Quaternion& quat, float scalar )
{
    return Quaternion( scalar*quat.w, scalar*quat.x, scalar*quat.y, scalar*quat.z );
}

Quaternion operator * ( const Quaternion& a, const Quaternion& b )
{
    return Quaternion( a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
                  a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
                  a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z,
                  a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x );
}
