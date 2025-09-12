#pragma once

#include <optional>
#include "ResourceOwner.h"

template<typename T>
class ResourceHolder : public ResourceOwner {

	// T must derive from Resource
	static_assert(std::is_base_of<NotCopyable, T>::value);

	bool TryCreate(std::string_view name) {
		nlohmann::json data = GetData(name);

		if (data == nullptr) {
			// resource data not in file
			PrintMessage("ERROR", name, "Could not find resource data in file");
			return true;
		}

		if (!DependenciesLoaded(name)) {
			PrintMessage("ERROR", name, "Dependencies not loaded");
			return true;
		}

		std::optional<T> resource = Construct(data);

		if (resource.has_value()) {
			Resources.try_emplace(std::string(name), std::move(resource.value()));
			PrintMessage("INFO", name, "LOADED");
			return false;
		}

		return true;
	}

public:

	ResourceHolder(ResourceType type) : ResourceOwner(type) {

	}

	// tries to load resource from json file
	bool TryLoad(std::string_view name) override {
		auto it = Resources.find(std::string(name));

		PrintMessage("INFO", name, "Trying to load");

		if (it != Resources.end()) {
			// resource already loaded
			return true;
		}

		return TryCreate(name);
	}

	bool TryUnload(std::string_view name) override {
		auto it = Resources.find(std::string(name));

		if (it == Resources.end())
			return true;

		Resources.erase(it);

		return false;
	}

	bool TryReload(std::string_view name) override {
		auto it = Resources.find(std::string(name));

		if (it == Resources.end())
			return true;

		Resources.erase(it);

		return TryCreate(name);
	}

	bool TryReloadAll() override {

		bool concern = false;

		std::unordered_set<std::string> names;

		for (const auto& [name, resource] : Resources)
			names.insert(name);

		for (std::string_view name : names)
			if (TryReload(name))
				concern = true;

		return concern;
	}

	bool TryUnloadAll() override {
		Resources.clear();

		return false;
	}

	bool IsLoaded(std::string_view name) const override {
		return Resources.contains(std::string(name));
	}

	const T& Get(std::string_view name) const {
		return Resources.at(std::string(name));
	}

	T& Get(std::string_view name) {
		return Resources.at(std::string(name));
	}

protected:
	virtual std::optional<T> Construct(const nlohmann::json& data) const = 0;

	std::unordered_map<std::string, T> Resources;

};


class TextureOwner : public ResourceHolder<Texture> {

public:
	TextureOwner() : ResourceHolder(ResourceType::TEXTURE) {

	}

private:
	std::optional<Texture> Construct(const nlohmann::json& data) const override;
};

class ModelOwner : public ResourceHolder<TinyModel> {

public:
	ModelOwner() : ResourceHolder(ResourceType::MODEL) {

	}

private:
	std::optional<TinyModel> Construct(const nlohmann::json& data) const override;
};

class CubemapOwner : public ResourceHolder<Cubemap> {

public:
	CubemapOwner() : ResourceHolder(ResourceType::CUBEMAP) {

	}

private:
	std::optional<Cubemap> Construct(const nlohmann::json& data) const override;
};

class FontOwner : public ResourceHolder<Font> {

public:
	FontOwner() : ResourceHolder(ResourceType::FONT) {

	}

private:
	std::optional<Font> Construct(const nlohmann::json& data) const override;
};

class ShaderOwner : public ResourceHolder<Shader> {

public:
	ShaderOwner() : ResourceHolder(ResourceType::SHADER) {

	}

private:
	std::optional<Shader> Construct(const nlohmann::json& data) const override;
};

class InterfaceOwner : public ResourceHolder<Interface> {

public:
	InterfaceOwner() : ResourceHolder(ResourceType::INTERFACE) {

	}

private:
	std::optional<SetBatch<ResourceType>> GetDependenciesFromJson(const nlohmann::json& data) const override;
	std::optional<Interface> Construct(const nlohmann::json& data) const override;
};

class SceneOwner : public ResourceHolder<Scene> {

public:
	SceneOwner() : ResourceHolder(ResourceType::SCENE) {

	}

private:
	std::optional<SetBatch<ResourceType>> GetDependenciesFromJson(const nlohmann::json& data) const override;
	std::optional<Scene> Construct(const nlohmann::json& data) const override;
};