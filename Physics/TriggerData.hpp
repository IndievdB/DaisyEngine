#pragma once

#include "../Vendor/entt/entt.hpp"
#include <unordered_set>

struct TriggerData
{
	std::unordered_set<entt::entity> currentFrameCollisions;
	std::unordered_set<entt::entity> lastFrameCollisions;

	void Add(const entt::entity& entity)
	{
		currentFrameCollisions.insert(entity);
	}

	void NextFrame()
	{
		lastFrameCollisions = currentFrameCollisions;
		currentFrameCollisions.clear();
	}

	bool IsEmpty()
	{
		return (lastFrameCollisions.empty() && currentFrameCollisions.empty());
	}
};