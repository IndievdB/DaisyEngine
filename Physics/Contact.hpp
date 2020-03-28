#pragma once

#include "RigidBody.hpp"

class Contact
{
	friend class ContactResolver;

public:

	RigidBody* body[2];
	float friction;
	float restitution;
	glm::vec3 contactPoint;
	glm::vec3 contactNormal;
	float penetration;
	void setBodyData(RigidBody* one, RigidBody* two, float friction, float restitution);

//protected:

	cyMatrix3 contactToWorld;
	glm::vec3 contactVelocity;
	float desiredDeltaVelocity;
	glm::vec3 relativeContactPosition[2];

protected:

	void calculateInternals(float duration);
	void swapBodies();
	void matchAwakeState();
	void calculateDesiredDeltaVelocity(float duration);
	glm::vec3 calculateLocalVelocity(unsigned bodyIndex, float duration);
	void calculateContactBasis();
	void applyImpulse(const glm::vec3& impulse, RigidBody& body, glm::vec3& velocityChange, glm::vec3& rotationChange);
	void applyVelocityChange(glm::vec3 velocityChange[2], glm::vec3 rotationChange[2]);
	void applyPositionChange(glm::vec3 linearChange[2], glm::vec3 angularChange[2], float penetration);
	glm::vec3 calculateFrictionlessImpulse(cyMatrix3* inverseInertiaTensor);
	glm::vec3 calculateFrictionImpulse(cyMatrix3* inverseInertiaTensor);
};