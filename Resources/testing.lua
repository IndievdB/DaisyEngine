temp = 4

function Update()
	v = game.GetVector()
	game.printMessage(v.x)
	game.printMessage(v.y)
	game.printMessage(v.z)
	v.x = 2;
	v.y = temp
end