#pragma once

#include <glm/glm.hpp>
#include <algorithm>

struct AABB
{
	glm::vec2 position;
	glm::vec2 size;
	glm::vec2 normal;

	void initialize(glm::vec2 p, glm::vec2 s);

	void attach(glm::vec2 p);

	bool AABB_overlap(AABB& other);

	bool ray_vs_AABB(glm::vec2 origin, AABB& other, glm::vec2& vel, glm::vec2& cp, glm::vec2& cn, float& ct);

	bool dynamic_AABB_vs_AABB(AABB& other, glm::vec2& vel, glm::vec2& cp, glm::vec2& cn, float& ct, float delta_time);

	void resolve_collision(AABB& aabb, glm::vec2& vel, float delta_time);
};