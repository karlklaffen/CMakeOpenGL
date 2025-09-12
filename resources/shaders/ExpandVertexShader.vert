#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;

uniform float Out;

void main()
{
	TexCoords = aTexCoords;

	gl_Position = Projection * View * Model * vec4(aPos + aNormal * Out, 1.0);
};