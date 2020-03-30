#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../Core/Math/Vector3.hpp"
#include "../Core/Math/Quaternion.hpp"

static Vector3 TransformVector(glm::mat4 matrix, Vector3 vector)
{
	return Vector3
	(
		vector.x * matrix[0][0] +
		vector.y * matrix[1][0] +
		vector.z * matrix[2][0] + matrix[3][0],

		vector.x * matrix[0][1] +
		vector.y * matrix[1][1] +
		vector.z * matrix[2][1] + matrix[3][1],

		vector.x * matrix[0][2] +
		vector.y * matrix[1][2] +
		vector.z * matrix[2][2] + matrix[3][2]
	);
}

static Vector3 Normalize(Vector3 vector)
{
	if (vector.LengthSquared() == 0)
		return vector;

	return vector.Normalized();
}

static Quaternion Normalize(Quaternion quaternion)
{
	if (quaternion.MagnitudeSquared() == 0)
		return quaternion;

	return quaternion.Normalized();
}

class cyMatrix4
{
public:
	float data[12];

	cyMatrix4()
	{
		data[1] = data[2] = data[3] = data[4] = data[6] =
			data[7] = data[8] = data[9] = data[11] = 0;
		data[0] = data[5] = data[10] = 1;
	}

	cyMatrix4(glm::mat4 glmMatrix)
	{
		data[0] = glmMatrix[0][0];
		data[1] = glmMatrix[1][0];
		data[2] = glmMatrix[2][0];
		data[3] = glmMatrix[3][0];

		data[4] = glmMatrix[0][1];
		data[5] = glmMatrix[1][1];
		data[6] = glmMatrix[2][1];
		data[7] = glmMatrix[3][1];

		data[8] = glmMatrix[0][2];
		data[9] = glmMatrix[1][2];
		data[10] = glmMatrix[2][2];
		data[11] = glmMatrix[3][2];
	}

	/*
	 * Sets the cyMatrix to be a diagonal cyMatrix with the given coefficients.
	 */
	void setDiagonal(float a, float b, float c)
	{
		data[0] = a;
		data[5] = b;
		data[10] = c;
	}

	/**
	 * Returns a cyMatrix which is this cyMatrix multiplied by the given
	 * other cyMatrix.
	 */
	cyMatrix4 operator*(const cyMatrix4& o) const
	{
		cyMatrix4 result;
		result.data[0] = (o.data[0] * data[0]) + (o.data[4] * data[1]) + (o.data[8] * data[2]);
		result.data[4] = (o.data[0] * data[4]) + (o.data[4] * data[5]) + (o.data[8] * data[6]);
		result.data[8] = (o.data[0] * data[8]) + (o.data[4] * data[9]) + (o.data[8] * data[10]);

		result.data[1] = (o.data[1] * data[0]) + (o.data[5] * data[1]) + (o.data[9] * data[2]);
		result.data[5] = (o.data[1] * data[4]) + (o.data[5] * data[5]) + (o.data[9] * data[6]);
		result.data[9] = (o.data[1] * data[8]) + (o.data[5] * data[9]) + (o.data[9] * data[10]);

		result.data[2] = (o.data[2] * data[0]) + (o.data[6] * data[1]) + (o.data[10] * data[2]);
		result.data[6] = (o.data[2] * data[4]) + (o.data[6] * data[5]) + (o.data[10] * data[6]);
		result.data[10] = (o.data[2] * data[8]) + (o.data[6] * data[9]) + (o.data[10] * data[10]);

		result.data[3] = (o.data[3] * data[0]) + (o.data[7] * data[1]) + (o.data[11] * data[2]) + data[3];
		result.data[7] = (o.data[3] * data[4]) + (o.data[7] * data[5]) + (o.data[11] * data[6]) + data[7];
		result.data[11] = (o.data[3] * data[8]) + (o.data[7] * data[9]) + (o.data[11] * data[10]) + data[11];

		return result;
	}

	/**
	 * Transform the given vector by this cyMatrix.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 operator*(const Vector3& vector) const
	{
		return Vector3(
			vector.x * data[0] +
			vector.y * data[1] +
			vector.z * data[2] + data[3],

			vector.x * data[4] +
			vector.y * data[5] +
			vector.z * data[6] + data[7],

			vector.x * data[8] +
			vector.y * data[9] +
			vector.z * data[10] + data[11]
		);
	}

	/**
	 * Transform the given vector by this cyMatrix.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 transform(const Vector3& vector) const
	{
		return (*this) * vector;
	}

	/**
	 * Returns the determinant of the cyMatrix.
	 */
	float getDeterminant() const;

	/**
	 * Sets the cyMatrix to be the inverse of the given cyMatrix.
	 *
	 * @param m The cyMatrix to invert and use to set this.
	 */
	void setInverse(const cyMatrix4& m);

	/** Returns a new cyMatrix containing the inverse of this cyMatrix. */
	cyMatrix4 inverse() const
	{
		cyMatrix4 result;
		result.setInverse(*this);
		return result;
	}

	/**
	 * Inverts the cyMatrix.
	 */
	void invert()
	{
		setInverse(*this);
	}

	/**
	 * Transform the given direction vector by this cyMatrix.
	 *
	 * @note When a direction is converted between frames of
	 * reference, there is no translation required.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 transformDirection(const Vector3& vector) const
	{
		return Vector3(
			vector.x * data[0] +
			vector.y * data[1] +
			vector.z * data[2],

			vector.x * data[4] +
			vector.y * data[5] +
			vector.z * data[6],

			vector.x * data[8] +
			vector.y * data[9] +
			vector.z * data[10]
		);
	}

	/**
	 * Transform the given direction vector by the
	 * transformational inverse of this cyMatrix.
	 *
	 * @note This function relies on the fact that the inverse of
	 * a pure rotation cyMatrix is its transpose. It separates the
	 * translational and rotation components, transposes the
	 * rotation, and multiplies out. If the cyMatrix is not a
	 * scale and shear free transform cyMatrix, then this function
	 * will not give correct results.
	 *
	 * @note When a direction is converted between frames of
	 * reference, there is no translation required.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 transformInverseDirection(const Vector3& vector) const
	{
		return Vector3(
			vector.x * data[0] +
			vector.y * data[4] +
			vector.z * data[8],

			vector.x * data[1] +
			vector.y * data[5] +
			vector.z * data[9],

			vector.x * data[2] +
			vector.y * data[6] +
			vector.z * data[10]
		);
	}

	/**
	 * Transform the given vector by the transformational inverse
	 * of this cyMatrix.
	 *
	 * @note This function relies on the fact that the inverse of
	 * a pure rotation cyMatrix is its transpose. It separates the
	 * translational and rotation components, transposes the
	 * rotation, and multiplies out. If the cyMatrix is not a
	 * scale and shear free transform cyMatrix, then this function
	 * will not give correct results.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 transformInverse(const Vector3& vector) const
	{
		Vector3 tmp = vector;
		tmp.x -= data[3];
		tmp.y -= data[7];
		tmp.z -= data[11];
		return Vector3(
			tmp.x * data[0] +
			tmp.y * data[4] +
			tmp.z * data[8],

			tmp.x * data[1] +
			tmp.y * data[5] +
			tmp.z * data[9],

			tmp.x * data[2] +
			tmp.y * data[6] +
			tmp.z * data[10]
		);
	}

	/**
	 * Gets a vector representing one axis (i.e. one column) in the cyMatrix.
	 *
	 * @param i The row to return. Row 3 corresponds to the position
	 * of the transform cyMatrix.
	 *
	 * @return The vector.
	 */
	Vector3 getAxisVector(int i) const
	{
		return Vector3(data[i], data[i + 4], data[i + 8]);
	}

	/**
	 * Sets this cyMatrix to be the rotation cyMatrix corresponding to
	 * the given Quaternion.
	 */
	void setOrientationAndPos(const Quaternion& q, const Vector3& pos)
	{
		data[0] = 1 - (2 * q.y * q.y + 2 * q.z * q.z);
		data[1] = 2 * q.x * q.y + 2 * q.z * q.w;
		data[2] = 2 * q.x * q.z - 2 * q.y * q.w;
		data[3] = pos.x;

		data[4] = 2 * q.x * q.y - 2 * q.z * q.w;
		data[5] = 1 - (2 * q.x * q.x + 2 * q.z * q.z);
		data[6] = 2 * q.y * q.z + 2 * q.x * q.w;
		data[7] = pos.y;

		data[8] = 2 * q.x * q.z + 2 * q.y * q.w;
		data[9] = 2 * q.y * q.z - 2 * q.x * q.w;
		data[10] = 1 - (2 * q.x * q.x + 2 * q.y * q.y);
		data[11] = pos.z;
	}

	/**
	 * Fills the given array with this transform cyMatrix, so it is
	 * usable as an open-gl transform cyMatrix. OpenGL uses a column
	 * major format, so that the values are transposed as they are
	 * written.
	 */
	void fillGLArray(float array[16]) const
	{
		array[0] = (float)data[0];
		array[1] = (float)data[4];
		array[2] = (float)data[8];
		array[3] = (float)0;

		array[4] = (float)data[1];
		array[5] = (float)data[5];
		array[6] = (float)data[9];
		array[7] = (float)0;

		array[8] = (float)data[2];
		array[9] = (float)data[6];
		array[10] = (float)data[10];
		array[11] = (float)0;

		array[12] = (float)data[3];
		array[13] = (float)data[7];
		array[14] = (float)data[11];
		array[15] = (float)1;
	}
};

class cyMatrix3
{
public:
	/**
	 * Holds the tensor cyMatrix data in array form.
	 */
	float data[9];

	// ... Other cyMatrix3 code as before ...

	/**
	 * Creates a new cyMatrix.
	 */
	cyMatrix3()
	{
		data[0] = data[1] = data[2] = data[3] = data[4] = data[5] =
			data[6] = data[7] = data[8] = 0;
	}

	/**
	 * Creates a new cyMatrix with the given three vectors making
	 * up its columns.
	 */
	cyMatrix3(const Vector3& compOne, const Vector3& compTwo,
		const Vector3& compThree)
	{
		setComponents(compOne, compTwo, compThree);
	}

	/**
	 * Creates a new cyMatrix with explicit coefficients.
	 */
	cyMatrix3(float c0, float c1, float c2, float c3, float c4, float c5,
		float c6, float c7, float c8)
	{
		data[0] = c0; data[1] = c1; data[2] = c2;
		data[3] = c3; data[4] = c4; data[5] = c5;
		data[6] = c6; data[7] = c7; data[8] = c8;
	}

	/**
	 * Sets the cyMatrix to be a diagonal cyMatrix with the given
	 * values along the leading diagonal.
	 */
	void setDiagonal(float a, float b, float c)
	{
		setInertiaTensorCoeffs(a, b, c);
	}

	/**
	 * Sets the value of the cyMatrix from inertia tensor values.
	 */
	void setInertiaTensorCoeffs(float ix, float iy, float iz,
		float ixy = 0, float ixz = 0, float iyz = 0)
	{
		data[0] = ix;
		data[1] = data[3] = -ixy;
		data[2] = data[6] = -ixz;
		data[4] = iy;
		data[5] = data[7] = -iyz;
		data[8] = iz;
	}

	/**
	 * Sets the value of the cyMatrix as an inertia tensor of
	 * a rectangular block aligned with the body's coordinate
	 * system with the given axis half-sizes and mass.
	 */
	void setBlockInertiaTensor(const Vector3& halfSizes, float mass)
	{
		Vector3 squares = Vector3(halfSizes.x*halfSizes.x, halfSizes.y * halfSizes.y, halfSizes.z * halfSizes.z);
		setInertiaTensorCoeffs(0.3f * mass * (squares.y + squares.z),
			0.3f * mass * (squares.x + squares.z),
			0.3f * mass * (squares.x + squares.y));
	}

	/**
	 * Sets the cyMatrix to be a skew symmetric cyMatrix based on
	 * the given vector. The skew symmetric cyMatrix is the equivalent
	 * of the vector product. So if a,b are vectors. a x b = A_s b
	 * where A_s is the skew symmetric form of a.
	 */
	void setSkewSymmetric(const Vector3 vector)
	{
		data[0] = data[4] = data[8] = 0;
		data[1] = -vector.z;
		data[2] = vector.y;
		data[3] = vector.z;
		data[5] = -vector.x;
		data[6] = -vector.y;
		data[7] = vector.x;
	}

	/**
	 * Sets the cyMatrix values from the given three vector components.
	 * These are arranged as the three columns of the vector.
	 */
	void setComponents(const Vector3& compOne, const Vector3& compTwo,
		const Vector3& compThree)
	{
		data[0] = compOne.x;
		data[1] = compTwo.x;
		data[2] = compThree.x;
		data[3] = compOne.y;
		data[4] = compTwo.y;
		data[5] = compThree.y;
		data[6] = compOne.z;
		data[7] = compTwo.z;
		data[8] = compThree.z;

	}

	/**
	 * Transform the given vector by this cyMatrix.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 operator*(const Vector3& vector) const
	{
		return Vector3(
			vector.x * data[0] + vector.y * data[1] + vector.z * data[2],
			vector.x * data[3] + vector.y * data[4] + vector.z * data[5],
			vector.x * data[6] + vector.y * data[7] + vector.z * data[8]
		);
	}

	/**
	 * Transform the given vector by this cyMatrix.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 transform(const Vector3& vector) const
	{
		return (*this) * vector;
	}

	/**
	 * Transform the given vector by the transpose of this cyMatrix.
	 *
	 * @param vector The vector to transform.
	 */
	Vector3 transformTranspose(const Vector3& vector) const
	{
		return Vector3(
			vector.x * data[0] + vector.y * data[3] + vector.z * data[6],
			vector.x * data[1] + vector.y * data[4] + vector.z * data[7],
			vector.x * data[2] + vector.y * data[5] + vector.z * data[8]
		);
	}

	/**
	 * Gets a vector representing one row in the cyMatrix.
	 *
	 * @param i The row to return.
	 */
	Vector3 getRowVector(int i) const
	{
		return Vector3(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
	}

	/**
	 * Gets a vector representing one axis (i.e. one column) in the cyMatrix.
	 *
	 * @param i The row to return.
	 *
	 * @return The vector.
	 */
	Vector3 getAxisVector(int i) const
	{
		return Vector3(data[i], data[i + 3], data[i + 6]);
	}

	/**
	 * Sets the cyMatrix to be the inverse of the given cyMatrix.
	 *
	 * @param m The cyMatrix to invert and use to set this.
	 */
	void setInverse(const cyMatrix3& m)
	{
		float t4 = m.data[0] * m.data[4];
		float t6 = m.data[0] * m.data[5];
		float t8 = m.data[1] * m.data[3];
		float t10 = m.data[2] * m.data[3];
		float t12 = m.data[1] * m.data[6];
		float t14 = m.data[2] * m.data[6];

		// Calculate the determinant
		float t16 = (t4 * m.data[8] - t6 * m.data[7] - t8 * m.data[8] +
			t10 * m.data[7] + t12 * m.data[5] - t14 * m.data[4]);

		// Make sure the determinant is non-zero.
		if (t16 == (float)0.0f) return;
		float t17 = 1 / t16;

		data[0] = (m.data[4] * m.data[8] - m.data[5] * m.data[7]) * t17;
		data[1] = -(m.data[1] * m.data[8] - m.data[2] * m.data[7]) * t17;
		data[2] = (m.data[1] * m.data[5] - m.data[2] * m.data[4]) * t17;
		data[3] = -(m.data[3] * m.data[8] - m.data[5] * m.data[6]) * t17;
		data[4] = (m.data[0] * m.data[8] - t14) * t17;
		data[5] = -(t6 - t10) * t17;
		data[6] = (m.data[3] * m.data[7] - m.data[4] * m.data[6]) * t17;
		data[7] = -(m.data[0] * m.data[7] - t12) * t17;
		data[8] = (t4 - t8) * t17;
	}

	/** Returns a new cyMatrix containing the inverse of this cyMatrix. */
	cyMatrix3 inverse() const
	{
		cyMatrix3 result;
		result.setInverse(*this);
		return result;
	}

	/**
	 * Inverts the cyMatrix.
	 */
	void invert()
	{
		setInverse(*this);
	}

	/**
	 * Sets the cyMatrix to be the transpose of the given cyMatrix.
	 *
	 * @param m The cyMatrix to transpose and use to set this.
	 */
	void setTranspose(const cyMatrix3& m)
	{
		data[0] = m.data[0];
		data[1] = m.data[3];
		data[2] = m.data[6];
		data[3] = m.data[1];
		data[4] = m.data[4];
		data[5] = m.data[7];
		data[6] = m.data[2];
		data[7] = m.data[5];
		data[8] = m.data[8];
	}

	/** Returns a new cyMatrix containing the transpose of this cyMatrix. */
	cyMatrix3 transpose() const
	{
		cyMatrix3 result;
		result.setTranspose(*this);
		return result;
	}

	/**
	 * Returns a cyMatrix which is this cyMatrix multiplied by the given
	 * other cyMatrix.
	 */
	cyMatrix3 operator*(const cyMatrix3& o) const
	{
		return cyMatrix3(
			data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6],
			data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7],
			data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8],

			data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6],
			data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7],
			data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8],

			data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6],
			data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7],
			data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8]
		);
	}

	/**
	 * Multiplies this cyMatrix in place by the given other cyMatrix.
	 */
	void operator*=(const cyMatrix3& o)
	{
		float t1;
		float t2;
		float t3;

		t1 = data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6];
		t2 = data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7];
		t3 = data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8];
		data[0] = t1;
		data[1] = t2;
		data[2] = t3;

		t1 = data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6];
		t2 = data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7];
		t3 = data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8];
		data[3] = t1;
		data[4] = t2;
		data[5] = t3;

		t1 = data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6];
		t2 = data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7];
		t3 = data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8];
		data[6] = t1;
		data[7] = t2;
		data[8] = t3;
	}

	/**
	 * Multiplies this cyMatrix in place by the given scalar.
	 */
	void operator*=(const float scalar)
	{
		data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
		data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
		data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
	}

	/**
	 * Does a component-wise addition of this cyMatrix and the given
	 * cyMatrix.
	 */
	void operator+=(const cyMatrix3& o)
	{
		data[0] += o.data[0]; data[1] += o.data[1]; data[2] += o.data[2];
		data[3] += o.data[3]; data[4] += o.data[4]; data[5] += o.data[5];
		data[6] += o.data[6]; data[7] += o.data[7]; data[8] += o.data[8];
	}

	/**
	 * Sets this cyMatrix to be the rotation cyMatrix corresponding to
	 * the given Quaternion.
	 */
	void setOrientation(const Quaternion& q)
	{
		data[0] = 1 - (2 * q.z * q.y + 2 * q.z * q.z);
		data[1] = 2 * q.x * q.y + 2 * q.z * q.w;
		data[2] = 2 * q.x * q.z - 2 * q.y * q.w;
		data[3] = 2 * q.x * q.y - 2 * q.z * q.w;
		data[4] = 1 - (2 * q.x * q.x + 2 * q.z * q.z);
		data[5] = 2 * q.y * q.z + 2 * q.x * q.w;
		data[6] = 2 * q.x * q.z + 2 * q.y * q.w;
		data[7] = 2 * q.y * q.z - 2 * q.x * q.w;
		data[8] = 1 - (2 * q.x * q.x + 2 * q.y * q.y);
	}

	/**
	 * Interpolates a couple of matrices.
	 */
	static cyMatrix3 linearInterpolate(const cyMatrix3& a, const cyMatrix3& b, float prop);
};