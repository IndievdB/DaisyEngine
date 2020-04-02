#pragma once

#include "../Vendor/entt/entt.hpp"

class RenderSystem
{
public:
	void RenderAll(std::shared_ptr<entt::registry> registry);
};

