#include "Quaternion.hpp"
#include "Matrix3x3.hpp"
#include "Vector3.hpp"
#include "Mathf.hpp"

// ========================================================================
// CONSTRUCTORS
// ========================================================================

Matrix3x3::Matrix3x3( const Quaternion& quat )
{
    *this = Rotation(quat);
}

// ========================================================================
// PUBLIC METHODS
// ========================================================================

Matrix3x3 Matrix3x3::Adjoint() const
{
    Matrix3x3 result;
    
    // compute transpose of cofactors
    result.mV[0] = mV[4]*mV[8] - mV[5]*mV[7];
    result.mV[1] = mV[2]*mV[7] - mV[1]*mV[8];
    result.mV[2] = mV[1]*mV[5] - mV[2]*mV[4];
    
    result.mV[3] = mV[5]*mV[6] - mV[3]*mV[8];
    result.mV[4] = mV[0]*mV[8] - mV[2]*mV[6];
    result.mV[5] = mV[2]*mV[3] - mV[0]*mV[5];
    
    result.mV[6] = mV[3]*mV[7] - mV[4]*mV[6];
    result.mV[7] = mV[1]*mV[6] - mV[0]*mV[7];
    result.mV[8] = mV[0]*mV[4] - mV[1]*mV[3];
    
    return result;
}

void Matrix3x3::Clean()
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        if ( Mathf::IsZero( mV[i] ) )
            mV[i] = 0.0f;
    }
}

float Matrix3x3::Determinant() const
{
    return mV[0]*(mV[4]*mV[8] - mV[5]*mV[7])
    + mV[3]*(mV[2]*mV[7] - mV[1]*mV[8])
    + mV[6]*(mV[1]*mV[5] - mV[2]*mV[4]);
}

void Matrix3x3::GetAxisAngle( Vector3& axis, float& angle )
{
    float trace = mV[0] + mV[4] + mV[8];
    float cosTheta = 0.5f*(trace - 1.0f);
    angle = acosf( cosTheta );
    
    // angle is zero, axis can be anything
    if ( Mathf::IsZero( angle ) )
    {
        axis = Vector3::right;
    }
    // standard case
    else if ( angle < kPI-kEpsilon )
    {
        axis.Set( mV[5]-mV[7], mV[6]-mV[2], mV[1]-mV[3] );
        axis.Normalize();
    }
    // angle is 180 degrees
    else
    {
        unsigned int i = 0;
        if ( mV[4] > mV[0] )
            i = 1;
        if ( mV[8] > mV[i + 3*i] )
            i = 2;
        unsigned int j = (i+1)%3;
        unsigned int k = (j+1)%3;
        float s = sqrtf( mV[i + 3*i] - mV[j + 3*j] - mV[k + 3*k] + 1.0f );
        axis[i] = 0.5f*s;
        float recip = 1.0f/s;
        axis[j] = (mV[i + 3*j])*recip;
        axis[k] = (mV[k + 3*i])*recip;
    }
}

Vector3 Matrix3x3::GetColumn( unsigned int i ) const
{
    return Vector3( mV[3*i], mV[3*i+1], mV[3*i+2] );
}

void Matrix3x3::GetColumns( Vector3& col1, Vector3& col2, Vector3& col3 ) const
{
    col1.x = mV[0];
    col1.y = mV[1];
    col1.z = mV[2];
    
    col2.x = mV[3];
    col2.y = mV[4];
    col2.z = mV[5];
    
    col3.x = mV[6];
    col3.y = mV[7];
    col3.z = mV[8];
}

Vector3 Matrix3x3::GetEulerAnglesXYZ() const
{
    float Cx, Sx;
    float Cy, Sy;
    float Cz, Sz;
    
    Sy = mV[6];
    Cy = sqrtf(fmax(0.0f, 1.0f - Sy * Sy));
    
    // normal case
    if ( !Mathf::IsZero( Cy ) )
    {
        float factor = 1.0f/Cy;
        Sx = -mV[7]*factor;
        Cx = mV[8]*factor;
        Sz = -mV[3]*factor;
        Cz = mV[0]*factor;
    }
    // x and z axes aligned
    else
    {
        Sz = 0.0f;
        Cz = 1.0f;
        Sx = mV[5];
        Cx = mV[4];
    }

    return Vector3(atan2f( Sx, Cx ), atan2f( Sy, Cy ), atan2f( Sz, Cz ));
}

Vector3 Matrix3x3::GetEulerAnglesZYX() const
{
    float Cx, Sx;
    float Cy, Sy;
    float Cz, Sz;

    Sy = -mV[2];
    Cy = sqrtf(fmax(0.0f, 1.0f - Sy * Sy));

    // normal case
    if (!Mathf::IsZero(Cy))
    {
        float factor = 1.0f / Cy;
        Sx = mV[1] * factor;
        Cx = mV[0] * factor;
        Sz = mV[5] * factor;
        Cz = mV[8] * factor;
    }
    // x and z axes aligned
    else
    {
        Sz = 0.0f;
        Cz = 1.0f;
        Sx = mV[3];
        Cx = mV[4];
    }

    return Vector3(atan2f(Sz, Cz), atan2f(Sy, Cy), atan2f(Sx, Cx));
}

Vector3 Matrix3x3::GetRow( unsigned int i ) const
{
    return Vector3( mV[i], mV[i+3], mV[i+6] );
}

void Matrix3x3::GetRows( Vector3& row1, Vector3& row2, Vector3& row3 ) const
{
    row1.x = mV[0];
    row1.y = mV[3];
    row1.z = mV[6];
    
    row2.x = mV[1];
    row2.y = mV[4];
    row2.z = mV[7];
    
    row3.x = mV[2];
    row3.y = mV[5];
    row3.z = mV[8];
}

void Matrix3x3::Identity()
{
    mV[0] = 1.0f;
    mV[1] = 0.0f;
    mV[2] = 0.0f;
    mV[3] = 0.0f;
    mV[4] = 1.0f;
    mV[5] = 0.0f;
    mV[6] = 0.0f;
    mV[7] = 0.0f;
    mV[8] = 1.0f;
}

void Matrix3x3::Invert()
{
    *this = Matrix3x3::Inverse(*this);
}

bool Matrix3x3::IsIdentity() const
{
    return Mathf::AreEqual( 1.0f, mV[0] )
    && Mathf::AreEqual( 1.0f, mV[4] )
    && Mathf::AreEqual( 1.0f, mV[8] )
    && Mathf::IsZero( mV[1] )
    && Mathf::IsZero( mV[2] )
    && Mathf::IsZero( mV[3] )
    && Mathf::IsZero( mV[5] )
    && Mathf::IsZero( mV[6] )
    && Mathf::IsZero( mV[7] );
}

bool Matrix3x3::IsZero() const
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        if ( !Mathf::IsZero( mV[i] ) )
            return false;
    }
    
    return true;
}

void Matrix3x3::SetColumns( const Vector3& col1, const Vector3& col2, const Vector3& col3 )
{
    mV[0] = col1.x;
    mV[1] = col1.y;
    mV[2] = col1.z;
    
    mV[3] = col2.x;
    mV[4] = col2.y;
    mV[5] = col2.z;
    
    mV[6] = col3.x;
    mV[7] = col3.y;
    mV[8] = col3.z;
}

void Matrix3x3::SetDiagonal(float a, float b, float c)
{
	mV[0] = a;
	mV[4] = b;
	mV[8] = c;
}

void Matrix3x3::SetRows( const Vector3& row1, const Vector3& row2, const Vector3& row3 )
{
    mV[0] = row1.x;
    mV[3] = row1.y;
    mV[6] = row1.z;
    
    mV[1] = row2.x;
    mV[4] = row2.y;
    mV[7] = row2.z;
    
    mV[2] = row3.x;
    mV[5] = row3.y;
    mV[8] = row3.z;
}

float Matrix3x3::Trace() const
{
    return mV[0] + mV[4] + mV[8];
}

Matrix3x3& Matrix3x3::Transpose()
{
    float temp = mV[1];
    mV[1] = mV[3];
    mV[3] = temp;
    
    temp = mV[2];
    mV[2] = mV[6];
    mV[6] = temp;
    
    temp = mV[5];
    mV[5] = mV[7];
    mV[7] = temp;
    
    return *this;
}

// ========================================================================
// STATIC METHODS
// ========================================================================

Matrix3x3 Matrix3x3::Inverse( const Matrix3x3& mat )
{
    Matrix3x3 result;
    
    // compute determinant
    
    float cofactor0 = mat.mV[4]*mat.mV[8] - mat.mV[5]*mat.mV[7];
    float cofactor3 = mat.mV[2]*mat.mV[7] - mat.mV[1]*mat.mV[8];
    float cofactor6 = mat.mV[1]*mat.mV[5] - mat.mV[2]*mat.mV[4];
    float det = mat.mV[0]*cofactor0 + mat.mV[3]*cofactor3 + mat.mV[6]*cofactor6;
    
    if (Mathf::IsZero( det ))
    {
        return result;
    }
    
    // create adjoint matrix and multiply by 1/det to get inverse
    
    float invDet = 1.0f/det;
    
    result.mV[0] = invDet*cofactor0;
    result.mV[1] = invDet*cofactor3;
    result.mV[2] = invDet*cofactor6;
    
    result.mV[3] = invDet*(mat.mV[5]*mat.mV[6] - mat.mV[3]*mat.mV[8]);
    result.mV[4] = invDet*(mat.mV[0]*mat.mV[8] - mat.mV[2]*mat.mV[6]);
    result.mV[5] = invDet*(mat.mV[2]*mat.mV[3] - mat.mV[0]*mat.mV[5]);
    
    result.mV[6] = invDet*(mat.mV[3]*mat.mV[7] - mat.mV[4]*mat.mV[6]);
    result.mV[7] = invDet*(mat.mV[1]*mat.mV[6] - mat.mV[0]*mat.mV[7]);
    result.mV[8] = invDet*(mat.mV[0]*mat.mV[4] - mat.mV[1]*mat.mV[3]);
    
    return result;
}

Matrix3x3 Matrix3x3::Rotation( const Quaternion& quat )
{
    float xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
    
    xs = quat.x+quat.x;
    ys = quat.y+quat.y;
    zs = quat.z+quat.z;
    wx = quat.w*xs;
    wy = quat.w*ys;
    wz = quat.w*zs;
    xx = quat.x*xs;
    xy = quat.x*ys;
    xz = quat.x*zs;
    yy = quat.y*ys;
    yz = quat.y*zs;
    zz = quat.z*zs;
    
    Matrix3x3 result;
    
    result[0] = 1.0f - (yy + zz);
    result[3] = xy - wz;
    result[6] = xz + wy;
    
    result[1] = xy + wz;
    result[4] = 1.0f - (xx + zz);
    result[7] = yz - wx;
    
    result[2] = xz - wy;
    result[5] = yz + wx;
    result[8] = 1.0f - (xx + yy);
    
    return result;
}

Matrix3x3 Matrix3x3::Rotation( float zRot, float yRot, float xRot )
{
    float Cx = cosf(xRot);
    float Sx = sinf(xRot);
    
    float Cy = cosf(yRot);
    float Sy = sinf(yRot);
    
    float Cz = cosf(zRot);
    float Sz = sinf(zRot);
    
    Matrix3x3 result;
    
    result[0] =  (Cy * Cz);
    result[3] = -(Cy * Sz);
    result[6] =  Sy;
    
    result[1] =  (Sx * Sy * Cz) + (Cx * Sz);
    result[4] = -(Sx * Sy * Sz) + (Cx * Cz);
    result[7] = -(Sx * Cy);
    
    result[2] = -(Cx * Sy * Cz) + (Sx * Sz);
    result[5] =  (Cx * Sy * Sz) + (Sx * Cz);
    result[8] =  (Cx * Cy);
    
    return result;
}

Matrix3x3 Matrix3x3::Rotation( const Vector3& axis, float angle )
{
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;
    
    Vector3 nAxis = axis.Normalized();
    
    // intermediate values
    float tx = t*nAxis.x;  float ty = t*nAxis.y;  float tz = t*nAxis.z;
    float sx = s*nAxis.x;  float sy = s*nAxis.y;  float sz = s*nAxis.z;
    float txy = tx*nAxis.y; float tyz = ty*nAxis.z; float txz = tx*nAxis.z;
 
    Matrix3x3 result;
    
    // set matrix
    result[0] = tx*nAxis.x + c;
    result[3] = txy - sz;
    result[6] = txz + sy;
    result[1] = txy + sz;
    result[4] = ty*nAxis.y + c;
    result[7] = tyz - sx;
    result[2] = txz - sy;
    result[5] = tyz + sx;
    result[8] = tz*nAxis.z + c;
    
    return result;
}

Matrix3x3 Matrix3x3::RotationX( float angle )
{
    float sintheta = sinf(angle);
    float costheta = cosf(angle);
    
    Matrix3x3 result;
    
    result[0] = 1.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    result[3] = 0.0f;
    result[4] = costheta;
    result[5] = sintheta;
    result[6] = 0.0f;
    result[7] = -sintheta;
    result[8] = costheta;
    
    return result;
}

Matrix3x3 Matrix3x3::RotationY( float angle )
{
    float sintheta = sinf(angle);
    float costheta = cosf(angle);
    
    Matrix3x3 result;
    
    result[0] = costheta;
    result[1] = 0.0f;
    result[2] = -sintheta;
    result[3] = 0.0f;
    result[4] = 1.0f;
    result[5] = 0.0f;
    result[6] = sintheta;
    result[7] = 0.0f;
    result[8] = costheta;
    
    return result;
}

Matrix3x3 Matrix3x3::RotationZ( float angle )
{
    float sintheta = sinf(angle);
    float costheta = cosf(angle);
    
    Matrix3x3 result;
    
    result[0] = costheta;
    result[1] = sintheta;
    result[2] = 0.0f;
    result[3] = -sintheta;
    result[4] = costheta;
    result[5] = 0.0f;
    result[6] = 0.0f;
    result[7] = 0.0f;
    result[8] = 1.0f;
    
    return result;
}

Matrix3x3 Matrix3x3::Scaling( const Vector3& factors )
{
    Matrix3x3 result;
    
    result[0] = factors.x;
    result[1] = 0.0f;
    result[2] = 0.0f;
    result[3] = 0.0f;
    result[4] = factors.y;
    result[5] = 0.0f;
    result[6] = 0.0f;
    result[7] = 0.0f;
    result[8] = factors.z;
    
    return result;
}

Matrix3x3 Matrix3x3::SkewSymmetric(const Vector3& vector)
{
	Matrix3x3 result;

	result[0] = result[4] = result[8] = 0;
	result[1] = -vector.z;
	result[2] = vector.y;
	result[3] = vector.z;
	result[5] = -vector.x;
	result[6] = -vector.y;
	result[7] = vector.x;

	return result;
}

Matrix3x3 Matrix3x3::Transpose(const Matrix3x3& mat)
{
	Matrix3x3 result;

	result[0] = mat[0];
	result[1] = mat[3];
	result[2] = mat[6];
	result[3] = mat[1];
	result[4] = mat[4];
	result[5] = mat[7];
	result[6] = mat[2];
	result[7] = mat[5];
	result[8] = mat[8];

	return result;
}

// ========================================================================
// FRIEND OPERATORS
// ========================================================================

std::ostream& operator<<(std::ostream& os, const Matrix3x3& matrix)
{
    // row
    for (unsigned int i = 0; i < 3; ++i)
    {
        os << "| ";
        // column
        for (unsigned int j = 0; j < 3; ++j )
        {
            os << matrix.mV[ j*3 + i ] << ' ';
        }
    }
    
    os << "|";
    
    return os;
}

// ========================================================================
// MEMBER OPERATORS
// ========================================================================

Matrix3x3& Matrix3x3::operator += ( const Matrix3x3& other )
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        mV[i] += other.mV[i];
    }
    
    return *this;
}

Matrix3x3& Matrix3x3::operator -= ( const Matrix3x3& other )
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        mV[i] -= other.mV[i];
    }
    
    return *this;
}

Matrix3x3& Matrix3x3::operator *= ( const Matrix3x3& other )
{
    Matrix3x3 result;
    
    result.mV[0] = mV[0]*other.mV[0] + mV[3]*other.mV[1] + mV[6]*other.mV[2];
    result.mV[1] = mV[1]*other.mV[0] + mV[4]*other.mV[1] + mV[7]*other.mV[2];
    result.mV[2] = mV[2]*other.mV[0] + mV[5]*other.mV[1] + mV[8]*other.mV[2];
    result.mV[3] = mV[0]*other.mV[3] + mV[3]*other.mV[4] + mV[6]*other.mV[5];
    result.mV[4] = mV[1]*other.mV[3] + mV[4]*other.mV[4] + mV[7]*other.mV[5];
    result.mV[5] = mV[2]*other.mV[3] + mV[5]*other.mV[4] + mV[8]*other.mV[5];
    result.mV[6] = mV[0]*other.mV[6] + mV[3]*other.mV[7] + mV[6]*other.mV[8];
    result.mV[7] = mV[1]*other.mV[6] + mV[4]*other.mV[7] + mV[7]*other.mV[8];
    result.mV[8] = mV[2]*other.mV[6] + mV[5]*other.mV[7] + mV[8]*other.mV[8];
    
    for (unsigned int i = 0; i < 9; ++i)
    {
        mV[i] = result.mV[i];
    }
    
    return *this;
}

Matrix3x3& Matrix3x3::operator *= ( float scalar )
{
    mV[0] *= scalar;
    mV[1] *= scalar;
    mV[2] *= scalar;
    mV[3] *= scalar;
    mV[4] *= scalar;
    mV[5] *= scalar;
    mV[6] *= scalar;
    mV[7] *= scalar;
    mV[8] *= scalar;
    
    return *this;
}

Matrix3x3 Matrix3x3::operator - () const
{
    Matrix3x3 result;
    
    for (unsigned int i = 0; i < 9; ++i)
    {
        result.mV[i] = -mV[i];
    }
    
    return result;
}

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Matrix3x3& a, const Matrix3x3& b)
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        if ( !Mathf::AreEqual(a.mV[i], b.mV[i]) )
            return false;
    }
    
    return true;
}

bool operator != (const Matrix3x3& a, const Matrix3x3& b)
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        if ( !Mathf::AreEqual(a.mV[i], b.mV[i]) )
            return true;
    }
    
    return false;
}

Matrix3x3 operator + (const Matrix3x3& a, const Matrix3x3& b)
{
    Matrix3x3 result;
    
    for (unsigned int i = 0; i < 9; ++i)
    {
        result.mV[i] = a.mV[i] + b.mV[i];
    }
    
    return result;
}

Matrix3x3 operator - (const Matrix3x3& a, const Matrix3x3& b)
{
    Matrix3x3 result;
    
    for (unsigned int i = 0; i < 9; ++i)
    {
        result.mV[i] = a.mV[i] - b.mV[i];
    }
    
    return result;
}

Matrix3x3 operator * (const Matrix3x3& a, const Matrix3x3& b)
{
    Matrix3x3 result;
    
    result.mV[0] = a.mV[0]*b.mV[0] + a.mV[3]*b.mV[1] + a.mV[6]*b.mV[2];
    result.mV[1] = a.mV[1]*b.mV[0] + a.mV[4]*b.mV[1] + a.mV[7]*b.mV[2];
    result.mV[2] = a.mV[2]*b.mV[0] + a.mV[5]*b.mV[1] + a.mV[8]*b.mV[2];
    result.mV[3] = a.mV[0]*b.mV[3] + a.mV[3]*b.mV[4] + a.mV[6]*b.mV[5];
    result.mV[4] = a.mV[1]*b.mV[3] + a.mV[4]*b.mV[4] + a.mV[7]*b.mV[5];
    result.mV[5] = a.mV[2]*b.mV[3] + a.mV[5]*b.mV[4] + a.mV[8]*b.mV[5];
    result.mV[6] = a.mV[0]*b.mV[6] + a.mV[3]*b.mV[7] + a.mV[6]*b.mV[8];
    result.mV[7] = a.mV[1]*b.mV[6] + a.mV[4]*b.mV[7] + a.mV[7]*b.mV[8];
    result.mV[8] = a.mV[2]*b.mV[6] + a.mV[5]*b.mV[7] + a.mV[8]*b.mV[8];
    
    return result;
}

Vector3 operator * (const Matrix3x3& matrix, const Vector3& vector)
{
    Vector3 result;
    
    result.x = matrix.mV[0]*vector.x + matrix.mV[3]*vector.y + matrix.mV[6]*vector.z;
    result.y = matrix.mV[1]*vector.x + matrix.mV[4]*vector.y + matrix.mV[7]*vector.z;
    result.z = matrix.mV[2]*vector.x + matrix.mV[5]*vector.y + matrix.mV[8]*vector.z;
    
    return result;
}

Vector3 operator * (const Vector3& vector, const Matrix3x3& matrix)
{
    Vector3 result;
    
    result.x = matrix.mV[0]*vector.x + matrix.mV[1]*vector.y + matrix.mV[2]*vector.z;
    result.y = matrix.mV[3]*vector.x + matrix.mV[4]*vector.y + matrix.mV[5]*vector.z;
    result.z = matrix.mV[6]*vector.x + matrix.mV[7]*vector.y + matrix.mV[8]*vector.z;
    
    return result;
}

Matrix3x3 operator * (const Matrix3x3& matrix, float scalar)
{
    Matrix3x3 result;
    
    result.mV[0] = matrix.mV[0] * scalar;
    result.mV[1] = matrix.mV[1] * scalar;
    result.mV[2] = matrix.mV[2] * scalar;
    result.mV[3] = matrix.mV[3] * scalar;
    result.mV[4] = matrix.mV[4] * scalar;
    result.mV[5] = matrix.mV[5] * scalar;
    result.mV[6] = matrix.mV[6] * scalar;
    result.mV[7] = matrix.mV[7] * scalar;
    result.mV[8] = matrix.mV[8] * scalar;
    
    return result;
}

Matrix3x3 operator * (float scalar, const Matrix3x3& matrix)
{
    Matrix3x3 result;
    
    result.mV[0] = matrix.mV[0] * scalar;
    result.mV[1] = matrix.mV[1] * scalar;
    result.mV[2] = matrix.mV[2] * scalar;
    result.mV[3] = matrix.mV[3] * scalar;
    result.mV[4] = matrix.mV[4] * scalar;
    result.mV[5] = matrix.mV[5] * scalar;
    result.mV[6] = matrix.mV[6] * scalar;
    result.mV[7] = matrix.mV[7] * scalar;
    result.mV[8] = matrix.mV[8] * scalar;
    
    return result;
}
