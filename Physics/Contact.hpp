#pragma once

#include "RigidBody.hpp"
#include "../Core/Math/Vector3.hpp"

class Contact
{
	friend class ContactResolver;

public:

	RigidBody* body[2];
	float friction;
	float restitution;
	Vector3 contactPoint;
	Vector3 contactNormal;
	float penetration;
	void setBodyData(RigidBody* one, RigidBody* two, float friction, float restitution);

//protected:

	cyMatrix3 contactToWorld;
	Vector3 contactVelocity;
	float desiredDeltaVelocity;
	Vector3 relativeContactPosition[2];

protected:

	void calculateInternals(float duration);
	void swapBodies();
	void matchAwakeState();
	void calculateDesiredDeltaVelocity(float duration);
	Vector3 calculateLocalVelocity(unsigned bodyIndex, float duration);
	void calculateContactBasis();
	void applyImpulse(const Vector3& impulse, RigidBody& body, Vector3& velocityChange, Vector3& rotationChange);
	void applyVelocityChange(Vector3 velocityChange[2], Vector3 rotationChange[2]);
	void applyPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], float penetration);
	Vector3 calculateFrictionlessImpulse(cyMatrix3* inverseInertiaTensor);
	Vector3 calculateFrictionImpulse(cyMatrix3* inverseInertiaTensor);
};