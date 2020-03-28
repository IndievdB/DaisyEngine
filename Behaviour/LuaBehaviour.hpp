#pragma once

#include <string>

class LuaBehaviour
{
public:
	LuaBehaviour(std::string scriptPath) : scriptPath(scriptPath) {}
	void Update();
private:
	std::string scriptPath;
};