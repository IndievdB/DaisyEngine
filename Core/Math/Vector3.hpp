#ifndef Vector3_hpp
#define Vector3_hpp

#include <iostream>

class Vector3
{
public:
    // ========================================================================
    // STATIC PROPERTIES
    // ========================================================================
    
    static Vector3 back, forward, down, up, left, right, zero, one;
    
    // ========================================================================
    // PROPERTIES
    // ========================================================================
    
    float x, y, z;
    
    // ========================================================================
    // CONSTRUCTORS
    // ========================================================================
    
    inline Vector3 () { Zero(); }
    inline Vector3 ( float _x, float _y, float _z ) : x(_x), y(_y), z(_z) {}
    
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
    Vector3 Normalized() const;
    inline void Set( float _x, float _y, float _z );
    inline void Zero();
    
    // ========================================================================
    // STATIC METHODS
    // ========================================================================
    
    static float Angle(const Vector3& from, const Vector3& to);
    static bool Colinear(const Vector3& a, const Vector3& b);
    static Vector3 Cross( const Vector3& a, const Vector3& b );
    static float Distance( const Vector3& a, const Vector3& b );
    static float DistanceSquared( const Vector3& a, const Vector3& b );
    static float Dot( const Vector3& a, const Vector3& b );
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
    static Vector3 Max(const Vector3& a, const Vector3& b);
    static Vector3 Min(const Vector3& a, const Vector3& b);
    static void OrthoNormalize(Vector3& normal, Vector3& tangent, Vector3& binormal);
    static Vector3 Project(const Vector3& vector, const Vector3& onNormal);
    static Vector3 Reflect(const Vector3& inDirection, const Vector3& inNormal);
    static float SignedAngle(const Vector3& from, const Vector3& to, const Vector3& axis);
    static Vector3 Slerp(Vector3 a, Vector3 b, float t);
    
    // ========================================================================
    // FRIEND OPERATORS
    // ========================================================================
    
    friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
    
    // ========================================================================
    // MEMBER OPERATORS
    // ========================================================================

    inline float& operator [] ( unsigned int i ) { return (&x)[i]; }
    inline float operator [] ( unsigned int i ) const { return (&x)[i]; }
    Vector3& operator += (const Vector3& other);
    Vector3& operator -= (const Vector3& other);
    Vector3& operator *= ( float scalar );
    Vector3& operator /= ( float scalar );
    Vector3 operator - () const;
};

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Vector3& a, const Vector3& b);
bool operator != (const Vector3& a, const Vector3& b);
Vector3 operator + (const Vector3& a, const Vector3& b);
Vector3 operator - (const Vector3& a, const Vector3& b);
Vector3 operator * (float scalar, const Vector3& vector);
Vector3 operator * (const Vector3& vector, float scalar);
Vector3 operator / (const Vector3& vector, float scalar);

// ========================================================================
// INLINES
// ========================================================================

inline void Vector3::Set( float _x, float _y, float _z )
{
    x = _x; y = _y; z = _z;
}

inline void Vector3::Zero()
{
    x = y = z = 0.0f;
}

#endif
