#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <optional>

#include "VertexHelper.h"
#include "Utils.h"
#include "Timer.h"

class SelectedFieldInfo {

public:
	SelectedFieldInfo();
	~SelectedFieldInfo();

	bool SelectField(std::string_view interfaceName, std::string_view fieldName, glm::uvec2 cursorPos);
	bool UpdateCursorRect();
	bool TryAddChar(char c);
	bool TryRemoveChar(bool left);
	bool TryMoveCursor(Utils::Direction dir);

	unsigned int GetClosestCursorIndex(glm::uvec2 cursorPos);
	bool TryUpdateBlinkTime(float deltaTime);

	bool TryResetBlink();

	std::string_view GetInterfaceName() const;
	std::string_view GetFieldName() const;
	unsigned int GetCursorVAO() const;
	bool IsCursorRendered() const;

private:
	unsigned int GetClosestHorizontalChar(unsigned int start, unsigned int end, unsigned int x);

	std::string InterfaceName, FieldName;
	unsigned int CursorVAO, CursorVBO, CursorEBO;
	unsigned int CursorIndex;

	std::optional<Timer> BlinkTimer;
	bool CursorRendered;
};