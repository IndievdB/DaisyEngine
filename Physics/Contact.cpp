#include "Contact.hpp"
#include <memory.h>
#include <assert.h>
#include <iostream>

void Contact::setBodyData(RigidBody* one, RigidBody* two, float friction, float restitution)
{
	Contact::body[0] = one;
	Contact::body[1] = two;
	Contact::friction = friction;
	Contact::restitution = restitution;
}

void Contact::matchAwakeState()
{
	if (!body[1]) return;

	bool body0awake = body[0]->getAwake();
	bool body1awake = body[1]->getAwake();

	if (body0awake ^ body1awake) {
		if (body0awake) body[1]->setAwake();
		else body[0]->setAwake();
	}
}

void Contact::swapBodies()
{
	contactNormal *= -1;

	RigidBody* temp = body[0];
	body[0] = body[1];
	body[1] = temp;
}

inline void Contact::calculateContactBasis()
{
	Vector3 contactTangent[2];

	if (abs(contactNormal.x) > abs(contactNormal.y))
	{
		const float s = (float)1.0f / sqrt(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);

		contactTangent[0].x = contactNormal.z * s;
		contactTangent[0].y = 0;
		contactTangent[0].z = -contactNormal.x * s;

		contactTangent[1].x = contactNormal.y * contactTangent[0].x;
		contactTangent[1].y = contactNormal.z * contactTangent[0].x -
			contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
	}
	else
	{
		const float s = (float)1.0 / sqrt(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);

		contactTangent[0].x = 0;
		contactTangent[0].y = -contactNormal.z * s;
		contactTangent[0].z = contactNormal.y * s;

		contactTangent[1].x = contactNormal.y * contactTangent[0].z - contactNormal.z * contactTangent[0].y;
		contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = contactNormal.x * contactTangent[0].y;
	}

	contactToWorld.setComponents(
		contactNormal,
		contactTangent[0],
		contactTangent[1]);
}

Vector3 Contact::calculateLocalVelocity(unsigned bodyIndex, float duration)
{
	RigidBody* thisBody = body[bodyIndex];
	Vector3 velocity = Vector3::Cross(thisBody->getRotation(), relativeContactPosition[bodyIndex]);
	velocity += thisBody->getVelocity();
	Vector3 contactVelocity = contactToWorld.transformTranspose(velocity);
	Vector3 accVelocity = thisBody->getLastFrameAcceleration() * duration;
	accVelocity = contactToWorld.transformTranspose(accVelocity);
	accVelocity.x = 0;
	contactVelocity += accVelocity;
	return contactVelocity;
}

void Contact::calculateDesiredDeltaVelocity(float duration)
{
	const static float velocityLimit = (float)0.25f;

	float velocityFromAcc = 0;

	if (body[0]->getAwake())
	{
		velocityFromAcc += Vector3::Dot(body[0]->getLastFrameAcceleration() * duration, contactNormal);
	}

	if (body[1] && body[1]->getAwake())
	{
		velocityFromAcc -= Vector3::Dot(body[1]->getLastFrameAcceleration() * duration, contactNormal);
	}

	float thisRestitution = restitution;
	if (abs(contactVelocity.x) < velocityLimit)
	{
		thisRestitution = (float)0.0f;
	}

	desiredDeltaVelocity =
		-contactVelocity.x
		- thisRestitution * (contactVelocity.x - velocityFromAcc);
}

void Contact::calculateInternals(float duration)
{
	// Check if the first object is NULL, and swap if it is.
	if (!body[0]) swapBodies();
	assert(body[0]);

	// Calculate an set of axis at the contact point.
	calculateContactBasis();

	// Store the relative position of the contact relative to each body
	relativeContactPosition[0] = contactPoint - body[0]->getPosition();
	if (body[1]) {
		relativeContactPosition[1] = contactPoint - body[1]->getPosition();
	}

	// Find the relative velocity of the bodies at the contact point.
	contactVelocity = calculateLocalVelocity(0, duration);
	if (body[1]) {
		contactVelocity -= calculateLocalVelocity(1, duration);
	}

	// Calculate the desired change in velocity for resolution
	calculateDesiredDeltaVelocity(duration);
}

void Contact::applyVelocityChange(Vector3 velocityChange[2],
	Vector3 rotationChange[2])
{
	// Get hold of the inverse mass and inverse inertia tensor, both in
	// world coordinates.
	cyMatrix3 inverseInertiaTensor[2];
	body[0]->getInverseInertiaTensorWorld(inverseInertiaTensor[0]);
	if (body[1])
		body[1]->getInverseInertiaTensorWorld(inverseInertiaTensor[1]);

	// We will calculate the impulse for each contact axis
	Vector3 impulseContact;

	if (friction == (float)0.0)
	{
		// Use the short format for frictionless contacts
		impulseContact = calculateFrictionlessImpulse(inverseInertiaTensor);
	}
	else
	{
		// Otherwise we may have impulses that aren't in the direction of the
		// contact, so we need the more complex version.
		impulseContact = calculateFrictionImpulse(inverseInertiaTensor);
	}

	// Convert impulse to world coordinates
	Vector3 impulse = contactToWorld.transform(impulseContact);

	// Split in the impulse into linear and rotational components
	Vector3 impulsiveTorque = Vector3::Cross(relativeContactPosition[0], impulse);
	rotationChange[0] = inverseInertiaTensor[0].transform(impulsiveTorque);
	velocityChange[0] = Vector3();
	velocityChange[0] += impulse * body[0]->getInverseMass();

	// Apply the changes
   	body[0]->addVelocity(velocityChange[0]);
	body[0]->addRotation(rotationChange[0]);

	if (body[1])
	{
		// Work out body one's linear and angular changes
		Vector3 impulsiveTorque = Vector3::Cross(impulse, relativeContactPosition[1]);
		rotationChange[1] = inverseInertiaTensor[1].transform(impulsiveTorque);
		velocityChange[1] = Vector3();
		velocityChange[1] += impulse * -body[1]->getInverseMass();

		// And apply them.
		body[1]->addVelocity(velocityChange[1]);
		body[1]->addRotation(rotationChange[1]);
	}
}

inline
Vector3 Contact::calculateFrictionlessImpulse(cyMatrix3* inverseInertiaTensor)
{
	Vector3 impulseContact;

	// Build a vector that shows the change in velocity in
	// world space for a unit impulse in the direction of the contact
	// normal.
	Vector3 deltaVelWorld = Vector3::Cross(relativeContactPosition[0], contactNormal);
	deltaVelWorld = inverseInertiaTensor[0].transform(deltaVelWorld);
	deltaVelWorld = Vector3::Cross(deltaVelWorld, relativeContactPosition[0]);

	// Work out the change in velocity in contact coordiantes.
	float deltaVelocity = Vector3::Dot (deltaVelWorld, contactNormal);

	// Add the linear component of velocity change
	deltaVelocity += body[0]->getInverseMass();

	// Check if we need to the second body's data
	if (body[1])
	{
		// Go through the same transformation sequence again
		Vector3 deltaVelWorld = Vector3::Cross(relativeContactPosition[1], contactNormal);
		deltaVelWorld = inverseInertiaTensor[1].transform(deltaVelWorld);
		deltaVelWorld = Vector3::Cross(deltaVelWorld, relativeContactPosition[1]);

		// Add the change in velocity due to rotation
		deltaVelocity += Vector3::Dot(deltaVelWorld, contactNormal);

		// Add the change in velocity due to linear motion
		deltaVelocity += body[1]->getInverseMass();
	}

	// Calculate the required size of the impulse
	impulseContact.x = desiredDeltaVelocity / deltaVelocity;
	impulseContact.y = 0;
	impulseContact.z = 0;
	return impulseContact;
}

inline
Vector3 Contact::calculateFrictionImpulse(cyMatrix3* inverseInertiaTensor)
{
	Vector3 impulseContact;
	float inverseMass = body[0]->getInverseMass();

	// The equivalent of a cross product in matrices is multiplication
	// by a skew symmetric cyMatrix - we build the cyMatrix for converting
	// between linear and angular quantities.
	cyMatrix3 impulseToTorque;
	impulseToTorque.setSkewSymmetric(relativeContactPosition[0]);

	// Build the cyMatrix to convert contact impulse to change in velocity
	// in world coordinates.
	cyMatrix3 deltaVelWorld = impulseToTorque;
	deltaVelWorld *= inverseInertiaTensor[0];
	deltaVelWorld *= impulseToTorque;
	deltaVelWorld *= -1;

	// Check if we need to add body two's data
	if (body[1])
	{
		// Set the cross product cyMatrix
		impulseToTorque.setSkewSymmetric(relativeContactPosition[1]);

		// Calculate the velocity change cyMatrix
		cyMatrix3 deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 *= inverseInertiaTensor[1];
		deltaVelWorld2 *= impulseToTorque;
		deltaVelWorld2 *= -1;

		// Add to the total delta velocity.
		deltaVelWorld += deltaVelWorld2;

		// Add to the inverse mass
		inverseMass += body[1]->getInverseMass();
	}

	// Do a change of basis to convert into contact coordinates.
	cyMatrix3 deltaVelocity = contactToWorld.transpose();
	deltaVelocity *= deltaVelWorld;
	deltaVelocity *= contactToWorld;

	// Add in the linear velocity change
	deltaVelocity.data[0] += inverseMass;
	deltaVelocity.data[4] += inverseMass;
	deltaVelocity.data[8] += inverseMass;

	// Invert to get the impulse needed per unit velocity
	cyMatrix3 impulsecyMatrix = deltaVelocity.inverse();

	// Find the target velocities to kill
	Vector3 velKill(desiredDeltaVelocity,
		-contactVelocity.y,
		-contactVelocity.z);

	// Find the impulse to kill target velocities
	impulseContact = impulsecyMatrix.transform(velKill);

	// Check for exceeding friction
	float planarImpulse = sqrt(
		impulseContact.y * impulseContact.y +
		impulseContact.z * impulseContact.z
	);
	if (planarImpulse > impulseContact.x * friction)
	{
		// We need to use dynamic friction
		impulseContact.y /= planarImpulse;
		impulseContact.z /= planarImpulse;

		impulseContact.x = deltaVelocity.data[0] +
			deltaVelocity.data[1] * friction * impulseContact.y +
			deltaVelocity.data[2] * friction * impulseContact.z;
		impulseContact.x = desiredDeltaVelocity / impulseContact.x;
		impulseContact.y *= friction * impulseContact.x;
		impulseContact.z *= friction * impulseContact.x;
	}
	return impulseContact;
}

void Contact::applyPositionChange(Vector3 linearChange[2],
	Vector3 angularChange[2],
	float penetration)
{
	const float angularLimit = (float)0.2f;
	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0;
	float linearInertia[2];
	float angularInertia[2];

	// We need to work out the inertia of each object in the direction
	// of the contact normal, due to angular inertia only.
	for (unsigned i = 0; i < 2; i++) if (body[i])
	{
		cyMatrix3 inverseInertiaTensor;
		body[i]->getInverseInertiaTensorWorld(inverseInertiaTensor);

		// Use the same procedure as for calculating frictionless
		// velocity change to work out the angular inertia.
		Vector3 angularInertiaWorld =
			Vector3::Cross(relativeContactPosition[i], contactNormal);
		angularInertiaWorld =
			inverseInertiaTensor.transform(angularInertiaWorld);
		angularInertiaWorld =
			Vector3::Cross(angularInertiaWorld, relativeContactPosition[i]);
		angularInertia[i] = Vector3::Dot(angularInertiaWorld, contactNormal);

		// The linear component is simply the inverse mass
		linearInertia[i] = body[i]->getInverseMass();

		// Keep track of the total inertia from all components
		totalInertia += linearInertia[i] + angularInertia[i];

		// We break the loop here so that the totalInertia value is
		// completely calculated (by both iterations) before
		// continuing.
	}

	// Loop through again calculating and applying the changes
	for (unsigned i = 0; i < 2; i++) if (body[i])
	{
		// The linear and angular movements required are in proportion to
		// the two inverse inertias.
		float sign = (i == 0) ? 1 : -1;
		angularMove[i] =
			sign * penetration * (angularInertia[i] / totalInertia);
		linearMove[i] =
			sign * penetration * (linearInertia[i] / totalInertia);

		// To avoid angular projections that are too great (when mass is large
		// but inertia tensor is small) limit the angular move.
		Vector3 projection = relativeContactPosition[i];
		projection += contactNormal * Vector3::Dot(-relativeContactPosition[i], contactNormal);

		// Use the small angle approximation for the sine of the angle (i.e.
		// the magnitude would be sine(angularLimit) * projection.magnitude
		// but we approximate sine(angularLimit) to angularLimit).
		float maxMagnitude = angularLimit * projection.Length();

		if (angularMove[i] < -maxMagnitude)
		{
			float totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = -maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}
		else if (angularMove[i] > maxMagnitude)
		{
			float totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}

		// We have the linear amount of movement required by turning
		// the rigid body (in angularMove[i]). We now need to
		// calculate the desired rotation to achieve that.
		if (angularMove[i] == 0)
		{
			// Easy case - no angular movement means no rotation.
			angularChange[i] = Vector3();
		}
		else
		{
			// Work out the direction we'd like to rotate in.
			Vector3 targetAngularDirection = Vector3::Cross(relativeContactPosition[i], contactNormal);

			cyMatrix3 inverseInertiaTensor;
			body[i]->getInverseInertiaTensorWorld(inverseInertiaTensor);

			// Work out the direction we'd need to rotate to achieve that
			angularChange[i] =
				inverseInertiaTensor.transform(targetAngularDirection) *
				(angularMove[i] / angularInertia[i]);
		}

		// Velocity change is easier - it is just the linear movement
		// along the contact normal.
		linearChange[i] = contactNormal * linearMove[i];

		// Now we can start to apply the values we've calculated.
		// Apply the linear movement
		Vector3 pos;
		body[i]->getPosition(pos);
		//std::cout << "a " << pos.x << "  |  " << linearMove[i] << "  |  " << contactNormal.x << "   " << contactNormal.y << "   " << contactNormal.z << std::endl;
		pos += contactNormal * linearMove[i];
		//std::cout << "b " << pos.x << "   " << pos.y << "   " << pos.z << std::endl;
		body[i]->setPosition(pos);

		// And the change in orientation
		Quaternion q;
		body[i]->getOrientation(q);

		Quaternion temp(0, angularChange[i].x, angularChange[i].y, angularChange[i].z);
		temp *= q;
		q.w += temp.w * 0.5f;
		q.x += temp.x * 0.5f;
		q.y += temp.y * 0.5f;
		q.z += temp.z * 0.5f;
		
		//q.addScaledVector(angularChange[i], ((float)1.0));
		body[i]->setOrientation(q);

		// We need to calculate the derived data for any body that is
		// asleep, so that the changes are reflected in the object's
		// data. Otherwise the resolution will not change the position
		// of the object, and the next collision detection round will
		// have the same penetration.
		if (!body[i]->getAwake()) body[i]->calculateDerivedData();
	}
}