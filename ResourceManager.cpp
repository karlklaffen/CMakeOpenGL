#include "ResourceManager.h"

void ResourceManager::SetPath(const std::filesystem::path& path) {
	ResourcePath = path;

	SetupType(ResourceType::TEXTURE, "Texture", "textures");
	SetupType(ResourceType::MODEL, "Model", "models");
	SetupType(ResourceType::CUBEMAP, "Cubemap", "cubemaps");
	SetupType(ResourceType::FONT, "Font", "fonts");
	SetupType(ResourceType::SHADER, "Shader", "shaders");
	SetupType(ResourceType::INTERFACE, "Interface", "interfaces");
	SetupType(ResourceType::SCENE, "Scene", "scenes");
}

void ResourceManager::SetupType(ResourceType type, std::string_view displayName, std::string_view identifier) {
	TypeTranslator.AddName(type, displayName);
	GetResourceOwner(type).SwapCatalogSource(ResourcePath / "catalogs", identifier);
}

const std::filesystem::path& ResourceManager::GetResourcePath() {
	return ResourcePath;
}

bool ResourceManager::Enumerate(const SetBatch<ResourceType>& batch, LoadType loadType, std::optional<ResourceType> executeType) {
	const std::unordered_map<ResourceType, std::unordered_set<std::string>> resources = batch.GetSets();

	if (executeType.has_value()) {
		ResourceType type = executeType.value();
		auto it = resources.find(type);

		if (it == resources.end())
			return true;

		return EnumerateSpecific(type, it->second, loadType);
	}

	bool concern = false;

	for (const auto& [type, names] : resources) {
		if (EnumerateSpecific(type, names, loadType))
			concern = true;
	}

	return concern;
}

bool ResourceManager::EnumerateSpecific(ResourceType type, const std::unordered_set<std::string>& names, LoadType loadType) {
	ResourceOwner& owner = GetResourceOwner(type);

	switch (loadType) {
	case LoadType::LOAD:
		owner.TryLoadMultiple(names);
		break;
	case LoadType::UNLOAD:
		owner.TryUnloadMultiple(names);
		break;
	case LoadType::RELOAD:
		owner.TryReloadMultiple(names);
		break;
	}

	return false;
}

bool ResourceManager::LoadResourceBatch(const SetBatch<ResourceType>& batch) {
	bool concern = false;
	for (auto& [type, group] : batch.GetSets()) {
		if (GetResourceOwner(type).TryLoadMultiple(group))
			concern = true;
	}

	return concern;
}

bool ResourceManager::UnloadResourceBatch(const SetBatch<ResourceType>& batch) {
	bool concern = false;

	for (auto& [type, group] : batch.GetSets()) {
		if (GetResourceOwner(type).TryUnloadMultiple(group))
			concern = true;
	}

	return concern;
}

bool ResourceManager::ReloadResourceBatch(const SetBatch<ResourceType>& batch) {
	bool concern = false;

	for (auto& [type, group] : batch.GetSets()) {
		if (GetResourceOwner(type).TryReloadMultiple(group))
			concern = true;
	}

	return concern;
}

bool ResourceManager::SmartEnumerate(SetBatch<ResourceType>& batch, std::optional<LoadType> loadType, std::optional<ResourceType> executeType) {

	const std::unordered_map<ResourceType, std::unordered_set<std::string>>& resources = batch.GetSets();

	SetBatch<ResourceType> surfaceLevelBatch = batch;

	// loop through resources of resource batch
	for (const auto& [type, group] : resources) {

		const ResourceOwner& holder = GetResourceOwner(type);

		for (std::string_view name : group) {
			// get surface-level dependencies of the resource
			std::optional<SetBatch<ResourceType>> surfaceLevelDepsOptional = holder.GetDependencies(name);

			if (!surfaceLevelDepsOptional.has_value())
				return true;

			SetBatch<ResourceType> surfaceLevelDeps = surfaceLevelDepsOptional.value();
			// get all extra dependencies of the resource (this batch includes surface level deps)
			SetBatch<ResourceType> allDeps = surfaceLevelDeps;
			SmartEnumerate(allDeps, loadType, executeType);
			// if load, load the surface-level dependencies
			// recursive approach means that lower, base dependencies will be loaded before things that rely on them
			if (loadType.has_value())
				Enumerate(surfaceLevelDeps, loadType.value(), executeType);

			// add new dependencies to this batch
			batch.Merge(allDeps);
		}
	}

	if (loadType.has_value())
		Enumerate(surfaceLevelBatch, loadType.value(), executeType);
	return false; // TODO: return actual value
}

bool ResourceManager::SmartEnumerateSpecific(ResourceType type, const std::unordered_set<std::string>& names, LoadType loadType) {
	SetBatch<ResourceType> batch{ type, names };
	return SmartEnumerate(batch, loadType);
}

TextureOwner& ResourceManager::GetTextures() {
	return Textures;
}

ModelOwner& ResourceManager::GetModels() {
	return Models;
}

CubemapOwner& ResourceManager::GetCubemaps() {
	return Cubemaps;
}

FontOwner& ResourceManager::GetFonts() {
	return Fonts;
}

ShaderOwner& ResourceManager::GetShaders() {
	return Shaders;
}

InterfaceOwner& ResourceManager::GetInterfaces() {
	return Interfaces;
}

SceneOwner& ResourceManager::GetScenes() {
	return Scenes;
}

//bool ResourceManager::SmartUnloadLoad(const SetBatch<ResourceType>& prev, const SetBatch<ResourceType>& next) {
//	SetBatch<ResourceType> prevAll = LoopThruAllDependenciesOfSetBatch<ResourceType>(prev, false);
//	SetBatch<ResourceType> nextAll = LoopThruAllDependenciesOfSetBatch<ResourceType>(next, false);
//
//	//std::cout << "nextAll: " << nextAll.ToString() << std::endl;
//
//	SetBatch<ResourceType> needUnload = SetBatch<ResourceType>::InFirstNotSecond(prevAll, nextAll);
//
//	// unload things that we no longer need
//
//	//std::cout << "unload" << std::endl;
//	UnloadSetBatch<ResourceType>(needUnload);
//
//	// try to load resources recursively from new batch
//	// some things will already be loaded, not an issue
//	// using this function instead of LoadSetBatch<ResourceType> guarantees that resources will be loaded in the correct order
//	//std::cout << "load" << std::endl;
//	LoopThruAllDependenciesOfSetBatch<ResourceType>(next, true);
//
//	return false;
//}

//bool ResourceManager::SmartLoad(const SetBatch<ResourceType>& batch) {
//	LoopThroughDependencies(batch, LoadType::LOAD);
//
//	return false;
//}
//
//bool ResourceManager::SmartUnload(const SetBatch<ResourceType>& batch) {
//	LoopThroughDependencies(batch, UnloadSetBatch<ResourceType>);
//
//	return false;
//}
//
//bool ResourceManager::SmartReload(const SetBatch<ResourceType>& batch) {
//	LoopThroughDependencies(batch, ReloadSetBatch<ResourceType>);
//	return false;
//}

bool ResourceManager::SmartUnloadLoad(const SetBatch<ResourceType>& toLoad, const SetBatch<ResourceType>& alreadyLoaded) {
	SetBatch<ResourceType> toLoadAll = toLoad;
	SmartEnumerate(toLoadAll);
	SetBatch<ResourceType> alreadyLoadedAll = alreadyLoaded;
	SmartEnumerate(alreadyLoadedAll);
	
	//std::cout << "nextAll: " << nextAll.ToString() << std::endl;
	
	SetBatch<ResourceType> needUnload = alreadyLoadedAll - toLoadAll;
	SetBatch<ResourceType> needLoad = toLoadAll - alreadyLoadedAll;
	
	// unload things that we no longer need
	
	//std::cout << "unload" << std::endl;
	UnloadResourceBatch(needUnload);
	LoadResourceBatch(needLoad);

	return false;
}

bool ResourceManager::UnloadAll() {
	Textures.TryUnloadAll();
	Models.TryUnloadAll();
	Cubemaps.TryUnloadAll();
	Fonts.TryUnloadAll();
	Shaders.TryUnloadAll();
	Interfaces.TryUnloadAll();
	Scenes.TryUnloadAll();

	return false;
}

//bool ResourceManager::ExecuteOnAllOfType(const SetBatch<ResourceType>& batch, ResourceType type, const std::function<bool(const SetBatch<ResourceType>& batch)>& func) {
//	LoopThroughDependencies()
//}

bool ResourceManager::ResourceBatchLoaded(const SetBatch<ResourceType>& batch) {
	for (const auto& [type, group] : batch.GetSets()) {
		ResourceOwner& owner = GetResourceOwner(type);
		for (std::string_view name : group) {
			if (!owner.IsLoaded(name))
				return false;
		}
	}

	return true;
}

const SmartEnum<ResourceType>& ResourceManager::GetTypeTranslator() {
	return TypeTranslator;
}

ResourceOwner& ResourceManager::GetResourceOwner(ResourceType type) {
	switch (type) {
	case ResourceType::TEXTURE:
		return Textures;
	case ResourceType::MODEL:
		return Models;
	case ResourceType::CUBEMAP:
		return Cubemaps;
	case ResourceType::FONT:
		return Fonts;
	case ResourceType::SHADER:
		return Shaders;
	case ResourceType::INTERFACE:
		return Interfaces;
	case ResourceType::SCENE:
		return Scenes;
	}
}