print ("start")


function Update(entity)
	t = GetTransform(entity)

	if Input.GetKey('A') then
		t.position.x = t.position.x + 0.1
	end

	if Input.GetKey('D') then
		t.position.x = t.position.x - 0.1
	end

	if Input.GetKey('Q') then
		t.position.y = t.position.y + 0.1
	end

	if Input.GetKey('R') then
		t.position.y = t.position.y - 0.1
	end

	if Input.GetKey('W') then
		t.position.z = t.position.z + 0.1
	end

	if Input.GetKey('S') then
		t.position.z = t.position.z - 0.1
	end
	
end

print ("end")