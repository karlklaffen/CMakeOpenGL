#pragma once

#include "SetBatch.h"
#include "Texture.h"
#include "Model.h"
#include "Font.h"
#include "Cubemap.h"
#include "Interface.h"
#include "Shader.h"
#include "Scene.h"

#include <string>
#include <vector>
#include <functional>
#include <type_traits>
#include <optional>
#include <iomanip>

enum class ResourceType {
	TEXTURE,
	MODEL,
	CUBEMAP,
	FONT,
	SHADER,
	INTERFACE,
	SCENE
};

enum class JsonType {
	NUMBER,
	BOOLEAN,
	FLOAT,
	INTEGER,
	UNSIGNED_INT,
	STRING,
	OBJECT,
	ARRAY
};

class ResourceOwner {

public:
	ResourceOwner(ResourceType type);

	void SwapCatalogSource(const std::filesystem::path& folderPath, std::string_view identifier);

	std::optional<SetBatch<ResourceType>> GetDependencies(std::string_view name) const;

	bool TryLoadMultiple(const std::unordered_set<std::string>& names);
	bool TryUnloadMultiple(const std::unordered_set<std::string>& names);
	bool TryReloadMultiple(const std::unordered_set<std::string>& names);

	virtual bool TryLoad(std::string_view name) = 0;
	virtual bool TryUnload(std::string_view name) = 0;
	virtual bool TryReload(std::string_view name) = 0;

	virtual bool TryUnloadAll() = 0;
	virtual bool TryReloadAll() = 0;

	virtual bool IsLoaded(std::string_view name) const = 0;

protected:

	nlohmann::json GetDataFromFile(const std::filesystem::path& path) const;
	nlohmann::json GetData(std::string_view name) const;

	//std::string GetTypeString() const;
	std::string GetSpecificResourceInfo(std::string_view resourceName) const;

	void PrintMessage(std::string_view type, std::string_view resourceName, std::string_view info) const;
	void PrintMessage(std::string_view type, std::string_view info) const;

	bool IsIncorrectType(const nlohmann::json& val, std::string_view dataName, JsonType type) const;
	bool HandleIfInvalid(const nlohmann::json& base, std::string_view dataName, JsonType type, bool hasDefault) const;

	bool DependenciesLoaded(std::string_view name) const;

	virtual std::optional<SetBatch<ResourceType>> GetDependenciesFromJson(const nlohmann::json& data) const;

	std::filesystem::path CatalogFolderPath;
	std::string Identifier;

	ResourceType Type;

protected:

	template<typename T>
	bool IsInvalidElseAssign(const nlohmann::json& base, std::string_view dataName, JsonType type, T& insert, bool hasDefault = false) const {

		// check that data exists/is valid
		nlohmann::json::const_iterator it = base.find(dataName);

		if (it == base.end()) {
			if (hasDefault) // if member has a default value, don't worry if it's not specified
				return false;

			PrintMessage("ERROR", std::format("Failed construction when trying to find required member {}", dataName));
			return true; // if no default, error
		}

		const nlohmann::json& val = it.value();

		if (IsIncorrectType(it.value(), dataName, type))
			return true;

		insert = val; // assign it if it is valid

		return false; // assign if it is if it is is it if
	}

	template<typename T, typename K>
	bool IsInvalidElseAssign(const nlohmann::json& base, std::string_view dataName, JsonType keyType, T& insert, const std::unordered_map<K, T>& translator, bool hasDefault = false) const {

		nlohmann::json::const_iterator it = base.find(dataName);

		if (it == base.end()) {
			if (hasDefault) // if member has a default value, don't worry if it's not specified
				return false;

			PrintMessage("ERROR", std::format("Failed construction when trying to find required member {}", dataName));
			return true; // if no default, error
		}

		const nlohmann::json& val = it.value();

		if (IsIncorrectType(val, dataName, keyType))
			return true;

		auto mapIt = translator.find(val);

		if (mapIt == translator.end()) {
			PrintMessage("ERROR", std::format("Failed construction when trying to parse required member {} as option", dataName));
			return true;
		}

		insert = translator.at(val);
		return false;
	}

	template<typename T>
	bool IsInvalidElseAssignVector(const nlohmann::json& base, std::string_view dataName, JsonType type, std::vector<T>& insert, bool hasDefault = true) const {
		nlohmann::json::const_iterator it = base.find(dataName);

		if (it == base.end()) {
			if (hasDefault) // if member has a default value, don't worry if it's not specified
				return false;

			PrintMessage("ERROR", std::format("Failed construction when trying to find required member {}", dataName));
			return true; // if no default, error
		}

		const nlohmann::json& val = it.value();

		if (IsIncorrectType(val, dataName, JsonType::ARRAY))
			return true;

		for (const nlohmann::json& element : val) {
			if (IsIncorrectType(element, std::format("{}[element]", dataName), type))
				return true;
			insert.push_back(element);
		}

		return false;
	}

	template<typename T, size_t S>
	bool IsInvalidElseAssignArray(const nlohmann::json& base, std::string_view dataName, JsonType type, std::array<T, S>& insert, bool hasDefault = true) const {
		nlohmann::json::const_iterator it = base.find(dataName);

		if (it == base.end()) {
			if (hasDefault) // if member has a default value, don't worry if it's not specified
				return false;

			PrintMessage("ERROR", std::format("Failed construction when trying to find required member {}", dataName));
			return true; // if no default, error
		}

		const nlohmann::json& val = it.value();

		if (IsIncorrectType(val, dataName, JsonType::ARRAY))
			return true;

		if (val.size() != S) {
			PrintMessage("ERROR", std::format("Failed construction when trying to parse required member {} as member has array size {} when it should be {}", dataName, val.size(), S));
			return true;
		}

		for (unsigned int i = 0; i < S; i++) {
			if (IsIncorrectType(val.at(i), std::format("{}[element]", dataName), type))
				return true;
			insert.at(i) = val.at(i);
		}

		return false;
	}

};