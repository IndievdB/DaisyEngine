pitch = 0
yaw = 0
roll = 0

function Update(entity)
	t = GetTransform(entity)
	right = t:GetRight()
	up = t:GetUp()
	forward = t:GetForward()

	if Input.GetKey('A') then
		t.position.x = t.position.x + right.x * 0.2
		t.position.y = t.position.y + right.y * 0.2
		t.position.z = t.position.z + right.z * 0.2
	end

	if Input.GetKey('D') then
		t.position.x = t.position.x - right.x * 0.2
		t.position.y = t.position.y - right.y * 0.2
		t.position.z = t.position.z - right.z * 0.2
	end

	if Input.GetKey('Q') then
		t.position.x = t.position.x - up.x * 0.2
		t.position.y = t.position.y - up.y * 0.2
		t.position.z = t.position.z - up.z * 0.2
	end

	if Input.GetKey('E') then
		t.position.x = t.position.x + up.x * 0.2
		t.position.y = t.position.y + up.y * 0.2
		t.position.z = t.position.z + up.z * 0.2
	end

	if Input.GetKey('W') then
		t.position.x = t.position.x + forward.x * 0.2
		t.position.y = t.position.y + forward.y * 0.2
		t.position.z = t.position.z + forward.z * 0.2
	end

	if Input.GetKey('S') then
		t.position.x = t.position.x - forward.x * 0.2
		t.position.y = t.position.y - forward.y * 0.2
		t.position.z = t.position.z - forward.z * 0.2
	end

	pitch = pitch + Input.GetMouseDelta().y * 0.1
	yaw = yaw - Input.GetMouseDelta().x * 0.1

	t:SetRotation(pitch, yaw, roll)
	
end