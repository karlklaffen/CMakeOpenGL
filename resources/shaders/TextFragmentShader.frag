#version 460 core

in vec2 TexCoords;

out vec4 Color;

uniform sampler2D Texture;
uniform vec3 TexColor;

void main()
{
	Color = vec4(TexColor.rgb, texture(Texture, TexCoords).r);
}