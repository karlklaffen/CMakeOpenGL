#pragma once

#include <vector>
#include <array>
#include <map>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "VertexHelper.h"
#include "NotCopyable.h"
#include "ErrorManager.h"
#include "GLEnums.h"
#include "Utils.h"
#include "Text.h"
#include "VertexGroup.h"

class Interface : public NotCopyable {

public:

	enum class WidgetState {
		UNDEFINED,
		HOVERED,
		NOT_HOVERED,
		PRESSED
	};

	enum class WidgetAction {
		HOVER,
		UNHOVER,
		PRESS,
		UNPRESS
	};

	enum TextCursorMode {
		BAR,
		OVERLAY,
		UNDERSCORE
	};

	struct WidgetFuncData {
		WidgetAction Action; // what action happened to the widget?
		Interface* InterfaceRef; // to affect the interface (change other elements)
		std::string WidgetName; // name to reference widget for changing
	};

	struct WidgetUpdate {
		WidgetState OldState;
		std::string Name;
	};

	struct Widget { // widgets and texts are meant to be "cheap" objects, are not referenced (have ID, loaded/unloaded) like normal resources
		Widget(std::string_view funcName, VertexHelper::PosAnchor anchor, int x, int y, unsigned int w, unsigned int h, const std::unordered_map<std::string, Utils::Rect>& textureSources, std::string_view activeTextureSourceName) : FuncName(funcName), Anchor(anchor), Rect{ x, y, w, h }, TextureSources(textureSources), ActiveTextureSourceName(activeTextureSourceName), State(WidgetState::UNDEFINED) {

		}

		std::string FuncName;
		VertexHelper::PosAnchor Anchor;
		Utils::Rect Rect;
		std::unordered_map<std::string, Utils::Rect> TextureSources;
		std::string ActiveTextureSourceName;

		WidgetState State;
	};

	struct Field {
		std::string TextRef;
		bool MovableCursor;
		float CursorBlinkHoldTime;
		unsigned int MaxChars;
		TextCursorMode CursorMode;
	};

	struct Slider {
		unsigned int Value;
		unsigned int MaxValue;
		std::string ConfirmWidgetName;
		std::string FuncName;
		Utils::Rect Rect;
	};

	Interface(GLEnums::CursorMode mode, std::string_view backgroundTexName, bool warpBg, std::string_view widgetTexName, const std::map<std::string, Widget>& widgets, const std::unordered_map<std::string, Text>& texts, const std::unordered_map<std::string, Field>& fields, const std::unordered_map<std::string, Slider>& sliders);
	~Interface();
	
	Interface(const Interface& original) = default;
	Interface(Interface&& original) = default;
	
	unsigned int GetBackgroundVAO() const;
	unsigned int GetBackgroundVBO() const;
	unsigned int GetBackgroundEBO() const;
	unsigned int GetWidgetVAO() const;

	std::string_view GetBackgroundTexName() const;
	std::string_view GetWidgetTexName() const;

	glm::uvec2 GetWidthHeight() const;

	const std::map<std::string, Widget>& GetWidgets() const;
	const std::unordered_map<std::string, Text>& GetTexts() const;
	std::unordered_map<std::string, Text>& GetTexts();
	const std::unordered_map<std::string, Field>& GetFields() const;
	std::unordered_map<std::string, Field>& GetFields();

	GLEnums::CursorMode GetMode() const;

	WidgetState UpdateWidgetState(int x, int y, bool mousePressed, std::string_view wKey); // returns Error widget state when state change is not needed for widget
	std::vector<WidgetUpdate> UpdateWidgetStates(int x, int y, bool mousePressed);

	void ChangeWidgetVertices(std::string_view key, Utils::Rect rect);
	bool ChangeWidgetTextureSource(std::string_view key, std::string_view newTextureSourceName/*int newTexX, int newTexY, int newTexWidth, int newTexHeight*/);
	bool ChangeWidgetTextureSourceAndVertices(std::string_view key, Utils::Rect rect, std::string_view newTextureSourceName/*int newTexX, int newTexY, int newTexWidth, int newTexHeight*/);

	bool TrySetTextString(std::string_view funcName, std::string_view newString);

	void Resize(unsigned int w, unsigned int h);

	std::vector<WidgetFuncData> UpdateWidgets(unsigned int cursorX, unsigned int cursorY, bool mousePressed);

	void ResetWidgetStates();

	void AddText(std::string_view name, const Text& text);

private:
	void ChangeWidgetData(const std::map<std::string, Widget>::const_iterator it, std::array<float, 16> newData) const;

	/*VertexGroup BackgroundGroup;
	VertexGroup WidgetGroup;*/
	unsigned int BackgroundVAO, BackgroundVBO, BackgroundEBO;
	unsigned int WidgetVAO, WidgetVBO, WidgetEBO;

	std::string BackgroundTexName;
	std::string WidgetTexName;

	std::map<std::string, Widget> Widgets; // ordered because widget data needs to be stored contiguously to be parallel to GPU buffer
	std::unordered_map<std::string, Text> Texts;
	std::unordered_map<std::string, Field> Fields;
	std::unordered_map<std::string, Slider> Sliders;

	unsigned int Width, Height;

	bool ConvertedWidgets;
	bool WarpBackground;

	GLEnums::CursorMode Mode;
};

