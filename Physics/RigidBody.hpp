#pragma once

#include "cycloneMath.hpp"

class RigidBody
{
protected:

	float inverseMass;
	cyMatrix3 inverseInertiaTensor;
	float linearDamping;
	float angularDamping;
	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 velocity;
	glm::vec3 rotation;
	cyMatrix3 inverseInertiaTensorWorld;
	float motion;
	bool isAwake;
	bool canSleep;
	cyMatrix4 transformcyMatrix;
	glm::vec3 forceAccum;
	glm::vec3 torqueAccum;
	glm::vec3 acceleration;
	glm::vec3 lastFrameAcceleration;

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
	void setPosition(const glm::vec3& position);
	void getPosition(glm::vec3& position) const;
	glm::vec3 getPosition() const;
	void setOrientation(const glm::quat& orientation);
	void setOrientation(const float r, const float i, const float j, const float k);
	void getOrientation(glm::quat& orientation) const;
	glm::quat getOrientation() const;
	void getOrientation(cyMatrix3& cyMatrix) const;
	void getOrientation(float cyMatrix[9]) const;
	void getTransform(cyMatrix4& transform) const;
	void getTransform(float cyMatrix[16]) const;
	void getGLTransform(float cyMatrix[16]) const;
	cyMatrix4 getTransform() const;
	glm::vec3 getPointInLocalSpace(const glm::vec3& point) const;
	glm::vec3 getPointInWorldSpace(const glm::vec3& point) const;
	glm::vec3 getDirectionInLocalSpace(const glm::vec3& direction) const;
	glm::vec3 getDirectionInWorldSpace(const glm::vec3& direction) const;
	void setVelocity(const glm::vec3& velocity);
	void setVelocity(const float x, const float y, const float z);
	void getVelocity(glm::vec3& velocity) const;
	glm::vec3 getVelocity() const;
	void addVelocity(const glm::vec3& deltaVelocity);
	void setRotation(const glm::vec3& rotation);
	void setRotation(const float x, const float y, const float z);
	void getRotation(glm::vec3& rotation) const;
	glm::vec3 getRotation() const;
	void addRotation(const glm::vec3& deltaRotation);

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
	void getLastFrameAcceleration(glm::vec3& linearAcceleration) const;
	glm::vec3 getLastFrameAcceleration() const;
	void clearAccumulators();
	void addForce(const glm::vec3& force);
	void addForceAtPoint(const glm::vec3& force, const glm::vec3& point);
	void addForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point);
	void addTorque(const glm::vec3& torque);
	void setAcceleration(const glm::vec3& acceleration);
	void setAcceleration(const float x, const float y, const float z);
	void getAcceleration(glm::vec3& acceleration) const;
	glm::vec3 getAcceleration() const;
};