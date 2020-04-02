#include "Quaternion.hpp"
#include "Matrix4x4.hpp"
#include "Matrix3x3.hpp"
#include "Vector4.hpp"
#include "Vector3.hpp"
#include "Mathf.hpp"
#include "../Transform.hpp"

// ========================================================================
// CONSTRUCTORS
// ========================================================================

Matrix4x4::Matrix4x4( const Quaternion& quat )
{
    *this = Rotation(quat);
}

Matrix4x4::Matrix4x4( const Matrix3x3& matrix )
{
    *this = Rotation(matrix);
}

// ========================================================================
// PUBLIC METHODS
// ========================================================================

void Matrix4x4::AffineInvert()
{
    *this = AffineInverse( *this );
}

void Matrix4x4::Clean()
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        if ( Mathf::IsZero( mV[i] ) )
            mV[i] = 0.0f;
    }
}

void Matrix4x4::GetAxisAngle( Vector3& axis, float& angle )
{
    float trace = mV[0] + mV[5] + mV[10];
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
        axis.Set( mV[6]-mV[9], mV[8]-mV[2], mV[1]-mV[4] );
        axis.Normalize();
    }
    // angle is 180 degrees
    else
    {
        unsigned int i = 0;
        if ( mV[5] > mV[0] )
            i = 1;
        if ( mV[10] > mV[i + 4*i] )
            i = 2;
        
        unsigned int j = (i+1)%3;
        unsigned int k = (j+1)%3;
        float s = sqrtf( mV[i + 4*i] - mV[j + 4*j] - mV[k + 4*k] + 1.0f );
        axis[i] = 0.5f*s;
        float recip = 1.0f/s;
        axis[j] = (mV[i + 4*j])*recip;
        axis[k] = (mV[k + 4*i])*recip;
    }
}

Vector4 Matrix4x4::GetColumn( unsigned int i ) const
{
    return Vector4( mV[4*i], mV[4*i+1], mV[4*i+2], mV[4*i+3] );
}

void Matrix4x4::GetColumns( Vector4& col1, Vector4& col2, Vector4& col3, Vector4& col4 ) const
{
    col1.x = mV[0];
    col1.y = mV[1];
    col1.z = mV[2];
    col1.w = mV[3];
    
    col2.x = mV[4];
    col2.y = mV[5];
    col2.z = mV[6];
    col2.w = mV[7];
    
    col3.x = mV[8];
    col3.y = mV[9];
    col3.z = mV[10];
    col3.w = mV[11];
    
    col4.x = mV[12];
    col4.y = mV[13];
    col4.z = mV[14];
    col4.w = mV[15];
}

Vector3 Matrix4x4::GetEulerAngles() const
{
    float Cx, Sx;
    float Cy, Sy;
    float Cz, Sz;
    
    Sy = mV[8];
    Cy = sqrtf( 1.0f - Sy*Sy );
    
    // normal case
    if ( !Mathf::IsZero( Cy ) )
    {
        float factor = 1.0f/Cy;
        Sx = -mV[9]*factor;
        Cx = mV[10]*factor;
        Sz = -mV[4]*factor;
        Cz = mV[0]*factor;
    }
    // x and z axes aligned
    else
    {
        Sz = 0.0f;
        Cz = 1.0f;
        Sx = mV[6];
        Cx = mV[5];
    }
    
    return Vector3(atan2f( Sx, Cx ), atan2f( Sy, Cy ), atan2f( Sz, Cz ));
}

Vector4 Matrix4x4::GetRow( unsigned int i ) const
{
    return Vector4( mV[i], mV[i+4], mV[i+8], mV[i+12] );
}

void Matrix4x4::GetRows( Vector4& row1, Vector4& row2, Vector4& row3, Vector4& row4 ) const
{
    row1.x = mV[0];
    row1.y = mV[4];
    row1.z = mV[8];
    row1.w = mV[12];
    
    row2.x = mV[1];
    row2.y = mV[5];
    row2.z = mV[9];
    row2.w = mV[13];
    
    row3.x = mV[2];
    row3.y = mV[6];
    row3.z = mV[10];
    row3.w = mV[14];
    
    row4.x = mV[3];
    row4.y = mV[7];
    row4.z = mV[11];
    row4.w = mV[15];
}

void Matrix4x4::Identity()
{
    mV[0] = 1.0f;
    mV[1] = 0.0f;
    mV[2] = 0.0f;
    mV[3] = 0.0f;
    
    mV[4] = 0.0f;
    mV[5] = 1.0f;
    mV[6] = 0.0f;
    mV[7] = 0.0f;
    
    mV[8] = 0.0f;
    mV[9] = 0.0f;
    mV[10] = 1.0f;
    mV[11] = 0.0f;
    
    mV[12] = 0.0f;
    mV[13] = 0.0f;
    mV[14] = 0.0f;
    mV[15] = 1.0f;
}

bool Matrix4x4::IsIdentity() const
{
    return Mathf::AreEqual( 1.0f, mV[0] )
    && Mathf::AreEqual( 1.0f, mV[5] )
    && Mathf::AreEqual( 1.0f, mV[10] )
    && Mathf::AreEqual( 1.0f, mV[15] )
    && Mathf::IsZero( mV[1] )
    && Mathf::IsZero( mV[2] )
    && Mathf::IsZero( mV[3] )
    && Mathf::IsZero( mV[4] )
    && Mathf::IsZero( mV[6] )
    && Mathf::IsZero( mV[7] )
    && Mathf::IsZero( mV[8] )
    && Mathf::IsZero( mV[9] )
    && Mathf::IsZero( mV[11] )
    && Mathf::IsZero( mV[12] )
    && Mathf::IsZero( mV[13] )
    && Mathf::IsZero( mV[14] );
}

bool Matrix4x4::IsZero() const
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        if ( !Mathf::IsZero( mV[i] ) )
            return false;
    }
    return true;
}

void Matrix4x4::SetColumns( const Vector4& col1, const Vector4& col2, const Vector4& col3, const Vector4& col4 )
{
    mV[0] = col1.x;
    mV[1] = col1.y;
    mV[2] = col1.z;
    mV[3] = col1.w;
    
    mV[4] = col2.x;
    mV[5] = col2.y;
    mV[6] = col2.z;
    mV[7] = col2.w;
    
    mV[8] = col3.x;
    mV[9] = col3.y;
    mV[10] = col3.z;
    mV[11] = col3.w;
    
    mV[12] = col4.x;
    mV[13] = col4.y;
    mV[14] = col4.z;
    mV[15] = col4.w;
}

void Matrix4x4::SetRows( const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4 )
{
    mV[0] = row1.x;
    mV[4] = row1.y;
    mV[8] = row1.z;
    mV[12] = row1.w;
    
    mV[1] = row2.x;
    mV[5] = row2.y;
    mV[9] = row2.z;
    mV[13] = row2.w;
    
    mV[2] = row3.x;
    mV[6] = row3.y;
    mV[10] = row3.z;
    mV[14] = row3.w;
    
    mV[3] = row4.x;
    mV[7] = row4.y;
    mV[11] = row4.z;
    mV[15] = row4.w;
}

Vector3 Matrix4x4::TransformVector( const Vector3& vector ) const
{
    Vector3 result;
    
    result.x = mV[0]*vector.x + mV[4]*vector.y + mV[8]*vector.z;
    result.y = mV[1]*vector.x + mV[5]*vector.y + mV[9]*vector.z;
    result.z = mV[2]*vector.x + mV[6]*vector.y + mV[10]*vector.z;
    
    return result;
}

Vector3 Matrix4x4::TransformPoint( const Vector3& point ) const
{
    Vector3 result;
    
    result.x = mV[0]*point.x + mV[4]*point.y + mV[8]*point.z + mV[12];
    result.y = mV[1]*point.x + mV[5]*point.y + mV[9]*point.z + mV[13];
    result.z = mV[2]*point.x + mV[6]*point.y + mV[10]*point.z + mV[14];
    
    return result;
}

Matrix4x4& Matrix4x4::Transpose()
{
    float temp = mV[1];
    mV[1] = mV[4];
    mV[4] = temp;
    
    temp = mV[2];
    mV[2] = mV[8];
    mV[8] = temp;
    
    temp = mV[3];
    mV[3] = mV[12];
    mV[12] = temp;
    
    temp = mV[6];
    mV[6] = mV[9];
    mV[9] = temp;
    
    temp = mV[7];
    mV[7] = mV[13];
    mV[13] = temp;
    
    temp = mV[11];
    mV[11] = mV[14];
    mV[14] = temp;
    
    return *this;
}

// ========================================================================
// STATIC METHODS
// ========================================================================

Matrix4x4 Matrix4x4::AffineInverse( const Matrix4x4& mat )
{
    Matrix4x4 result;
    
    // compute upper left 3x3 matrix determinant
    float cofactor0 = mat.mV[5]*mat.mV[10] - mat.mV[6]*mat.mV[9];
    float cofactor4 = mat.mV[2]*mat.mV[9] - mat.mV[1]*mat.mV[10];
    float cofactor8 = mat.mV[1]*mat.mV[6] - mat.mV[2]*mat.mV[5];
    float det = mat.mV[0]*cofactor0 + mat.mV[4]*cofactor4 + mat.mV[8]*cofactor8;
    
    if (Mathf::IsZero( det ))
        return result;
    
    // create adjunct matrix and multiply by 1/det to get upper 3x3
    float invDet = 1.0f/det;
    result.mV[0] = invDet*cofactor0;
    result.mV[1] = invDet*cofactor4;
    result.mV[2] = invDet*cofactor8;
    
    result.mV[4] = invDet*(mat.mV[6]*mat.mV[8] - mat.mV[4]*mat.mV[10]);
    result.mV[5] = invDet*(mat.mV[0]*mat.mV[10] - mat.mV[2]*mat.mV[8]);
    result.mV[6] = invDet*(mat.mV[2]*mat.mV[4] - mat.mV[0]*mat.mV[6]);
    
    result.mV[8] = invDet*(mat.mV[4]*mat.mV[9] - mat.mV[5]*mat.mV[8]);
    result.mV[9] = invDet*(mat.mV[1]*mat.mV[8] - mat.mV[0]*mat.mV[9]);
    result.mV[10] = invDet*(mat.mV[0]*mat.mV[5] - mat.mV[1]*mat.mV[4]);
    
    // multiply -translation by inverted 3x3 to get its inverse
    
    result.mV[12] = -result.mV[0]*mat.mV[12] - result.mV[4]*mat.mV[13] - result.mV[8]*mat.mV[14];
    result.mV[13] = -result.mV[1]*mat.mV[12] - result.mV[5]*mat.mV[13] - result.mV[9]*mat.mV[14];
    result.mV[14] = -result.mV[2]*mat.mV[12] - result.mV[6]*mat.mV[13] - result.mV[10]*mat.mV[14];
    
    return result;
}

Matrix4x4 Matrix4x4::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	Vector3 f = (center - eye).Normalized();
	Vector3 s = Vector3::Cross(f, up).Normalized();
	Vector3 u = Vector3::Cross(s,f);

	Matrix4x4 result;

	result.mV[0] = s.x;
	result.mV[4] = s.y;
	result.mV[8] = s.z;

	result.mV[1] = u.x;
	result.mV[5] = u.y;
	result.mV[9] = u.z;
	
	result.mV[2] = -f.x;
	result.mV[6] = -f.y;
	result.mV[10] = -f.z;
	
	result.mV[12] = -Vector3::Dot(s, eye);
	result.mV[13] = -Vector3::Dot(u, eye);
	result.mV[14] = Vector3::Dot(f, eye);

	return result;
}

Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float near, float far)
{
	Matrix4x4 result;

	result[0] = 2 / (right - left);
	result[1] = 0;
	result[2] = 0;
	result[3] = 0;

	result[4] = 0;
	result[5] = 2 / (top - bottom);
	result[6] = 0;
	result[7] = 0;

	result[8] = 0;
	result[9] = 0;
	result[10] = -2 / (far - near);
	result[11] = 0;

	result[12] = -(right + left) / (right - left);
	result[13] = -(top + bottom) / (top - bottom);
	result[14] = -(far + near) / (far - near);
	result[15] = 1;

	return result;
}

Matrix4x4 Matrix4x4::Perspective(float fov, float aspect, float near, float far)
{
	float yScale = 1.0 / tan(fov / 2);
	float xScale = yScale / aspect;
	float nearmfar = near - far;

	Matrix4x4 result;

	result.mV[0] = xScale;
	result.mV[1] = 0;
	result.mV[2] = 0;
	result.mV[3] = 0;
	result.mV[4] = 0;
	result.mV[5] = yScale;
	result.mV[6] = 0;
	result.mV[7] = 0;
	result.mV[8] = 0;
	result.mV[9] = 0;
	result.mV[10] = (far + near) / nearmfar;
	result.mV[11] = -1;
	result.mV[12] = 0;
	result.mV[13] = 0;
	result.mV[14] = 2 * far * near / nearmfar;
	result.mV[15] = 0;

	return result;
}

Matrix4x4 Matrix4x4::Rotation(const Matrix3x3& matrix)
{
    Matrix4x4 result;
    
    result.mV[0] = matrix.mV[0];
    result.mV[1] = matrix.mV[1];
    result.mV[2] = matrix.mV[2];
    result.mV[3] = 0;
    result.mV[4] = matrix.mV[3];
    result.mV[5] = matrix.mV[4];
    result.mV[6] = matrix.mV[5];
    result.mV[7] = 0;
    result.mV[8] = matrix.mV[6];
    result.mV[9] = matrix.mV[7];
    result.mV[10] = matrix.mV[8];
    result.mV[11] = 0;
    result.mV[12] = 0;
    result.mV[13] = 0;
    result.mV[14] = 0;
    result.mV[15] = 1;
    
    return result;
}

Matrix4x4 Matrix4x4::Rotation( const Quaternion& quat )
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
    
    Matrix4x4 result;
    
    result.mV[0] = 1.0f - (yy + zz);
    result.mV[4] = xy - wz;
    result.mV[8] = xz + wy;
    result.mV[12] = 0.0f;
    
    result.mV[1] = xy + wz;
    result.mV[5] = 1.0f - (xx + zz);
    result.mV[9] = yz - wx;
    result.mV[13] = 0.0f;
    
    result.mV[2] = xz - wy;
    result.mV[6] = yz + wx;
    result.mV[10] = 1.0f - (xx + yy);
    result.mV[14] = 0.0f;
    
    result.mV[3] = 0.0f;
    result.mV[7] = 0.0f;
    result.mV[11] = 0.0f;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::Rotation( float zRot, float yRot, float xRot )
{
    float Cx = cosf(xRot);
    float Sx = sinf(xRot);
    
    float Cy = cosf(yRot);
    float Sy = sinf(yRot);
    
    float Cz = cosf(zRot);
    float Sz = sinf(zRot);
    
    Matrix4x4 result;
    
    result.mV[0] =  (Cy * Cz);
    result.mV[4] = -(Cy * Sz);
    result.mV[8] =  Sy;
    result.mV[12] = 0.0f;
    
    result.mV[1] =  (Sx * Sy * Cz) + (Cx * Sz);
    result.mV[5] = -(Sx * Sy * Sz) + (Cx * Cz);
    result.mV[9] = -(Sx * Cy);
    result.mV[13] = 0.0f;
    
    result.mV[2] = -(Cx * Sy * Cz) + (Sx * Sz);
    result.mV[6] =  (Cx * Sy * Sz) + (Sx * Cz);
    result.mV[10] =  (Cx * Cy);
    result.mV[14] = 0.0f;
    
    result.mV[3] = 0.0f;
    result.mV[7] = 0.0f;
    result.mV[11] = 0.0f;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::Rotation( const Vector3& axis, float angle )
{
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;
    
    Vector3 nAxis = axis;
    nAxis.Normalize();
    
    // intermediate values
    float tx = t*nAxis.x;  float ty = t*nAxis.y;  float tz = t*nAxis.z;
    float sx = s*nAxis.x;  float sy = s*nAxis.y;  float sz = s*nAxis.z;
    float txy = tx*nAxis.y; float tyz = ty*nAxis.z; float txz = tx*nAxis.z;
    
    Matrix4x4 result;
    
    // set matrix
    result.mV[0] = tx*nAxis.x + c;
    result.mV[4] = txy - sz;
    result.mV[8] = txz + sy;
    result.mV[12] = 0.0f;
    
    result.mV[1] = txy + sz;
    result.mV[5] = ty*nAxis.y + c;
    result.mV[9] = tyz - sx;
    result.mV[13] = 0.0f;
    
    result.mV[2] = txz - sy;
    result.mV[6] = tyz + sx;
    result.mV[10] = tz*nAxis.z + c;
    result.mV[14] = 0.0f;
    
    result.mV[3] = 0.0f;
    result.mV[7] = 0.0f;
    result.mV[11] = 0.0f;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::RotationX( float angle )
{
    float sintheta = sinf(angle);
    float costheta = cosf(angle);
    
    Matrix4x4 result;
    
    result.mV[0] = 1.0f;
    result.mV[1] = 0.0f;
    result.mV[2] = 0.0f;
    result.mV[3] = 0.0f;
    result.mV[4] = 0.0f;
    result.mV[5] = costheta;
    result.mV[6] = sintheta;
    result.mV[7] = 0.0f;
    result.mV[8] = 0.0f;
    result.mV[9] = -sintheta;
    result.mV[10] = costheta;
    result.mV[11] = 0.0f;
    result.mV[12] = 0.0f;
    result.mV[13] = 0.0f;
    result.mV[14] = 0.0f;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::RotationY( float angle )
{
    float sintheta = sinf(angle);
    float costheta = cosf(angle);
    
    Matrix4x4 result;
    
    result.mV[0] = costheta;
    result.mV[1] = 0.0f;
    result.mV[2] = -sintheta;
    result.mV[3] = 0.0f;
    result.mV[4] = 0.0f;
    result.mV[5] = 1.0f;
    result.mV[6] = 0.0f;
    result.mV[7] = 0.0f;
    result.mV[8] = sintheta;
    result.mV[9] = 0.0f;
    result.mV[10] = costheta;
    result.mV[11] = 0.0f;
    result.mV[12] = 0.0f;
    result.mV[13] = 0.0f;
    result.mV[14] = 0.0f;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::RotationZ( float angle )
{
    float sintheta = sinf(angle);
    float costheta = cosf(angle);
    
    Matrix4x4 result;
    
    result.mV[0] = costheta;
    result.mV[1] = sintheta;
    result.mV[2] = 0.0f;
    result.mV[3] = 0.0f;
    result.mV[4] = -sintheta;
    result.mV[5] = costheta;
    result.mV[6] = 0.0f;
    result.mV[7] = 0.0f;
    result.mV[8] = 0.0f;
    result.mV[9] = 0.0f;
    result.mV[10] = 1.0f;
    result.mV[11] = 0.0f;
    result.mV[12] = 0.0f;
    result.mV[13] = 0.0f;
    result.mV[14] = 0.0f;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::Scaling( const Vector3& factors )
{
    Matrix4x4 result;
    
    result.mV[0] = factors.x;
    result.mV[1] = 0.0f;
    result.mV[2] = 0.0f;
    result.mV[3] = 0.0f;
    result.mV[4] = 0.0f;
    result.mV[5] = factors.y;
    result.mV[6] = 0.0f;
    result.mV[7] = 0.0f;
    result.mV[8] = 0.0f;
    result.mV[9] = 0.0f;
    result.mV[10] = factors.z;
    result.mV[11] = 0.0f;
    result.mV[12] = 0.0f;
    result.mV[13] = 0.0f;
    result.mV[14] = 0.0f;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::Translation( const Vector3& vector )
{
    Matrix4x4 result;
    
    result.mV[0] = 1.0f;
    result.mV[1] = 0.0f;
    result.mV[2] = 0.0f;
    result.mV[3] = 0.0f;
    result.mV[4] = 0.0f;
    result.mV[5] = 1.0f;
    result.mV[6] = 0.0f;
    result.mV[7] = 0.0f;
    result.mV[8] = 0.0f;
    result.mV[9] = 0.0f;
    result.mV[10] = 1.0f;
    result.mV[11] = 0.0f;
    result.mV[12] = vector.x;
    result.mV[13] = vector.y;
    result.mV[14] = vector.z;
    result.mV[15] = 1.0f;
    
    return result;
}

Matrix4x4 Matrix4x4::Transformation(Vector3 translation, Vector3 scale, Quaternion rotation)
{
	return Translation(translation)* Rotation(rotation)* Scaling(scale);
}

Matrix4x4 Matrix4x4::Transformation(Vector3 translation, Quaternion rotation)
{
	return Translation(translation) * Rotation(rotation);
}

Matrix4x4 Matrix4x4::Transformation(Transform transform)
{
	return Transformation(transform.position, transform.scale, transform.rotation);
}

// ========================================================================
// FRIEND OPERATORS
// ========================================================================

std::ostream& operator<<(std::ostream& os, const Matrix4x4& matrix)
{
    // row
    for (unsigned int i = 0; i < 4; ++i)
    {
        os << "| ";
        // column
        for (unsigned int j = 0; j < 4; ++j )
        {
            os << matrix.mV[ j*4 + i ] << ' ';
        }
    }
    
    os << "|";
    
    return os;
}

// ========================================================================
// MEMBER OPERATORS
// ========================================================================

Matrix4x4& Matrix4x4::operator += ( const Matrix4x4& other )
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        mV[i] += other.mV[i];
    }
    
    return *this;
}

Matrix4x4& Matrix4x4::operator -= ( const Matrix4x4& other )
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        mV[i] -= other.mV[i];
    }
    
    return *this;
}

Matrix4x4& Matrix4x4::operator*=( const Matrix4x4& other )
{
    Matrix4x4 result;
    
    result.mV[0] = mV[0]*other.mV[0] + mV[4]*other.mV[1] + mV[8]*other.mV[2] + mV[12]*other.mV[3];
    result.mV[1] = mV[1]*other.mV[0] + mV[5]*other.mV[1] + mV[9]*other.mV[2] + mV[13]*other.mV[3];
    result.mV[2] = mV[2]*other.mV[0] + mV[6]*other.mV[1] + mV[10]*other.mV[2] + mV[14]*other.mV[3];
    result.mV[3] = mV[3]*other.mV[0] + mV[7]*other.mV[1] + mV[11]*other.mV[2] + mV[15]*other.mV[3];
    
    result.mV[4] = mV[0]*other.mV[4] + mV[4]*other.mV[5] + mV[8]*other.mV[6] + mV[12]*other.mV[7];
    result.mV[5] = mV[1]*other.mV[4] + mV[5]*other.mV[5] + mV[9]*other.mV[6] + mV[13]*other.mV[7];
    result.mV[6] = mV[2]*other.mV[4] + mV[6]*other.mV[5] + mV[10]*other.mV[6] + mV[14]*other.mV[7];
    result.mV[7] = mV[3]*other.mV[4] + mV[7]*other.mV[5] + mV[11]*other.mV[6] + mV[15]*other.mV[7];
    
    result.mV[8] = mV[0]*other.mV[8] + mV[4]*other.mV[9] + mV[8]*other.mV[10] + mV[12]*other.mV[11];
    result.mV[9] = mV[1]*other.mV[8] + mV[5]*other.mV[9] + mV[9]*other.mV[10] + mV[13]*other.mV[11];
    result.mV[10] = mV[2]*other.mV[8] + mV[6]*other.mV[9] + mV[10]*other.mV[10] + mV[14]*other.mV[11];
    result.mV[11] = mV[3]*other.mV[8] + mV[7]*other.mV[9] + mV[11]*other.mV[10] + mV[15]*other.mV[11];
    
    result.mV[12] = mV[0]*other.mV[12] + mV[4]*other.mV[13] + mV[8]*other.mV[14] + mV[12]*other.mV[15];
    result.mV[13] = mV[1]*other.mV[12] + mV[5]*other.mV[13] + mV[9]*other.mV[14] + mV[13]*other.mV[15];
    result.mV[14] = mV[2]*other.mV[12] + mV[6]*other.mV[13] + mV[10]*other.mV[14] + mV[14]*other.mV[15];
    result.mV[15] = mV[3]*other.mV[12] + mV[7]*other.mV[13] + mV[11]*other.mV[14] + mV[15]*other.mV[15];
    
    for (unsigned int i = 0; i < 16; ++i)
    {
        mV[i] = result.mV[i];
    }
    
    return *this;
}

Matrix4x4& Matrix4x4::operator *= ( float scalar )
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
    mV[9] *= scalar;
    mV[10] *= scalar;
    mV[11] *= scalar;
    mV[12] *= scalar;
    mV[13] *= scalar;
    mV[14] *= scalar;
    mV[15] *= scalar;
    
    return *this;
}

Matrix4x4 Matrix4x4::operator - () const
{
    Matrix4x4 result;
    
    for (unsigned int i = 0; i < 16; ++i)
    {
        result.mV[i] = -mV[i];
    }
    
    return result;
}

// ========================================================================
// NON-MEMBER OPERATORS
// ========================================================================

bool operator == (const Matrix4x4& a, const Matrix4x4& b)
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        if ( !Mathf::AreEqual(a.mV[i], b.mV[i]) )
            return false;
    }
    
    return true;
}

bool operator != (const Matrix4x4& a, const Matrix4x4& b)
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        if ( !Mathf::AreEqual(a.mV[i], b.mV[i]) )
            return true;
    }
    
    return false;
}

Matrix4x4 operator + (const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 result;
    
    for (unsigned int i = 0; i < 16; ++i)
    {
        result.mV[i] = a.mV[i] + b.mV[i];
    }
    
    return result;
}

Matrix4x4 operator - (const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 result;
    
    for (unsigned int i = 0; i < 16; ++i)
    {
        result.mV[i] = a.mV[i] - b.mV[i];
    }
    
    return result;
}

Matrix4x4 operator * (const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 result;
    
    result.mV[0] = a.mV[0]*b.mV[0] + a.mV[4]*b[1] + a.mV[8]*b.mV[2] + a.mV[12]*b.mV[3];
    result.mV[1] = a.mV[1]*b.mV[0] + a.mV[5]*b.mV[1] + a.mV[9]*b.mV[2] + a.mV[13]*b.mV[3];
    result.mV[2] = a.mV[2]*b.mV[0] + a.mV[6]*b.mV[1] + a.mV[10]*b.mV[2] + a.mV[14]*b.mV[3];
    result.mV[3] = a.mV[3]*b.mV[0] + a.mV[7]*b.mV[1] + a.mV[11]*b.mV[2] + a.mV[15]*b.mV[3];
    
    result.mV[4] = a.mV[0]*b.mV[4] + a.mV[4]*b.mV[5] + a.mV[8]*b.mV[6] + a.mV[12]*b.mV[7];
    result.mV[5] = a.mV[1]*b.mV[4] + a.mV[5]*b.mV[5] + a.mV[9]*b.mV[6] + a.mV[13]*b.mV[7];
    result.mV[6] = a.mV[2]*b.mV[4] + a.mV[6]*b.mV[5] + a.mV[10]*b.mV[6] + a.mV[14]*b.mV[7];
    result.mV[7] = a.mV[3]*b.mV[4] + a.mV[7]*b.mV[5] + a.mV[11]*b.mV[6] + a.mV[15]*b.mV[7];
    
    result.mV[8] = a.mV[0]*b.mV[8] + a.mV[4]*b.mV[9] + a.mV[8]*b.mV[10] + a.mV[12]*b.mV[11];
    result.mV[9] = a.mV[1]*b.mV[8] + a.mV[5]*b.mV[9] + a.mV[9]*b.mV[10] + a.mV[13]*b.mV[11];
    result.mV[10] = a.mV[2]*b.mV[8] + a.mV[6]*b.mV[9] + a.mV[10]*b.mV[10] + a.mV[14]*b.mV[11];
    result.mV[11] = a.mV[3]*b.mV[8] + a.mV[7]*b.mV[9] + a.mV[11]*b.mV[10] + a.mV[15]*b.mV[11];
    
    result.mV[12] = a.mV[0]*b.mV[12] + a.mV[4]*b.mV[13] + a.mV[8]*b.mV[14] + a.mV[12]*b.mV[15];
    result.mV[13] = a.mV[1]*b.mV[12] + a.mV[5]*b.mV[13] + a.mV[9]*b.mV[14] + a.mV[13]*b.mV[15];
    result.mV[14] = a.mV[2]*b.mV[12] + a.mV[6]*b.mV[13] + a.mV[10]*b.mV[14] + a.mV[14]*b.mV[15];
    result.mV[15] = a.mV[3]*b.mV[12] + a.mV[7]*b.mV[13] + a.mV[11]*b.mV[14] + a.mV[15]*b.mV[15];
    
    return result;
}

Vector4 operator * (const Matrix4x4& matrix, const Vector4& vector)
{
    Vector4 result;
    
    result.x = matrix.mV[0]*vector.x + matrix.mV[1]*vector.y + matrix.mV[2]*vector.z + matrix.mV[3]*vector.w;
    result.y = matrix.mV[4]*vector.x + matrix.mV[5]*vector.y + matrix.mV[6]*vector.z + matrix.mV[7]*vector.w;
    result.z = matrix.mV[8]*vector.x + matrix.mV[9]*vector.y + matrix.mV[10]*vector.z + matrix.mV[11]*vector.w;
    result.w = matrix.mV[12]*vector.x + matrix.mV[13]*vector.y + matrix.mV[14]*vector.z + matrix.mV[15]*vector.w;
    
    return result;
}

Vector4 operator * (const Vector4& vector, const Matrix4x4& matrix)
{
    Vector4 result;
    
    result.x = matrix.mV[0]*vector.x + matrix.mV[1]*vector.y + matrix.mV[2]*vector.z + matrix.mV[3]*vector.w;
    result.y = matrix.mV[4]*vector.x + matrix.mV[5]*vector.y + matrix.mV[6]*vector.z + matrix.mV[7]*vector.w;
    result.z = matrix.mV[8]*vector.x + matrix.mV[9]*vector.y + matrix.mV[10]*vector.z + matrix.mV[11]*vector.w;
    result.w = matrix.mV[12]*vector.x + matrix.mV[13]*vector.y + matrix.mV[14]*vector.z + matrix.mV[15]*vector.w;
    
    return result;
}

Matrix4x4 operator * (const Matrix4x4& matrix, float scalar)
{
    Matrix4x4 result;
    
    result.mV[0] = matrix.mV[0] * scalar;
    result.mV[1] = matrix.mV[1] * scalar;
    result.mV[2] = matrix.mV[2] * scalar;
    result.mV[3] = matrix.mV[3] * scalar;
    result.mV[4] = matrix.mV[4] * scalar;
    result.mV[5] = matrix.mV[5] * scalar;
    result.mV[6] = matrix.mV[6] * scalar;
    result.mV[7] = matrix.mV[7] * scalar;
    result.mV[8] = matrix.mV[8] * scalar;
    result.mV[9] = matrix.mV[9] * scalar;
    result.mV[10] = matrix.mV[10] * scalar;
    result.mV[11] = matrix.mV[11] * scalar;
    result.mV[12] = matrix.mV[12] * scalar;
    result.mV[13] = matrix.mV[13] * scalar;
    result.mV[14] = matrix.mV[14] * scalar;
    result.mV[15] = matrix.mV[15] * scalar;
    
    return result;
}

Matrix4x4 operator * (float scalar, const Matrix4x4& matrix)
{
    Matrix4x4 result;
    
    result.mV[0] = matrix.mV[0] * scalar;
    result.mV[1] = matrix.mV[1] * scalar;
    result.mV[2] = matrix.mV[2] * scalar;
    result.mV[3] = matrix.mV[3] * scalar;
    result.mV[4] = matrix.mV[4] * scalar;
    result.mV[5] = matrix.mV[5] * scalar;
    result.mV[6] = matrix.mV[6] * scalar;
    result.mV[7] = matrix.mV[7] * scalar;
    result.mV[8] = matrix.mV[8] * scalar;
    result.mV[9] = matrix.mV[9] * scalar;
    result.mV[10] = matrix.mV[10] * scalar;
    result.mV[11] = matrix.mV[11] * scalar;
    result.mV[12] = matrix.mV[12] * scalar;
    result.mV[13] = matrix.mV[13] * scalar;
    result.mV[14] = matrix.mV[14] * scalar;
    result.mV[15] = matrix.mV[15] * scalar;
    
    return result;
}
