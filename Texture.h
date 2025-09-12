#pragma once

#include <filesystem>
#include <iostream>
#include <vector>
#include <stdexcept>

#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "json.hpp"

#include "NotCopyable.h"

class Texture : public NotCopyable {

public:
	Texture(const std::filesystem::path& path, unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR, unsigned int magFilter = GL_LINEAR, bool compressed = true, bool flipped = true);

	unsigned int GetID() const;
	//const std::filesystem::path& GetPath() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	Texture(const Texture& original) = default;
	Texture(Texture&& original) = default;

	~Texture();

private:

	unsigned int ID, Width, Height;
};

