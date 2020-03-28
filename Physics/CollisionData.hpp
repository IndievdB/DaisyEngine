#pragma once

#include "Contact.hpp"

struct CollisionData
{
	Contact contactArray[256];
	int currentContact = 0;
	int contactsLeft;
	int contactCount = 0;
	float friction;
	float restitution;
	float tolerance;

	bool hasMoreContacts()
	{
		return contactsLeft > 0;
	}

	void reset(int maxContacts)
	{
		contactsLeft = maxContacts;
		contactCount = 0;
		currentContact = 0;
	}

	void addContacts(int count)
	{
		contactsLeft -= count;
		contactCount += count;
		currentContact += count;
	}
};