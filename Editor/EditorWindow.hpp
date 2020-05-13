#pragma once

class EditorWindow
{
public:
	virtual void Update() = 0;
	virtual const char* GetName() = 0;
	bool isOpen = true;
};