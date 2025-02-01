#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

#include "Texture.h"

struct Shader
{
	uint32_t id;

	void initialize(const char* vertex_path, const char* fragment_path);

	void use();

	void set_uniform_3_f(const char* name, const glm::vec3 vector);

	void set_uniform_4_f(const char* name, const glm::vec4 vector);

	void set_uniform_mat4_f(const char* name, const glm::mat4 matrix);

	void set_uniform_texture(const char* name, Texture texture);
};