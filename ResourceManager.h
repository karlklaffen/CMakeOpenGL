#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <variant>
#include <optional>

#include "json.hpp"

#include "ResourceHolders.h"

#include "VertexHelper.h"
#include "ErrorManager.h"
#include "Utils.h"
#include "SmartEnum.h"

class ResourceManager {

public:

	enum class LoadType {
		LOAD,
		UNLOAD,
		RELOAD
	};

	static void SetPath(const std::filesystem::path& path);
	static void SetupType(ResourceType type, std::string_view displayName, std::string_view identifier);

	static const std::filesystem::path& GetResourcePath();

	// Apply specific load function to all resources
	static bool Enumerate(const SetBatch<ResourceType>& batch, LoadType loadType, std::optional<ResourceType> executeType = {});
	static bool EnumerateSpecific(ResourceType type, const std::unordered_set<std::string>& names, LoadType loadType);

	// Apply specific load function to all resources, including dependencies first
	static bool SmartEnumerate(SetBatch<ResourceType>& batch, std::optional<LoadType> loadType = {}, std::optional<ResourceType> executeType = {});
	static bool SmartEnumerateSpecific(ResourceType type, const std::unordered_set<std::string>& names, LoadType loadType);

	static bool SmartUnloadLoad(const SetBatch<ResourceType>& load, const SetBatch<ResourceType>& all);
	/*static bool SmartLoad(const SetBatch<ResourceType>& batch, std::optional<ResourceType> executeType = {});
	static bool SmartUnload(const SetBatch<ResourceType>& batch, std::optional<ResourceType> executeT);
	static bool SmartReload(const SetBatch<ResourceType>& batch);*/

	static bool UnloadAll();

	//static bool ExecuteOnAllOfType(const SetBatch<ResourceType>& batch, ResourceType type, const std::function<bool(const SetBatch<ResourceType>& batch)>& func);

	static bool ResourceBatchLoaded(const SetBatch<ResourceType>& batch);

	static const SmartEnum<ResourceType>& GetTypeTranslator();

	static TextureOwner& GetTextures();
	static ModelOwner& GetModels();
	static CubemapOwner& GetCubemaps();
	static FontOwner& GetFonts();
	static ShaderOwner& GetShaders();
	static InterfaceOwner& GetInterfaces();
	static SceneOwner& GetScenes();

private:

	static bool LoadResourceBatch(const SetBatch<ResourceType>& batch);
	static bool UnloadResourceBatch(const SetBatch<ResourceType>& batch);
	static bool ReloadResourceBatch(const SetBatch<ResourceType>& batch);

	static ResourceOwner& GetResourceOwner(ResourceType type);
	
	inline static std::filesystem::path ResourcePath;
	inline static SmartEnum<ResourceType> TypeTranslator;

	inline static TextureOwner Textures;
	inline static ModelOwner Models;
	inline static CubemapOwner Cubemaps;
	inline static FontOwner Fonts;
	inline static ShaderOwner Shaders;
	inline static InterfaceOwner Interfaces;
	inline static SceneOwner Scenes;
};