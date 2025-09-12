#pragma once

#include "Application.h"
#include "Callbackable.h"

namespace CallbackManager { // call functions in both framework and base game as framework may need to do behind-the-scenes stuff (auto-resizing interfaces, setting key values, etc.), whereas game should have freedom to also handle callbacks

	static Application* Engine;
	static Callbackable* Game;

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) { // test making callback manager a class
		Engine->FramebufferSizeCallback(width, height);
		Game->FramebufferSizeCallback(width, height);
	}

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Engine->KeyCallback((GLEnums::Key)key, (GLEnums::KeyAction)action, scancode, mods);
		Game->KeyCallback((GLEnums::Key)key, (GLEnums::KeyAction)action, scancode, mods); // we don't care about differentiating between GLFW_PRESS and GLFW_REPEAT as no one will actually use GLFW_REPEAT
	}

	static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
		double newYPos = Engine->GetWindowHeight() - yPos;
		Engine->CursorPosCallback(xPos, newYPos);
		Game->CursorPosCallback(xPos, newYPos);
	}

	static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
		Engine->ScrollCallback(xOffset, -yOffset);
		Game->ScrollCallback(xOffset, -yOffset);
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		Engine->MouseButtonCallback((GLEnums::MouseButton)button, action == GLFW_PRESS, mods);
		Game->MouseButtonCallback((GLEnums::MouseButton)button, action == GLFW_PRESS, mods);
	}

	static void CharCallback(GLFWwindow* window, unsigned int codepoint) {
		Engine->CharCallback((char)codepoint);
		Game->CharCallback((char)codepoint);
	}

	static void InitCallbacks(Application* engine, Callbackable* game) {
		GLFWwindow* window = engine->GetWindowPointer();
		glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetCursorPosCallback(window, CursorPosCallback);
		glfwSetScrollCallback(window, ScrollCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetCharCallback(window, CharCallback);
		Engine = engine;
		Game = game;
	}

};