#include "AABB.h"

void AABB::initialize(glm::vec2 p, glm::vec2 s)
{
	position = p;
	size = s;
	normal = glm::vec2(0, 0);
}

void AABB::attach(glm::vec2 p)
{
	position = p;
}

bool AABB::AABB_overlap(AABB& other)
{
	return (position.x < other.position.x + other.size.x && position.x + size.x > other.position.x && 
		    position.y < other.position.y + other.size.y && position.y + size.y > other.position.y);
}

bool AABB::ray_vs_AABB(glm::vec2 origin, AABB& other, glm::vec2& vel, glm::vec2& cp, glm::vec2& cn, float& ct)
{
	cp = glm::vec2(0.0f, 0.0f);
	cn = glm::vec2(0.0f, 0.0f);

	glm::vec2 invdir = 1.0f / vel;

	glm::vec2 t_near = (other.position - origin) * invdir;
	glm::vec2 t_far = (other.position + other.size - origin) * invdir;

	if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
	if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

	if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

	if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

	ct = std::max(t_near.x, t_near.y);

	float t_hit_far = std::min(t_far.x, t_far.y);

	if (t_hit_far < 0)
		return false;

	cp = origin + ct * vel;

	if (t_near.x > t_near.y)
	{
		if (invdir.x < 0)
			cn = glm::vec2(1, 0);
		else
			cn = glm::vec2(-1, 0);
	}
	else if (t_near.x < t_near.y)
	{
		if (invdir.y < 0)
			cn = glm::vec2(0, 1);
		else
			cn = glm::vec2(0, -1);
	}

	return true;
}

bool AABB::dynamic_AABB_vs_AABB(AABB& other, glm::vec2& vel, glm::vec2& cp, glm::vec2& cn, float& ct, float delta_time)
{
	if (vel.x == 0 && vel.y == 0)
		return false;

	AABB expanded_target;
	expanded_target.position.x = other.position.x - size.x / 2;
	expanded_target.position.y = other.position.y - size.y / 2;
	expanded_target.size.x = other.size.x + size.x;
	expanded_target.size.y = other.size.y + size.y;

	glm::vec2 new_vel = vel * delta_time;
	if (ray_vs_AABB(glm::vec2(position.x + size.x / 2, position.y + size.y / 2), expanded_target, new_vel, cp, cn, ct))
		return (ct >= 0.0f && ct < 1.0f);
	else
		return false;
}

void AABB::resolve_collision(AABB& aabb, glm::vec2& vel, float delta_time)
{
	glm::vec2 cp, cn;
	float ct = 0.0f;

	if (dynamic_AABB_vs_AABB(aabb, vel, cp, cn, ct, delta_time))
	{
		vel += cn * glm::vec2(std::abs(vel.x), std::abs(vel.y)) * (1 - ct);

		normal = cn;
	}
}