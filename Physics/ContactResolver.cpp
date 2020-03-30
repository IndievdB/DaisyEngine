#include "ContactResolver.hpp"

ContactResolver::ContactResolver(unsigned iterations, float velocityEpsilon, float positionEpsilon)
{
	setIterations(iterations, iterations);
	setEpsilon(velocityEpsilon, positionEpsilon);
}

ContactResolver::ContactResolver(unsigned velocityIterations, unsigned positionIterations, float velocityEpsilon, float positionEpsilon)
{
	setIterations(velocityIterations);
	setEpsilon(velocityEpsilon, positionEpsilon);
}

void ContactResolver::setIterations(unsigned iterations)
{
	setIterations(iterations, iterations);
}

void ContactResolver::setIterations(unsigned velocityIterations, unsigned positionIterations)
{
	ContactResolver::velocityIterations = velocityIterations;
	ContactResolver::positionIterations = positionIterations;
}

void ContactResolver::setEpsilon(float velocityEpsilon, float positionEpsilon)
{
	ContactResolver::velocityEpsilon = velocityEpsilon;
	ContactResolver::positionEpsilon = positionEpsilon;
}

void ContactResolver::resolveContacts(Contact* contacts, unsigned numContacts, float duration)
{
	// Make sure we have something to do.
	if (numContacts == 0) return;
	if (!isValid()) return;

	// Prepare the contacts for processing
	prepareContacts(contacts, numContacts, duration);

	// Resolve the interpenetration problems with the contacts.
	adjustPositions(contacts, numContacts, duration);

	// Resolve the velocity problems with the contacts.
	adjustVelocities(contacts, numContacts, duration);
}

bool temp = true;
void ContactResolver::prepareContacts(Contact* contacts, unsigned numContacts, float duration)
{
	// Generate contact velocity and axis information.
	Contact* lastContact = contacts + numContacts;
	for (Contact* contact = contacts; contact < lastContact; contact++)
	{
		// Calculate the internal contact data (inertia, basis, etc).
		contact->calculateInternals(duration);
	}
}

void ContactResolver::adjustVelocities(Contact* c, unsigned numContacts, float duration)
{
	Vector3 velocityChange[2], rotationChange[2];
	Vector3 deltaVel;

	// iteratively handle impacts in order of severity.
	velocityIterationsUsed = 0;
	while (velocityIterationsUsed < velocityIterations)
	{
		// Find contact with maximum magnitude of probable velocity change.
		float max = velocityEpsilon;
		unsigned index = numContacts;
		for (unsigned i = 0; i < numContacts; i++)
		{
			if (c[i].desiredDeltaVelocity > max)
			{
				max = c[i].desiredDeltaVelocity;
				index = i;
			}
		}
		if (index == numContacts) break;

		// Match the awake state at the contact
		c[index].matchAwakeState();

		// Do the resolution on the contact that came out top.
		c[index].applyVelocityChange(velocityChange, rotationChange);

		// With the change in velocity of the two bodies, the update of
		// contact velocities means that some of the relative closing
		// velocities need recomputing.
		for (unsigned i = 0; i < numContacts; i++)
		{
			// Check each body in the contact
			for (unsigned b = 0; b < 2; b++) if (c[i].body[b])
			{
				// Check for a match with each body in the newly
				// resolved contact
				for (unsigned d = 0; d < 2; d++)
				{
					if (c[i].body[b] == c[index].body[d])
					{
						deltaVel = velocityChange[d] + Vector3::Cross(rotationChange[d], c[i].relativeContactPosition[b]);

						// The sign of the change is negative if we're dealing
						// with the second body in a contact.
						c[i].contactVelocity += (Matrix3x3::Transpose(c[i].contactToWorld) * deltaVel) * (b ? -1.0f : 1.0f);
						c[i].calculateDesiredDeltaVelocity(duration);
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}

void ContactResolver::adjustPositions(Contact* c, unsigned numContacts, float duration)
{
	unsigned i, index;
	Vector3 linearChange[2], angularChange[2];
	float max;
	Vector3 deltaPosition;

	// iteratively resolve interpenetrations in order of severity.
	positionIterationsUsed = 0;
	while (positionIterationsUsed < positionIterations)
	{
		// Find biggest penetration
		max = positionEpsilon;
		index = numContacts;
		for (i = 0; i < numContacts; i++)
		{
			if (c[i].penetration > max)
			{
				max = c[i].penetration;
				index = i;
			}
		}
		if (index == numContacts) break;

		// Match the awake state at the contact
		c[index].matchAwakeState();

		// Resolve the penetration.
		c[index].applyPositionChange( linearChange, angularChange, max);

		// Again this action may have changed the penetration of other
		// bodies, so we update contacts.
		for (i = 0; i < numContacts; i++)
		{
			// Check each body in the contact
			for (unsigned b = 0; b < 2; b++) if (c[i].body[b])
			{
				// Check for a match with each body in the newly
				// resolved contact
				for (unsigned d = 0; d < 2; d++)
				{
					if (c[i].body[b] == c[index].body[d])
					{
						deltaPosition = linearChange[d] + Vector3::Cross(angularChange[d], c[i].relativeContactPosition[b]);

						// The sign of the change is positive if we're
						// dealing with the second body in a contact
						// and negative otherwise (because we're
						// subtracting the resolution)..
						c[i].penetration += Vector3::Dot(deltaPosition, c[i].contactNormal) * (b ? 1.0f : -1.0f);
					}
				}
			}
		}
		positionIterationsUsed++;
	}
}
