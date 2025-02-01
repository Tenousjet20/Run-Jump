#version 330 core

out vec4 Color;

in vec2 TexCoord;

uniform sampler2D UTexture;
uniform vec3 UColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(UTexture, TexCoord).r);
    Color = vec4(UColor, 1.0) * sampled;
}