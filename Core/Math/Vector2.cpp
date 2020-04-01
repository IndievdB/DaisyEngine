#include "Vector2.hpp"
#include "Mathf.hpp"


// ========================================================================
// STATIC PROPERTIES
// ========================================================================

Vector2 Vector2::down( 0.0f, -1.0f );
Vector2 Vector2::up( 0.0f, 1.0f );
Vector2 Vector2::left( -1.0f, 0.0f );
Vector2 Vector2::right( 1.0f, 0.0f );
Vector2 Vector2::zero( 0.0f, 0.0f );
Vector2 Vector2::one( 1.0f, 1.0f );

// ========================================================================
// PUBLIC METHODS
// ========================================================================

void Vector2::Clean()
{
    if ( Mathf::IsZero( x ) )
        x = 0.0f;
    
    if ( Mathf::IsZero( y ) )
        y = 0.0f;
}

bool Vector2::IsUnit() const
{
    return Mathf::IsZero(1.0f - x*x - y*y);
}

bool Vector2::IsZero() const
{
    return Mathf::IsZero(x*x + y*y);
}

float Vector2::Length() const
{
    return sqrtf( x*x + y*y );
}

float Vector2::LengthSquared() const
{
    return (x*x + y*y);
}

float Vector2::MaxComponent() const
{
	return fmaxf(x, y);
}

void Vector2::Normalize()
{
    // Get the length squared of vector first for speed.
    float lengthsq = x*x + y*y;
    
    // If length squared of the vector is zero, zero out the vector for safety.
    if ( Mathf::IsZero( lengthsq ) )
    {
        Zero();
    }
    // Otherwise, divide the components by the vector's length
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        x *= factor;
        y *= factor;
    }
}

Vector2 Vector2::Normalized() const
{
    float lengthsq = x*x + y*y;
    
    if ( Mathf::IsZero( lengthsq ) )
    {
        return Vector2::zero;
    }
    else
    {
        float factor = 1.0f/sqrtf( lengthsq );
        return Vector2 (x,y) * factor;
    }
}

// ========================================================================
// STATIC METHODS
// ========================================================================

float Vector2::Angle(const Vector2& from, const Vector2& to)
{
    float dot = Vector2::Dot(from, to);
    float cross = from.x*to.y - from.y*to.x;
    float angle = atan2(cross, dot);
    
    return fabsf(angle);
}

float Vector2::Distance( const Vector2& a, const Vector2& b )
{
    float x = a.x - b.x;
    float y = a.y - b.y;
    
    return sqrtf( x*x + y*y );
}

float Vector2::DistanceSquared( const Vector2& a, const Vector2& b )
{
    float x = a.x - b.x;
    float y = a.y - b.y;
    
    return ( x*x + y*y );
}

float Vector2::Dot( const Vector2& a, const Vector2& b )
{
    return (a.x*b.x + a.y*b.y);
}

Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, float t)
{
    return a + t*(b-a);
}

Vector2 Vector2::Max(const Vector2& a, const Vector2& b)
{
    return (a.LengthSquared() > b.LengthSquared()) ? a : b;
}

Vector2 Vector2::Min(const Vector2& a, const Vector2& b)
{
    return (a.LengthSquared() < b.LengthSquared()) ? a : b;
}

Vector2 Vector2::Project(const Vector2& vector, const Vector2& onNormal)
{
    if (vector.IsZero() || onNormal.IsZero()) return Vector2::zero;
    
    return (Vector2::Dot(vector, onNormal) / onNormal.LengthSquared()) * onNormal;
}

Vector2 Vector2::Reflect(const Vector2& inDirection, const Vector2& inNormal)
{
    if (inNormal.IsZero()) return inDirection;
    
    Vector2 projection = Vector2::Dot(inDirection, inNormal) * inNormal / inNormal.LengthSquared();
    
    return inDirection - 2 * projection;
}

float Vector2::SignedAngle(const Vector2& from, const Vector2& to)
{
    float dot = Vector2::Dot(from, to);
    float cross = from.x*to.y - from.y*to.x;
    float angle = atan2(cross, dot);
    
    return angle;
}

Vector2 Vector2::Slerp(Vector2 a, Vector2 b, float t)
{
    // For smiple use cases, use a simple solution.
    if (a.IsZero() || b.IsZero())
        return Vector2::Lerp(a, b, t);
    
    // Ensure a and b fit on a unit circle.
    a.Normalize();
    b.Normalize();
    
    float dot = Vector2::Dot(a, b);
    float theta = acos(dot)*t; // Angle between a and result.
    
    // Get the orthonormal vector relative to a.
    Vector2 bNorm = b - dot*a;
    bNorm.Normalize();
    
    // Use the unit circle to find the result.
    return (cos(theta)*a) + (bNorm*sin(theta));
}

// ========================================================================
// FRIEND OPERATORS
// ========================================================================

std::ostream& operator<<(std::ostream& os, const Vector2& vector)
{
    os << "<" << vector.x << "," << vector.y << ">";
    return os;
}

// ========================================================================
// MEMBER OPERATORS
// ========================================================================

Vector2& Vector2::operator += (const Vector2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator -= (const Vector2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator *= ( float scalar )
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator /= ( float scalar )
{
    x /= scalar;
    y /= scalar;
    return *this;
}

Vector2 Vector2::operator - () const
{
    return Vector2(-x, -y);
}

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Vector2& a, const Vector2& b)
{
    if ( Mathf::AreEqual( a.x, b.x ) && Mathf::AreEqual( a.y, b.y ) )
        return true;
    
    return false;
}

bool operator != (const Vector2& a, const Vector2& b)
{
    if ( Mathf::AreEqual( a.x, b.x ) && Mathf::AreEqual( a.y, b.y ) )
        return false;
    
    return true;
}

Vector2 operator + (const Vector2& a, const Vector2& b)
{
    return Vector2( a.x + b.x, a.y + b.y );
}

Vector2 operator - (const Vector2& a, const Vector2& b)
{
    return Vector2( a.x - b.x, a.y - b.y );
}

Vector2 operator * ( float scalar, const Vector2& vector )
{
    return Vector2( scalar*vector.x, scalar*vector.y );
}

Vector2 operator * (const Vector2& vector, float scalar)
{
    return Vector2( scalar*vector.x, scalar*vector.y );
}

Vector2 operator / (const Vector2& vector, float scalar)
{
    return Vector2( vector.x/scalar, vector.y/scalar );
}
