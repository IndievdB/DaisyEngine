#pragma once

#include <algorithm>
#include <string>

#include "../Vendor/entt/entt.hpp"

struct EntityName
{
	char name[128];

	EntityName(char const nameData[])
	{
		strcpy_s(name, nameData);
	};

	EntityName(std::shared_ptr<entt::registry> registry, std::string value)
	{
		EntityName::GetUnique(registry, value);
		strcpy_s(name, value.c_str());
	}

	static void GetUnique(std::shared_ptr<entt::registry> registry, std::string& value)
	{
		std::vector<std::string> existingNames;
		registry->view<EntityName>().each([&existingNames](auto& entityName)
		{
			existingNames.push_back(entityName.name);
		});

		if (std::find(existingNames.begin(), existingNames.end(), value) == existingNames.end())
		{
			return;
		}

		int i = 1;
		
		while (std::find(existingNames.begin(), existingNames.end(), value + " " + std::to_string(i)) != existingNames.end())
			i++;
		
		value += " " + std::to_string(i);
	}
};