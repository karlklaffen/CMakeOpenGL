#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <filesystem>
#include <iostream>

#include "stb_image.h"
#include "NotCopyable.h"
#include "VertexHelper.h"

class Cubemap : public NotCopyable {
public:
	Cubemap(std::filesystem::path dir);

	Cubemap(const Cubemap& original) = default;
	Cubemap(Cubemap&& original) = default;

	unsigned int GetVAO() const;
	unsigned int GetVBO() const;
	unsigned int GetEBO() const;
	unsigned int GetTextureID() const;

	~Cubemap();

private:

	unsigned int TextureID, VAO, VBO, EBO;
};

