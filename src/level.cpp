#include "Level.h"

void Level::initialize()
{
	tiles.reserve(LEVEL_WIDTH * LEVEL_HEIGHT);
	spikes.reserve(1);

	level = 6;

	for (int i = 0; i < MAX_LEVELS; i++)
	{
		coins_collected[i] = 0;
	}
}

void Level::load_level_data(uint32_t level, glm::vec2& player_position, glm::vec2& player_velocity)
{
	player_velocity = glm::vec2(0, 0);

	tiles.clear();
	spikes.clear();
	flag.attach(glm::vec2(-1 * TILE_SIZE, 0));
	coin.attach(glm::vec2(-1 * TILE_SIZE, 0));

	std::ifstream file(RESOURCES_PATH "levels/level" + std::to_string(level) + ".txt");

	std::string text;

	float y = 1.25;

	while (getline(file, text))
	{
		y--;

		for (int x = 0; x < text.length(); x++)
		{
			if (text[x] == '#')
			{
				AABB aabb;
				aabb.initialize(glm::vec2(x * TILE_SIZE, y * TILE_SIZE), glm::vec2(TILE_SIZE));
				tiles.emplace_back(aabb);
			}
			else if (text[x] == 'p')
			{
				player_velocity = glm::vec2(0, 0);
				player_position = glm::vec2(x * TILE_SIZE, y * TILE_SIZE + (TILE_SIZE / 2));
			}
			else if (text[x] == 'f')
			{
				flag.attach(glm::vec2(x * TILE_SIZE + (TILE_SIZE / 2), y * TILE_SIZE));
			}
			else if (text[x] == 's')
			{
				AABB aabb;
				aabb.initialize(glm::vec2(x * TILE_SIZE, y * TILE_SIZE - (TILE_SIZE / 2)), glm::vec2(TILE_SIZE));
				spikes.emplace_back(aabb);
			}
			else if (text[x] == 'c')
			{
				for (int i = 0; i < MAX_LEVELS; i++)
				{
					if (i == level - 1)
					{
						if (coins_collected[i] == 0)
						{
							coin.attach(glm::vec2(x * TILE_SIZE + (coin.size.x / 2), y * TILE_SIZE - (coin.size.y / 2)));
						}
					}
				}
			}
		}
	}

	file.close();
}