#pragma once

#include <string>
#include <algorithm>

#include "VertexHelper.h"
#include "Font.h"

class Text {

public:
	Text(std::string_view fontName, std::string_view string, const glm::vec3& color, VertexHelper::PosAnchor anchor, glm::ivec2 pos, float scale, unsigned int lineSpacing = 0, std::optional<unsigned int> boundingWidth = {}, std::optional<unsigned int> boundingHeight = {});
	
	//void UpdateWindowDims(unsigned int w, unsigned int h);
	//glm::uvec2 AddToLine(unsigned int charIndex, const std::string& added);
	void Configure(unsigned int windowWidth, unsigned int windowHeight);

	bool AddChar(unsigned int index, char c);
	bool RemoveChar(unsigned int index);

	void SetString(std::string_view str);
	const std::string& GetString() const;
	const std::vector<Utils::Rect>& GetCharRects() const;
	const std::string& GetFontName() const;
	const glm::vec3& GetColor() const;
	VertexHelper::PosAnchor GetAnchor() const;
	const glm::vec2& GetPos() const;
	float GetScale() const;
	unsigned int GetLineSpacing() const;
	const Utils::Rect& GetBoundingBox() const;
	
private:

	// returns actual pos (bottom left) of the line
	glm::uvec2 AddLineCharRects(unsigned int startIndex, unsigned int endIndex, int x, int y, int offX, int offY, const std::vector<glm::uvec2>& poses);
	std::string FontName;
	std::string String;
	glm::vec3 Color;
	VertexHelper::PosAnchor Anchor;
	glm::ivec2 Pos;
	float Scale;
	unsigned int LineSpacing;

	unsigned int WindowWidth;
	unsigned int WindowHeight;

	std::vector<Utils::Rect> CharRects;
	Utils::Rect BoundingBox;

	bool ResizableWidth;
	bool ResizableHeight;

}; // TODO: Add way to have background of text