#pragma once

#include <glad/glad.h>
#include "stb_image.h"

struct Texture
{
	uint32_t id;
	int width, height, nr_channels;
	unsigned char* data;

	void load(const char* path);

	void bind();
};