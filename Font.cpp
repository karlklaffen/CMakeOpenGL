#include "Font.h"

Font::Font(std::filesystem::path path, unsigned int height) : Height(height) {
	//Resource(ResourceType::FONT, "Path = " + path.string() + ", Height = " + std::to_string(height)) {
	
	FT_Face face;

	if (FT_New_Face(FreeType, path.string().c_str(), NextIndex, &face)) {
		std::cout << "Font Error: Could not load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, height);

	for (unsigned char c = 0; c < 128; c++) {

		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "Font Error: Could not load char " << c << std::endl;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Glyphs[c] = { texture, {face->glyph->bitmap.width, face->glyph->bitmap.rows}, {face->glyph->bitmap_left, face->glyph->bitmap_top}, static_cast<unsigned int>(face->glyph->advance.x) };
	}

	FT_Done_Face(face);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);

	std::array<unsigned int, 6> indices = VertexHelper::GetRectIndices(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	NextIndex++;
}

Font::~Font() {

	if (IsDeprecated())
		return;

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	for (auto& [c, g] : Glyphs)
		glDeleteTextures(1, &g.TextureID);
}

unsigned int Font::GetHeight() const {
	return Height;
}

unsigned int Font::GetVAO() const {
	return VAO;
}

unsigned int Font::GetVBO() const {
	return VBO;
}

const std::map<char, Glyph>& Font::GetGlyphs() const {
	return Glyphs;
}

bool Font::InitLibrary() {

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (FT_Init_FreeType(&FreeType)) {
		std::cout << "Font Error: Could not Init FreeType" << std::endl;
		return true;
	}
	return false;
}

void Font::CleanLibrary() {
	FT_Done_FreeType(FreeType);
}