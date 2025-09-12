#pragma once

#include <iostream>
#include <filesystem>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "VertexHelper.h"
#include "NotCopyable.h"

struct Glyph {
	unsigned int TextureID;
	glm::uvec2 Size;
	glm::uvec2 Offset;
	unsigned int Advance;
};

class Font : public NotCopyable {

public:
	static bool InitLibrary();
	static void CleanLibrary();

	Font(std::filesystem::path path, unsigned int height);
	~Font();

	Font(const Font& original) = default;
	Font(Font&& original) = default;

	unsigned int GetHeight() const;
	unsigned int GetVAO() const;
	unsigned int GetVBO() const;
	const std::map<char, Glyph>& GetGlyphs() const;

private:

	std::map<char, Glyph> Glyphs;
	unsigned int VAO, VBO, EBO;

	unsigned int Height;

	inline static FT_Library FreeType;
	inline static int NextIndex = 0;
	inline static int NumInstances = 0;
};