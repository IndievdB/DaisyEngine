#include "CollisionDetector.hpp"
#include "IntersectionTests.hpp"
#include <iostream>

unsigned CollisionDetector::SphereAndHalfSpace(const SphereCollider& sphere, const Transform& sphereTransform, RigidBody* sphereRigidBody, const PlaneCollider& plane, CollisionData& data)
{
	// Make sure we have contacts
	if (data.contactsLeft <= 0) return 0;

	float radius = sphere.radius * glm::compMax(sphereTransform.scale);

	// Cache the sphere position
	glm::vec3 position = sphereTransform.position;

	// Find the distance from the plane
	float ballDistance = glm::dot(plane.normal, position) - radius - plane.offset;
	
	if (ballDistance >= 0) return 0;

	// Create the contact - it has a normal in the plane direction.

	data.contactArray[data.currentContact].contactNormal = plane.normal;
	data.contactArray[data.currentContact].penetration = -ballDistance;
	data.contactArray[data.currentContact].contactPoint = position - plane.normal * (ballDistance + sphere.radius);
	data.contactArray[data.currentContact].setBodyData(sphereRigidBody, NULL, data.friction, data.restitution);

	data.addContacts(1);

	return 1;
}

unsigned CollisionDetector::SphereAndSphere(const SphereCollider& one, const Transform& oneTransform, RigidBody* oneRigidBody, const SphereCollider& two, const Transform& twoTransform, RigidBody* twoRigidBody, CollisionData& data)
{
	// Make sure we have contacts
	if (data.contactsLeft <= 0) return 0;

	// Cache the sphere positions
	glm::vec3 positionOne = oneTransform.position;
	glm::vec3 positionTwo = twoTransform.position;

	float radiusOne = one.radius * glm::compMax(oneTransform.scale);
	float radiusTwo = two.radius * glm::compMax(twoTransform.scale);

	// Find the vector between the objects
	glm::vec3 midline = positionOne - positionTwo;
	float size = glm::length (midline);

	// See if it is large enough.
	if (size <= 0.0f || size >= radiusOne + radiusTwo)
	{
		return 0;
	}

	// We manually create the normal, because we have the
	// size to hand.
	glm::vec3 normal = midline * (1.0f / size);

	data.contactArray[data.currentContact].contactNormal = normal;
	data.contactArray[data.currentContact].contactPoint = positionOne + midline * 0.5f;
	data.contactArray[data.currentContact].penetration = (radiusOne + radiusTwo - size);
	data.contactArray[data.currentContact].setBodyData(oneRigidBody, twoRigidBody, data.friction, data.restitution);

	data.addContacts(1);

	return 1;
}

unsigned CollisionDetector::BoxAndHalfSpace(const BoxCollider& box, const Transform& boxTransform, RigidBody* boxRigidBody, const PlaneCollider& plane, CollisionData& data)
{
	// Make sure we have contacts
	if (data.contactsLeft <= 0) return 0;

	// Check for intersection
	if (!IntersectionTests::BoxAndHalfSpace(box, boxTransform, plane))
	{
		return 0;
	}

	// We have an intersection, so find the intersection points. We can make
	// do with only checking vertices. If the box is resting on a plane
	// or on an edge, it will be reported as four or two contact points.

	// Go through each combination of + and - for each half-size
	static float mults[8][3] = { {1,1,1},{-1,1,1},{1,-1,1},{-1,-1,1},{1,1,-1},{-1,1,-1},{1,-1,-1},{-1,-1,-1} };

	int contactsUsed = 0;
	for (int i = 0; i < 8; i++)
	{
		// Calculate the position of each vertex
		glm::vec3 vertexPos (mults[i][0], mults[i][1], mults[i][2]);
		vertexPos.x *= box.halfSize.x;
		vertexPos.y *= box.halfSize.y;
		vertexPos.z *= box.halfSize.z;
		//vertexPos = box.trans.transform(vertexPos);
		

		vertexPos = TransformVector(Transform::ToMatrixGLM(boxTransform), vertexPos);

		// Calculate the distance from the plane
		float vertexDistance = glm::dot(vertexPos, plane.normal);

		// Compare this to the plane's distance
		if (vertexDistance <= plane.offset)
		{
			// Create the contact data.

			// The contact point is halfway between the vertex and the
			// plane - we multiply the direction by half the separation
			// distance and add the vertex location.
			data.contactArray[data.currentContact].contactPoint = plane.normal;
			data.contactArray[data.currentContact].contactPoint *= (vertexDistance - plane.offset);
			data.contactArray[data.currentContact].contactPoint += vertexPos;
			data.contactArray[data.currentContact].contactNormal = plane.normal;
			data.contactArray[data.currentContact].penetration = plane.offset - vertexDistance;

			// Write the appropriate data
			data.contactArray[data.currentContact].setBodyData(boxRigidBody, NULL, data.friction, data.restitution);

			// Move onto the next contact
			data.addContacts(1);
			contactsUsed++;

			if (data.contactsLeft == 0)
				return contactsUsed;
		}
	}

	return contactsUsed;
}

static float transformToAxis(const BoxCollider& box, const Transform& boxTransform, const glm::vec3& axis)
{
	return
		box.halfSize.x * abs(glm::dot(axis, Transform::GetAxisVector(boxTransform, 0))) +
		box.halfSize.y * abs(glm::dot(axis, Transform::GetAxisVector(boxTransform, 1))) +
		box.halfSize.z * abs(glm::dot(axis, Transform::GetAxisVector(boxTransform, 2)));
}

static float penetrationOnAxis(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform, const glm::vec3& axis, const glm::vec3& toCentre)
{
	// Project the half-size of one onto axis
	float oneProject = transformToAxis(one, oneTransform, axis);
	float twoProject = transformToAxis(two, twoTransform, axis);

	// Project this onto the axis
	float distance = abs(glm::dot(toCentre, axis));

	// Return the overlap (i.e. positive indicates
	// overlap, negative indicates separation).
	return oneProject + twoProject - distance;
}



static inline bool tryAxis(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform, glm::vec3 axis, const glm::vec3& toCentre, unsigned index, float& smallestPenetration, unsigned& smallestCase)
{
	// Make sure we have a normalized axis, and don't check almost parallel axes
	if (glm::length2(axis) < 0.0001) return true;
	axis = Normalize(axis);

	float penetration = penetrationOnAxis(one, oneTransform, two, twoTransform, axis, toCentre);

	if (penetration < 0) return false;
	if (penetration < smallestPenetration)
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}
	return true;
}

static void fillPointFaceBoxBox(const BoxCollider& one, const Transform& oneTransform, RigidBody* oneRigidBody, const BoxCollider& two, const Transform& twoTransform, RigidBody* twoRigidBody, const glm::vec3& toCentre, CollisionData& data, int best, float pen)
{
	// We know which axis the collision is on (i.e. best),
	// but we need to work out which of the two faces on
	// this axis.
	glm::vec3 normal = Transform::GetAxisVector(oneTransform, best);
	if (glm::dot(Transform::GetAxisVector(oneTransform, best), toCentre) > 0)
	{
		normal = normal * -1.0f;
	}

	// Work out which vertex of box two we're colliding with.
	// Using toCentre doesn't work!
	glm::vec3 vertex = two.halfSize;
	if (glm::dot(Transform::GetAxisVector(twoTransform, 0), normal) < 0) vertex.x = -vertex.x;
	if (glm::dot(Transform::GetAxisVector(twoTransform, 1), normal) < 0) vertex.y = -vertex.y;
	if (glm::dot(Transform::GetAxisVector(twoTransform, 2), normal) < 0) vertex.z = -vertex.z;

	// Create the contact data
	data.contactArray[data.currentContact].contactNormal = normal;
	data.contactArray[data.currentContact].penetration = pen;
	data.contactArray[data.currentContact].contactPoint = TransformVector(Transform::ToMatrixGLM(twoTransform), vertex);
	data.contactArray[data.currentContact].setBodyData(oneRigidBody, twoRigidBody, data.friction, data.restitution);
}

static glm::vec3 contactPoint(glm::vec3 pOne, glm::vec3 dOne, float oneSize, glm::vec3 pTwo, glm::vec3 dTwo, float twoSize, bool useOne)
{
	glm::vec3 toSt, cOne, cTwo;
	float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	float denom, mua, mub;

	smOne = glm::length2 (dOne);
	smTwo = glm::length2 (dTwo);
	dpOneTwo = glm::dot(dTwo, dOne);

	toSt = pOne - pTwo;
	dpStaOne = glm::dot(dOne, toSt);
	dpStaTwo = glm::dot(dTwo, toSt);

	denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	// Zero denominator indicates parrallel lines
	if (abs(denom) < 0.0001f)
	{
		return useOne ? pOne : pTwo;
	}

	mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	// If either of the edges has the nearest point out
	// of bounds, then the edges aren't crossed, we have
	// an edge-face contact. Our point is on the edge, which
	// we know from the useOne parameter.
	if (mua > oneSize || mua< -oneSize || mub> twoSize || mub < -twoSize)
	{
		return useOne ? pOne : pTwo;
	}
	else
	{
		cOne = pOne + dOne * mua;
		cTwo = pTwo + dTwo * mub;

		return cOne * 0.5f + cTwo * 0.5f;
	}
}

unsigned CollisionDetector::BoxAndBox(const BoxCollider& one, const Transform& oneTransform, RigidBody* oneRigidBody, const BoxCollider& two, const Transform& twoTransform, RigidBody* twoRigidBody, CollisionData& data)
{
	// Find the vector between the two centres
	glm::vec3 toCentre = Transform::GetAxisVector(twoTransform, 3) - Transform::GetAxisVector(oneTransform, 3);

	//std::cout << toCentre.x << "   " << toCentre.y << "   " << toCentre.z << std::endl;

	// We start assuming there is no contact
	float pen = INFINITY;
	unsigned best = 0xffffff;

	// Now we check each axes, returning if it gives us
	// a separating axis, and keeping track of the axis with
	// the smallest penetration otherwise.
	if (!tryAxis(one, oneTransform, two, twoTransform, Transform::GetAxisVector(oneTransform, 0), toCentre, (0), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, Transform::GetAxisVector(oneTransform, 1), toCentre, (1), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, Transform::GetAxisVector(oneTransform, 2), toCentre, (2), pen, best)) return 0;

	if (!tryAxis(one, oneTransform, two, twoTransform, Transform::GetAxisVector(oneTransform, 0), toCentre, (3), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, Transform::GetAxisVector(oneTransform, 1), toCentre, (4), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, Transform::GetAxisVector(oneTransform, 2), toCentre, (5), pen, best)) return 0;

	// Store the best axis-major, in case we run into almost
	// parallel edge collisions later
	int bestSingleAxis = best;

	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 0), Transform::GetAxisVector(twoTransform, 0)), toCentre, (6), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 0), Transform::GetAxisVector(twoTransform, 1)), toCentre, (7), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 0), Transform::GetAxisVector(twoTransform, 2)), toCentre, (8), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 1), Transform::GetAxisVector(twoTransform, 0)), toCentre, (9), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 1), Transform::GetAxisVector(twoTransform, 1)), toCentre, (10), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 1), Transform::GetAxisVector(twoTransform, 2)), toCentre, (11), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 2), Transform::GetAxisVector(twoTransform, 0)), toCentre, (12), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 2), Transform::GetAxisVector(twoTransform, 1)), toCentre, (13), pen, best)) return 0;
	if (!tryAxis(one, oneTransform, two, twoTransform, glm::cross(Transform::GetAxisVector(oneTransform, 2), Transform::GetAxisVector(twoTransform, 2)), toCentre, (14), pen, best)) return 0;

	assert(best != 0xffffff);

	// We now know there's a collision, and we know which
	// of the axes gave the smallest penetration. We now
	// can deal with it in different ways depending on
	// the case.
	if (best < 3)
	{
		// We've got a vertex of box two on a face of box one.
		fillPointFaceBoxBox(one, oneTransform, oneRigidBody, two, twoTransform, twoRigidBody, toCentre, data, best, pen);
		data.addContacts(1);
		return 1;
	}
	else if (best < 6)
	{
		// We've got a vertex of box one on a face of box two.
		// We use the same algorithm as above, but swap around
		// one and two (and therefore also the vector between their
		// centres).
		fillPointFaceBoxBox(two, twoTransform, twoRigidBody, one, oneTransform, oneRigidBody, toCentre * -1.0f, data, best - 3, pen);
		data.addContacts(1);
		return 1;
	}
	else
	{
		// We've got an edge-edge contact. Find out which axes
		best -= 6;
		int oneAxisIndex = best / 3;
		int twoAxisIndex = best % 3;
		glm::vec3 oneAxis = Transform::GetAxisVector(oneTransform, oneAxisIndex);
		glm::vec3 twoAxis = Transform::GetAxisVector(twoTransform, twoAxisIndex);
		glm::vec3 axis = glm::cross(oneAxis, twoAxis);
		axis = Normalize(axis);

		// The axis should point from box one to box two.
		if (glm::dot(axis, toCentre) > 0) axis = axis * -1.0f;

		// We have the axes, but not the edges: each axis has 4 edges parallel
		// to it, we need to find which of the 4 for each object. We do
		// that by finding the point in the centre of the edge. We know
		// its component in the direction of the box's collision axis is zero
		// (its a mid-point) and we determine which of the extremes in each
		// of the other axes is closest.
		glm::vec3 ptOnOneEdge = one.halfSize;
		glm::vec3 ptOnTwoEdge = two.halfSize;
		for (int i = 0; i < 3; i++)
		{
			if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
			else if (glm::dot(Transform::GetAxisVector(oneTransform, i), axis) > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

			if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
			else if (glm::dot(Transform::GetAxisVector(twoTransform, i), axis) < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
		}

		// Move them into world coordinates (they are already oriented
		// correctly, since they have been derived from the axes).
		ptOnOneEdge = TransformVector(Transform::ToMatrixGLM(oneTransform), ptOnOneEdge);
		ptOnTwoEdge = TransformVector(Transform::ToMatrixGLM(twoTransform), ptOnTwoEdge);

		// So we have a point and a direction for the colliding edges.
		// We need to find out point of closest approach of the two
		// line-segments.
		glm::vec3 vertex = contactPoint(
			ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
			ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
			bestSingleAxis > 2
		);

		data.contactArray[data.currentContact].penetration = pen;
		data.contactArray[data.currentContact].contactNormal = axis;
		data.contactArray[data.currentContact].contactPoint = vertex;
		data.contactArray[data.currentContact].setBodyData(oneRigidBody, twoRigidBody,
			data.friction, data.restitution);
		data.addContacts(1);
		return 1;
	}

	return 0;
}

unsigned CollisionDetector::BoxAndSphere(const BoxCollider& box, const Transform& boxTransform, RigidBody* boxRigidBody, const SphereCollider& sphere, const Transform& sphereTransform, RigidBody* sphereRigidBody, CollisionData& data)
{
	// Transform the centre of the sphere into box coordinates
	glm::vec3 centre = Transform::GetAxisVector(sphereTransform, 3);

	cyMatrix4 cyMatrix(Transform::ToMatrixGLMNoScale(boxTransform));
	glm::vec3 relCentre = cyMatrix.transformInverse(centre);

	float radius = sphere.radius * glm::compMax(sphereTransform.scale);
	glm::vec3 boxHalfSize(box.halfSize.x * boxTransform.scale.x, box.halfSize.y * boxTransform.scale.y, box.halfSize.z * boxTransform.scale.z);


	// Early out check to see if we can exclude the contact
	if (abs(relCentre.x) - radius > boxHalfSize.x ||
		abs(relCentre.y) - radius > boxHalfSize.y ||
		abs(relCentre.z) - radius > boxHalfSize.z)
	{
		return 0;
	}
	
	glm::vec3 closestPt;
	float dist;

	// Clamp each coordinate to the box.
	dist = relCentre.x;
	if (dist > boxHalfSize.x) dist = boxHalfSize.x;
	if (dist < -boxHalfSize.x) dist = -boxHalfSize.x;
	closestPt.x = dist;

	dist = relCentre.y;
	if (dist > boxHalfSize.y) dist = boxHalfSize.y;
	if (dist < -boxHalfSize.y) dist = -boxHalfSize.y;
	closestPt.y = dist;

	dist = relCentre.z;
	if (dist > boxHalfSize.z) dist = boxHalfSize.z;
	if (dist < -boxHalfSize.z) dist = -boxHalfSize.z;
	closestPt.z = dist;

	// Check we're in contact
	dist = glm::length2 (closestPt - relCentre);
	if (dist > radius * radius) return 0;

	// Compile the contact
	glm::vec3 closestPtWorld = TransformVector(Transform::ToMatrixGLMNoScale(boxTransform), closestPt);

	/*glm::vec3 temp = glm::normalize(closestPtWorld - centre);
	glm::vec3 temp2 = closestPtWorld - centre;
	std::cout << "cptw | " << closestPtWorld.x << ", " << closestPtWorld.y << ", " << closestPtWorld.z << std::endl;
	std::cout << "cntr | " << centre.x << ", " << centre.y << ", " << centre.z << std::endl;
	std::cout << "tmp2 | " << temp2.x << ", " << temp2.y << ", " << temp2.z << std::endl;
	std::cout << "temp | " << temp.x << ", " << temp.y << ", " << temp.z << std::endl;*/

	data.contactArray[data.currentContact].contactNormal = Normalize(closestPtWorld - centre);
	data.contactArray[data.currentContact].contactPoint = closestPtWorld;
	data.contactArray[data.currentContact].penetration = radius - sqrt(dist);
	data.contactArray[data.currentContact].setBodyData(boxRigidBody, sphereRigidBody, data.friction, data.restitution);

	data.addContacts(1);
	
	return 1;
}