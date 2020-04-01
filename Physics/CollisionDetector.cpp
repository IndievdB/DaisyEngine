#include "CollisionDetector.hpp"
#include "IntersectionTests.hpp"

unsigned CollisionDetector::SphereAndPlane(const std::shared_ptr<entt::registry> registry, const entt::entity& sphere, const entt::entity& plane, CollisionData& data)
{
	SphereCollider& sphereCollider = registry->get<SphereCollider>(sphere);
	Transform& sphereTransform = registry->get<Transform>(sphere);
	RigidBody* sphereRigidBody = registry->try_get<RigidBody>(sphere);

	PlaneCollider& planeCollider = registry->get<PlaneCollider>(plane);
	RigidBody* planeRigidBody = registry->try_get<RigidBody>(plane);
	
	if (sphereCollider.isTrigger || planeCollider.isTrigger)
	{
		if (IntersectionTests::SphereAndHalfSpace(sphereCollider, sphereTransform, planeCollider))
		{
			sphereCollider.triggerData.Add(plane);
			planeCollider.triggerData.Add(sphere);
		}

		return 0;
	}

	if (sphereRigidBody == nullptr) return 0;

	// Make sure we have contacts
	if (data.contactsLeft <= 0) return 0;

	float radius = sphereCollider.radius * sphereTransform.scale.MaxComponent();

	// Find the distance from the plane
	float ballDistance = Vector3::Dot(planeCollider.normal, sphereTransform.position) - radius - planeCollider.offset;

	if (ballDistance >= 0) return 0;

	// Create the contact - it has a normal in the plane direction.

	data.contactArray[data.currentContact].contactNormal = planeCollider.normal;
	data.contactArray[data.currentContact].penetration = -ballDistance;
	data.contactArray[data.currentContact].contactPoint = sphereTransform.position - planeCollider.normal * (ballDistance + sphereCollider.radius);
	data.contactArray[data.currentContact].setBodyData(sphereRigidBody, &sphereTransform, nullptr, nullptr, data.friction, data.restitution);

	data.addContacts(1);

	return 1;
}

unsigned CollisionDetector::SphereAndSphere(const std::shared_ptr<entt::registry> registry, const entt::entity& one, const entt::entity& two, CollisionData& data)
{
	SphereCollider& oneCollider = registry->get<SphereCollider>(one);
	Transform& oneTransform = registry->get<Transform>(one);
	RigidBody* oneRigidBody = registry->try_get<RigidBody>(one);

	SphereCollider& twoCollider = registry->get<SphereCollider>(two);
	Transform& twoTransform = registry->get<Transform>(two);
	RigidBody* twoRigidBody = registry->try_get<RigidBody>(two);

	if (oneCollider.isTrigger || twoCollider.isTrigger)
	{
		if (IntersectionTests::SphereAndSphere(oneCollider, oneTransform, twoCollider, twoTransform))
		{
			oneCollider.triggerData.Add(two);
			twoCollider.triggerData.Add(one);
		}

		return 0;
	}

	if (oneRigidBody == nullptr && twoRigidBody == nullptr) return 0;

	// Make sure we have contacts
	if (data.contactsLeft <= 0) return 0;

	float radiusOne = oneCollider.radius * oneTransform.scale.MaxComponent();
	float radiusTwo = twoCollider.radius * twoTransform.scale.MaxComponent();

	// Find the vector between the objects
	Vector3 midline = oneTransform.position - twoTransform.position;
	float size = midline.Length();

	// See if it is large enough.
	if (size <= 0.0f || size >= radiusOne + radiusTwo)
	{
		return 0;
	}

	// We manually create the normal, because we have the
	// size to hand.
	Vector3 normal = midline * (1.0f / size);

	data.contactArray[data.currentContact].contactNormal = normal;
	data.contactArray[data.currentContact].contactPoint = oneTransform.position + midline * 0.5f;
	data.contactArray[data.currentContact].penetration = (radiusOne + radiusTwo - size);
	data.contactArray[data.currentContact].setBodyData(oneRigidBody, &oneTransform, twoRigidBody, &twoTransform, data.friction, data.restitution);

	data.addContacts(1);

	return 1;
}

unsigned CollisionDetector::BoxAndPlane(const std::shared_ptr<entt::registry> registry, const entt::entity& box, const entt::entity& plane, CollisionData& data)
{
	BoxCollider& boxCollider = registry->get<BoxCollider>(box);
	Transform& boxTransform = registry->get<Transform>(box);
	RigidBody* boxRigidBody = registry->try_get<RigidBody>(box);

	PlaneCollider& planeCollider = registry->get<PlaneCollider>(plane);
	RigidBody* planeRigidBody = registry->try_get<RigidBody>(plane);

	if (boxCollider.isTrigger || planeCollider.isTrigger)
	{
		if (IntersectionTests::BoxAndHalfSpace(boxCollider, boxTransform, planeCollider))
		{
			boxCollider.triggerData.Add(plane);
			planeCollider.triggerData.Add(box);
		}

		return 0;
	}

	if (boxRigidBody == nullptr) return 0;

	// Make sure we have contacts
	if (data.contactsLeft <= 0) return 0;

	// Check for intersection
	if (!IntersectionTests::BoxAndHalfSpace(boxCollider, boxTransform, planeCollider))
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
		Vector3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
		vertexPos.x *= boxCollider.halfSize.x;
		vertexPos.y *= boxCollider.halfSize.y;
		vertexPos.z *= boxCollider.halfSize.z;
		//vertexPos = box.trans.transform(vertexPos);

		vertexPos = Matrix4x4::Transformation(boxTransform).TransformPoint(vertexPos);

		// Calculate the distance from the plane
		float vertexDistance = Vector3::Dot(vertexPos, planeCollider.normal);

		// Compare this to the plane's distance
		if (vertexDistance <= planeCollider.offset)
		{
			// Create the contact data.

			// The contact point is halfway between the vertex and the
			// plane - we multiply the direction by half the separation
			// distance and add the vertex location.
			data.contactArray[data.currentContact].contactPoint = planeCollider.normal;
			data.contactArray[data.currentContact].contactPoint *= (vertexDistance - planeCollider.offset);
			data.contactArray[data.currentContact].contactPoint += vertexPos;
			data.contactArray[data.currentContact].contactNormal = planeCollider.normal;
			data.contactArray[data.currentContact].penetration = planeCollider.offset - vertexDistance;

			// Write the appropriate data
			data.contactArray[data.currentContact].setBodyData(boxRigidBody, &boxTransform, nullptr, nullptr, data.friction, data.restitution);

			// Move onto the next contact
			data.addContacts(1);
			contactsUsed++;

			if (data.contactsLeft == 0)
				return contactsUsed;
		}
	}

	return contactsUsed;
}

static float transformToAxis(const BoxCollider& box, const Transform& boxTransform, const Vector3& axis)
{
	return
		box.halfSize.x * abs(Vector3::Dot(axis, Matrix4x4::Transformation(boxTransform).GetColumn(0))) +
		box.halfSize.y * abs(Vector3::Dot(axis, Matrix4x4::Transformation(boxTransform).GetColumn(1))) +
		box.halfSize.z * abs(Vector3::Dot(axis, Matrix4x4::Transformation(boxTransform).GetColumn(2)));
}

static float penetrationOnAxis(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform, const Vector3& axis, const Vector3& toCentre)
{
	// Project the half-size of one onto axis
	float oneProject = transformToAxis(one, oneTransform, axis);
	float twoProject = transformToAxis(two, twoTransform, axis);

	// Project this onto the axis
	float distance = abs(Vector3::Dot(toCentre, axis));

	// Return the overlap (i.e. positive indicates
	// overlap, negative indicates separation).
	return oneProject + twoProject - distance;
}

static inline bool tryAxis(const BoxCollider& one, const Transform& oneTransform, const BoxCollider& two, const Transform& twoTransform, Vector3 axis, const Vector3& toCentre, unsigned index, float& smallestPenetration, unsigned& smallestCase)
{
	// Make sure we have a normalized axis, and don't check almost parallel axes
	if (axis.LengthSquared() < 0.0001) return true;
	axis.Normalize();

	float penetration = penetrationOnAxis(one, oneTransform, two, twoTransform, axis, toCentre);

	if (penetration < 0) return false;
	if (penetration < smallestPenetration)
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}
	return true;
}

static void fillPointFaceBoxBox(const BoxCollider& one, Transform& oneTransform, RigidBody* oneRigidBody, const BoxCollider& two, Transform& twoTransform, RigidBody* twoRigidBody, const Vector3& toCentre, CollisionData& data, int best, float pen)
{
	// We know which axis the collision is on (i.e. best),
	// but we need to work out which of the two faces on
	// this axis.
	Vector3 normal = Matrix4x4::Transformation(oneTransform).GetColumn(best);
	if (Vector3::Dot(Matrix4x4::Transformation(oneTransform).GetColumn(best), toCentre) > 0)
	{
		normal = normal * -1.0f;
	}

	// Work out which vertex of box two we're colliding with.
	// Using toCentre doesn't work!
	Vector3 vertex = two.halfSize;
	if (Vector3::Dot(Matrix4x4::Transformation(twoTransform).GetColumn(0), normal) < 0) vertex.x = -vertex.x;
	if (Vector3::Dot(Matrix4x4::Transformation(twoTransform).GetColumn(1), normal) < 0) vertex.y = -vertex.y;
	if (Vector3::Dot(Matrix4x4::Transformation(twoTransform).GetColumn(2), normal) < 0) vertex.z = -vertex.z;

	// Create the contact data
	data.contactArray[data.currentContact].contactNormal = normal;
	data.contactArray[data.currentContact].penetration = pen;
	data.contactArray[data.currentContact].contactPoint = Matrix4x4::Transformation(twoTransform).TransformPoint(vertex);
	data.contactArray[data.currentContact].setBodyData(oneRigidBody, &oneTransform, twoRigidBody, &twoTransform, data.friction, data.restitution);
}

static Vector3 contactPoint(Vector3 pOne, Vector3 dOne, float oneSize, Vector3 pTwo, Vector3 dTwo, float twoSize, bool useOne)
{
	Vector3 toSt, cOne, cTwo;
	float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	float denom, mua, mub;

	smOne = dOne.LengthSquared();
	smTwo = dTwo.LengthSquared();
	dpOneTwo = Vector3::Dot(dTwo, dOne);

	toSt = pOne - pTwo;
	dpStaOne = Vector3::Dot(dOne, toSt);
	dpStaTwo = Vector3::Dot(dTwo, toSt);

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

unsigned CollisionDetector::BoxAndBox(const std::shared_ptr<entt::registry> registry, const entt::entity& one, const entt::entity& two, CollisionData& data)
{
	BoxCollider& oneCollider = registry->get<BoxCollider>(one);
	Transform& oneTransform = registry->get<Transform>(one);
	RigidBody* oneRigidBody = registry->try_get<RigidBody>(one);

	BoxCollider& twoCollider = registry->get<BoxCollider>(two);
	Transform& twoTransform = registry->get<Transform>(two);
	RigidBody* twoRigidBody = registry->try_get<RigidBody>(two);

	if (oneCollider.isTrigger || twoCollider.isTrigger)
	{
		if (IntersectionTests::BoxAndBox(oneCollider, oneTransform, twoCollider, twoTransform))
		{
			oneCollider.triggerData.Add(two);
			twoCollider.triggerData.Add(one);
		}

		return 0;
	}

	if (oneRigidBody == nullptr && twoRigidBody == nullptr) return 0;

	if (data.contactsLeft <= 0) return 0;

	// Find the vector between the two centres
	Vector3 toCentre = twoTransform.position - oneTransform.position;

	//std::cout << toCentre.x << "   " << toCentre.y << "   " << toCentre.z << std::endl;

	// We start assuming there is no contact
	float pen = INFINITY;
	unsigned best = 0xffffff;

	// Now we check each axes, returning if it gives us
	// a separating axis, and keeping track of the axis with
	// the smallest penetration otherwise.
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Matrix4x4::Transformation(oneTransform).GetColumn(0), toCentre, (0), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Matrix4x4::Transformation(oneTransform).GetColumn(1), toCentre, (1), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Matrix4x4::Transformation(oneTransform).GetColumn(2), toCentre, (2), pen, best)) return 0;

	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Matrix4x4::Transformation(oneTransform).GetColumn(0), toCentre, (3), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Matrix4x4::Transformation(oneTransform).GetColumn(1), toCentre, (4), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Matrix4x4::Transformation(oneTransform).GetColumn(2), toCentre, (5), pen, best)) return 0;

	// Store the best axis-major, in case we run into almost
	// parallel edge collisions later
	int bestSingleAxis = best;

	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(0), Matrix4x4::Transformation(twoTransform).GetColumn(0)), toCentre, (6), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(0), Matrix4x4::Transformation(twoTransform).GetColumn(1)), toCentre, (7), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(0), Matrix4x4::Transformation(twoTransform).GetColumn(2)), toCentre, (8), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(1), Matrix4x4::Transformation(twoTransform).GetColumn(0)), toCentre, (9), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(1), Matrix4x4::Transformation(twoTransform).GetColumn(1)), toCentre, (10), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(1), Matrix4x4::Transformation(twoTransform).GetColumn(2)), toCentre, (11), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(2), Matrix4x4::Transformation(twoTransform).GetColumn(0)), toCentre, (12), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(2), Matrix4x4::Transformation(twoTransform).GetColumn(1)), toCentre, (13), pen, best)) return 0;
	if (!tryAxis(oneCollider, oneTransform, twoCollider, twoTransform, Vector3::Cross(Matrix4x4::Transformation(oneTransform).GetColumn(2), Matrix4x4::Transformation(twoTransform).GetColumn(2)), toCentre, (14), pen, best)) return 0;

	// We now know there's a collision, and we know which
	// of the axes gave the smallest penetration. We now
	// can deal with it in different ways depending on
	// the case.
	if (best < 3)
	{
		// We've got a vertex of box two on a face of box one.
		fillPointFaceBoxBox(oneCollider, oneTransform, oneRigidBody, twoCollider, twoTransform, twoRigidBody, toCentre, data, best, pen);
		data.addContacts(1);
		return 1;
	}
	else if (best < 6)
	{
		// We've got a vertex of box one on a face of box two.
		// We use the same algorithm as above, but swap around
		// one and two (and therefore also the vector between their
		// centres).
		fillPointFaceBoxBox(twoCollider, twoTransform, twoRigidBody, oneCollider, oneTransform, oneRigidBody, toCentre * -1.0f, data, best - 3, pen);
		data.addContacts(1);
		return 1;
	}
	else
	{
		// We've got an edge-edge contact. Find out which axes
		best -= 6;
		int oneAxisIndex = best / 3;
		int twoAxisIndex = best % 3;
		Vector3 oneAxis = Matrix4x4::Transformation(oneTransform).GetColumn(oneAxisIndex);
		Vector3 twoAxis = Matrix4x4::Transformation(twoTransform).GetColumn(twoAxisIndex);
		Vector3 axis = Vector3::Cross(oneAxis, twoAxis);
		axis.Normalize();

		// The axis should point from box one to box two.
		if (Vector3::Dot(axis, toCentre) > 0) axis = axis * -1.0f;

		// We have the axes, but not the edges: each axis has 4 edges parallel
		// to it, we need to find which of the 4 for each object. We do
		// that by finding the point in the centre of the edge. We know
		// its component in the direction of the box's collision axis is zero
		// (its a mid-point) and we determine which of the extremes in each
		// of the other axes is closest.
		Vector3 ptOnOneEdge = oneCollider.halfSize;
		Vector3 ptOnTwoEdge = twoCollider.halfSize;
		for (int i = 0; i < 3; i++)
		{
			if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
			else if (Vector3::Dot(Matrix4x4::Transformation(oneTransform).GetColumn(i), axis) > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

			if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
			else if (Vector3::Dot(Matrix4x4::Transformation(twoTransform).GetColumn(i), axis) < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
		}

		// Move them into world coordinates (they are already oriented
		// correctly, since they have been derived from the axes).
		ptOnOneEdge = Matrix4x4::Transformation(oneTransform).TransformPoint(ptOnOneEdge);
		ptOnTwoEdge = Matrix4x4::Transformation(twoTransform).TransformPoint(ptOnTwoEdge);

		// So we have a point and a direction for the colliding edges.
		// We need to find out point of closest approach of the two
		// line-segments.
		Vector3 vertex = contactPoint(
			ptOnOneEdge, oneAxis, oneCollider.halfSize[oneAxisIndex],
			ptOnTwoEdge, twoAxis, twoCollider.halfSize[twoAxisIndex],
			bestSingleAxis > 2
		);

		data.contactArray[data.currentContact].penetration = pen;
		data.contactArray[data.currentContact].contactNormal = axis;
		data.contactArray[data.currentContact].contactPoint = vertex;
		data.contactArray[data.currentContact].setBodyData(oneRigidBody, &oneTransform, twoRigidBody, &twoTransform, data.friction, data.restitution);
		data.addContacts(1);
		return 1;
	}

	return 0;
}

unsigned CollisionDetector::BoxAndSphere(const std::shared_ptr<entt::registry> registry, const entt::entity& box, const entt::entity& sphere, CollisionData& data)
{
	BoxCollider& boxCollider = registry->get<BoxCollider>(box);
	Transform& boxTransform = registry->get<Transform>(box);
	RigidBody* boxRigidBody = registry->try_get<RigidBody>(box);

	SphereCollider& sphereCollider = registry->get<SphereCollider>(sphere);
	Transform& sphereTransform = registry->get<Transform>(sphere);
	RigidBody* sphereRigidBody = registry->try_get<RigidBody>(sphere);

	if (boxCollider.isTrigger || sphereCollider.isTrigger)
	{
		if (IntersectionTests::BoxAndSphere(boxCollider, boxTransform, sphereCollider, sphereTransform))
		{
			boxCollider.triggerData.Add(sphere);
			sphereCollider.triggerData.Add(box);
		}

		return 0;
	}

	if (boxRigidBody == nullptr && sphereRigidBody == nullptr) return 0;

	// Transform the centre of the sphere into box coordinates
	Vector3 relCentre = Matrix4x4::AffineInverse(Matrix4x4::Transformation(boxTransform.position, boxTransform.rotation)).TransformPoint(sphereTransform.position);

	float radius = sphereCollider.radius * sphereTransform.scale.MaxComponent();
	Vector3 boxHalfSize(boxCollider.halfSize.x * boxTransform.scale.x, boxCollider.halfSize.y * boxTransform.scale.y, boxCollider.halfSize.z * boxTransform.scale.z);

	// Early out check to see if we can exclude the contact
	if (abs(relCentre.x) - radius > boxHalfSize.x ||
		abs(relCentre.y) - radius > boxHalfSize.y ||
		abs(relCentre.z) - radius > boxHalfSize.z)
	{
		return 0;
	}

	Vector3 closestPt;
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
	dist = (closestPt - relCentre).LengthSquared();
	if (dist > radius * radius) return 0;

	// Compile the contact
	Vector3 closestPtWorld = Matrix4x4::Transformation(boxTransform.position, boxTransform.rotation).TransformPoint(closestPt);

	data.contactArray[data.currentContact].contactNormal = (closestPtWorld - sphereTransform.position).Normalized();
	data.contactArray[data.currentContact].contactPoint = closestPtWorld;
	data.contactArray[data.currentContact].penetration = radius - sqrt(dist);
	data.contactArray[data.currentContact].setBodyData(boxRigidBody, &boxTransform, sphereRigidBody, &sphereTransform, data.friction, data.restitution);

	data.addContacts(1);

	return 1;
}