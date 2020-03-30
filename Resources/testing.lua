pitch = 0
yaw = 0

function Update(entity)
	t = GetTransform(entity)

	if Input.GetKey('A') then
		t.position = t.position + t:GetRight() * 0.2
	end

	if Input.GetKey('D') then
		t.position = t.position - t:GetRight() * 0.2
	end

	if Input.GetKey('E') then
		t.position = t.position + t:GetUp() * 0.2
	end

	if Input.GetKey('Q') then
		t.position = t.position - t:GetUp() * 0.2
	end

	if Input.GetKey('W') then
		t.position = t.position + t:GetForward() * 0.2
	end

	if Input.GetKey('S') then
		t.position = t.position - t:GetForward() * 0.2
	end

	pitch = pitch + Input.GetMouseDelta().y * 0.1
	yaw = yaw - Input.GetMouseDelta().x * 0.1

	t:SetRotation(pitch, yaw, 0)
end