#pragma once

#include "Contact.hpp"

class ContactResolver
{
protected:

	unsigned velocityIterations;
	unsigned positionIterations;
	float velocityEpsilon;
	float positionEpsilon;

public:

	unsigned velocityIterationsUsed;
	unsigned positionIterationsUsed;

private:

	bool validSettings;

public:

	ContactResolver(unsigned iterations, float velocityEpsilon = (float)0.01, float positionEpsilon = (float)0.01);
	ContactResolver(unsigned velocityIterations, unsigned positionIterations, float velocityEpsilon = (float)0.01, float positionEpsilon = (float)0.01);

	bool isValid()
	{
		return (velocityIterations > 0) && (positionIterations > 0) && (positionEpsilon >= 0.0f) && (positionEpsilon >= 0.0f);
	}

	void setIterations(unsigned velocityIterations, unsigned positionIterations);
	void setIterations(unsigned iterations);
	void setEpsilon(float velocityEpsilon, float positionEpsilon);
	void resolveContacts(Contact* contactArray, unsigned numContacts, float duration);

protected:

	void prepareContacts(Contact* contactArray, unsigned numContacts, float duration);
	void adjustVelocities(Contact* contactArray, unsigned numContacts, float duration);
	void adjustPositions(Contact* contacts, unsigned numContacts, float duration);
};