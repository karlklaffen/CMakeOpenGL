#pragma once

//#define USE_INDIRECT

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtx/string_cast.hpp>

#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <random>
#include <set>
#include <sstream>

#include "stb_image.h"

#include "Shader.h"
#include "Scene.h"
#include "Font.h"
#include "Interface.h"
#include "VertexHelper.h"
#include "Initializable.h"
#include "Utils.h"
#include "VertexGroup.h"
#include "SetBatch.h"
#include "prettyprint.hpp"

#include "FunctionManager.h"
#include "ErrorManager.h"
#include "Callbackable.h"

#include "GLEnums.h"
#include "SelectedFieldInfo.h"

#include "GLFWindowHolder.h"
#include "ResourceManager.h"

struct DrawElementsIndirectCommand {
	unsigned int count;
	unsigned int instanceCount;
	unsigned int firstIndex;
	int baseVertex;
	unsigned int baseInstance;
};

struct SceneInterfaceBatch {
	std::unordered_set<std::string> SceneNames;
	std::unordered_set<std::string> InterfaceNames;
};

class Application : public Initializable, public Callbackable {

public:
	Application(std::string_view title, unsigned short w, unsigned short h, const std::filesystem::path& resourcesPath, unsigned short versionMajor = 4, unsigned short versionMinor = 6);
	~Application();

	void FramebufferSizeCallback(int width, int height) override;
	void KeyCallback(GLEnums::Key key, GLEnums::KeyAction action, int scancode, int mods) override;
	void CursorPosCallback(double xPos, double yPos) override;
	void ScrollCallback(double xOffset, double yOffset) override;
	void MouseButtonCallback(GLEnums::MouseButton button, bool press, int mods) override;
	void CharCallback(char character) override;


	GLFWwindow* GetWindowPointer();

	bool Render() const;
	void PollEvents();

	bool TryLoadResources(ResourceType type, const std::unordered_set<std::string>& names);
	bool TryUnloadResources(ResourceType type, const std::unordered_set<std::string>& names);
	bool TryReloadAllOfType(ResourceType type);
	bool TryReloadAll();
	bool TryMakeLoadedOnly(const SetBatch<ResourceType>& batch);
	/*bool LoadScenes(const std::unordered_set<std::string>& names);
	bool LoadInterfaces(const std::unordered_set<std::string>& names);*/

	bool SwapScene(std::string_view name);

	bool EnableInterface(std::string_view name);
	bool EnableInterfaces(const std::unordered_set<std::string>& names);

	bool DisableInterface(std::string_view names);
	bool DisableInterfaces(const std::unordered_set<std::string>& names);

	bool InterfaceEnabled(std::string_view name);
	bool InterfacesEnabled(const std::unordered_set<std::string>& names);

	bool SwapInterfaces(const std::unordered_set<std::string>& first, const std::unordered_set<std::string>& second);

	void AddScrollFunction(std::string_view name, const std::function<void(double xOffset, double yOffset)>& func);
	void BindScrollFunction(std::string_view name);

	void AddButtonFunction(std::string_view name, const std::function<void(bool press)>& func);
	void BindKeyButtonFunction(GLEnums::Key key, std::string_view name);
	void BindMouseButtonFunction(GLEnums::MouseButton button, std::string_view name);

	void AddWidgetFunction(std::string_view name, const std::function<void(const Interface::WidgetFuncData& data)>& func);

	void SetCursorMode(GLEnums::CursorMode mode);
	GLEnums::CursorMode GetCursorMode();

	bool KeyHeld(GLEnums::Key key);
	bool MouseButtonHeld(GLEnums::MouseButton button);

	// updates delta time, returns false if game should end
	bool Continue();
	void UpdateTime();

	//bool AddSceneInterfaceBatch(std::string_view name, const std::unordered_set<std::string>& scenes, const std::unordered_set<std::string>& interfaces);
	//bool RemoveSceneInterfaceBatch(std::string_view name);
	//void LoadSceneInterfaceBatch(std::string_view name, std::string_view loadingInterfaceName = "");

	std::string_view GetActiveSceneName();

	float GetDeltaTime() const;
	const glm::vec2& GetCursorOffset() const;

	unsigned int GetWindowWidth() const;
	unsigned int GetWindowHeight() const;

	void Stop();

private:

	bool RenderScene(std::string_view sceneName) const;
	bool RenderUI(std::string_view interfaceName) const;
	bool RenderText(const Text& text) const;

	void UpdateFrame(unsigned int w, unsigned int h);

	GLEnums::CursorMode GetPriorityMode(const std::unordered_set<std::string>& interfaceNames);

	void UpdateInterfaceWidgets();

	bool SelectField(std::string_view interfaceName, std::string_view fieldName, glm::uvec2 cursorPos);
	bool DeselectField();

	FunctionManager Functions;

	GLFWwindow* Window;

	double LastFrameTime;
	double DeltaTime;

	glm::vec3 SelectionColor;

	std::unordered_map<std::string, std::array<float, 9>> Kernels;
	std::string ActiveKernel;

	/*std::unordered_map<std::string, SceneInterfaceBatch> SceneInterfaceBatches;
	std::string LoadedSceneInterfaceBatch;*/

	std::string ActiveScene;

	std::unordered_set<std::string> ActiveInterfaces;

	std::vector<DrawElementsIndirectCommand> IndirectCommands;
	unsigned int BigVAO, BigVBO, BigEBO, BigMBO, IBO;

	VertexGroup FrameGroup;
	unsigned int/* FrameVAO, FrameVBO, FrameEBO,*/ FBO, RBO, FrameTextureID;

	std::default_random_engine RandomGenerator;

	bool Initialized;

	int WindowWidth, WindowHeight;

	double MouseSensitivity;
	glm::vec2 CursorPos;
	glm::vec2 CursorOffset;
	bool HasMovedMouse;

	std::string ActiveScrollFuncName;

	std::optional<SelectedFieldInfo> FieldSelectionInfo;

	std::unordered_map<GLEnums::Key, std::string> KeyButtonFuncMaps;
	std::unordered_map<GLEnums::MouseButton, std::string> MouseButtonFuncMaps; // GLFW mouse button to func id

	SetBatch<ResourceType> LoadedResources;
		
	inline static std::array<bool, 349> Keys;
	inline static std::array<bool, 8> Mouse;
};