#include "SelectedFieldInfo.h"
#include "ResourceManager.h"

SelectedFieldInfo::SelectedFieldInfo() {
	glGenVertexArrays(1, &CursorVAO);
	glGenBuffers(1, &CursorVBO);
	glGenBuffers(1, &CursorEBO);

	glBindVertexArray(CursorVAO);

	glBindBuffer(GL_ARRAY_BUFFER, CursorVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CursorEBO);

	std::array<unsigned int, 6> indices = VertexHelper::GetRectIndices(0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

	// pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

SelectedFieldInfo::~SelectedFieldInfo() {
	glDeleteVertexArrays(1, &CursorVAO);
	glDeleteBuffers(1, &CursorVBO);
	glDeleteBuffers(1, &CursorEBO);
}

bool SelectedFieldInfo::SelectField(std::string_view interfaceName, std::string_view fieldName, glm::uvec2 cursorPos) {
	
	InterfaceName = interfaceName;
	FieldName = fieldName;

	Interface& interface = ResourceManager::GetInterfaces().Get(interfaceName);
	Interface::Field& field = interface.GetFields().at(std::string(fieldName));
	Text& text = interface.GetTexts().at(field.TextRef);

	CursorIndex = field.MovableCursor ? GetClosestCursorIndex(cursorPos) : text.GetString().size();
	CursorRendered = true;

	if (field.CursorBlinkHoldTime > 0)
		BlinkTimer.emplace(field.CursorBlinkHoldTime);
	else
		TryResetBlink();

	UpdateCursorRect();
	return false;
}

bool SelectedFieldInfo::UpdateCursorRect() {
	const Interface& interfaceRef = ResourceManager::GetInterfaces().Get(InterfaceName);
	const Interface::Field& selectedField = interfaceRef.GetFields().at(FieldName);
	const Text& selectedText = interfaceRef.GetTexts().at(selectedField.TextRef);

	Utils::Rect cursorRect{ 0, 0, 0, 0 };
	const Utils::Rect& charRect = selectedText.GetCharRects().at(CursorIndex);

	switch (selectedField.CursorMode) {
	case Interface::TextCursorMode::OVERLAY:
		cursorRect = charRect;
		break;
	case Interface::TextCursorMode::BAR:
		cursorRect = { charRect.X - 2, charRect.Y, 1, charRect.Height };
		break;
	case Interface::TextCursorMode::UNDERSCORE:
		cursorRect = { charRect.X, charRect.Y - 2, charRect.Width, 1 };
	}

	glBindVertexArray(CursorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, CursorVBO);

	std::array<float, 8> data = VertexHelper::GetRectVertices(cursorRect);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, data.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return false;
}

bool SelectedFieldInfo::TryAddChar(char c) {
	Interface& interface = ResourceManager::GetInterfaces().Get(InterfaceName);
	Interface::Field& field = interface.GetFields().at(FieldName);

	Text& text = interface.GetTexts().at(field.TextRef);

	if (field.MaxChars == text.GetString().size())
		return true;

	text.AddChar(CursorIndex, c);

	CursorIndex++;
	UpdateCursorRect();

	return false;
}

bool SelectedFieldInfo::TryRemoveChar(bool left) {
	Interface& interface = ResourceManager::GetInterfaces().Get(InterfaceName);
	Interface::Field& field = interface.GetFields().at(FieldName);
	Text& text = interface.GetTexts().at(field.TextRef);

	if (left) { // backspace
		if (CursorIndex == 0)
			return true;
		else {
			text.RemoveChar(CursorIndex - 1);
			CursorIndex--;
			TryResetBlink();
			UpdateCursorRect();
		}
	}
	else { // delete
		if (CursorIndex == text.GetString().size())
			return true;
		else {
			text.RemoveChar(CursorIndex);
			TryResetBlink();
			UpdateCursorRect();
		}
	}
	return false;
}

bool SelectedFieldInfo::TryMoveCursor(Utils::Direction dir) {
	Interface& interface = ResourceManager::GetInterfaces().Get(InterfaceName);
	Interface::Field& field = interface.GetFields().at(FieldName);

	if (!field.MovableCursor)
		return true;

	const Text& text = interface.GetTexts().at(field.TextRef);

	switch (dir) {
	case Utils::Direction::LEFT:
		if (CursorIndex == 0)
			return true;
		CursorIndex--;
		break;
	case Utils::Direction::RIGHT:
		if (CursorIndex >= text.GetString().size())
			return true;
		CursorIndex++;
		break;
	case Utils::Direction::UP: {
		std::string_view str = text.GetString();

		if (CursorIndex == 0)
			break;

		const std::vector<Utils::Rect>& charRects = text.GetCharRects();

		unsigned int start = CursorIndex;

		if (CursorIndex == str.size() || str.at(CursorIndex) == '\n')
			start--;

		size_t prevLineLastCharIndex = Utils::FindFirstCharLoopingBackwards(str, '\n', start);

		if (prevLineLastCharIndex == 0) {
			CursorIndex = 0;
			break;
		}

		size_t prevLineFirstCharIndex = Utils::FindFirstCharLoopingBackwards(str, '\n', prevLineLastCharIndex - 1);

		if (str.at(prevLineFirstCharIndex) == '\n')
			prevLineFirstCharIndex++;

		const Utils::Rect& mainRect = charRects.at(CursorIndex);

		CursorIndex = GetClosestHorizontalChar(prevLineFirstCharIndex, prevLineLastCharIndex, mainRect.X);
	} break;
	case Utils::Direction::DOWN: {
		std::string_view str = text.GetString();

		if (CursorIndex == str.size())
			break;

		const std::vector<Utils::Rect>& charRects = text.GetCharRects();

		unsigned int start = CursorIndex;

		size_t prevLineFirstCharIndex = CursorIndex;
		
		if (str.at(CursorIndex) != '\n') {
			prevLineFirstCharIndex = str.find('\n', start);

			if (prevLineFirstCharIndex == std::string_view::npos) {
				CursorIndex = str.size();
				break;
			}
		}

		prevLineFirstCharIndex++;

		size_t prevLineLastCharIndex = str.find('\n', prevLineFirstCharIndex);

		if (prevLineLastCharIndex == std::string_view::npos)
			prevLineLastCharIndex = str.size();

		const Utils::Rect& mainRect = charRects.at(CursorIndex);

		CursorIndex = GetClosestHorizontalChar(prevLineFirstCharIndex, prevLineLastCharIndex, mainRect.X);
	} break;
	}

	TryResetBlink();
	UpdateCursorRect();

	return false;
}

unsigned int SelectedFieldInfo::GetClosestCursorIndex(glm::uvec2 cursorPos) {
	Interface& interface = ResourceManager::GetInterfaces().Get(InterfaceName);
	Interface::Field& field = interface.GetFields().at(FieldName);
	Text& text = interface.GetTexts().at(field.TextRef);
	const std::vector<Utils::Rect>& charRects = text.GetCharRects();

	unsigned int i = 0;

	// loop until we get to the correct line
	while (i < charRects.size()) {

		const Utils::Rect& testRect = charRects.at(i);

		size_t lastCharOnLinePos = text.GetString().find('\n', i);

		if (lastCharOnLinePos == std::string::npos) // if we are on the last line
			lastCharOnLinePos = text.GetString().size(); // position of new character to insert

		// if we are at the correct line
		if (testRect.Y <= cursorPos.y) { // see below note, think this is correct
			return GetClosestHorizontalChar(i, lastCharOnLinePos, cursorPos.x);
		}
		else {
			i = lastCharOnLinePos + 1; // set to first char of next line
		}
	}

	return 0;
}

bool SelectedFieldInfo::TryUpdateBlinkTime(float deltaTime) {
	if (!BlinkTimer.has_value())
		return true;

	Timer& timer = BlinkTimer.value();
	if (timer.Run(true, deltaTime))
		CursorRendered = !CursorRendered;

	return false;
}

bool SelectedFieldInfo::TryResetBlink() {
	CursorRendered = true;
	if (BlinkTimer.has_value())
		BlinkTimer.value().Reset();

	return false;
}

std::string_view SelectedFieldInfo::GetInterfaceName() const {
	return InterfaceName;
}

std::string_view SelectedFieldInfo::GetFieldName() const {
	return FieldName;
}

unsigned int SelectedFieldInfo::GetCursorVAO() const {
	return CursorVAO;
}

bool SelectedFieldInfo::IsCursorRendered() const {
	return CursorRendered;
}

unsigned int SelectedFieldInfo::GetClosestHorizontalChar(unsigned int start, unsigned int end, unsigned int x) {
	Interface& interface = ResourceManager::GetInterfaces().Get(InterfaceName);
	Interface::Field& field = interface.GetFields().at(FieldName);
	Text& text = interface.GetTexts().at(field.TextRef);

	for (unsigned int i = start; i <= end; i++) {
		const Utils::Rect& rect = text.GetCharRects().at(i);
		float added = 0;
		switch (field.CursorMode) {
		case Interface::TextCursorMode::BAR:
			added = rect.Width / 2.0f; break;
		case Interface::TextCursorMode::OVERLAY: 
		default:
			added = rect.Width; break;
		}

		if (rect.X + added >= x) { // TODO: See if > or >= is good with -1 adjustments in bounding box and taking char rects into account
			return i;
		}
	}

	return end;
}
