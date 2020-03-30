#pragma once

#include "../Core/Transform.hpp"
#include "../Core/Math/Vector3.hpp"
#include "../Core/Math/Quaternion.hpp"
#include "../Core/Math/Matrix3x3.hpp"
#include "../Core/Math/Matrix4x4.hpp"

class RigidBody
{
protected:

	float inverseMass;
	Matrix3x3 inverseInertiaTensor;
	float linearDamping;
	float angularDamping;
	Vector3 velocity;
	Vector3 rotationVelocity;
	Matrix3x3 inverseInertiaTensorWorld;
	float motion;
	bool isAwake;
	bool canSleep;
	Matrix4x4 transformcyMatrix;
	Vector3 forceAccum;
	Vector3 torqueAccum;
	Vector3 acceleration;
	Vector3 lastFrameAcceleration;

public:

	void calculateDerivedData(Transform& transform);
	void integrate(Transform& transform, float duration);
	void setMass(const float mass);
	float getMass() const;
	void setInverseMass(const float inverseMass);
	float getInverseMass() const;
	bool hasFiniteMass() const;
	void setInertiaTensor(const Matrix3x3& inertiaTensor);
	void getInertiaTensor(Matrix3x3& inertiaTensor) const;
	Matrix3x3 getInertiaTensor() const;
	void getInertiaTensorWorld(Matrix3x3& inertiaTensor) const;
	Matrix3x3 getInertiaTensorWorld() const;
	void setInverseInertiaTensor(const Matrix3x3& inverseInertiaTensor);
	void getInverseInertiaTensor(Matrix3x3& inverseInertiaTensor) const;
	Matrix3x3 getInverseInertiaTensor() const;
	void getInverseInertiaTensorWorld(Matrix3x3& inverseInertiaTensor) const;
	Matrix3x3 getInverseInertiaTensorWorld() const;
	void setDamping(const float linearDamping, const float angularDamping);
	void setLinearDamping(const float linearDamping);
	float getLinearDamping() const;
	void setAngularDamping(const float angularDamping);
	float getAngularDamping() const;
	void getTransform(Matrix4x4& transform) const;
	void getTransform(float cyMatrix[16]) const;
	void getGLTransform(float cyMatrix[16]) const;
	Matrix4x4 getTransform() const;
	Vector3 getPointInLocalSpace(const Vector3& point) const;
	Vector3 getPointInWorldSpace(const Vector3& point) const;
	Vector3 getDirectionInLocalSpace(const Vector3& direction) const;
	Vector3 getDirectionInWorldSpace(const Vector3& direction) const;
	void setVelocity(const Vector3& velocity);
	void setVelocity(const float x, const float y, const float z);
	void getVelocity(Vector3& velocity) const;
	Vector3 getVelocity() const;
	void addVelocity(const Vector3& deltaVelocity);
	void setRotation(const Vector3& rotation);
	void setRotation(const float x, const float y, const float z);
	void getRotation(Vector3& rotation) const;
	Vector3 getRotation() const;
	void addRotation(const Vector3& deltaRotation);

	bool getAwake() const
	{
		return isAwake;
	}

	void setAwake(const bool awake = true);

	bool getCanSleep() const
	{
		return canSleep;
	}

	void setCanSleep(const bool canSleep = true);
	void getLastFrameAcceleration(Vector3& linearAcceleration) const;
	Vector3 getLastFrameAcceleration() const;
	void clearAccumulators();
	void addForce(const Vector3& force);
	void addForceAtPoint(const Transform& transform, const Vector3& force, const Vector3& point);
	void addForceAtBodyPoint(const Transform& transform, const Vector3& force, const Vector3& point);
	void addTorque(const Vector3& torque);
	void setAcceleration(const Vector3& acceleration);
	void setAcceleration(const float x, const float y, const float z);
	void getAcceleration(Vector3& acceleration) const;
	Vector3 getAcceleration() const;
};