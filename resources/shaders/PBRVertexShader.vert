#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aTangent;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in mat4 Model; // next location = 7

uniform mat4 View;
uniform mat4 Projection;

out vec3 WorldPos;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
	WorldPos = vec3(Model * vec4(aPos, 1.0));
//	Normal = normalize(mat3(transpose(inverse(Model))) * aNormal);
	TexCoords = aTexCoords;

//	mat3 normMat = transpose(inverse(mat3(Model))); // more precise than model mat for normals
	vec3 T = normalize(vec3(Model * vec4(aTangent.xyz, 0.0))); // bring into world space
	vec3 N = normalize(vec3(Model * vec4(aNormal, 0.0)));
	vec3 B = cross(N, T) * aTangent.w;

//	T = normalize(T - dot(T, N) * N); // Gram-Schmidt process for re-orthogonalizing vectors so that they are perpendicular

	// used for converting world space to tangent space
	TBN = mat3(T, B, N); // transpose = inverse for orthogonal, is cheaper


	gl_Position = Projection * View * vec4(WorldPos, 1.0);
};