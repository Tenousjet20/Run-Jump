#include "App.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void App::initialize()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE, NULL, NULL);

	glfwSwapInterval(1);

	glfwMakeContextCurrent(window);

	renderer.initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	InitAudioDevice();

	state = TITLE;

	tile_texture.load(RESOURCES_PATH   "img/tile.png");
	player_texture.load(RESOURCES_PATH "img/player.png");
	flag_texture.load(RESOURCES_PATH   "img/flag.png");
	spike_texture.load(RESOURCES_PATH  "img/spikes.png");
	coin_texture.load(RESOURCES_PATH   "img/coin.png");

	music = LoadMusicStream(RESOURCES_PATH "music/Meeting Place NES.wav");
	PlayMusicStream(music);

	win_sound = LoadSound(RESOURCES_PATH "sounds/win.wav");
	SetSoundVolume(win_sound, 2.5f);

	lose_sound = LoadSound(RESOURCES_PATH "sounds/lose.wav");
	SetSoundVolume(lose_sound, 3.0f);

	coin_sound = LoadSound(RESOURCES_PATH "sounds/coin.wav");
	SetSoundVolume(coin_sound, 2.5f);

	player.initialize(glm::vec2(player_texture.width, player_texture.height));

	level.initialize();
	level.flag.initialize(glm::vec2(0, 0), glm::vec2(flag_texture.width, flag_texture.height));
	level.coin.initialize(glm::vec2(0, 0), glm::vec2(coin_texture.width, coin_texture.height));

	level.load_level_data(level.level, player.position, player.velocity);

	renderer.view = glm::translate(renderer.view, glm::vec3(0, SCREEN_HEIGHT, 0));

	time = MAX_TIME;

	title_timer = 0;
}

void App::update_frame()
{
	while (!glfwWindowShouldClose(window))
	{
		glm::vec3 background_color(163, 216, 215);
		glm::vec3 text_color(255);

		glClearColor(background_color.x / 255.0f, background_color.y / 255.0f, background_color.z / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		UpdateMusicStream(music);

		if (state == GAME)
		{
			time -= delta_time;

			player.get_input(window, delta_time);

			glm::vec2 cp, cn;
			float t = 0, min_t = INFINITY;
			std::vector<std::pair<int, float>> z;

			for (size_t i = 1; i < level.tiles.size(); i++)
			{
				if (player.aabb.dynamic_AABB_vs_AABB(level.tiles[i], player.velocity, cp, cn, t, delta_time))
				{
					z.push_back({ i, t });
				}
			}

			std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
				{
					return a.second < b.second;
				});

			for (auto j : z)
				player.aabb.resolve_collision(level.tiles[j.first], player.velocity, delta_time);

			if (player.aabb.AABB_overlap(level.coin))
			{
				PlaySound(coin_sound);
				time += 5;
				level.coin.attach(glm::vec2(-TILE_SIZE, 0));
			}

			if (player.aabb.AABB_overlap(level.flag))
			{
				time = level.level == 5 ? MAX_TIME / 3 : MAX_TIME;
				PlaySound(win_sound);

				if (level.coin.position.x == -TILE_SIZE)
				{
					level.coins_collected[level.level - 1] = 1;
				}

				if (level.level != 7)
				{
					level.level++;
					level.load_level_data(level.level, player.position, player.velocity);
				}
				else
				{
					level.level = 6;
					level.load_level_data(level.level, player.position, player.velocity);
					state = TITLE;
				}
			}

			for (AABB& spike : level.spikes)
			{
				if (player.aabb.AABB_overlap(spike))
				{
					time = MAX_TIME;

					PlaySound(lose_sound);
					level.load_level_data(level.level, player.position, player.velocity);
				}
			}

			if (time <= 0)
			{
				if (level.level != 6)
				{
					time = MAX_TIME;

					PlaySound(lose_sound);
					level.load_level_data(level.level, player.position, player.velocity);
				}
				else
				{
					state = TITLE;
				}
			}

			player.update_velocity(delta_time);

			for (AABB& tile : level.tiles)
			{
				renderer.draw_texture(tile_texture, tile.position, glm::vec2(TILE_SIZE), false);
			}

			for (AABB& spike : level.spikes)
			{
				renderer.draw_texture(spike_texture, spike.position, glm::vec2(spike_texture.width, spike_texture.height), false);
			}

			renderer.draw_texture(flag_texture, level.flag.position, level.flag.size, false);

			renderer.draw_texture(coin_texture, level.coin.position, level.coin.size, false);

			renderer.draw_texture(player_texture, player.position, player.aabb.size, player.flip_x);
			
			if (level.level == 1)
			{
				renderer.draw_text("Press A & D to move.", glm::vec2(SCREEN_WIDTH / 8, -SCREEN_HEIGHT / 4 - (SCREEN_HEIGHT / 2) * 1.25), 0.5f, text_color);
				renderer.draw_text("Press space to jump.", glm::vec2(SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 4 - (SCREEN_HEIGHT / 4) * 1.25), 0.5f, text_color);
			}
			else if (level.level == 2)
			{
				renderer.draw_text("You can press Space, while", glm::vec2(SCREEN_WIDTH / 4 + (SCREEN_WIDTH / 2) - TILE_SIZE * 10, -SCREEN_HEIGHT / 4 - (SCREEN_HEIGHT / 4)), 0.5f, text_color);
				renderer.draw_text("on a wall, to wall jump.", glm::vec2(SCREEN_WIDTH / 4 + (SCREEN_WIDTH / 2) - TILE_SIZE * 10, -SCREEN_HEIGHT / 4 - (SCREEN_HEIGHT / 4) - TILE_SIZE), 0.5f, text_color);
			}
			else if (level.level == 6)
			{
				renderer.draw_text("Congratulations!", glm::vec2(SCREEN_WIDTH / 4 + (TILE_SIZE / 2), -SCREEN_HEIGHT / 4), 1.25f, text_color);
				renderer.draw_text("You beat the game!", glm::vec2(SCREEN_WIDTH / 2 - (TILE_SIZE * 6.5), -SCREEN_HEIGHT / 4 - (TILE_SIZE * 4)), 0.75f, text_color);
				renderer.draw_text("Thank you for playing!", glm::vec2(SCREEN_WIDTH / 2 - (TILE_SIZE * 8), -SCREEN_HEIGHT / 4 - (TILE_SIZE * 6)), 0.75f, text_color);
			}

			std::string time_string = "Time: 00:" + std::to_string((int)time);
			renderer.draw_text(time_string, glm::vec2(SCREEN_WIDTH / 7 + (SCREEN_WIDTH / 4), -TILE_SIZE * 2), 0.75f, text_color);
		}
		else
		{
			title_timer += delta_time;

			int coins_collected = 0;

			for (int i = 0; i < MAX_LEVELS; i++)
			{
				coins_collected += level.coins_collected[i];
			}

			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && coins_collected >= MAX_LEVELS)
			{
				state = GAME;
				PlaySound(win_sound);

				time = MAX_TIME;

				level.level = 7;

				level.load_level_data(level.level, player.position, player.velocity);
			}
			
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && title_timer > 1)
			{
				state = GAME;
				PlaySound(win_sound);

				time = MAX_TIME;

				level.level = 1;

				level.load_level_data(level.level, player.position, player.velocity);
			}

			for (AABB& tile : level.tiles)
			{
				renderer.draw_texture(tile_texture, tile.position, glm::vec2(TILE_SIZE), false);
			}

			renderer.draw_text(WINDOW_TITLE, glm::vec2(SCREEN_WIDTH / 4 + (TILE_SIZE * 4), -SCREEN_HEIGHT / 4), 1.25f, text_color);
		
			renderer.draw_text("Press Space to play.", glm::vec2(SCREEN_WIDTH / 4 + TILE_SIZE * 2.75f, -SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 4) - (TILE_SIZE / 2) - (TILE_SIZE / 4)), 0.75f, text_color);

			std::string coins_string;
			if (coins_collected >= 5)
			{
				coins_string = "Press C to play the Secret Level.";
			}
			else
			{
				coins_string = "Coins collected: " + std::to_string(coins_collected) + "/5";
			}

			renderer.draw_text(coins_string, glm::vec2(TILE_SIZE + (TILE_SIZE / 4), -SCREEN_HEIGHT + TILE_SIZE), 0.75f, glm::vec3(255, 220, 41));
		}

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

void App::close()
{
	CloseAudioDevice();

	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}