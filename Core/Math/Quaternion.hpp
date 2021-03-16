#ifndef Quaternion_hpp
#define Quaternion_hpp

#include <iostream>

class Vector3;
class Matrix3x3;

class Quaternion
{
public:
    // ========================================================================
    // STATIC PROPERTIES
    // ========================================================================
    
    static Quaternion zero, identity;
    
    // ========================================================================
    // PROPERTIES
    // ========================================================================
    
    float w, x, y, z;
    
    // ========================================================================
    // CONSTRUCTORS
    // ========================================================================
    
    inline Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    inline Quaternion( float _w, float _x, float _y, float _z ) : w(_w), x(_x), y(_y), z(_z) {}
    Quaternion ( float zRot, float yRot, float xRot );
    Quaternion( const Vector3& axis, float angle );
    Quaternion( const Vector3& from, const Vector3& to );
    explicit Quaternion( const Vector3& vector );
    explicit Quaternion( const Matrix3x3& rotation );
    
    // ========================================================================
    // PUBLIC METHODS
    // ========================================================================
    
    void Clean();
    void GetAxisAngle( Vector3& axis, float& angle );
    Vector3 GetEulerAnglesXYZ() const;
    Vector3 GetEulerAnglesZYX() const;
    void Invert();
    bool IsIdentity() const;
    bool IsUnit() const;
    bool IsZero() const;
	float Magnitude() const;
	float MagnitudeSquared() const;
    float Norm() const;
    void Normalize();
    Quaternion Normalized() const;
    inline void Set( float _w, float _x, float _y, float _z );
    void SetXYZ( float xRot, float yRot, float zRot);
    void SetZYX(float zRot, float yRot, float xRot);
    void Set( const Vector3& axis, float angle );
    void Set( const Vector3& from, const Vector3& to );
    void Set( const Matrix3x3& matrix );
    void SetConjugate();
    inline void SetIdentity();
    Vector3 Rotate( const Vector3& vector ) const;
    inline void Zero();
    
    // ========================================================================
    // STATIC METHODS
    // ========================================================================
    
    static float Angle(const Quaternion& from, const Quaternion& to);
    static Quaternion Conjugate( const Quaternion& quat );
    static float Dot( const Quaternion& a, const Quaternion& b );
    static Quaternion Inverse( const Quaternion& quat );
    static Quaternion Lerp( const Quaternion& a, const Quaternion& b, float t );
    static Quaternion Slerp( const Quaternion& a, const Quaternion& b, float t );
    
    // ========================================================================
    // FRIEND OPERATORS
    // ========================================================================
    
    friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat);
    
    // ========================================================================
    // MEMBER OPERATORS
    // ========================================================================
    
    inline float& operator [] ( unsigned int i ) { return (&x)[i]; }
    inline float operator [] ( unsigned int i ) const { return (&x)[i]; }
    Quaternion& operator += ( const Quaternion& other );
    Quaternion& operator -= ( const Quaternion& other );
    Quaternion& operator *= ( const Quaternion& other );
    Quaternion& operator *= ( float scalar );
    Quaternion operator - () const;
};

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Quaternion& a, const Quaternion& b);
bool operator != (const Quaternion& a, const Quaternion& b);
Quaternion operator + (const Quaternion& a, const Quaternion& b);
Quaternion operator - (const Quaternion& a, const Quaternion& b);
Quaternion operator * (float scalar, const Quaternion& quat);
Quaternion operator * (const Quaternion& quat, float scalar);
Quaternion operator * (const Quaternion& a, const Quaternion& b);

// ========================================================================
// INLINES
// ========================================================================

inline void Quaternion::Set( float _w, float _x, float _y, float _z )
{
    w = _w; x = _x; y = _y; z = _z;
}

inline void Quaternion::SetIdentity()
{
    x = y = z = 0.0f;
    w = 1.0f;
}

inline void Quaternion::Zero()
{
    x = y = z = w = 0.0f;
}

#endif
