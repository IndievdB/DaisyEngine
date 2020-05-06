bool PointInSphere(vec3 cameraPos, vec4 light, const float nearPlane, const float farPlane)
{
	return distance(cameraPos, light.xyz) <= abs(light.w * (light.z * (farPlane - nearPlane)));
}

//Checks if a sphere intersects or is inside a given frustum
bool SphereCubeColliding(vec4 frustum[6], vec4 sphere)
{
	float c;
	c = min(dot(sphere.xyz, frustum[0].xyz) + frustum[0].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[1].xyz) + frustum[1].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[2].xyz) + frustum[2].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[3].xyz) + frustum[3].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[4].xyz) + frustum[4].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[5].xyz) + frustum[5].w + sphere.w,	 0.0);

	return c == 0.0;
}

bool CollideTest(vec4 frustum[6], vec4 sphere, float zRadius)
{
	bool withinLeftBounds = (sphere.x + sphere.w >= frustum[0].w);
	bool withinRightBounds = (sphere.x - sphere.w <= frustum[1].w);

	bool withinTopBounds = (sphere.y + sphere.w >= frustum[4].w);
	bool withinBottomBounds = (sphere.y - sphere.w <= frustum[5].w);

	bool withinFrontBounds = (sphere.z + zRadius >= frustum[2].w);
	bool withinBackBounds = (sphere.z - zRadius <= frustum[3].w);

	//

	bool withinLeftBoundsINV = (-(sphere.x + sphere.w) >= frustum[0].w);
	bool withinRightBoundsINV = (-(sphere.x - sphere.w) <= frustum[1].w);

	bool withinTopBoundsINV = (-(sphere.y + sphere.w) >= frustum[4].w);
	bool withinBottomBoundsINV = (-(sphere.y - sphere.w) <= frustum[5].w);
	
	//

	return (withinLeftBounds || withinLeftBoundsINV) && (withinRightBounds || withinRightBoundsINV) &&
	(withinTopBounds || withinTopBoundsINV) && (withinBottomBounds || withinBottomBoundsINV) &&
	withinFrontBounds && withinBackBounds;
}

bool CollideTest2(float left, float right, float bottom, float top, float front, float back, vec4 sphere, float zRadius)
{
	bool withinLeftBounds = (sphere.x + sphere.w >= left);
	bool withinRightBounds = (sphere.x - sphere.w <= right);

	bool withinTopBounds = (sphere.y + sphere.w >= top);
	bool withinBottomBounds = (sphere.y - sphere.w <= bottom);

	bool withinFrontBounds = (sphere.z + zRadius >= front);
	bool withinBackBounds = (sphere.z - zRadius <= back);

	//

	bool withinLeftBoundsINV = (-(sphere.x + sphere.w) >= left);
	bool withinRightBoundsINV = (-(sphere.x - sphere.w) <= right);

	bool withinTopBoundsINV = (-(sphere.y + sphere.w) >= top);
	bool withinBottomBoundsINV = (-(sphere.y - sphere.w) <= bottom);
	
	//

	return (withinLeftBounds || withinLeftBoundsINV) && (withinRightBounds || withinRightBoundsINV) &&
	(withinTopBounds || withinTopBoundsINV) && (withinBottomBounds || withinBottomBoundsINV) &&
	withinFrontBounds && withinBackBounds;
}

//Builds a normalized plane from view projection matrix
vec4 BuildPlane(const mat4 mtx, int row, float sign)
{
	vec3 normal = vec3(
		sign * mtx[0][row] + mtx[0][3], //x
		sign * mtx[1][row] + mtx[1][3],	//y
		sign * mtx[2][row] + mtx[2][3]  //z
	);

	float dist = sign * mtx[3][row] + mtx[3][3];

	//catches exception but extra branching
	//	float len = length(normal)
	//	if (len <= 0.0) return vec4(0,0,0,0);

	//direct calc - assumes normal is never of length zero!
	float invlen = 1.f / length(normal);
	return vec4(normal * invlen, dist * invlen);
}

//Builds a world space view frustum from viewProj matrix (Taken from Richard's nclgl library)
void FrustumFromMatrix(const mat4 mtx, out vec4 splanesp[6])
{
	splanesp[0] = BuildPlane(mtx, 0, 1.0); //X+
	splanesp[1] = BuildPlane(mtx, 0, -1.0); //X-
	splanesp[2] = BuildPlane(mtx, 1, 1.0); //Y+
	splanesp[3] = BuildPlane(mtx, 1, -1.0); //Y-
	splanesp[4] = BuildPlane(mtx, 2, 1.0); //Z+
	splanesp[5] = BuildPlane(mtx, 2, -1.0); //Z-
}