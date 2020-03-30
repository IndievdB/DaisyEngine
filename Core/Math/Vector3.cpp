#include "Vector3.hpp"
#include "Mathf.hpp"

// ========================================================================
// STATIC PROPERTIES
// ========================================================================

Vector3 Vector3::back( 0.0f, 0.0f, -1.0f );
Vector3 Vector3::forward( 0.0f, 0.0f, 1.0f );
Vector3 Vector3::down( 0.0f, -1.0f, 0.0f );
Vector3 Vector3::up( 0.0f, 1.0f, 0.0f );
Vector3 Vector3::left( -1.0f, 0.0f, 0.0f );
Vector3 Vector3::right( 1.0f, 0.0f, 0.0f );
Vector3 Vector3::zero( 0.0f, 0.0f, 0.0f );
Vector3 Vector3::one( 1.0f, 1.0f, 1.0f );

// ========================================================================
// PUBLIC METHODS
// ========================================================================

void Vector3::Clean()
{
    if (Mathf::IsZero(x))
        x = 0.0f;
    
    if (Mathf::IsZero(y))
        y = 0.0f;
    
    if (Mathf::IsZero(z))
        z = 0.0f;
}

bool Vector3::IsUnit() const
{
    return Mathf::IsZero(1.0f - x*x - y*y - z*z);
}

bool Vector3::IsZero() const
{
    return Mathf::IsZero(x*x + y*y + z*z);
}

float Vector3::Length() const
{
    return sqrtf( x*x + y*y + z*z );
}

float Vector3::LengthSquared() const
{
    return (x*x + y*y + z*z);
}

float Vector3::MaxComponent() const
{
	return fmaxf(fmaxf(x, y),z);
}

void Vector3::Normalize()
{
    float lengthsq = x*x + y*y + z*z;
    
    if ( Mathf::IsZero( lengthsq ) )
    {
        Zero();
    }
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        x *= factor;
        y *= factor;
        z *= factor;
    }
}

Vector3 Vector3::Normalized() const
{
    float lengthsq = x*x + y*y + z*z;
    
    if ( Mathf::IsZero( lengthsq ) )
    {
        return Vector3::zero;
    }
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        return Vector3 (x,y,z) * factor;
    }
}

// ========================================================================
// STATIC METHODS
// ========================================================================

float Vector3::Angle(const Vector3& from, const Vector3& to)
{
    return atan2(Vector3::Cross(from, to).Length(), Vector3::Dot(from,to));
}

bool Vector3::Colinear(const Vector3& a, const Vector3& b)
{
    return Vector3::Cross(a, b).Length() == 0;
}

Vector3 Vector3::Cross( const Vector3& a, const Vector3& b )
{
    return Vector3( a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x );
}

float Vector3::Distance( const Vector3& a, const Vector3& b )
{
    float x = a.x - b.x;
    float y = a.y - b.y;
    float z = a.z - b.z;
    
    return sqrtf( x*x + y*y + z*z );
}

float Vector3::DistanceSquared( const Vector3& a, const Vector3& b )
{
    float x = a.x - b.x;
    float y = a.y - b.y;
    float z = a.z - b.z;
    
    return ( x*x + y*y + z*z );
}

float Vector3::Dot( const Vector3& a, const Vector3& b )
{
    return (a.x*b.x + a.y*b.y + a.z*b.z);
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t)
{
    return a + t*(b-a);
}

Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
{
    return (a.LengthSquared() > b.LengthSquared()) ? a : b;
}

Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
{
    return (a.LengthSquared() < b.LengthSquared()) ? a : b;
}

void Vector3::OrthoNormalize(Vector3& normal, Vector3& tangent, Vector3& binormal)
{
    // Create first basis vector.
    
    if (normal.IsZero())
    {
        normal = Vector3::right;
    }
    else
    {
        normal.Normalize();
    }
    
    // Create second basis vector.
    
    if (Vector3::Colinear(tangent, normal) || tangent.IsZero())
    {
        tangent.x = normal.z;
        tangent.y = normal.x;
        tangent.z = normal.y;
    }
    
    tangent = tangent - Vector3::Project(tangent, normal);
    tangent.Normalize();
    
    // Create third basis vector.
    
    if (Vector3::Colinear(binormal, normal) || Vector3::Colinear(binormal, tangent) || binormal.IsZero())
    {
        binormal.x = tangent.z;
        binormal.y = tangent.x;
        binormal.z = tangent.y;
    }
    
    binormal = binormal - Vector3::Project(binormal, normal) - Vector3::Project(binormal, tangent);
    binormal.Normalize();
}

Vector3 Vector3::Project(const Vector3& vector, const Vector3& onNormal)
{
    if (vector.IsZero() || onNormal.IsZero()) return Vector3::zero;
    
    return (Vector3::Dot(vector, onNormal) / onNormal.LengthSquared()) * onNormal;
}

Vector3 Vector3::Reflect(const Vector3& inDirection, const Vector3& inNormal)
{
    if (inNormal.IsZero()) return inDirection;
    
    Vector3 projection = Vector3::Dot(inDirection, inNormal) * inNormal / inNormal.LengthSquared();
    
    return inDirection - 2 * projection;
}

float Vector3::SignedAngle(const Vector3& from, const Vector3& to, const Vector3& axis)
{
    float dot = Vector3::Dot(from, to);
    Vector3 cross = Vector3::Cross(from, to);
    
    float angle = atan2(cross.Length(), dot);
    
    if (Vector3::Dot(axis, cross) < 0)
        angle = -angle;
    
    return angle;
}

Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t)
{
    // For smiple use cases, use a simple solution.
    if (a.IsZero() || b.IsZero())
        return Vector3::Lerp(a, b, t);
    
    // Ensure a and b fit on a unit sphere.
    a.Normalize();
    b.Normalize();
    
    float dot = Vector3::Dot(a, b);
    float theta = acos(dot)*t; // Angle between a and result.
    
    // Get the orthonormal vector relative to a.
    Vector3 bNorm = b - dot*a;
    bNorm.Normalize();
    
    // Use the unit circle to find the result.
    return (a*cos(theta)) + (bNorm*sin(theta));
}

// ========================================================================
// FRIEND OPERATORS
// ========================================================================

std::ostream& operator<<(std::ostream& os, const Vector3& vector)
{
    os << "<" << vector.x << "," << vector.y << "," << vector.z << ">";
    return os;
}

// ========================================================================
// MEMBER OPERATORS
// ========================================================================

Vector3& Vector3::operator += (const Vector3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator -= (const Vector3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator *= ( float scalar )
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator /= ( float scalar )
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vector3 Vector3::operator - () const
{
    return Vector3(-x, -y, -z);
}

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Vector3& a, const Vector3& b)
{
    if ( Mathf::AreEqual( a.x, b.x )
        && Mathf::AreEqual( a.y, b.y )
        && Mathf::AreEqual( a.z, b.z ) )
        return true;
    
    return false;
}

bool operator != (const Vector3& a, const Vector3& b)
{
    if ( Mathf::AreEqual( a.x, b.x )
        && Mathf::AreEqual( a.y, b.y )
        && Mathf::AreEqual( a.z, b.z ) )
        return false;
    
    return true;
}

Vector3 operator + (const Vector3& a, const Vector3& b)
{
    return Vector3( a.x + b.x, a.y + b.y, a.z + b.z );
}

Vector3 operator - (const Vector3& a, const Vector3& b)
{
    return Vector3( a.x - b.x, a.y - b.y, a.z - b.z );
}

Vector3 operator * ( float scalar, const Vector3& vector )
{
    return Vector3( scalar*vector.x, scalar*vector.y, scalar*vector.z );
}

Vector3 operator * (const Vector3& vector, float scalar)
{
    return Vector3( scalar*vector.x, scalar*vector.y, scalar*vector.z );
}

Vector3 operator / (const Vector3& vector, float scalar)
{
    return Vector3( vector.x/scalar, vector.y/scalar, vector.z/scalar );
}
