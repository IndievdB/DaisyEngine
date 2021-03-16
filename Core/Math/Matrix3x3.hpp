#ifndef Matrix3x3_hpp
#define Matrix3x3_hpp

#include <iostream>

class Vector3;
class Quaternion;

class Matrix3x3
{
public:
    // ========================================================================
    // PROPERTIES
    // ========================================================================
    
    float mV[9];

    // ========================================================================
    // CONSTRUCTORS
    // ========================================================================
    
    inline Matrix3x3() { Identity(); }
    explicit Matrix3x3( const Quaternion& quat );
    
    // ========================================================================
    // PUBLIC METHODS
    // ========================================================================
    
    Matrix3x3 Adjoint() const;
    void Clean();
    float Determinant() const;
    void GetAxisAngle( Vector3& axis, float& angle );
    Vector3 GetColumn( unsigned int i ) const;
    void GetColumns( Vector3& col1, Vector3& col2, Vector3& col3 ) const;
    Vector3 GetEulerAnglesXYZ() const;
    Vector3 GetEulerAnglesZYX() const;
    Vector3 GetRow( unsigned int i ) const;
    void GetRows( Vector3& row1, Vector3& row2, Vector3& row3 ) const;
    void Identity();
    void Invert();
    bool IsIdentity() const;
    bool IsZero() const;
    void SetColumns( const Vector3& col1, const Vector3& col2, const Vector3& col3 );
	void SetDiagonal(float a, float b, float c);
    void SetRows( const Vector3& row1, const Vector3& row2, const Vector3& row3 );
    float Trace() const;
    Matrix3x3& Transpose();
    
    // ========================================================================
    // STATIC METHODS
    // ========================================================================
    
    static Matrix3x3 Inverse( const Matrix3x3& mat );
    static Matrix3x3 Rotation( const Quaternion& quat );
    static Matrix3x3 Rotation( float zRot, float yRot, float xRot );
    static Matrix3x3 Rotation( const Vector3& axis, float angle );
    static Matrix3x3 RotationX( float angle );
    static Matrix3x3 RotationY( float angle );
    static Matrix3x3 RotationZ( float angle );
	static Matrix3x3 Scaling( const Vector3& factors );
	static Matrix3x3 SkewSymmetric( const Vector3& vector );
	static Matrix3x3 Transpose(const Matrix3x3& mat);
    
    // ========================================================================
    // FRIEND OPERATORS
    // ========================================================================
    
    friend std::ostream& operator<<(std::ostream& os, const Matrix3x3& matrix);
    
    // ========================================================================
    // MEMBER OPERATORS
    // ========================================================================
    
    inline float& operator () (unsigned int i, unsigned int j);
    inline float operator () (unsigned int i, unsigned int j) const;
    operator const float *() const { return mV; }
    operator float *() { return mV; }
    Matrix3x3& operator += ( const Matrix3x3& other );
    Matrix3x3& operator -= ( const Matrix3x3& other );
    Matrix3x3& operator *= ( const Matrix3x3& other );
    Matrix3x3& operator *= ( float scalar );
    Matrix3x3 operator - () const;
};

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Matrix3x3& a, const Matrix3x3& b);
bool operator != (const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator + (const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator - (const Matrix3x3& a, const Matrix3x3& b);
Matrix3x3 operator * (const Matrix3x3& a, const Matrix3x3& b);
Vector3 operator * (const Matrix3x3& matrix, const Vector3& vector);
Vector3 operator * (const Vector3& vector, const Matrix3x3& matrix);
Matrix3x3 operator * (const Matrix3x3& matrix, float scalar);
Matrix3x3 operator * (float scalar, const Matrix3x3& matrix);

// ========================================================================
// INLINES
// ========================================================================

inline float& Matrix3x3::operator()(unsigned int i, unsigned int j)
{
    return mV[i + 3*j];
}

inline float Matrix3x3::operator()(unsigned int i, unsigned int j) const
{
    return mV[i + 3*j];
}

#endif
