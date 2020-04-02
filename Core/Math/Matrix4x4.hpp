#ifndef Matrix4x4_hpp
#define Matrix4x4_hpp

#include <iostream>

class Vector3;
class Vector4;
class Matrix3x3;
class Quaternion;
class Transform;

class Matrix4x4
{
public:
    // ========================================================================
    // PROPERTIES
    // ========================================================================
    
    float mV[16];
    
    // ========================================================================
    // CONSTRUCTORS
    // ========================================================================
    
    inline Matrix4x4() { Identity(); }
    explicit Matrix4x4( const Quaternion& quat );
    explicit Matrix4x4( const Matrix3x3& matrix );
    
    // ========================================================================
    // PUBLIC METHODS
    // ========================================================================
    
    void AffineInvert();
    void Clean();
    void GetAxisAngle( Vector3& axis, float& angle );
    Vector4 GetColumn( unsigned int i ) const;
    void GetColumns( Vector4& col1, Vector4& col2, Vector4& col3, Vector4& col4 ) const;
    Vector3 GetEulerAngles() const;
    Vector4 GetRow( unsigned int i ) const;
    void GetRows( Vector4& row1, Vector4& row2, Vector4& row3, Vector4& row4 ) const;
    void Identity();
    bool IsIdentity() const;
    bool IsZero() const;
    void SetColumns( const Vector4& col1, const Vector4& col2, const Vector4& col3, const Vector4& col4 );
    void SetRows( const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4 );
    Vector3 TransformVector( const Vector3& vector ) const;
    Vector3 TransformPoint( const Vector3& point ) const;
    Matrix4x4& Transpose();

    // ========================================================================
    // STATIC METHODS
    // ========================================================================
    
    static Matrix4x4 AffineInverse( const Matrix4x4& mat );
	static Matrix4x4 LookAt( const Vector3& eye, const Vector3& center, const Vector3& up);
	static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float near, float far);
	static Matrix4x4 Perspective( float fov, float aspect, float near, float far);
    static Matrix4x4 Rotation( const Matrix3x3& matrix );
    static Matrix4x4 Rotation( const Quaternion& quat );
    static Matrix4x4 Rotation( float zRot, float yRot, float xRot );
    static Matrix4x4 Rotation( const Vector3& axis, float angle );
    static Matrix4x4 RotationX( float angle );
    static Matrix4x4 RotationY( float angle );
    static Matrix4x4 RotationZ( float angle );
    static Matrix4x4 Scaling( const Vector3& factors );
	static Matrix4x4 Transformation( Vector3 translation, Vector3 scale, Quaternion rotation );
	static Matrix4x4 Transformation( Vector3 translation, Quaternion rotation );
	static Matrix4x4 Transformation ( Transform transform );
    static Matrix4x4 Translation( const Vector3& vector );
    
    // ========================================================================
    // FRIEND OPERATORS
    // ========================================================================
    
    friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& matrix);
    
    // ========================================================================
    // MEMBER OPERATORS
    // ========================================================================
    
    inline float& operator () (unsigned int i, unsigned int j);
    inline float operator () (unsigned int i, unsigned int j) const;
    operator const float *() const { return mV; }
    operator float *() { return mV; }
    Matrix4x4& operator += ( const Matrix4x4& other );
    Matrix4x4& operator -= ( const Matrix4x4& other );
    Matrix4x4& operator *= ( const Matrix4x4& other );
    Matrix4x4& operator *= ( float scalar );
    Matrix4x4 operator - () const;
};

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Matrix4x4& a, const Matrix4x4& b);
bool operator != (const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator + (const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator - (const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 operator * (const Matrix4x4& a, const Matrix4x4& b);
Vector4 operator * (const Matrix4x4& matrix, const Vector4& vector);
Vector4 operator * (const Vector4& vector, const Matrix4x4& matrix);
Matrix4x4 operator * (const Matrix4x4& matrix, float scalar);
Matrix4x4 operator * (float scalar, const Matrix4x4& matrix);

// ========================================================================
// INLINES
// ========================================================================

inline float& Matrix4x4::operator()(unsigned int i, unsigned int j)
{
    return mV[i + 4*j];
}

inline float Matrix4x4::operator()(unsigned int i, unsigned int j) const
{
    return mV[i + 4*j];
}

#endif