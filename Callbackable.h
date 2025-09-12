#pragma once

#include "GLEnums.h"

class Callbackable {

public:
	virtual void FramebufferSizeCallback(int width, int height) = 0;

	virtual void KeyCallback(GLEnums::Key key, GLEnums::KeyAction action, int scancode, int mods) = 0;

	virtual void CursorPosCallback(double xPos, double yPos) = 0;

	virtual void ScrollCallback(double xOffset, double yOffset) = 0;

	virtual void MouseButtonCallback(GLEnums::MouseButton button, bool press, int mods) = 0; // default mods does not have special type for now

	virtual void CharCallback(char character) = 0;
};