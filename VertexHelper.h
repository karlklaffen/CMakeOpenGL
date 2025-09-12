#pragma once

#include <glm/glm.hpp>

#include <array>
#include <unordered_map>
#include <vector>
#include <limits>

#include "Utils.h"

namespace VertexHelper {

	enum class CoordsLoc {
		X_START,
		X_END,
		Y_START,
		Y_END
	};

	enum class PosAnchorDim {
		CENTER,
		START,
		END
	};

	struct PosAnchor {
		PosAnchorDim X;
		PosAnchorDim Y;
	};

	/*static PosAnchor TranslatePosAnchor(std::string_view anchor) {
		std::unordered_map<std::string_view, VertexHelper::PosAnchor> anchorTranslations = {
		{"NONE", VertexHelper::PosAnchor::NONE},
		{"CENTER", VertexHelper::PosAnchor::CENTER },
		{"TOP_LEFT", VertexHelper::PosAnchor::TOP_LEFT},
		{"TOP_RIGHT", VertexHelper::PosAnchor::TOP_RIGHT},
		{"BOTTOM_LEFT", VertexHelper::PosAnchor::BOTTOM_LEFT},
		{"BOTTOM_RIGHT", VertexHelper::PosAnchor::BOTTOM_RIGHT}
		};

		return anchorTranslations.at(anchor);
	}*/

	static std::array<float, 8> GetRectVertices(float x, float y, float w, float h) {
		return { x, y, // top right
				x + w, y, // bottom right
				x + w, y + h, // bottom left
				x, y + h };
	}

	static std::array<float, 8> GetRectVertices(Utils::Rect rect) {
		return GetRectVertices(rect.X, rect.Y, rect.Width, rect.Height);
	}

	static std::unordered_map<CoordsLoc, float> GetRectTextureCoords(float tx, float ty, float tw, float th, float totalWidth, float totalHeight) {
		std::unordered_map<CoordsLoc, float> m;
		m[CoordsLoc::X_START] = tx / totalWidth;
		m[CoordsLoc::Y_START] = ty / totalHeight;
		m[CoordsLoc::X_END] = (tx + tw) / totalWidth;
		m[CoordsLoc::Y_END] = (ty + th) / totalHeight;
		return m;
	} // TODO: Change map to unordered_map
	
	static std::unordered_map<CoordsLoc, float> GetRectTextureCoords(Utils::Rect rect, float totalWidth, float totalHeight) {
		return GetRectTextureCoords(rect.X, rect.Y, rect.Width, rect.Height, totalWidth, totalHeight);
	}

	static std::array<float, 16> GetRectVerticesAndTextureCoords(float x, float y, float w, float h, bool flipY = false) {
		float ys = flipY ? 1.0f : 0.0f;
		float ye = flipY ? 0.0f : 1.0f;

		return { x, y, 0.0f, ys, // top right
				 x + w, y, 1.0f, ys, // bottom right
				 x + w, y + h, 1.0f, ye, // bottom left
				 x, y + h, 0.0f, ye }; // top left
	}

	static std::array<float, 16> GetRectVerticesAndTextureCoords(Utils::Rect rect, bool flipY = false) {
		return GetRectVerticesAndTextureCoords(rect.X, rect.Y, rect.Width, rect.Height, flipY);
	}

	static std::array<float, 16> GetRectVerticesAndTextureCoords(bool flipY = false) {
		return GetRectVerticesAndTextureCoords(-1.0f, -1.0f, 2.0f, 2.0f, flipY);
	}

	static std::array<float, 16> GetRectVerticesAndTextureCoords(float x, float y, float w, float h, std::unordered_map<CoordsLoc, float> coords) {
		return { x, y, coords[CoordsLoc::X_START], coords[CoordsLoc::Y_START], // top right
				x + w, y, coords[CoordsLoc::X_END], coords[CoordsLoc::Y_START], // bottom right
				x + w, y + h, coords[CoordsLoc::X_END], coords[CoordsLoc::Y_END], // bottom left
				x, y + h, coords[CoordsLoc::X_START], coords[CoordsLoc::Y_END] }; // top left
	}

	static std::array<unsigned int, 6> GetRectIndices(unsigned int offset) {
		unsigned int o = offset * 4;
		return { o + 0, o + 1, o + 2,
				 o + 2, o + 3, o + 0 };
	}

	static std::array<float, 16> GetRectVerticesAndTextureCoords(float x, float y, float w, float h, float tx, float ty, float tw, float th, float totalWidth, float totalHeight) { // all in terms of pixels
		return GetRectVerticesAndTextureCoords(x, y, w, h, GetRectTextureCoords(tx, ty, tw, th, totalWidth, totalHeight));
	}

	static std::array<float, 16> GetRectVerticesAndTextureCoords(unsigned int texWidth, unsigned int texHeight, unsigned int screenWidth, unsigned int screenHeight, bool warp) {
		float xOffset = 0, yOffset = 0;

		float scaledBgWidth = 0, scaledBgHeight = 0;

		float screenAspect = screenWidth / static_cast<float>(screenHeight);
		float texAspect = texWidth / static_cast<float>(texHeight);

		if (warp) { // warp image aspect ratio to fit to screen
			scaledBgWidth = screenWidth;
			scaledBgHeight = screenHeight;
		}
		else { // cut off parts of image to keep it same aspect ratio yet fit completely in screen
			if (texAspect <= screenAspect) { // fit by width
				scaledBgHeight = screenWidth / texAspect;
				scaledBgWidth = texAspect * scaledBgHeight;
				yOffset = (static_cast<float>(screenHeight) - scaledBgHeight) / 2;
			}
			else { // fit by height
				scaledBgWidth = texAspect * screenHeight;
				scaledBgHeight = scaledBgWidth / texAspect;
				xOffset = (static_cast<float>(screenWidth) - scaledBgWidth) / 2;
			}
		}

		return GetRectVerticesAndTextureCoords(xOffset, yOffset, scaledBgWidth, scaledBgHeight);
	}

	inline static glm::uvec2 GetNonAnchorPos(PosAnchor anchor, int x, int y, unsigned int w, unsigned int h, unsigned int sWidth, unsigned int sHeight) {

		switch (anchor.X) {
		case PosAnchorDim::CENTER:
			x += sWidth / 2 - w / 2;
			break;
		case PosAnchorDim::START:
			break;
		case PosAnchorDim::END:
			x += sWidth - w;
			break;
		}

		switch (anchor.Y) {
		case PosAnchorDim::CENTER:
			y += sHeight / 2 - h / 2;
			break;
		case PosAnchorDim::START:
			break;
		case PosAnchorDim::END:
			y += sHeight - h;
			break;
		}

		/*switch (anchor) {
		case PosAnchor::CENTER:
			x += sWidth / 2 - w / 2;
			y += sHeight / 2 - h / 2;
			break;
		case PosAnchor::TOP_LEFT:
			y += sHeight - h;
			break;
		case PosAnchor::TOP_RIGHT:
			x += sWidth - w;
			y += sHeight - h;
			break;
		case PosAnchor::BOTTOM_LEFT:
			break;
		case PosAnchor::BOTTOM_RIGHT:
			x += sWidth - w;
			break;

		}*/

		return { x, y };
	}

	inline static glm::uvec2 GetResizedPos(PosAnchor anchor, int x, int y, unsigned int w, unsigned int h, unsigned int sWidth, unsigned int sHeight, unsigned int prevSWidth, unsigned int prevSHeight) {
		/*switch (anchor) {
		case PosAnchor::CENTER:
			x = sWidth / 2 + (x - prevSWidth / 2);
			y = sHeight / 2 + (y - prevSHeight / 2);
			break;
		case PosAnchor::TOP_LEFT:
			y = sHeight - (prevSHeight - y);
			break;
		case PosAnchor::TOP_RIGHT:
			x = sWidth - (prevSWidth - x);
			y = sHeight - (prevSHeight - y);
			break;
		case PosAnchor::BOTTOM_LEFT:
			break;
		case PosAnchor::BOTTOM_RIGHT:
			x = sWidth - (prevSWidth - x);
			break;
		}*/

		switch (anchor.X) {
		case PosAnchorDim::CENTER:
			x = sWidth / 2 + (x - prevSWidth / 2);
			break;
		case PosAnchorDim::START:
			break;
		case PosAnchorDim::END:
			x = sWidth - (prevSWidth - x);
			break;
		}

		switch (anchor.Y) {
		case PosAnchorDim::CENTER:
			y = sHeight / 2 + (y - prevSHeight / 2);
			break;
		case PosAnchorDim::START:
			break;
		case PosAnchorDim::END:
			y = sHeight - (prevSHeight - y);
			break;
		}

		return { x, y };
	}

	//inline static std::vector<glm::vec2> GetResizedPoses(PosAnchor anchor, const std::vector<glm::vec2>& poses, unsigned int w, unsigned int h, unsigned int sWidth, unsigned int sHeight, unsigned int prevSWidth, unsigned int prevSHeight) {
	//	/*int min = std::numeric_limits<int>::min();
	//	int max = std::numeric_limits<int>::max();

	//	int minX = min, minY = min, maxX = max, maxY = max;
	//	for (glm::vec2 p : poses) {
	//		if (p.x < minX)
	//			minX = p.x;
	//		else if (p.x > minX)

	//	}

	//	switch (anchor) {
	//	case PosAnchor::CENTER:
	//		
	//	}*/
	//}

	inline static std::array<float, 24> GetCubemapVertices() {
		return {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		};
	}

	inline static std::array<unsigned int, 36> GetCubemapIndices() {
		return {
		2, 0, 4, 4, 6, 2,
		1, 0, 2, 2, 3, 1,
		4, 5, 7, 7, 6, 4,
		1, 3, 7, 7, 5, 1,
		2, 6, 7, 7, 3, 2,
		0, 1, 4, 4, 1, 5
		};
	}

}