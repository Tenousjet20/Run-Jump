#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include "AABB.h"

#define LEVEL_WIDTH 23
#define LEVEL_HEIGHT 40
#define TILE_SIZE 32

#define MAX_LEVELS 5

struct Level
{
	std::vector<AABB> tiles;
	std::vector<AABB> spikes;
	AABB flag;
	AABB coin;
	uint32_t level;
	int coins_collected[MAX_LEVELS];

	void initialize();

	void load_level_data(uint32_t level, glm::vec2& player_position, glm::vec2& player_velocity);
};