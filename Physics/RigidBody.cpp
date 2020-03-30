#include "RigidBody.hpp"

#include <iostream>

static float sleepEpsilon = 0.3f;

static inline void _checkInverseInertiaTensor(const cyMatrix3& iitWorld)
{
	// TODO: Perform a validity check in an assert.
}

bool madeContact = false;


static inline void _transformInertiaTensor(cyMatrix3& iitWorld,
	const Quaternion& q,
	const cyMatrix3& iitBody,
	const cyMatrix4& rotmat)
{
	float t4 = rotmat.data[0] * iitBody.data[0] +
		rotmat.data[1] * iitBody.data[3] +
		rotmat.data[2] * iitBody.data[6];
	float t9 = rotmat.data[0] * iitBody.data[1] +
		rotmat.data[1] * iitBody.data[4] +
		rotmat.data[2] * iitBody.data[7];
	float t14 = rotmat.data[0] * iitBody.data[2] +
		rotmat.data[1] * iitBody.data[5] +
		rotmat.data[2] * iitBody.data[8];
	float t28 = rotmat.data[4] * iitBody.data[0] +
		rotmat.data[5] * iitBody.data[3] +
		rotmat.data[6] * iitBody.data[6];
	float t33 = rotmat.data[4] * iitBody.data[1] +
		rotmat.data[5] * iitBody.data[4] +
		rotmat.data[6] * iitBody.data[7];
	float t38 = rotmat.data[4] * iitBody.data[2] +
		rotmat.data[5] * iitBody.data[5] +
		rotmat.data[6] * iitBody.data[8];
	float t52 = rotmat.data[8] * iitBody.data[0] +
		rotmat.data[9] * iitBody.data[3] +
		rotmat.data[10] * iitBody.data[6];
	float t57 = rotmat.data[8] * iitBody.data[1] +
		rotmat.data[9] * iitBody.data[4] +
		rotmat.data[10] * iitBody.data[7];
	float t62 = rotmat.data[8] * iitBody.data[2] +
		rotmat.data[9] * iitBody.data[5] +
		rotmat.data[10] * iitBody.data[8];

	iitWorld.data[0] = t4 * rotmat.data[0] +
		t9 * rotmat.data[1] +
		t14 * rotmat.data[2];
	iitWorld.data[1] = t4 * rotmat.data[4] +
		t9 * rotmat.data[5] +
		t14 * rotmat.data[6];
	iitWorld.data[2] = t4 * rotmat.data[8] +
		t9 * rotmat.data[9] +
		t14 * rotmat.data[10];
	iitWorld.data[3] = t28 * rotmat.data[0] +
		t33 * rotmat.data[1] +
		t38 * rotmat.data[2];
	iitWorld.data[4] = t28 * rotmat.data[4] +
		t33 * rotmat.data[5] +
		t38 * rotmat.data[6];
	iitWorld.data[5] = t28 * rotmat.data[8] +
		t33 * rotmat.data[9] +
		t38 * rotmat.data[10];
	iitWorld.data[6] = t52 * rotmat.data[0] +
		t57 * rotmat.data[1] +
		t62 * rotmat.data[2];
	iitWorld.data[7] = t52 * rotmat.data[4] +
		t57 * rotmat.data[5] +
		t62 * rotmat.data[6];
	iitWorld.data[8] = t52 * rotmat.data[8] +
		t57 * rotmat.data[9] +
		t62 * rotmat.data[10];
}

static inline void _calculateTransformcyMatrix(cyMatrix4& transformcyMatrix,
	const Vector3& position,
	const Quaternion& orientation)
{
	transformcyMatrix.data[0] = 1 - 2 * orientation.y * orientation.y -
		2 * orientation.z * orientation.z;
	transformcyMatrix.data[1] = 2 * orientation.x * orientation.y -
		2 * orientation.w * orientation.z;
	transformcyMatrix.data[2] = 2 * orientation.x * orientation.z +
		2 * orientation.w * orientation.y;
	transformcyMatrix.data[3] = position.x;

	transformcyMatrix.data[4] = 2 * orientation.x * orientation.y +
		2 * orientation.w * orientation.z;
	transformcyMatrix.data[5] = 1 - 2 * orientation.x * orientation.x -
		2 * orientation.z * orientation.z;
	transformcyMatrix.data[6] = 2 * orientation.y * orientation.z -
		2 * orientation.w * orientation.x;
	transformcyMatrix.data[7] = position.y;

	transformcyMatrix.data[8] = 2 * orientation.x * orientation.z -
		2 * orientation.w * orientation.y;
	transformcyMatrix.data[9] = 2 * orientation.y * orientation.z +
		2 * orientation.w * orientation.x;
	transformcyMatrix.data[10] = 1 - 2 * orientation.x * orientation.x -
		2 * orientation.y * orientation.y;
	transformcyMatrix.data[11] = position.z;
}


void RigidBody::calculateDerivedData()
{
	orientation = Normalize(orientation);

	// Calculate the transform cyMatrix for the body.
	_calculateTransformcyMatrix(transformcyMatrix, position, orientation);

	// Calculate the inertiaTensor in world space.
	_transformInertiaTensor(inverseInertiaTensorWorld,
		orientation,
		inverseInertiaTensor,
		transformcyMatrix);

}

void RigidBody::integrate(float duration)
{
	if (!isAwake) return;

	if (madeContact)
	{
        //PrintVec3(rotation);
		//std::cout << "here" << std::endl;
	}

	// Calculate linear acceleration from force inputs.
	lastFrameAcceleration = acceleration;
	//lastFrameAcceleration.addScaledVector(forceAccum, inverseMass);
	lastFrameAcceleration += forceAccum * inverseMass;

	// Calculate angular acceleration from torque inputs.
	Vector3 angularAcceleration = inverseInertiaTensorWorld.transform(torqueAccum);

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

void RigidBody::setInertiaTensor(const cyMatrix3& inertiaTensor)
{
	inverseInertiaTensor.setInverse(inertiaTensor);
	_checkInverseInertiaTensor(inverseInertiaTensor);
}

void RigidBody::getInertiaTensor(cyMatrix3& inertiaTensor) const
{
	inertiaTensor.setInverse(inverseInertiaTensor);
}

cyMatrix3 RigidBody::getInertiaTensor() const
{
	cyMatrix3 it;
	getInertiaTensor(it);
	return it;
}

void RigidBody::getInertiaTensorWorld(cyMatrix3& inertiaTensor) const
{
	inertiaTensor.setInverse(inverseInertiaTensorWorld);
}

cyMatrix3 RigidBody::getInertiaTensorWorld() const
{
	cyMatrix3 it;
	getInertiaTensorWorld(it);
	return it;
}

void RigidBody::setInverseInertiaTensor(const cyMatrix3& inverseInertiaTensor)
{
	_checkInverseInertiaTensor(inverseInertiaTensor);
	RigidBody::inverseInertiaTensor = inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensor(cyMatrix3& inverseInertiaTensor) const
{
	inverseInertiaTensor = RigidBody::inverseInertiaTensor;
}

cyMatrix3 RigidBody::getInverseInertiaTensor() const
{
	return inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensorWorld(cyMatrix3& inverseInertiaTensor) const
{
	inverseInertiaTensor = inverseInertiaTensorWorld;
}

cyMatrix3 RigidBody::getInverseInertiaTensorWorld() const
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
	RigidBody::orientation = orientation;
	RigidBody::orientation = Normalize(RigidBody::orientation);
}

void RigidBody::setOrientation(const float r, const float i,
	const float j, const float k)
{
	orientation.w = r;
	orientation.x = i;
	orientation.y = j;
	orientation.z = k;
	orientation = Normalize(orientation);
}

void RigidBody::getOrientation(Quaternion& orientation) const
{
	orientation = RigidBody::orientation;
}

Quaternion RigidBody::getOrientation() const
{
	return orientation;
}

void RigidBody::getOrientation(cyMatrix3& cyMatrix) const
{
	getOrientation(cyMatrix.data);
}

void RigidBody::getOrientation(float cyMatrix[9]) const
{
	cyMatrix[0] = transformcyMatrix.data[0];
	cyMatrix[1] = transformcyMatrix.data[1];
	cyMatrix[2] = transformcyMatrix.data[2];

	cyMatrix[3] = transformcyMatrix.data[4];
	cyMatrix[4] = transformcyMatrix.data[5];
	cyMatrix[5] = transformcyMatrix.data[6];

	cyMatrix[6] = transformcyMatrix.data[8];
	cyMatrix[7] = transformcyMatrix.data[9];
	cyMatrix[8] = transformcyMatrix.data[10];
}

void RigidBody::getTransform(cyMatrix4& transform) const
{
	memcpy(transform.data, transformcyMatrix.data, sizeof(cyMatrix4));
}

void RigidBody::getTransform(float cyMatrix[16]) const
{
	memcpy(cyMatrix, transformcyMatrix.data, sizeof(float) * 12);
	cyMatrix[12] = cyMatrix[13] = cyMatrix[14] = 0;
	cyMatrix[15] = 1;
}

void RigidBody::getGLTransform(float cyMatrix[16]) const
{
	cyMatrix[0] = (float)transformcyMatrix.data[0];
	cyMatrix[1] = (float)transformcyMatrix.data[4];
	cyMatrix[2] = (float)transformcyMatrix.data[8];
	cyMatrix[3] = 0;

	cyMatrix[4] = (float)transformcyMatrix.data[1];
	cyMatrix[5] = (float)transformcyMatrix.data[5];
	cyMatrix[6] = (float)transformcyMatrix.data[9];
	cyMatrix[7] = 0;

	cyMatrix[8] = (float)transformcyMatrix.data[2];
	cyMatrix[9] = (float)transformcyMatrix.data[6];
	cyMatrix[10] = (float)transformcyMatrix.data[10];
	cyMatrix[11] = 0;

	cyMatrix[12] = (float)transformcyMatrix.data[3];
	cyMatrix[13] = (float)transformcyMatrix.data[7];
	cyMatrix[14] = (float)transformcyMatrix.data[11];
	cyMatrix[15] = 1;
}

cyMatrix4 RigidBody::getTransform() const
{
	return transformcyMatrix;
}


Vector3 RigidBody::getPointInLocalSpace(const Vector3& point) const
{
	return transformcyMatrix.transformInverse(point);
}

Vector3 RigidBody::getPointInWorldSpace(const Vector3& point) const
{
	return transformcyMatrix.transform(point);
}

Vector3 RigidBody::getDirectionInLocalSpace(const Vector3& direction) const
{
	return transformcyMatrix.transformInverseDirection(direction);
}

Vector3 RigidBody::getDirectionInWorldSpace(const Vector3& direction) const
{
	return transformcyMatrix.transformDirection(direction);
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

	madeContact = true;
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
