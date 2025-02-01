#include "Renderer.h"

void Renderer::initialize(float screen_width, float screen_height)
{
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	proj = glm::ortho(0.0f, screen_width, 0.0f, screen_height, -1.0f, 1.0f);
	view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 mvp = proj * view * model;

	shader.initialize(RESOURCES_PATH "shaders/shader.vs", RESOURCES_PATH "shaders/shader.fs");
	shader.set_uniform_mat4_f("mvp", mvp);

	font_shader.initialize(RESOURCES_PATH "shaders/shader.vs", RESOURCES_PATH "shaders/font_shader.fs");
	font_shader.set_uniform_mat4_f("mvp", mvp);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	FT_Library ft;
	FT_Init_FreeType(&ft);

	FT_Face face;
	FT_New_Face(ft, RESOURCES_PATH "fonts/PressStart2P-Regular.ttf", 0, &face);

	FT_Set_Pixel_Sizes(face, 0, 32);

	FT_Load_Char(face, 'X', FT_LOAD_RENDER);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			continue;
		}

		uint32_t texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    	Character character = {
    		texture,
    		glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
    		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
    		face->glyph->advance.x
    	};

    	characters.insert(std::pair<char, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Renderer::draw_texture(Texture texture, glm::vec2 position, glm::vec2 size, bool flip_x)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = proj * view * model;
	shader.set_uniform_mat4_f("mvp", mvp);
	shader.set_uniform_texture("UTexture", texture);

	texture.bind();

	float vertices[] = {
		 position.x,           position.y,          !flip_x ? 0.0f : 1.0f, 1.0f,
		 position.x,           position.y - size.y, !flip_x ? 0.0f : 1.0f, 0.0f,
		 position.x + size.x,  position.y - size.y, !flip_x ? 1.0f : 0.0f, 0.0f,
		 position.x + size.x,  position.y,          !flip_x ? 1.0f : 0.0f, 1.0f
	};

	uint32_t indices[] = {
		0, 1, 2,
		0, 3, 2
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::draw_text(std::string text, glm::vec2 position, float scale, glm::vec3 color)
{
	font_shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = proj * view * model;
	font_shader.set_uniform_mat4_f("mvp", mvp);
	font_shader.set_uniform_3_f("UColor", color / glm::vec3(255));

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		float xpos = position.x + ch.bearing.x * scale;
		float ypos = position.y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		float vertices[] = {
			 xpos,      ypos,     0.0f, 1.0f,
			 xpos,      ypos + h, 0.0f, 0.0f,
			 xpos + w,  ypos + h, 1.0f, 0.0f,
			 xpos + w,  ypos,     1.0f, 1.0f
		};

		uint32_t indices[] = {
			0, 1, 2,
			0, 3, 2
		};

		glBindTexture(GL_TEXTURE_2D, ch.texture_id);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		position.x += (ch.advance >> 6) * scale;
	}
}