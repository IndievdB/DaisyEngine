#pragma once

class Editor;

class EditorWindow
{
public:
	virtual void Update() = 0;
	virtual const char* GetName() = 0;
	bool isOpen = true;
	std::shared_ptr<Editor> editor;
};