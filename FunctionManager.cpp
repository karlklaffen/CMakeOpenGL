#include "FunctionManager.h"

void FunctionManager::CacheWidgetFunction(std::string_view name, const std::function<void(const Interface::WidgetFuncData& data)>& func) {
	WidgetFuncs.try_emplace(std::string(name), func);
}

void FunctionManager::RemoveWidgetFunction(std::string_view name) {
	WidgetFuncs.erase(std::string(name));
}

bool FunctionManager::QueueWidgetFunctionExecution(std::string_view name, const Interface::WidgetFuncData& data) {

	std::unordered_map<std::string, std::function<void(const Interface::WidgetFuncData& data)>>::iterator it = WidgetFuncs.find(std::string(name));

	if (it == WidgetFuncs.end())
		return true;

	//std::cout << "FunctionManager: Executing Widget Function with name (" << name << ") and data (WidgetAction = " << (int)data.Action << ", WidgetName = " << data.WidgetName << ")" << std::endl;

	/*WidgetFuncs.at(name)(data);*/
	FuncQueue.push(std::bind(it->second, data));
	return false;
}

void FunctionManager::CacheButtonFunction(std::string_view name, const std::function<void(bool press)>& func) {
	ButtonFuncs.try_emplace(std::string(name), func);
}

void FunctionManager::RemoveButtonFunction(std::string_view name) {
	ButtonFuncs.erase(std::string(name));
}

bool FunctionManager::QueueButtonFunctionExecution(std::string_view name, bool press) {
	std::unordered_map<std::string, std::function<void(bool press)>>::iterator it = ButtonFuncs.find(std::string(name));

	if (it == ButtonFuncs.end())
		return true;

	//std::cout << "FunctionManager: Executing Button Function with name (" << name << ") and press value (" << press << ")" << std::endl;

	//ButtonFuncs.at(name)(press);
	
	FuncQueue.push(std::bind(it->second, press));
	return false;
}

void FunctionManager::CacheScrollFunction(std::string_view name, const std::function<void(double xOffset, double yOffset)>& func) {
	ScrollFuncs.try_emplace(std::string(name), func);
}

void FunctionManager::RemoveScrollFunction(std::string_view name) {
	ScrollFuncs.erase(std::string(name));
}

bool FunctionManager::QueueScrollFunctionExecution(std::string_view name, double xOffset, double yOffset) {

	std::unordered_map<std::string, std::function<void(double xOffset, double yOffset)>>::iterator it = ScrollFuncs.find(std::string(name));

	if (it == ScrollFuncs.end())
		return true;

	//std::cout << "FunctionManager: Executing Scroll Function with name (" << name << ") and data (xOffset = " << xOffset << ", yOffset = " << yOffset << ")" << std::endl;

	//ScrollFuncs.at(name)(xOffset, yOffset);
	FuncQueue.push(std::bind(it->second, xOffset, yOffset));
	return false;
}

void FunctionManager::ExecuteQueuedFunctions() {

	while (!FuncQueue.empty()) {
		FuncQueue.front()();
		FuncQueue.pop();
	}
}