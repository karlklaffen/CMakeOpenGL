#include "ResourceHolders.h"
#include "ResourceManager.h"

std::optional<Texture> TextureOwner::Construct(const nlohmann::json& data) const {

	std::unordered_map<std::string, unsigned int> minFilterTranslations = {
		{"LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR},
		{"LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST},
		{"NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST},
		{"NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR},
		{"LINEAR", GL_LINEAR},
		{"NEAREST", GL_NEAREST}
	};

	std::unordered_map<std::string, unsigned int> magFilterTranslations = {
		{"LINEAR", GL_LINEAR},
		{"NEAREST", GL_NEAREST}
	};

	std::string source;
	if (IsInvalidElseAssign<std::string>(data, "source", JsonType::STRING, source))
		return {};

	std::filesystem::path path = ResourceManager::GetResourcePath() / "textures" / source;

	nlohmann::json filters = nullptr;
	if (IsInvalidElseAssign(data, "filters", JsonType::OBJECT, filters, true))
		return {};

	unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR, magFilter = GL_LINEAR;

	if (filters != nullptr) {
		if (IsInvalidElseAssign<unsigned int>(filters, "min", JsonType::STRING, minFilter, minFilterTranslations, true) ||
			IsInvalidElseAssign<unsigned int>(filters, "mag", JsonType::STRING, magFilter, magFilterTranslations, true))
			return {};
	}

	//Resources.try_emplace(std::string(name), path, minFilter, magFilter);

	return Texture{ path, minFilter, magFilter };
}

std::optional<TinyModel> ModelOwner::Construct(const nlohmann::json& data) const {
	std::string source;
	if (IsInvalidElseAssign<std::string>(data, "source", JsonType::STRING, source))
		return {};

	std::filesystem::path path = ResourceManager::GetResourcePath() / "models" / source;

	//Resources.try_emplace(std::string(name), path);

	return TinyModel{ path };
}

std::optional<Cubemap> CubemapOwner::Construct(const nlohmann::json& data) const {
	std::string textures;
	if (IsInvalidElseAssign<std::string>(data, "textures", JsonType::STRING, textures))
		return {};

	std::filesystem::path path = ResourceManager::GetResourcePath() / "cubemaps" / textures;

	//Resources.try_emplace(std::string(name), path);

	return Cubemap{ path };
}

std::optional<Font> FontOwner::Construct(const nlohmann::json& data) const {

	std::string source;
	unsigned int height;

	if (IsInvalidElseAssign<std::string>(data, "source", JsonType::STRING, source) ||
		IsInvalidElseAssign<unsigned int>(data, "height", JsonType::UNSIGNED_INT, height))
		return {};

	std::filesystem::path path = ResourceManager::GetResourcePath() / "fonts" / source;

	//Resources.try_emplace(std::string(name), path, height);

	return Font{ path, height };
}

std::optional<Shader> ShaderOwner::Construct(const nlohmann::json& data) const {

	std::string vertex, fragment;

	if (IsInvalidElseAssign<std::string>(data, "vertex", JsonType::STRING, vertex) ||
		IsInvalidElseAssign<std::string>(data, "fragment", JsonType::STRING, fragment))
		return {};

	std::filesystem::path vertexPath = ResourceManager::GetResourcePath() / "shaders" / vertex;
	std::filesystem::path fragmentPath = ResourceManager::GetResourcePath() / "shaders" / fragment;

	//Resources.try_emplace(std::string(name), vertexPath, fragmentPath);

	return Shader{ vertexPath, fragmentPath };
}

std::optional<SetBatch<ResourceType>> InterfaceOwner::GetDependenciesFromJson(const nlohmann::json& data) const {
	SetBatch<ResourceType> dependencies;

	nlohmann::json textures = nullptr;

	if (IsInvalidElseAssign<nlohmann::json>(data, "textures", JsonType::OBJECT, textures, true))
		return {};

	if (textures != nullptr) {

		nlohmann::json background = nullptr;
		nlohmann::json widget = nullptr;

		if (IsInvalidElseAssign<nlohmann::json>(textures, "background", JsonType::OBJECT, background, true) ||
			IsInvalidElseAssign<nlohmann::json>(textures, "widget", JsonType::OBJECT, widget, true))
			return {};

		if (background != nullptr) {
			std::string bgName;

			if (IsInvalidElseAssign<std::string>(background, "name", JsonType::STRING, bgName))
				return {};

			dependencies.TryAdd(ResourceType::TEXTURE, bgName);
		}

		if (widget != nullptr) {
			std::string wgName;

			if (IsInvalidElseAssign<std::string>(widget, "name", JsonType::STRING, wgName))
				return {};

			dependencies.TryAdd(ResourceType::TEXTURE, wgName);
		}
	}

	std::vector<nlohmann::json> texts;
	std::vector<nlohmann::json> fields;

	if (IsInvalidElseAssignVector<nlohmann::json>(data, "texts", JsonType::OBJECT, texts, true) ||
		IsInvalidElseAssignVector<nlohmann::json>(data, "fields", JsonType::OBJECT, fields, true))
		return {};

	if (!texts.empty())
		dependencies.TryAdd(ResourceType::SHADER, "TEXT");

	for (const nlohmann::json& text : texts) {

		nlohmann::json parameters = nullptr;
		
		if (IsInvalidElseAssign<nlohmann::json>(text, "parameters", JsonType::OBJECT, parameters))
			return {};

		std::string font;

		if (IsInvalidElseAssign<std::string>(parameters, "font", JsonType::STRING, font))
			return {};

		dependencies.TryAdd(ResourceType::FONT, font);
	}

	if (!fields.empty())
		dependencies.TryAdd(ResourceType::SHADER, "COLOR");

	dependencies.TryAdd(ResourceType::SHADER, "UI");

	return dependencies;
}

std::optional<Interface> InterfaceOwner::Construct(const nlohmann::json& data) const {

	std::unordered_map<std::string, Interface::WidgetState> stateTranslations = {
	{"UNDEFINED", Interface::WidgetState::UNDEFINED},
	{"HOVERED", Interface::WidgetState::HOVERED},
	{"NOT_HOVERED", Interface::WidgetState::NOT_HOVERED},
	{"PRESSED", Interface::WidgetState::PRESSED}
	};

	std::unordered_map<std::string, VertexHelper::PosAnchorDim> anchorXTranslations = {
		{"CENTER", VertexHelper::PosAnchorDim::CENTER },
		{"LEFT", VertexHelper::PosAnchorDim::START},
		{"RIGHT", VertexHelper::PosAnchorDim::END}
	};

	std::unordered_map<std::string, VertexHelper::PosAnchorDim> anchorYTranslations = {
		{"CENTER", VertexHelper::PosAnchorDim::CENTER },
		{"BOTTOM", VertexHelper::PosAnchorDim::START},
		{"TOP", VertexHelper::PosAnchorDim::END}
	};

	std::unordered_map<std::string, GLEnums::CursorMode> modeTranslations = {
		{"NORMAL", GLEnums::CursorMode::NORMAL},
		{"HIDDEN", GLEnums::CursorMode::HIDDEN },
		{"DISABLED", GLEnums::CursorMode::DISABLED }
	};

	std::unordered_map<std::string, Interface::TextCursorMode> textCursorModeTranslations = {
		{"BAR", Interface::TextCursorMode::BAR},
		{"OVERLAY", Interface::TextCursorMode::OVERLAY },
		{"UNDERSCORE", Interface::TextCursorMode::UNDERSCORE}
	};

	GLEnums::CursorMode mode = GLEnums::CursorMode::DISABLED;
	nlohmann::json textures = nullptr;
	std::vector<nlohmann::json> widgets;
	std::vector<nlohmann::json> texts;
	std::vector<nlohmann::json> fields;

	if (IsInvalidElseAssign<GLEnums::CursorMode>(data, "cursor mode", JsonType::STRING, mode, modeTranslations, true) ||
		IsInvalidElseAssign<nlohmann::json>(data, "textures", JsonType::OBJECT, textures, true) ||
		IsInvalidElseAssignVector<nlohmann::json>(data, "widgets", JsonType::OBJECT, widgets, true) ||
		IsInvalidElseAssignVector<nlohmann::json>(data, "texts", JsonType::OBJECT, texts, true) ||
		IsInvalidElseAssignVector<nlohmann::json>(data, "fields", JsonType::OBJECT, fields, true))
		return {};

	nlohmann::json bgTex = nullptr;
	nlohmann::json wgTex = nullptr;
	std::string bgTexName = "";
	std::string wgTexName = "";
	bool warpBg = false;

	if (textures != nullptr) {

		if (IsInvalidElseAssign<nlohmann::json>(textures, "background", JsonType::OBJECT, bgTex, true) ||
			IsInvalidElseAssign<nlohmann::json>(textures, "widget", JsonType::OBJECT, wgTex, true))
			return {};

		if (wgTex != nullptr) {
			if (IsInvalidElseAssign<std::string>(wgTex, "name", JsonType::STRING, wgTexName))
				return {};
		}

		if (bgTex != nullptr) {
			if (IsInvalidElseAssign<std::string>(bgTex, "name", JsonType::STRING, bgTexName) ||
				IsInvalidElseAssign<bool>(bgTex, "warp", JsonType::BOOLEAN, warpBg))
				return {};
		}
	}

	std::map<std::string, Interface::Widget> widgetMap;


	for (const nlohmann::json& widget : widgets) {

		std::string widgetName;
		nlohmann::json parameters;
		std::string function;
		nlohmann::json anchorJson = nullptr;
		VertexHelper::PosAnchorDim anchorX = VertexHelper::PosAnchorDim::START;
		VertexHelper::PosAnchorDim anchorY = VertexHelper::PosAnchorDim::START;
		nlohmann::json transformation;
		std::vector<nlohmann::json> textureSources;
		std::string initialSourceName = "";

		if (IsInvalidElseAssign<std::string>(widget, "name", JsonType::STRING, widgetName) ||
			IsInvalidElseAssign<nlohmann::json>(widget, "parameters", JsonType::OBJECT, parameters))
			return {};

		if (IsInvalidElseAssign<std::string>(parameters, "function", JsonType::STRING, function) ||
			IsInvalidElseAssign<nlohmann::json>(parameters, "anchor", JsonType::OBJECT, anchorJson, true) ||
			IsInvalidElseAssign<nlohmann::json>(parameters, "transformation", JsonType::OBJECT, transformation) ||
			IsInvalidElseAssignVector<nlohmann::json>(parameters, "texture sources", JsonType::OBJECT, textureSources, true) ||
			IsInvalidElseAssign<std::string>(parameters, "initial source name", JsonType::STRING, initialSourceName, true))
			return {};

		if (anchorJson != nullptr) {
			if (IsInvalidElseAssign<VertexHelper::PosAnchorDim>(anchorJson, "x", JsonType::STRING, anchorX, anchorXTranslations, true) ||
				IsInvalidElseAssign<VertexHelper::PosAnchorDim>(anchorJson, "y", JsonType::STRING, anchorY, anchorYTranslations, true))
				return {};
		}

		int transX;
		int transY;
		unsigned int transWidth;
		unsigned int transHeight;

		if (IsInvalidElseAssign<int>(transformation, "x", JsonType::INTEGER, transX) ||
			IsInvalidElseAssign<int>(transformation, "y", JsonType::INTEGER, transY) ||
			IsInvalidElseAssign<unsigned int>(transformation, "width", JsonType::UNSIGNED_INT, transWidth) ||
			IsInvalidElseAssign<unsigned int>(transformation, "height", JsonType::UNSIGNED_INT, transHeight))
			return {};

		std::unordered_map<std::string, Utils::Rect> textureSourceMap;

		if (textureSources != nullptr) {

			for (const nlohmann::json& textureSource : textureSources) {

				if (IsIncorrectType(textureSource, "texture sources[element]", JsonType::OBJECT))
					return {};

				std::string tsName;
				unsigned int sourceX;
				unsigned int sourceY;
				unsigned int sourceWidth;
				unsigned int sourceHeight;

				if (IsInvalidElseAssign<std::string>(textureSource, "name", JsonType::STRING, tsName) ||
					IsInvalidElseAssign<unsigned int>(textureSource, "x", JsonType::UNSIGNED_INT, sourceX) ||
					IsInvalidElseAssign<unsigned int>(textureSource, "y", JsonType::UNSIGNED_INT, sourceY) ||
					IsInvalidElseAssign<unsigned int>(textureSource, "width", JsonType::UNSIGNED_INT, sourceWidth) ||
					IsInvalidElseAssign<unsigned int>(textureSource, "height", JsonType::UNSIGNED_INT, sourceHeight))
					return {};

				Utils::Rect rect{ sourceX, sourceY, sourceWidth, sourceHeight };
				textureSourceMap.try_emplace(tsName, rect);
			}
		}

		if (initialSourceName != "" && textureSourceMap.empty()) {// ERROR
			PrintMessage("ERROR", "Initial source name unknown and source map is empty");
			return {};
		}

		widgetMap.try_emplace(widgetName, function, VertexHelper::PosAnchor{ anchorX, anchorY }, transX, transY, transWidth, transHeight, textureSourceMap, initialSourceName);
	}

	std::unordered_map<std::string, Text> textMap;

	for (const nlohmann::json& text : texts) {

		std::string textName;
		nlohmann::json parameters;

		if (IsInvalidElseAssign(text, "name", JsonType::STRING, textName) ||
			IsInvalidElseAssign(text, "parameters", JsonType::OBJECT, parameters))
			return {};

		std::string font;
		std::string string;
		std::array<float, 3> color;
		nlohmann::json anchorJson = nullptr;
		VertexHelper::PosAnchorDim anchorX = VertexHelper::PosAnchorDim::START;
		VertexHelper::PosAnchorDim anchorY = VertexHelper::PosAnchorDim::START;
		std::array<int, 2> pos;
		float scale = 1.0f;
		unsigned int lineSpacing = 10;

		if (IsInvalidElseAssign<std::string>(parameters, "font", JsonType::STRING, font) ||
			IsInvalidElseAssign<std::string>(parameters, "string", JsonType::STRING, string) ||
			IsInvalidElseAssignArray<float, 3>(parameters, "color", JsonType::FLOAT, color) ||
			IsInvalidElseAssign<nlohmann::json>(parameters, "anchor", JsonType::OBJECT, anchorJson, true) ||
			IsInvalidElseAssignArray<int, 2>(parameters, "pos", JsonType::INTEGER, pos) ||
			IsInvalidElseAssign<float>(parameters, "scale", JsonType::FLOAT, scale, true) ||
			IsInvalidElseAssign<unsigned int>(parameters, "line spacing", JsonType::UNSIGNED_INT, lineSpacing, true))
			return {};

		if (anchorJson != nullptr) {
			if (IsInvalidElseAssign<VertexHelper::PosAnchorDim>(anchorJson, "x", JsonType::STRING, anchorX, anchorXTranslations, true) ||
				IsInvalidElseAssign<VertexHelper::PosAnchorDim>(anchorJson, "y", JsonType::STRING, anchorY, anchorYTranslations, true))
				return {};
		}

		nlohmann::json bounding = nullptr;
		if (IsInvalidElseAssign(parameters, "bounding", JsonType::OBJECT, bounding, true))
			return {};

		std::optional<unsigned int> width;
		std::optional<unsigned int> height;

		nlohmann::json::const_iterator boundingIt = parameters.find("bounding");
		if (bounding != nullptr) {

			nlohmann::json::const_iterator widthIt = bounding.find("width");
			nlohmann::json::const_iterator heightIt = bounding.find("height");

			if (widthIt != bounding.end()) {

				nlohmann::json widthJson = widthIt.value();

				if (widthJson != "AUTO") {
					if (IsIncorrectType(widthJson, "width", JsonType::UNSIGNED_INT))
						return {};

					width.emplace(static_cast<unsigned int>(widthJson));
				}
			}

			if (heightIt != bounding.end()) {
				nlohmann::json heightJson = heightIt.value();

				if (heightJson != "AUTO") {
					if (IsIncorrectType(heightJson, "height", JsonType::UNSIGNED_INT))
						return {};

					height.emplace(static_cast<unsigned int>(heightJson));
				}
			}
		}
		textMap.try_emplace(textName, font, string, Utils::ToVec(color), VertexHelper::PosAnchor{anchorX, anchorY}, Utils::ToVec(pos), scale, lineSpacing, width, height);
	}

	std::unordered_map<std::string, Interface::Field> fieldMap;

	for (const nlohmann::json& field : fields) {

		std::string fieldName;
		nlohmann::json parameters;
		std::string textRef;
		nlohmann::json cursor;
		unsigned int maxChars = 99999;
		Interface::TextCursorMode textCursorMode = Interface::TextCursorMode::BAR;

		if (IsInvalidElseAssign<std::string>(field, "name", JsonType::STRING, fieldName) ||
			IsInvalidElseAssign<nlohmann::json>(field, "parameters", JsonType::OBJECT, parameters))
			return {};

		if (IsInvalidElseAssign<std::string>(parameters, "reference", JsonType::STRING, textRef) ||
			IsInvalidElseAssign<nlohmann::json>(parameters, "cursor", JsonType::OBJECT, cursor) ||
			IsInvalidElseAssign<unsigned int>(parameters, "max chars", JsonType::UNSIGNED_INT, maxChars, true) ||
			IsInvalidElseAssign<Interface::TextCursorMode>(parameters, "text cursor mode", JsonType::STRING, textCursorMode, textCursorModeTranslations, true))
			return {};

		bool cursorMovable = false;
		float cursorBlinkHoldTime = 0.5f;

		if (IsInvalidElseAssign<bool>(cursor, "movable", JsonType::BOOLEAN, cursorMovable, true) ||
			IsInvalidElseAssign<float>(cursor, "blink hold time", JsonType::FLOAT, cursorBlinkHoldTime, true))
			return {};

		fieldMap.try_emplace(fieldName, textRef, cursorMovable, cursorBlinkHoldTime, maxChars, textCursorMode);
	}

	std::unordered_map<std::string, Interface::Slider> sliderMap;

	//Resources.try_emplace(std::string(name), mode, bgTexName, wgTexName, widgetMap, textMap, fieldMap, sliderMap);

	return Interface{ mode, bgTexName, warpBg, wgTexName, widgetMap, textMap, fieldMap, sliderMap };
}

std::optional<SetBatch<ResourceType>> SceneOwner::GetDependenciesFromJson(const nlohmann::json& data) const {

	SetBatch<ResourceType> dependencies;

	std::unordered_map<std::string_view, Scene::LightingType> lightingMap{
		{"BLINN_PHONG", Scene::LightingType::BLINN_PHONG},
		{"PBR", Scene::LightingType::PBR}
	};

	Scene::LightingType lighting;

	if (IsInvalidElseAssign<Scene::LightingType>(data, "lighting", JsonType::STRING, lighting, lightingMap))
		return {};

	switch (lighting) {
	case Scene::LightingType::BLINN_PHONG:
		dependencies.TryAdd(ResourceType::SHADER, "MODEL");
		break;
	case Scene::LightingType::PBR:
		dependencies.TryAdd(ResourceType::SHADER, "PBR");
	}

	std::vector<nlohmann::json> objects;
	std::vector<std::string> skyboxes;

	if (IsInvalidElseAssignVector<nlohmann::json>(data, "objects", JsonType::OBJECT, objects, true) ||
		IsInvalidElseAssignVector<std::string>(data, "skyboxes", JsonType::STRING, skyboxes, true))
		return {};

	if (!objects.empty())
		dependencies.TryAdd(ResourceType::SHADER, "OUTLINE");

	for (const nlohmann::json& object : objects) {

		std::string modelName;
		if (IsInvalidElseAssign<std::string>(object, "model", JsonType::STRING, modelName))
			return {};

		dependencies.TryAdd(ResourceType::MODEL, modelName);
	}

	if (!skyboxes.empty())
		dependencies.TryAdd(ResourceType::SHADER, "SKYBOX");

	for (const std::string& skybox : skyboxes)
		dependencies.TryAdd(ResourceType::CUBEMAP, skybox);

	return dependencies;
}

std::optional<Scene> SceneOwner::Construct(const nlohmann::json& data) const {
	std::unordered_map<std::string, Scene::LightingType> lightingMap{
		{"BLINN_PHONG", Scene::LightingType::BLINN_PHONG},
		{"PBR", Scene::LightingType::PBR}
	};

	Scene::LightingType lighting;
	bool fog = false;
	std::array<float, 3> background = { 1.0f, 1.0f, 1.0f };
	std::vector<nlohmann::json> lights;
	std::vector<nlohmann::json> cameras;
	std::vector<nlohmann::json> objects;
	std::vector<std::string> skyboxes;

	if (IsInvalidElseAssign<Scene::LightingType>(data, "lighting", JsonType::STRING, lighting, lightingMap) ||
		IsInvalidElseAssign<bool>(data, "fog", JsonType::BOOLEAN, fog, true) ||
		IsInvalidElseAssignArray<float, 3>(data, "background", JsonType::FLOAT, background, true) ||
		IsInvalidElseAssignVector<nlohmann::json>(data, "lights", JsonType::OBJECT, lights, true) ||
		IsInvalidElseAssignVector<nlohmann::json>(data, "cameras", JsonType::OBJECT, cameras, true) ||
		IsInvalidElseAssignVector<nlohmann::json>(data, "objects", JsonType::OBJECT, objects, true) ||
		IsInvalidElseAssignVector<std::string>(data, "skyboxes", JsonType::STRING, skyboxes, true))
		return {};

	//if (ErrorManager::Assert(lightingMap.contains(lighting), "Invalid lighting type"))
	//return true;

	std::vector<PointLight> lightVector; // TODO: Consider using reserve for performance

	for (const nlohmann::json& light : lights) {

		std::array<float, 3> pos;
		std::array<float, 3> color;

		if (IsInvalidElseAssignArray<float, 3>(light, "pos", JsonType::FLOAT, pos) ||
			IsInvalidElseAssignArray<float, 3>(color, "color", JsonType::FLOAT, color))
			return {};

		lightVector.emplace_back(glm::make_vec3(pos.data()), glm::make_vec3(color.data()));
	}

	std::vector<Camera> cameraVector;

	for (const nlohmann::json& camera : cameras) {

		std::array<float, 3> pos;
		float yaw = 0.0f, pitch = 0.0f, fov = 45.0f;

		if (IsInvalidElseAssignArray<float, 3>(camera, "pos", JsonType::FLOAT, pos) ||
			IsInvalidElseAssign<float>(camera, "yaw", JsonType::FLOAT, yaw, true) ||
			IsInvalidElseAssign<float>(camera, "pitch", JsonType::FLOAT, pitch, true) ||
			IsInvalidElseAssign<float>(camera, "fov", JsonType::FLOAT, fov, true))
			return {};

		cameraVector.emplace_back(glm::make_vec3(pos.data()), glm::radians(yaw), glm::radians(pitch), glm::radians(fov));
	}

	std::unordered_map<std::string, Scene::SceneObject> objectMap;

	for (const nlohmann::json& object : objects) {

		std::string objName;
		std::string modelName;
		nlohmann::json transformJson = nullptr;
		glm::mat4 transform(1.0f);

		if (IsInvalidElseAssign<std::string>(object, "name", JsonType::STRING, objName) ||
			IsInvalidElseAssign<std::string>(object, "model", JsonType::STRING, modelName) ||
			IsInvalidElseAssign<nlohmann::json>(object, "transform", JsonType::OBJECT, transformJson, true))
			return {};

		if (transformJson != nullptr) {

			std::array<float, 3> translation = { 0.0f, 0.0f, 0.0f };
			nlohmann::json rotation = nullptr;
			std::array<float, 3> scale = { 1.0f, 1.0f, 1.0f };

			if (IsInvalidElseAssignArray<float, 3>(transformJson, "translation", JsonType::FLOAT, translation, true) ||
				IsInvalidElseAssign<nlohmann::json>(transformJson, "rotation", JsonType::OBJECT, rotation, true) ||
				IsInvalidElseAssignArray<float, 3>(transformJson, "scale", JsonType::FLOAT, scale, true))
				return {};

			if (translation != std::array{ 0.0f, 0.0f, 0.0f })
				transform = glm::translate(transform, Utils::ToVec(translation));

			if (rotation != nullptr) {

				std::array<float, 3> rotationVector;
				float rotationAngle;

				if (IsInvalidElseAssignArray<float, 3>(rotation, "vector", JsonType::FLOAT, rotationVector) ||
					IsInvalidElseAssign<float>(rotation, "angle", JsonType::FLOAT, rotationAngle))
					return {};

				glm::vec3 vRotation = Utils::ToVec(rotationVector);

				if (vRotation == glm::vec3(0.0f, 0.0f, 0.0f)) {
					PrintMessage("ERROR", "Rotation vector for an object in a scene cannot be (0, 0, 0)");
					return {};
				}

				transform = glm::rotate(transform, glm::radians(rotationAngle), glm::normalize(vRotation)); // translate then rotate because glm does opposite order
			}

			if (scale != std::array{ 1.0f, 1.0f, 1.0f })
				transform = glm::scale(transform, Utils::ToVec(scale));

		}

		objectMap.try_emplace(modelName, modelName, transform);
	}

	//Resources.try_emplace(std::string(name), lighting, fog, Utils::ToVec(background), lightVector, cameraVector, modelVector, skyboxes);

	return Scene{ lighting, fog, Utils::ToVec(background), lightVector, cameraVector, objectMap, skyboxes };
}