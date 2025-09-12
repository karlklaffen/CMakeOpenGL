#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <unordered_set>
#include <format>

#include "SmartEnum.h"

//enum class ResourceType {
//	TEXTURE,
//	MODEL,
//	CUBEMAP,
//	FONT,
//	SHADER,
//	INTERFACE,
//	SCENE
//};

class NotCopyable {

public:
	//Resource(ResourceType type, std::string_view details);
	NotCopyable();

	NotCopyable(NotCopyable&& original) noexcept;

	NotCopyable(const NotCopyable& original) = delete;
	NotCopyable& operator=(const NotCopyable& original) = delete;

protected:
	bool IsDeprecated() const;

private:
	bool Deprecated;
};