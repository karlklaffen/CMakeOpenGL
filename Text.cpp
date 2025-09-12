#include "Text.h"
#include "ResourceManager.h"

Text::Text(std::string_view fontName, std::string_view string, const glm::vec3& color, VertexHelper::PosAnchor anchor, glm::ivec2 pos, float scale, unsigned int lineSpacing, std::optional<unsigned int> boundingWidth, std::optional<unsigned int> boundingHeight) : FontName(fontName), Color(color), String(string), Anchor(anchor), Pos(pos), Scale(scale), LineSpacing(lineSpacing), CharRects(0), BoundingBox(0, 0, 0, 0), WindowWidth(0), WindowHeight(0), ResizableWidth(true), ResizableHeight(true) {
	if (boundingWidth.has_value()) {
		BoundingBox.Width = boundingWidth.value();
		ResizableWidth = false;
	}
	if (boundingHeight.has_value()) {
		BoundingBox.Height = boundingHeight.value();
		ResizableHeight = false;
	}
}

void Text::Configure(unsigned int windowWidth, unsigned int windowHeight) {

	WindowWidth = windowWidth;
	WindowHeight = windowHeight;

	CharRects.clear();
	CharRects.reserve(String.size() + 1);
	// char rects also includes rect for new character to insert

	const Font& font = ResourceManager::GetFonts().Get(FontName);

	const std::map<char, Glyph>& glyphs = font.GetGlyphs();
	unsigned int fontHeight = font.GetHeight() * Scale;

	int addedY = 0; // y to add to each new line of text

	unsigned int numLines = 1 + std::count(String.begin(), String.end(), '\n');

	switch (Anchor.Y) {
	case VertexHelper::PosAnchorDim::START:
		addedY = (numLines - 1) * (fontHeight + LineSpacing); // if on bottom then draw first piece of text up a bit
		break;
	case VertexHelper::PosAnchorDim::CENTER:
		addedY = (numLines - 1) * (fontHeight + LineSpacing) / 2; // if in center then move text up halfway compared to other movements]
		break;
	}

	unsigned int offX = 0;

	std::vector<glm::uvec2> poses(String.size() + 1);

	unsigned int firstCharOfLineIndex = 0;

	for (int i = 0; i < String.size(); i++) {

		if (String.at(i) == '\n') {

			AddLineCharRects(firstCharOfLineIndex, i, Pos.x, Pos.y, offX, addedY, poses);

			offX = 0;
			addedY -= LineSpacing + fontHeight;

			firstCharOfLineIndex = i + 1;
		}
		else {

			char c = String.at(i);

			const Glyph& g = glyphs.at(c);

			int x = offX + g.Offset.x * Scale; // x offset for drawing of letter relative to baseline

			int y = -((int)g.Size.y - (int)g.Offset.y) * Scale; // zero or negative to account for offset below baseline (like in g)

			poses[i] = { x, y };

			offX += (g.Advance >> 6) * Scale; // advance is in 1/64 pixels
		}
	}

	unsigned int newCharWhitespaceIndex = String.size();

	glm::uvec2 actualPos = AddLineCharRects(firstCharOfLineIndex, newCharWhitespaceIndex, Pos.x, Pos.y, offX, addedY, poses);

	int leastX = -1;
	int greatestX = -1;

	for (const Utils::Rect& rect : CharRects) {
		if (leastX == -1 || rect.X < leastX)
			leastX = rect.X;
		if (greatestX == -1 || rect.X + rect.Width > greatestX)
			greatestX = rect.X + rect.Width - 1; // -1 to account for including both edges
	}

	unsigned int calculatedHeight = fontHeight * numLines + LineSpacing * (numLines - 1);
	// TODO: For now bounding box is centered vertially on text, consider different implementation

	BoundingBox.X = leastX;

	if (ResizableWidth) {
		BoundingBox.Width = (unsigned int)(greatestX - leastX);
	}

	if (ResizableHeight) {
		BoundingBox.Height = calculatedHeight;
		BoundingBox.Y = (int)actualPos.y;
	}
	else {
		BoundingBox.Y = (calculatedHeight - BoundingBox.Height) / 2;
	}

	// TODO: Consider adding -1 to y values somewhere to account for including both ends
	//BoundingBox = { leastX, (int)actualPos.y, (unsigned int)(greatestX - leastX), fontHeight * numLines + LineSpacing * (numLines - 1)};}
}

bool Text::AddChar(unsigned int index, char c) {
	if (index > String.size())
		return true;

	String.insert(index, 1, c);
	Configure(WindowWidth, WindowHeight); // TODO: make it so that it only updates current line for better efficiency

	return false;
}

bool Text::RemoveChar(unsigned int index) {
	if (index >= String.size())
		return true;

	String.erase(index, 1);
	Configure(WindowWidth, WindowHeight);

	return false;
}

void Text::SetString(std::string_view s) {
	String = s;
	Configure(WindowWidth, WindowHeight);
}

const std::string& Text::GetString() const {
	return String;
}

const std::vector<Utils::Rect>& Text::GetCharRects() const {
	return CharRects;
}

const std::string& Text::GetFontName() const {
	return FontName;
}

const glm::vec3& Text::GetColor() const {
	return Color;
}

VertexHelper::PosAnchor Text::GetAnchor() const {
	return Anchor;
}

const glm::vec2& Text::GetPos() const {
	return Pos;
}

float Text::GetScale() const {
	return Scale;
}

unsigned int Text::GetLineSpacing() const {
	return LineSpacing;
}

const Utils::Rect& Text::GetBoundingBox() const {
	return BoundingBox;
}

glm::uvec2 Text::AddLineCharRects(unsigned int startIndex, unsigned int endWhitespaceIndex, int x, int y, int offX, int offY, const std::vector<glm::uvec2>& poses) {

	unsigned int whitespaceWidth = (unsigned int)(5 * Scale);

	const Font& font = ResourceManager::GetFonts().Get(FontName);
	const std::map<char, Glyph>& glyphs = font.GetGlyphs();
	unsigned int fontHeight = font.GetHeight() * Scale;

	glm::uvec2 actualPos = { 0, 0 };

	glm::uvec2 whitespaceEndCharPos = poses.at(endWhitespaceIndex);

	if (startIndex == endWhitespaceIndex) { // this is whitespace
		actualPos = VertexHelper::GetNonAnchorPos(Anchor, Pos.x, Pos.y + offY, whitespaceWidth, fontHeight, WindowWidth, WindowHeight);
	}
	else {

		const std::map<char, Glyph>& glyphs = font.GetGlyphs();

		glm::uvec2 endCharPos = poses.at(endWhitespaceIndex - 1);
		const Glyph& endGlyph = glyphs.at(String.at(endWhitespaceIndex - 1));

		// bottom left (start) pos for last line
		actualPos = VertexHelper::GetNonAnchorPos(Anchor, Pos.x, Pos.y + offY, endCharPos.x + endGlyph.Size.x * Scale, fontHeight, WindowWidth, WindowHeight);

		for (unsigned int j = startIndex; j < endWhitespaceIndex; j++) {

			glm::uvec2 pos = poses.at(j);
			const Glyph& g = glyphs.at(String.at(j));

			CharRects.push_back(Utils::Rect{ (int)(actualPos.x + pos.x), (int)(actualPos.y + pos.y), (unsigned int)(g.Size.x * Scale), (unsigned int)(g.Size.y * Scale) });
		}
	}

	CharRects.push_back(Utils::Rect{ (int)actualPos.x + offX, (int)actualPos.y, whitespaceWidth, fontHeight });

	//std::cout << "last char rect of line: " << CharRects.at(CharRects.size() - 1).ToString() << std::endl;

	return actualPos;
}
