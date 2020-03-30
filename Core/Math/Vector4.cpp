#include "Vector4.hpp"
#include "Mathf.hpp"

// ========================================================================
// STATIC PROPERTIES
// ========================================================================

Vector4 Vector4::zero( 0.0f, 0.0f, 0.0f, 0.0f );
Vector4 Vector4::one( 1.0f, 1.0f, 1.0f, 1.0f );

// ========================================================================
// PUBLIC METHODS
// ========================================================================

void Vector4::Clean()
{
    if (Mathf::IsZero(x))
    {
        x = 0.0f;
    }
    if (Mathf::IsZero(y))
    {
        y = 0.0f;
    }
    if (Mathf::IsZero(z))
    {
        z = 0.0f;
    }
    if (Mathf::IsZero(w))
    {
        w = 0.0f;
    }
}

bool Vector4::IsUnit() const
{
    return Mathf::IsZero(1.0f - x*x - y*y - z*z - w*w);
}

bool Vector4::IsZero() const
{
    return Mathf::IsZero(x*x + y*y + z*z + w*w);
}

float Vector4::Length() const
{
    return sqrtf( x*x + y*y + z*z + w*w );
}

float Vector4::LengthSquared() const
{
    return ( x*x + y*y + z*z + w*w );
}

void Vector4::Normalize()
{
    float lengthsq = x*x + y*y + z*z + w*w;
    
    if (Mathf::IsZero(lengthsq))
    {
        Zero();
    }
    else
    {
        float factor = 1.0f/sqrtf(lengthsq);
        x *= factor;
        y *= factor;
        z *= factor;
        w *= factor;
    }
}

Vector4 Vector4::Normalized() const
{
    float lengthsq = x*x + y*y + z*z + w*w;
    
    if ( Mathf::IsZero( lengthsq ) )
    {
        return Vector4::zero;
    }
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        return Vector4 (x,y,z,w) * factor;
    }
}

// ========================================================================
// STATIC METHODS
// ========================================================================

float Vector4::Dot( const Vector4& a, const Vector4& b )
{
    return (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
}

Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, float t)
{
    return a + t*(b-a);
}

Vector4 Vector4::Max(const Vector4& a, const Vector4& b)
{
    return (a.LengthSquared() > b.LengthSquared()) ? a : b;
}

Vector4 Vector4::Min(const Vector4& a, const Vector4& b)
{
    return (a.LengthSquared() < b.LengthSquared()) ? a : b;
}

// ========================================================================
// FRIEND OPERATORS
// ========================================================================

std::ostream& operator<<(std::ostream& os, const Vector4& vector)
{
    os << "<" << vector.x << "," << vector.y << "," << vector.z << "," << vector.w << ">";
    return os;
}

// ========================================================================
// MEMBER OPERATORS
// ========================================================================

Vector4& Vector4::operator += (const Vector4& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vector4& Vector4::operator -= (const Vector4& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vector4& Vector4::operator *= ( float scalar )
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vector4& Vector4::operator /= ( float scalar )
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

Vector4 Vector4::operator - () const
{
    return Vector4(-x, -y, -z, -w);
}

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Vector4& a, const Vector4& b)
{
    if ( Mathf::AreEqual( a.x, b.x )
        && Mathf::AreEqual( a.y, b.y )
        && Mathf::AreEqual( a.z, b.z )
        && Mathf::AreEqual( a.w, b.w ))
        return true;
    
    return false;
}

bool operator != (const Vector4& a, const Vector4& b)
{
    if ( Mathf::AreEqual( a.x, b.x )
        && Mathf::AreEqual( a.y, b.y )
        && Mathf::AreEqual( a.z, b.z )
        && Mathf::AreEqual( a.w, b.w ))
        return false;
    
    return true;
}

Vector4 operator + (const Vector4& a, const Vector4& b)
{
    return Vector4( a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w );
}

Vector4 operator - (const Vector4& a, const Vector4& b)
{
    return Vector4( a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w );
}

Vector4 operator * ( float scalar, const Vector4& vector )
{
    return Vector4( scalar*vector.x, scalar*vector.y, scalar*vector.z, scalar*vector.w );
}

Vector4 operator * (const Vector4& vector, float scalar)
{
    return Vector4( scalar*vector.x, scalar*vector.y, scalar*vector.z, scalar*vector.w );
}

Vector4 operator / (const Vector4& vector, float scalar)
{
    return Vector4( vector.x/scalar, vector.y/scalar, vector.z/scalar, vector.w/scalar );
}
