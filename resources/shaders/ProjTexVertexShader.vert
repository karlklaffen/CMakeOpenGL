#version 460 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 Projection;

void main()
{
	gl_Position = Projection * vec4(pos, 0.0, 1.0);
	TexCoords = texCoords;
};