#ifndef Vector4_hpp
#define Vector4_hpp

#include <iostream>

class Vector4
{
public:
    // ========================================================================
    // STATIC PROPERTIES
    // ========================================================================
    
    static Vector4 zero, one;
    
    // ========================================================================
    // PROPERTIES
    // ========================================================================
    
    float x, y, z, w;
    
    // ========================================================================
    // CONSTRUCTORS
    // ========================================================================
    
    inline Vector4() { Zero(); }
    inline Vector4( float _x, float _y, float _z, float _w ) : x(_x), y(_y), z(_z), w(_w) {}
    
    // ========================================================================
    // PUBLIC METHODS
    // ========================================================================
    
    void Clean();
    bool IsUnit() const;
    bool IsZero() const;
    float Length() const;
    float LengthSquared() const;
    void Normalize();
    Vector4 Normalized() const;
    inline void Set( float _x, float _y, float _z, float _w );
    inline void Zero();
    
    // ========================================================================
    // STATIC METHODS
    // ========================================================================
    
    static float Dot( const Vector4& a, const Vector4& b );
    static Vector4 Lerp(const Vector4& a, const Vector4& b, float t);
    static Vector4 Max(const Vector4& a, const Vector4& b);
    static Vector4 Min(const Vector4& a, const Vector4& b);
    
    // ========================================================================
    // FRIEND OPERATORS
    // ========================================================================
    
    friend std::ostream& operator<<(std::ostream& os, const Vector4& vector);
    
    // ========================================================================
    // MEMBER OPERATORS
    // ========================================================================
    
    inline float& operator [] ( unsigned int i ) { return (&x)[i]; }
    inline float operator [] ( unsigned int i ) const { return (&x)[i]; }
    Vector4& operator += (const Vector4& other);
    Vector4& operator -= (const Vector4& other);
    Vector4& operator *= ( float scalar );
    Vector4& operator /= ( float scalar );
    Vector4 operator - () const;
};

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Vector4& a, const Vector4& b);
bool operator != (const Vector4& a, const Vector4& b);
Vector4 operator + (const Vector4& a, const Vector4& b);
Vector4 operator - (const Vector4& a, const Vector4& b);
Vector4 operator * (float scalar, const Vector4& vector);
Vector4 operator * (const Vector4& vector, float scalar);
Vector4 operator / (const Vector4& vector, float scalar);

// ========================================================================
// INLINES
// ========================================================================

inline void Vector4::Set( float _x, float _y, float _z, float _w )
{
    x = _x; y = _y; z = _z; w = _w;
}

inline void Vector4::Zero()
{
    x = y = z = w = 0.0f;
}

#endif
