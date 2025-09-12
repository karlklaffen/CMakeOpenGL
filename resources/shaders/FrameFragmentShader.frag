#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D FrameTexture;
uniform float Kernel[9];
uniform bool KernelActive;

void main()
{
	if (KernelActive) {
		ivec2 size = textureSize(FrameTexture, 0);
		vec2 offset = vec2(2.0 / size.x, 2.0 / size.y);

		vec2 offsets[] = {
		{-offset.x, offset.y},
		{0, offset.y},
		{offset.x, offset.y},
		{-offset.x, 0},
		{0, 0},
		{offset.x, 0},
		{-offset.x, -offset.y},
		{0, -offset.y},
		{offset.x, -offset.y}
		};

		vec3 color = vec3(0.0);

		for (int i = 0; i < 9; i++) {
			color += Kernel[i] * texture(FrameTexture, TexCoords + offsets[i]).rgb;
		}

		FragColor = vec4(color, 1.0);
	}
	else {
		FragColor = texture(FrameTexture, TexCoords);
	}
}