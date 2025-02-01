#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <raudio.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#include "Renderer.h"
#include "Texture.h"
#include "Player.h"
#include "Level.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define WINDOW_TITLE "Run & Jump"

#define MAX_TIME 30

enum GameState
{
	TITLE = 0,
	GAME
};

struct App
{
	GLFWwindow* window;
	Renderer renderer;

	GameState state;

	float delta_time = 0.0f;
	float last_frame = 0.0f;

	Texture tile_texture;
	Texture player_texture;
	Texture flag_texture;
	Texture spike_texture;
	Texture coin_texture;

	Music music;
	Sound win_sound;
	Sound lose_sound;
	Sound coin_sound;

	Player player;
	Level level;

	float time;
	float title_timer;

	void initialize();

	void update_frame();

	void close();
};
