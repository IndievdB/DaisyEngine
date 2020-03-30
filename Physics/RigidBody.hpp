#pragma once

#include "cycloneMath.hpp"

class RigidBody
{
protected:

	float inverseMass;
	cyMatrix3 inverseInertiaTensor;
	float linearDamping;
	float angularDamping;
	Vector3 position;
	Quaternion orientation;
	Vector3 velocity;
	Vector3 rotation;
	cyMatrix3 inverseInertiaTensorWorld;
	float motion;
	bool isAwake;
	bool canSleep;
	cyMatrix4 transformcyMatrix;
	Vector3 forceAccum;
	Vector3 torqueAccum;
	Vector3 acceleration;
	Vector3 lastFrameAcceleration;

public:

	void calculateDerivedData();
	void integrate(float duration);
	void setMass(const float mass);
	float getMass() const;
	void setInverseMass(const float inverseMass);
	float getInverseMass() const;
	bool hasFiniteMass() const;
	void setInertiaTensor(const cyMatrix3& inertiaTensor);
	void getInertiaTensor(cyMatrix3& inertiaTensor) const;
	cyMatrix3 getInertiaTensor() const;
	void getInertiaTensorWorld(cyMatrix3& inertiaTensor) const;
	cyMatrix3 getInertiaTensorWorld() const;
	void setInverseInertiaTensor(const cyMatrix3& inverseInertiaTensor);
	void getInverseInertiaTensor(cyMatrix3& inverseInertiaTensor) const;
	cyMatrix3 getInverseInertiaTensor() const;
	void getInverseInertiaTensorWorld(cyMatrix3& inverseInertiaTensor) const;
	cyMatrix3 getInverseInertiaTensorWorld() const;
	void setDamping(const float linearDamping, const float angularDamping);
	void setLinearDamping(const float linearDamping);
	float getLinearDamping() const;
	void setAngularDamping(const float angularDamping);
	float getAngularDamping() const;
	void setPosition(const Vector3& position);
	void getPosition(Vector3& position) const;
	Vector3 getPosition() const;
	void setOrientation(const Quaternion& orientation);
	void setOrientation(const float r, const float i, const float j, const float k);
	void getOrientation(Quaternion& orientation) const;
	Quaternion getOrientation() const;
	void getOrientation(cyMatrix3& cyMatrix) const;
	void getOrientation(float cyMatrix[9]) const;
	void getTransform(cyMatrix4& transform) const;
	void getTransform(float cyMatrix[16]) const;
	void getGLTransform(float cyMatrix[16]) const;
	cyMatrix4 getTransform() const;
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
	void addForceAtPoint(const Vector3& force, const Vector3& point);
	void addForceAtBodyPoint(const Vector3& force, const Vector3& point);
	void addTorque(const Vector3& torque);
	void setAcceleration(const Vector3& acceleration);
	void setAcceleration(const float x, const float y, const float z);
	void getAcceleration(Vector3& acceleration) const;
	Vector3 getAcceleration() const;
};