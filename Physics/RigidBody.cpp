#include "RigidBody.hpp"

#include <iostream>

static float sleepEpsilon = 0.3f;

static inline void _checkInverseInertiaTensor(const Matrix3x3& iitWorld)
{
	// TODO: Perform a validity check in an assert.
}

static inline void _transformInertiaTensor(Matrix3x3& iitWorld, const Quaternion& q, const Matrix3x3& iitBody, const Matrix4x4& rotmat)
{
	float t4 = rotmat[0] * iitBody[0] + rotmat[4] * iitBody[1] + rotmat[8] * iitBody[2];
	float t9 = rotmat[0] * iitBody[3] + rotmat[4] * iitBody[4] + rotmat[8] * iitBody[5];
	float t14 = rotmat[0] * iitBody[6] + rotmat[4] * iitBody[7] + rotmat[8] * iitBody[8];
	float t28 = rotmat[1] * iitBody[0] + rotmat[5] * iitBody[1] + rotmat[9] * iitBody[2];
	float t33 = rotmat[1] * iitBody[3] + rotmat[5] * iitBody[4] + rotmat[9] * iitBody[5];
	float t38 = rotmat[1] * iitBody[6] + rotmat[5] * iitBody[7] + rotmat[9] * iitBody[8];
	float t52 = rotmat[2] * iitBody[0] + rotmat[6] * iitBody[1] + rotmat[10] * iitBody[2];
	float t57 = rotmat[2] * iitBody[3] + rotmat[6] * iitBody[4] + rotmat[10] * iitBody[5];
	float t62 = rotmat[2] * iitBody[6] + rotmat[6] * iitBody[7] + rotmat[10] * iitBody[8];

	iitWorld[0] = t4 * rotmat[0] + t9 * rotmat[4] + t14 * rotmat[8];
	iitWorld[3] = t4 * rotmat[1] + t9 * rotmat[5] + t14 * rotmat[9];
	iitWorld[6] = t4 * rotmat[2] + t9 * rotmat[6] + t14 * rotmat[10];
	iitWorld[1] = t28 * rotmat[0] + t33 * rotmat[4] + t38 * rotmat[8];
	iitWorld[4] = t28 * rotmat[1] + t33 * rotmat[5] + t38 * rotmat[9];
	iitWorld[7] = t28 * rotmat[2] + t33 * rotmat[6] + t38 * rotmat[10];
	iitWorld[2] = t52 * rotmat[0] + t57 * rotmat[4] + t62 * rotmat[8];
	iitWorld[5] = t52 * rotmat[1] + t57 * rotmat[5] + t62 * rotmat[9];
	iitWorld[8] = t52 * rotmat[2] + t57 * rotmat[6] + t62 * rotmat[10];
}

static inline void _calculateTransformcyMatrix(Matrix4x4& transformcyMatrix, const Vector3& position, const Quaternion& orientation)
{
	transformcyMatrix[0] = 1 - 2 * orientation.y * orientation.y - 2 * orientation.z * orientation.z;
	transformcyMatrix[4] = 2 * orientation.x * orientation.y - 2 * orientation.w * orientation.z;
	transformcyMatrix[8] = 2 * orientation.x * orientation.z + 2 * orientation.w * orientation.y;
	transformcyMatrix[12] = position.x;

	transformcyMatrix[1] = 2 * orientation.x * orientation.y + 2 * orientation.w * orientation.z;
	transformcyMatrix[5] = 1 - 2 * orientation.x * orientation.x - 2 * orientation.z * orientation.z;
	transformcyMatrix[9] = 2 * orientation.y * orientation.z - 2 * orientation.w * orientation.x;
	transformcyMatrix[13] = position.y;

	transformcyMatrix[2] = 2 * orientation.x * orientation.z - 2 * orientation.w * orientation.y;
	transformcyMatrix[6] = 2 * orientation.y * orientation.z + 2 * orientation.w * orientation.x;
	transformcyMatrix[10] = 1 - 2 * orientation.x * orientation.x - 2 * orientation.y * orientation.y;
	transformcyMatrix[14] = position.z;
}


void RigidBody::calculateDerivedData()
{
	orientation.Normalize();

	// Calculate the transform cyMatrix for the body.
	_calculateTransformcyMatrix(transformcyMatrix, position, orientation);

	// Calculate the inertiaTensor in world space.
	_transformInertiaTensor(inverseInertiaTensorWorld, orientation, inverseInertiaTensor, transformcyMatrix);

}

void RigidBody::integrate(float duration)
{
	if (!isAwake) return;

	// Calculate linear acceleration from force inputs.
	lastFrameAcceleration = acceleration;
	//lastFrameAcceleration.addScaledVector(forceAccum, inverseMass);
	lastFrameAcceleration += forceAccum * inverseMass;

	// Calculate angular acceleration from torque inputs.
	Vector3 angularAcceleration = inverseInertiaTensorWorld * torqueAccum;

	// Adjust velocities
	// Update linear velocity from both acceleration and impulse.
	velocity += lastFrameAcceleration * duration;

	// Update angular velocity from both acceleration and impulse.
	rotation += angularAcceleration * duration;

	// Impose drag.
	velocity *= pow(linearDamping, duration);
	rotation *= pow(angularDamping, duration);

	//velocity = Vector3(); // TEMP

	// Adjust positions
	// Update linear position.
	position += velocity * duration;

	// Update angular position.
	//orientation.addScaledVector(rotation, duration);
	Quaternion q(0, rotation.x* duration, rotation.y*duration, rotation.z*duration);
	q *= orientation;
	orientation.w += q.w * 0.5f;
	orientation.x += q.x * 0.5f;
	orientation.y += q.y * 0.5f;
	orientation.z += q.z * 0.5f;

	// Normalise the orientation, and update the matrices with the new
	// position and orientation
	calculateDerivedData();

	// Clear accumulators.
	clearAccumulators();

	// Update the kinetic energy store, and possibly put the body to
	// sleep.
	if (canSleep) {
		float currentMotion = Vector3::Dot(velocity, velocity) + Vector3::Dot(rotation, rotation);

		float bias = pow(0.5, duration);
		motion = bias * motion + (1 - bias) * currentMotion;

		if (motion < sleepEpsilon) setAwake(false);
		else if (motion > 10 * sleepEpsilon) motion = 10 * sleepEpsilon;
	}
}

void RigidBody::setMass(const float mass)
{
	RigidBody::inverseMass = ((float)1.0) / mass;
}

float RigidBody::getMass() const
{
	if (inverseMass == 0) {
		return INFINITY;
	}
	else {
		return ((float)1.0) / inverseMass;
	}
}

void RigidBody::setInverseMass(const float inverseMass)
{
	RigidBody::inverseMass = inverseMass;
}

float RigidBody::getInverseMass() const
{
	return inverseMass;
}

bool RigidBody::hasFiniteMass() const
{
	return inverseMass >= 0.0f;
}

void RigidBody::setInertiaTensor(const Matrix3x3& inertiaTensor)
{
	inverseInertiaTensor = Matrix3x3::Inverse(inertiaTensor);
	_checkInverseInertiaTensor(inverseInertiaTensor);
}

void RigidBody::getInertiaTensor(Matrix3x3& inertiaTensor) const
{
	inertiaTensor= Matrix3x3::Inverse(inverseInertiaTensor);
}

Matrix3x3 RigidBody::getInertiaTensor() const
{
	Matrix3x3 it;
	getInertiaTensor(it);
	return it;
}

void RigidBody::getInertiaTensorWorld(Matrix3x3& inertiaTensor) const
{
	inertiaTensor = Matrix3x3::Inverse(inverseInertiaTensorWorld);
}

Matrix3x3 RigidBody::getInertiaTensorWorld() const
{
	Matrix3x3 it;
	getInertiaTensorWorld(it);
	return it;
}

void RigidBody::setInverseInertiaTensor(const Matrix3x3& inverseInertiaTensor)
{
	_checkInverseInertiaTensor(inverseInertiaTensor);
	RigidBody::inverseInertiaTensor = inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensor(Matrix3x3& inverseInertiaTensor) const
{
	inverseInertiaTensor = RigidBody::inverseInertiaTensor;
}

Matrix3x3 RigidBody::getInverseInertiaTensor() const
{
	return inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensorWorld(Matrix3x3& inverseInertiaTensor) const
{
	inverseInertiaTensor = inverseInertiaTensorWorld;
}

Matrix3x3 RigidBody::getInverseInertiaTensorWorld() const
{
	return inverseInertiaTensorWorld;
}

void RigidBody::setDamping(const float linearDamping,
	const float angularDamping)
{
	RigidBody::linearDamping = linearDamping;
	RigidBody::angularDamping = angularDamping;
}

void RigidBody::setLinearDamping(const float linearDamping)
{
	RigidBody::linearDamping = linearDamping;
}

float RigidBody::getLinearDamping() const
{
	return linearDamping;
}

void RigidBody::setAngularDamping(const float angularDamping)
{
	RigidBody::angularDamping = angularDamping;
}

float RigidBody::getAngularDamping() const
{
	return angularDamping;
}

void RigidBody::setPosition(const Vector3& position)
{
	RigidBody::position = position;
}

void RigidBody::getPosition(Vector3& position) const
{
	position = RigidBody::position;
}

Vector3 RigidBody::getPosition() const
{
	return position;
}

void RigidBody::setOrientation(const Quaternion& orientation)
{
	RigidBody::orientation = orientation.Normalized();
}

void RigidBody::setOrientation(const float r, const float i,
	const float j, const float k)
{
	orientation.w = r;
	orientation.x = i;
	orientation.y = j;
	orientation.z = k;
	orientation.Normalize();
}

void RigidBody::getOrientation(Quaternion& orientation) const
{
	orientation = RigidBody::orientation;
}

Quaternion RigidBody::getOrientation() const
{
	return orientation;
}


void RigidBody::getTransform(Matrix4x4& transform) const
{
	memcpy(transform.mV, transformcyMatrix.mV, sizeof(Matrix4x4));
}

void RigidBody::getTransform(float matrixValues[16]) const
{
	memcpy(matrixValues, transformcyMatrix.mV, sizeof(float) * 12);
	matrixValues[3] = matrixValues[7] = matrixValues[11] = 0;
	matrixValues[15] = 1;
}

void RigidBody::getGLTransform(float matrixValues[16]) const
{
	matrixValues[0] = (float)transformcyMatrix[0];
	matrixValues[4] = (float)transformcyMatrix[1];
	matrixValues[8] = (float)transformcyMatrix[2];
	matrixValues[12] = 0;

	matrixValues[1] = (float)transformcyMatrix[4];
	matrixValues[5] = (float)transformcyMatrix[5];
	matrixValues[9] = (float)transformcyMatrix[6];
	matrixValues[13] = 0;

	matrixValues[2] = (float)transformcyMatrix[8];
	matrixValues[6] = (float)transformcyMatrix[9];
	matrixValues[10] = (float)transformcyMatrix[10];
	matrixValues[14] = 0;

	matrixValues[3] = (float)transformcyMatrix[12];
	matrixValues[7] = (float)transformcyMatrix[13];
	matrixValues[11] = (float)transformcyMatrix[14];
	matrixValues[15] = 1;
}

Matrix4x4 RigidBody::getTransform() const
{
	return transformcyMatrix;
}

Vector3 RigidBody::getPointInLocalSpace(const Vector3& point) const
{
	return Matrix4x4::AffineInverse(transformcyMatrix).TransformPoint(point);
}

Vector3 RigidBody::getPointInWorldSpace(const Vector3& point) const
{
	return transformcyMatrix.TransformPoint(point);
}

Vector3 RigidBody::getDirectionInLocalSpace(const Vector3& direction) const
{
	return Matrix4x4::AffineInverse(transformcyMatrix).TransformVector(direction);
}

Vector3 RigidBody::getDirectionInWorldSpace(const Vector3& direction) const
{
	return transformcyMatrix.TransformVector(direction);
}

void RigidBody::setVelocity(const Vector3& velocity)
{
	RigidBody::velocity = velocity;
}

void RigidBody::setVelocity(const float x, const float y, const float z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void RigidBody::getVelocity(Vector3& velocity) const
{
	velocity = RigidBody::velocity;
}

Vector3 RigidBody::getVelocity() const
{
	return velocity;
}

void RigidBody::addVelocity(const Vector3& deltaVelocity)
{
	velocity += deltaVelocity;
}

void RigidBody::setRotation(const Vector3& rotation)
{
	RigidBody::rotation = rotation;
}

void RigidBody::setRotation(const float x, const float y, const float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}

void RigidBody::getRotation(Vector3& rotation) const
{
	rotation = RigidBody::rotation;
}

Vector3 RigidBody::getRotation() const
{
	return rotation;
}

void RigidBody::addRotation(const Vector3& deltaRotation)
{
	rotation += deltaRotation;
}

void RigidBody::setAwake(const bool awake)
{
	if (awake) {
		isAwake = true;

		// Add a bit of motion to avoid it falling asleep immediately.
		motion = sleepEpsilon * 2.0f;
	}
	else {
		isAwake = false;
		velocity = Vector3();
		rotation = Vector3();
	}
}

void RigidBody::setCanSleep(const bool canSleep)
{
	RigidBody::canSleep = canSleep;

	if (!canSleep && !isAwake) setAwake();
}


void RigidBody::getLastFrameAcceleration(Vector3& acceleration) const
{
	acceleration = lastFrameAcceleration;
}

Vector3 RigidBody::getLastFrameAcceleration() const
{
	return lastFrameAcceleration;
}

void RigidBody::clearAccumulators()
{
	forceAccum = Vector3();
	torqueAccum = Vector3();
}

void RigidBody::addForce(const Vector3& force)
{
	forceAccum += force;
	isAwake = true;
}

void RigidBody::addForceAtBodyPoint(const Vector3& force,
	const Vector3& point)
{
	// Convert to coordinates relative to center of mass.
	Vector3 pt = getPointInWorldSpace(point);
	addForceAtPoint(force, pt);

}

void RigidBody::addForceAtPoint(const Vector3& force, const Vector3& point)
{
	// Convert to coordinates relative to center of mass.
	Vector3 pt = point;
	pt -= position;

	forceAccum += force;
	torqueAccum += Vector3(pt.y * force.z - pt.z * force.y, pt.z * force.x - pt.x * force.z, pt.x * force.y - pt.y * force.x);

	isAwake = true;
}

void RigidBody::addTorque(const Vector3& torque)
{
	torqueAccum += torque;
	isAwake = true;
}

void RigidBody::setAcceleration(const Vector3& acceleration)
{
	RigidBody::acceleration = acceleration;
}

void RigidBody::setAcceleration(const float x, const float y, const float z)
{
	acceleration.x = x;
	acceleration.y = y;
	acceleration.z = z;
}

void RigidBody::getAcceleration(Vector3& acceleration) const
{
	acceleration = RigidBody::acceleration;
}

Vector3 RigidBody::getAcceleration() const
{
	return acceleration;
}
