#include "TestGame.h"
#include "ResourceManager.h"

TestGame::TestGame(const std::string& name, unsigned short width, unsigned short height) : Engine(name, width, height, std::filesystem::current_path().parent_path() / "resources"), CameraMoveSpeed(2.0f), CameraRotateSpeed(0.01f), CameraZoomSpeed(2.0f),
Initializable("Game") {
	
	
	if (Engine.InitializationIncomplete()) {
		std::cout << "Engine not properly initialized" << std::endl;
		return; // error
	}

	CallbackManager::InitCallbacks(&Engine, this);

	ConfirmInitialization();

	// TODO: Define scene-interface batches into json file? Maybe not needed
	//Engine.AddSceneInterfaceBatch("B_TEST", { "S_LAKE" }, { "I_SCENE", "I_PAUSE" });

	Engine.AddWidgetFunction("F_UNPAUSE", [this](const Interface::WidgetFuncData& data) {
		switch (data.Action) {
		case Interface::WidgetAction::UNPRESS:
			Engine.SwapInterfaces({ "I_PAUSE" }, { "I_SCENE" });
			break;
		}
		}
	);

	Engine.AddWidgetFunction("F_QUIT", [this](const Interface::WidgetFuncData& data) {
		switch (data.Action) {
		case Interface::WidgetAction::UNPRESS:
			Engine.Stop();
			break;
		}
		}
	);

#ifdef USE_FUNCTIONS

	Engine.AddButtonFunction("F_SWAP", [this](bool press) {

		if (press) {
			Engine.SwapInterfaces({ "I_SCENE" }, { "I_PAUSE" });
		}

		}
	);

	Engine.AddButtonFunction("F_STOP", [this](bool press) {
		if (press) {
			Engine.Stop();
		}
		}
	);

	Engine.AddButtonFunction("F_RELOAD_SHADERS", [this](bool press) {
		if (press)
			Engine.TryReloadAllOfType(ResourceType::SHADER);
		}
	);

	Engine.AddScrollFunction("F_SPEED", [this](double xOffset, double yOffset) {
		float newSpeed = CameraRotateSpeed + yOffset;
		if (newSpeed > 0)
			CameraRotateSpeed = newSpeed;
		}
	);

	Engine.BindKeyButtonFunction(GLEnums::Key::P, "F_SWAP");
	Engine.BindKeyButtonFunction(GLEnums::Key::Q, "F_STOP");
	Engine.BindKeyButtonFunction(GLEnums::Key::R, "F_RELOAD_SHADERS");
	Engine.BindScrollFunction("F_SPEED");

#endif

	Engine.TryLoadResources(ResourceType::SCENE, { "S_LAKE" });
	Engine.TryLoadResources(ResourceType::INTERFACE, { "I_SCENE", "I_PAUSE" });

	Engine.SwapScene("S_LAKE");
	Engine.EnableInterface("I_PAUSE");
}

void TestGame::Run() {
	while (Engine.Continue()) {

		// update
		Update();

		Engine.Render();
		Engine.PollEvents();
	}
}

void TestGame::Update() {

	if (!Engine.GetActiveSceneName().empty()) {

		if (Engine.InterfaceEnabled("I_SCENE")) {

			Camera& activeCamera = ResourceManager::GetScenes().Get(Engine.GetActiveSceneName()).GetActiveCamera();
			const glm::vec3& forward = activeCamera.GetFPSForward();
			const glm::vec3& right = activeCamera.GetFPSRight();
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f);

			float dt = Engine.GetDeltaTime();

			if (Engine.KeyHeld(GLEnums::Key::W)) {
				activeCamera.Move(CameraMoveSpeed * forward * dt);
			}
			if (Engine.KeyHeld(GLEnums::Key::S)) {
				activeCamera.Move(-CameraMoveSpeed * forward * dt);
			}
			if (Engine.KeyHeld(GLEnums::Key::D)) {
				activeCamera.Move(CameraMoveSpeed * right * dt);
			}
			if (Engine.KeyHeld(GLEnums::Key::A)) {
				activeCamera.Move(-CameraMoveSpeed * right * dt);
			}
			if (Engine.KeyHeld(GLEnums::Key::SPACE)) {
				activeCamera.Move(CameraMoveSpeed * up * dt);
			}
			if (Engine.KeyHeld(GLEnums::Key::LEFT_SHIFT) || Engine.KeyHeld(GLEnums::Key::RIGHT_SHIFT)) {
				activeCamera.Move(-CameraMoveSpeed * up * dt);
			}

			const glm::vec3& pos = activeCamera.GetPos();

			std::string fpsString = std::format("FPS: {}", std::to_string(1 / Engine.GetDeltaTime()));
			std::string posString = std::format("POS: ({}, {}, {})", std::to_string(pos.x), std::to_string(pos.y), std::to_string(pos.z));

			if (Engine.InterfaceEnabled("I_SCENE")) {
				ResourceManager::GetInterfaces().Get("I_SCENE").TrySetTextString("T_INFO", fpsString + "\n" + posString);
			}
		}
	}
}

void TestGame::FramebufferSizeCallback(int width, int height) {
	
}

void TestGame::KeyCallback(GLEnums::Key key, GLEnums::KeyAction action, int scancode, int mods) {
#ifndef USE_FUNCTIONS

	if (action == GLEnums::KeyAction::REPEAT)
		return;

	bool press = action == GLEnums::KeyAction::PRESS;

	switch (key) {
	case GLEnums::Key::P:
		if (press)
			Engine.SwapInterfaces({ "I_SCENE" }, { "I_PAUSE" });
		break;
	case GLEnums::Key::R:
		if (press) {
			ResourceManager::GetShaders().TryReloadAll();
			std::cout << "Shaders Reloaded!" << std::endl;
		}
		break;
	case GLEnums::Key::B:
		/*if (press)
			ResourceManager::*/
		break;
	case GLEnums::Key::Q:
		if (press)
			Engine.Stop();
		break;
	}

#endif
}

void TestGame::CursorPosCallback(double xPos, double yPos) {
	if (Engine.GetCursorMode() == GLEnums::CursorMode::DISABLED) {
		if (!Engine.GetActiveSceneName().empty()) {
			glm::vec2 offset = Engine.GetCursorOffset();
			ResourceManager::GetScenes().Get(Engine.GetActiveSceneName()).MoveCameraBasedOnCursor(CameraRotateSpeed * offset.x, CameraRotateSpeed * offset.y);
		}
	}
}

void TestGame::ScrollCallback(double xOffset, double yOffset) {
#ifndef USE_FUNCTIONS

	if (!Engine.GetActiveSceneName().empty()) {
		float dt = Engine.GetDeltaTime();

		Scene& scene = ResourceManager::GetScenes().Get(Engine.GetActiveSceneName());
		Camera& cam = scene.GetActiveCamera();

		float newZoomDeg = glm::degrees(cam.GetFOVRad() + CameraZoomSpeed * yOffset * dt);

		if (newZoomDeg > 20.0f && newZoomDeg < 120.0f) {
			std::cout << newZoomDeg << std::endl;
			cam.SetFOVDeg(newZoomDeg);
		}
	}

#endif
}

void TestGame::MouseButtonCallback(GLEnums::MouseButton button, bool press, int mods) {
	if (!Engine.GetActiveSceneName().empty() && Engine.InterfaceEnabled("I_SCENE")) {
		switch (button) {
		case GLEnums::MouseButton::LEFT:
			if (press) {
				Scene& scene = ResourceManager::GetScenes().Get(Engine.GetActiveSceneName());
				const Camera& cam = scene.GetActiveCamera();
				std::optional<Scene::MeshPick> pick = scene.TryMousePick(cam.GetPos(), cam.GetForward());
				if (pick.has_value())
					scene.ToggleObjectOutline(pick.value().Name);
			}
			break;
		}
	}
}

void TestGame::CharCallback(char character) {

}
