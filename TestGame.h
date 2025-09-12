#pragma once

#define USE_FUNCTIONS

#include <string>
#include <filesystem>
#include "Application.h"
#include "CallbackManager.h"

class TestGame : public Initializable, public Callbackable {

public:
	TestGame(const std::string& name, unsigned short width, unsigned short height);

	void Run();

	void Update();

	// callback functions
	void FramebufferSizeCallback(int width, int height) override;
	void KeyCallback(GLEnums::Key key, GLEnums::KeyAction action, int scancode, int mods) override;
	void CursorPosCallback(double xPos, double yPos) override;
	void ScrollCallback(double xOffset, double yOffset) override;
	void MouseButtonCallback(GLEnums::MouseButton button, bool press, int mods) override;
	void CharCallback(char character) override;

private:

	Application Engine;

	float CameraMoveSpeed;
	float CameraRotateSpeed;
	float CameraZoomSpeed;
};

