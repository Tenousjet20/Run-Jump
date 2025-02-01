#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <raudio.h>
#include <cmath>
#include <iostream>

#include "AABB.h"

#define PLAYER_SPEED 200
#define PLAYER_GRAVITY 1000
#define PLAYER_JUMP_FORCE 525

struct Player
{
	glm::vec2 position;
	glm::vec2 velocity;
	AABB aabb;
	bool flip_x;

	Sound jump_sound;

	void initialize(glm::vec2 aabb_size);

	void get_input(GLFWwindow* window, float delta_time);

	void update_velocity(float delta_time);
};