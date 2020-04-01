#ifndef Vector2_hpp
#define Vector2_hpp

#include <iostream>

class Vector2
{
public:
    // ========================================================================
    // STATIC PROPERTIES
    // ========================================================================
    
    static Vector2 down, up, left, right, zero, one;
    
    // ========================================================================
    // PROPERTIES
    // ========================================================================
    
    float x, y;
    
    // ========================================================================
    // CONSTRUCTORS
    // ========================================================================
    
    inline Vector2() { Zero(); }
    inline Vector2( float _x, float _y ) : x(_x), y(_y) {}
    
    // ========================================================================
    // PUBLIC METHODS
    // ========================================================================
    
    void Clean();
    bool IsUnit() const;
    bool IsZero() const;
    float Length() const;
    float LengthSquared() const;
	float MaxComponent() const;
    void Normalize();
    Vector2 Normalized() const;
    Vector2 Perpendicular() const { return Vector2(-y, x); }
    inline void Set( float _x, float _y );
    inline void Zero();
    
    // ========================================================================
    // STATIC METHODS
    // ========================================================================
    
    static float Angle(const Vector2& from, const Vector2& to);
    static float Distance(const Vector2& a, const Vector2& b);
    static float DistanceSquared(const Vector2& a, const Vector2& b);
    static float Dot(const Vector2& a, const Vector2& b);
    static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);
    static Vector2 Max(const Vector2& a, const Vector2& b);
    static Vector2 Min(const Vector2& a, const Vector2& b);
    static Vector2 Project(const Vector2& vector, const Vector2& onNormal);
    static Vector2 Reflect(const Vector2& inDirection, const Vector2& inNormal);
    static float SignedAngle(const Vector2& from, const Vector2& to);
    static Vector2 Slerp(Vector2 a, Vector2 b, float t);
    
    // ========================================================================
    // FRIEND OPERATORS
    // ========================================================================
    
    friend std::ostream& operator<<(std::ostream& os, const Vector2& vector);
    
    // ========================================================================
    // MEMBER OPERATORS
    // ========================================================================

    inline float& operator [] ( unsigned int i ) { return (&x)[i]; }
    inline float operator [] ( unsigned int i ) const { return (&x)[i]; }
    Vector2& operator += (const Vector2& other);
    Vector2& operator -= (const Vector2& other);
    Vector2& operator *= ( float scalar );
    Vector2& operator /= ( float scalar );
    Vector2 operator - () const;
};

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Vector2& a, const Vector2& b);
bool operator != (const Vector2& a, const Vector2& b);
Vector2 operator + (const Vector2& a, const Vector2& b);
Vector2 operator - (const Vector2& a, const Vector2& b);
Vector2 operator * (float scalar, const Vector2& vector);
Vector2 operator * (const Vector2& vector, float scalar);
Vector2 operator / (const Vector2& vector, float scalar);

// ========================================================================
// INLINES
// ========================================================================

inline void Vector2::Set( float _x, float _y )
{
    x = _x; y = _y;
}

inline void Vector2::Zero()
{
    x = y = 0.0f;
}

#endif
