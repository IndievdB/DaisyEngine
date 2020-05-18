#pragma once

#include <string>

struct EntityName
{
	char name[128];

	EntityName(char const nameData[])
	{
		strcpy_s(name, nameData);
	};
};