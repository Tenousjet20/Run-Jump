#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <algorithm>
#include <map>

#include "Shader.h"
#include "Texture.h"

struct Character
{
	uint32_t texture_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	uint32_t advance;
};

struct Renderer
{
	Shader shader;
	Shader font_shader;
	uint32_t vao, vbo, ibo;
	glm::mat4 proj, view;
	std::map<char, Character> characters;

	void initialize(float screen_width, float screen_height);

	void draw_texture(Texture texture, glm::vec2 position, glm::vec2 size, bool flip_x);
	
	void draw_text(std::string text, glm::vec2 position, float scale, glm::vec3 color);
};