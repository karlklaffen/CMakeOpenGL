#include "Interface.h"
#include "ResourceManager.h"

Interface::Interface(GLEnums::CursorMode mode, std::string_view backgroundTexName, bool warpBg, std::string_view widgetTexName, const std::map<std::string, Widget>& widgets, const std::unordered_map<std::string, Text>& texts, const std::unordered_map<std::string, Field>& fields, const std::unordered_map<std::string, Slider>& sliders) : Width(0), Height(0), WarpBackground(warpBg), Widgets(widgets), Texts(texts), Fields(fields), Sliders(sliders), ConvertedWidgets(false), Mode(mode) {
	if (!backgroundTexName.empty()) {

		BackgroundTexName = backgroundTexName;

		glGenVertexArrays(1, &BackgroundVAO);
		glGenBuffers(1, &BackgroundVBO);
		glGenBuffers(1, &BackgroundEBO);

		glBindVertexArray(BackgroundVAO);

		glBindBuffer(GL_ARRAY_BUFFER, BackgroundVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BackgroundEBO);

		glEnableVertexAttribArray(0); // pos
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

		glEnableVertexAttribArray(1); // tex coord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	}

	if (!widgetTexName.empty()) {

		WidgetTexName = widgetTexName;

		glGenVertexArrays(1, &WidgetVAO);
		glGenBuffers(1, &WidgetVBO);
		glGenBuffers(1, &WidgetEBO);

		glBindVertexArray(WidgetVAO);

		glBindBuffer(GL_ARRAY_BUFFER, WidgetVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WidgetEBO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	}

	glBindVertexArray(0);
}

Interface::~Interface() {

	if (IsDeprecated())
		return;

	if (!BackgroundTexName.empty()) {
		glDeleteVertexArrays(1, &BackgroundVAO);
		glDeleteBuffers(1, &BackgroundVBO);
		glDeleteBuffers(1, &BackgroundEBO);
	}

	if (!WidgetTexName.empty()) {
		glDeleteVertexArrays(1, &WidgetVAO);
		glDeleteBuffers(1, &WidgetVBO);
		glDeleteBuffers(1, &WidgetEBO);
	}
}

unsigned int Interface::GetBackgroundVAO() const {
	//ErrorManager::Assert(BackgroundTex != nullptr, "Trying to access background when there is none");
	return BackgroundVAO;
}

unsigned int Interface::GetBackgroundVBO() const {
	//ErrorManager::Assert(BackgroundTex != nullptr, "Trying to access background when there is none");
	return BackgroundVBO;
}

unsigned int Interface::GetBackgroundEBO() const {
	//ErrorManager::Assert(BackgroundTex != nullptr, "Trying to access background when there is none");
	return BackgroundEBO;
}

unsigned int Interface::GetWidgetVAO() const {
	//ErrorManager::Assert(WidgetTex != nullptr, "Trying to access widget when there is none");
	return WidgetVAO;
}

std::string_view Interface::GetBackgroundTexName() const {
	return BackgroundTexName;
}

std::string_view Interface::GetWidgetTexName() const {
	return WidgetTexName;
}

const std::map<std::string, Interface::Widget>& Interface::GetWidgets() const {
	return Widgets;
}

const std::unordered_map<std::string, Text>& Interface::GetTexts() const {
	return Texts;
}

std::unordered_map<std::string, Text>& Interface::GetTexts() {
	return Texts;
}

const std::unordered_map<std::string, Interface::Field>& Interface::GetFields() const {
	return Fields;
}

std::unordered_map<std::string, Interface::Field>& Interface::GetFields() {
	return Fields;
}

GLEnums::CursorMode Interface::GetMode() const {
	return Mode;
}

glm::uvec2 Interface::GetWidthHeight() const {
	return { Width, Height };
}

Interface::WidgetState Interface::UpdateWidgetState(int x, int y, bool mousePressed, std::string_view wKey) {
	Widget& w = Widgets.at(std::string(wKey));

	bool hovered = Utils::PointInRect(x, y, w.Rect.X, w.Rect.Y, w.Rect.Width, w.Rect.Height);

	WidgetState oldState = w.State;

	if (!mousePressed) {
		if (!hovered && w.State != WidgetState::NOT_HOVERED) {
			w.State = WidgetState::NOT_HOVERED;
			return oldState;
		}
		else if (hovered && w.State != WidgetState::HOVERED) {
			w.State = WidgetState::HOVERED;
			std::cout << "new hovered" << std::endl;
			return oldState;
		}
	}
	else if (w.State == WidgetState::HOVERED) { // hovered over it and mouse pressed
		w.State = WidgetState::PRESSED;
		return oldState;
	}
	return WidgetState::UNDEFINED; // state has not changed
}

std::vector<Interface::WidgetUpdate> Interface::UpdateWidgetStates(int x, int y, bool mousePressed) {

	std::vector<WidgetUpdate> updates;

	for (const auto& [key, w] : Widgets) {
		if (w.FuncName == "NONE")
			continue;
		WidgetState oldState = UpdateWidgetState(x, y, mousePressed, key);
		if (oldState != WidgetState::UNDEFINED) { // UNDEFINED is code for "state hasn't changed"
			updates.emplace_back(oldState, key);
		}
	}
	return updates;
}

void Interface::ChangeWidgetData(std::map<std::string, Widget>::const_iterator it, std::array<float, 16> newData) const {

	unsigned int index = std::distance(Widgets.begin(), it);

	glBindVertexArray(WidgetVAO);
	glBindBuffer(GL_ARRAY_BUFFER, WidgetVBO);
	glBufferSubData(GL_ARRAY_BUFFER, index * 16 * sizeof(float), 16 * sizeof(float), newData.data());
}

void Interface::ChangeWidgetVertices(std::string_view key, Utils::Rect rect) {

	std::map<std::string, Widget>::iterator it = Widgets.find(std::string(key));

	Widget& ref = it->second;

	ref.Rect = rect;

	const Utils::Rect& sourceRect = ref.TextureSources.at(ref.ActiveTextureSourceName);

	const Texture& widgetTex = ResourceManager::GetTextures().Get(WidgetTexName);

	std::array<float, 16> newData = VertexHelper::GetRectVerticesAndTextureCoords(rect.X, rect.Y, rect.Width, rect.Height, sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height, widgetTex.GetWidth(), widgetTex.GetHeight());

	ChangeWidgetData(it, newData);
}

bool Interface::ChangeWidgetTextureSource(std::string_view key, std::string_view newTextureSourceName/*int newTexX, int newTexY, int newTexWidth, int newTexHeight*/) {

	if (WidgetTexName.empty())
		return true;

	std::map<std::string, Widget>::iterator it = Widgets.find(std::string(key));

	Widget& ref = it->second;

	std::array<float, 16> newData{ 0 };

	if (!newTextureSourceName.empty()) {

		std::unordered_map<std::string, Utils::Rect>::iterator sourceIt = ref.TextureSources.find(std::string(newTextureSourceName));

		if (sourceIt == ref.TextureSources.end())
			return true;

		const Utils::Rect& sourceRect = sourceIt->second;

		const Texture& widgetTex = ResourceManager::GetTextures().Get(WidgetTexName);

		newData = VertexHelper::GetRectVerticesAndTextureCoords(ref.Rect.X, ref.Rect.Y, ref.Rect.Width, ref.Rect.Height, sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height, widgetTex.GetWidth(), widgetTex.GetHeight());
	}

	ChangeWidgetData(it, newData);
	
	return false;
}

bool Interface::ChangeWidgetTextureSourceAndVertices(std::string_view key, Utils::Rect rect, std::string_view newTextureSourceName/*int newTexX, int newTexY, int newTexWidth, int newTexHeight*/) {

	if (WidgetTexName.empty())
		return true;

	std::map<std::string, Widget>::iterator it = Widgets.find(std::string(key));

	Widget& ref = it->second;

	ref.Rect = rect;

	std::array<float, 16> newData{ 0 };

	if (newTextureSourceName != "") {

		std::unordered_map<std::string, Utils::Rect>::iterator sourceIt = ref.TextureSources.find(std::string(newTextureSourceName));

		if (sourceIt == ref.TextureSources.end())
			return true;

		const Utils::Rect& sourceRect = sourceIt->second;

		const Texture& widgetTex = ResourceManager::GetTextures().Get(WidgetTexName);

		newData = VertexHelper::GetRectVerticesAndTextureCoords(rect.X, rect.Y, rect.Width, rect.Height, sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height, widgetTex.GetWidth(), widgetTex.GetHeight());
	}

	ChangeWidgetData(it, newData);

	return false;
}

bool Interface::TrySetTextString(std::string_view name, std::string_view newString) {
	std::unordered_map<std::string, Text>::iterator it = Texts.find(std::string(name));
	/*if (ErrorManager::Assert(it != Texts.end(), std::format("Text name ({}) not in interface when attempting to set string to ({})", name, newString)))
		return false;*/

	if (it == Texts.end())
		return true;

	it->second.SetString(newString);
	return false;
}

void Interface::Resize(unsigned int w, unsigned int h) {

	std::cout << "Interface: Resized to " << w << " x " << h << std::endl;

	if (!BackgroundTexName.empty()) {

		const Texture& backgroundTex = ResourceManager::GetTextures().Get(BackgroundTexName);

		std::array<float, 16> bgVertices = VertexHelper::GetRectVerticesAndTextureCoords(backgroundTex.GetWidth(), backgroundTex.GetHeight(), w, h, WarpBackground);
		std::array<unsigned int, 6> bgIndices = VertexHelper::GetRectIndices(0);

		glBindVertexArray(BackgroundVAO);

		glBindBuffer(GL_ARRAY_BUFFER, BackgroundVBO);
		glBufferData(GL_ARRAY_BUFFER, bgVertices.size() * sizeof(float), bgVertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BackgroundEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bgIndices.size() * sizeof(unsigned int), bgIndices.data(), GL_STATIC_DRAW);
	}

	if (!WidgetTexName.empty()) {

		std::vector<float> vertices = {};
		std::vector<unsigned int> indices = {};

		unsigned int index = 0;

		for (auto& [key, wid] : Widgets) {

			glm::uvec2 newPos = ConvertedWidgets ? VertexHelper::GetResizedPos(wid.Anchor, wid.Rect.X, wid.Rect.Y, wid.Rect.Width, wid.Rect.Height, w, h, Width, Height) : VertexHelper::GetNonAnchorPos(wid.Anchor, wid.Rect.X, wid.Rect.Y, wid.Rect.Width, wid.Rect.Height, w, h);

			wid.Rect.X = newPos.x;
			wid.Rect.Y = newPos.y;

			std::array<float, 16> verts{ 0 };
			std::array<unsigned int, 6> inds = VertexHelper::GetRectIndices(index);

			if (wid.ActiveTextureSourceName != "") {

				const Utils::Rect& sourceRect = wid.TextureSources.at(wid.ActiveTextureSourceName);

				const Texture& widgetTex = ResourceManager::GetTextures().Get(WidgetTexName);

				verts = VertexHelper::GetRectVerticesAndTextureCoords(wid.Rect.X, wid.Rect.Y, wid.Rect.Width, wid.Rect.Height, sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height, widgetTex.GetWidth(), widgetTex.GetHeight());
			}

			vertices.insert(vertices.end(), verts.begin(), verts.end());
			indices.insert(indices.end(), inds.begin(), inds.end());

			index++;
		}

		glBindVertexArray(WidgetVAO);

		glBindBuffer(GL_ARRAY_BUFFER, WidgetVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WidgetEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	}

	glBindVertexArray(0);

	Width = w;
	Height = h;

	ConvertedWidgets = true;

	for (auto& [name, text] : Texts) {
		text.Configure(w, h);
	}
}

std::vector<Interface::WidgetFuncData> Interface::UpdateWidgets(unsigned int cursorX, unsigned int cursorY, bool mousePressed) {

	std::vector<WidgetFuncData> data;

	for (WidgetUpdate& update : UpdateWidgetStates(cursorX, cursorY, mousePressed)) {

		Widget& wid = Widgets.at(update.Name);

		WidgetState oldState = update.OldState;
		WidgetState newState = wid.State;

		WidgetAction action = WidgetAction::HOVER;

		switch (newState) {
		case WidgetState::HOVERED:
			if (oldState == WidgetState::NOT_HOVERED)
				action = WidgetAction::HOVER;
			else if (oldState == WidgetState::PRESSED)
				action = WidgetAction::UNPRESS; // only consider unpressing button (executing button action) when cursor is still hovering above button
			break;
		case WidgetState::PRESSED:
			action = WidgetAction::PRESS;
			break;
		case WidgetState::NOT_HOVERED:
			action = WidgetAction::UNHOVER;
			break;
		}

		data.emplace_back(action, this, update.Name);

		// change texture source if source has name of an action

		const std::unordered_map<WidgetAction, std::string> actionTranslations = {
			{WidgetAction::HOVER, "HOVER"},
			{WidgetAction::UNHOVER, "UNHOVER"},
			{WidgetAction::PRESS, "PRESS"},
			{WidgetAction::UNPRESS, "UNPRESS"} };

		std::string_view translation = actionTranslations.at(action);

		std::unordered_map<std::string, Utils::Rect>::const_iterator actionIt = wid.TextureSources.find(std::string(translation));

		if (actionIt != wid.TextureSources.end()) {
			ChangeWidgetTextureSource(update.Name, translation);
		}
	}

	return data;
}

void Interface::ResetWidgetStates() {
	for (auto& [key, widget] : Widgets) {
		widget.State = WidgetState::NOT_HOVERED;
	}
}

void Interface::AddText(std::string_view name, const Text& text) {
	Texts.try_emplace(std::string(name), text);
}