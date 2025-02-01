#version 330 core

layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 tex_coord;

out vec2 TexCoord;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(vertex_position, 0.0, 1.0);
	TexCoord = tex_coord;
}