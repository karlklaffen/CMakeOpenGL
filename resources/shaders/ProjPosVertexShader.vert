#version 460 core

layout (location = 0) in vec2 pos;

uniform mat4 Projection;

void main()
{
	gl_Position = Projection * vec4(pos, 0.0, 1.0);
};