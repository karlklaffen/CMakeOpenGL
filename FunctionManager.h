#pragma once

#include <functional>
#include <map>
#include <string>
#include <iostream>
#include <queue>

#include "Interface.h"
#include "GLEnums.h"

class FunctionManager {

public:
	// TODO: Consider having Application use std::bind for functions so that they don't have to be separated based on parameters
	void CacheWidgetFunction(std::string_view name, const std::function<void(const Interface::WidgetFuncData& data)>& func);
	void RemoveWidgetFunction(std::string_view name);
	bool QueueWidgetFunctionExecution(std::string_view name, const Interface::WidgetFuncData& data);

	void CacheButtonFunction(std::string_view name, const std::function<void(bool press)>& func);
	void RemoveButtonFunction(std::string_view name);
	bool QueueButtonFunctionExecution(std::string_view name, bool press);

	void CacheScrollFunction(std::string_view name, const std::function<void(double xOffset, double yOffset)>& func);
	void RemoveScrollFunction(std::string_view name);
	bool QueueScrollFunctionExecution(std::string_view name, double xOffset, double yOffset);

	void ExecuteQueuedFunctions();

private:

	std::unordered_map<std::string, std::function<void(const Interface::WidgetFuncData& action)>> WidgetFuncs; // uses string because needs to be referentiable in resource file
	std::unordered_map<std::string, std::function<void(bool press)>> ButtonFuncs;
	std::unordered_map<std::string, std::function<void(double xOffset, double yOffset)>> ScrollFuncs;

	std::queue<std::function<void()>> FuncQueue;
};