#include "ResourceOwner.h"
#include "ResourceManager.h"

ResourceOwner::ResourceOwner(ResourceType type) : Type(type) {
}

void ResourceOwner::SwapCatalogSource(const std::filesystem::path& folderPath, std::string_view identifier) {
	CatalogFolderPath = folderPath;
	Identifier = identifier;
}

std::optional<SetBatch<ResourceType>> ResourceOwner::GetDependencies(std::string_view name) const {
	return GetDependenciesFromJson(GetData(name));
}

bool ResourceOwner::TryLoadMultiple(const std::unordered_set<std::string>& names) {
	bool concern = false;
	for (std::string_view name : names)
		if (TryLoad(name))
			concern = true;

	return concern;
}

bool ResourceOwner::TryUnloadMultiple(const std::unordered_set<std::string>& names) {
	bool concern = false;
	for (std::string_view name : names)
		if (TryUnload(name))
			concern = true;

	return concern;
}

bool ResourceOwner::TryReloadMultiple(const std::unordered_set<std::string>& names) {
	bool concern = false;
	for (std::string_view name : names)
		if (TryReload(name))
			concern = true;

	return concern;
}

//nlohmann::json ResourceOwner::GetData(std::string_view name) const {
//	std::ifstream file(CatalogPath);
//	nlohmann::json data = nlohmann::json::parse(file, nullptr, true, true);
//
//	for (const auto& [jsonName, resourceJson] : data.items()) {
//
//		if (jsonName == name) {
//			return resourceJson;
//		}
//	}
//
//	return nullptr;
//}

nlohmann::json ResourceOwner::GetDataFromFile(const std::filesystem::path& path) const {
	std::ifstream file(path);
	return nlohmann::json::parse(file, nullptr, true, true);
}

nlohmann::json ResourceOwner::GetData(std::string_view name) const {

	for (const std::filesystem::path& path : std::filesystem::directory_iterator(CatalogFolderPath)) {

		if (path.stem().string() == Identifier) { // e.g. for interfaces, "interfaces"

			if (std::filesystem::is_directory(path)) {

				for (const std::filesystem::path& filePath : std::filesystem::directory_iterator(path)) {
					
					if (std::filesystem::is_regular_file(filePath) && filePath.stem().string() == name)
						return GetDataFromFile(filePath); // file containing one interface
				}
			}
			else if (std::filesystem::is_regular_file(path) && path.extension().string() == ".json") { // file containing multiple resources

				nlohmann::json json = GetDataFromFile(path);

				nlohmann::json::const_iterator it = json.find(name);

				if (it != json.end())
					return it.value();
			}
		}
	}

	return nullptr;
}

std::string ResourceOwner::GetSpecificResourceInfo(std::string_view resourceName) const {
	return "Type = " + ResourceManager::GetTypeTranslator().GetName(Type) + ", Name = " + std::string(resourceName);
}

void ResourceOwner::PrintMessage(std::string_view type, std::string_view resourceName, std::string_view info) const {
	std::cout << std::format("{:.<30}({})", std::format("{}: {}", type, info), GetSpecificResourceInfo(resourceName)) << std::endl;
}

void ResourceOwner::PrintMessage(std::string_view type, std::string_view info) const {
	std::cout << std::format("{}: {}", type, info) << std::endl;
}

bool ResourceOwner::IsIncorrectType(const nlohmann::json& val, std::string_view dataName, JsonType type) const {
	bool good = false; // check that member data type is correct
	switch (type) {
	case JsonType::NUMBER:
		good = val.is_number(); break;
	case JsonType::BOOLEAN:
		good = val.is_boolean(); break;
	case JsonType::FLOAT:
		good = val.is_number_float(); break;
	case JsonType::INTEGER:
		good = val.is_number_integer(); break;
	case JsonType::UNSIGNED_INT:
		good = val.is_number_unsigned(); break;
	case JsonType::STRING:
		good = val.is_string(); break;
	case JsonType::OBJECT:
		good = val.is_object(); break;
	case JsonType::ARRAY:
		good = val.is_array(); break;
	}

	if (good)
		return false;

	PrintMessage("ERROR", std::format("Failed construction when trying to parse member {}", dataName));

	return true;
}

bool ResourceOwner::HandleIfInvalid(const nlohmann::json& base, std::string_view dataName, JsonType type, bool hasDefault) const {
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

	return false;
}

bool ResourceOwner::DependenciesLoaded(std::string_view name) const {
	std::optional<SetBatch<ResourceType>> optionalBatch = GetDependencies(name);

	// TODO: Do some error if this has no value
	if (!optionalBatch.has_value())
		return true;

	return ResourceManager::ResourceBatchLoaded(optionalBatch.value()); // TODO: Test if all dependencies are loaded recursively, not just surface level
}

std::optional<SetBatch<ResourceType>> ResourceOwner::GetDependenciesFromJson(const nlohmann::json& data) const {
	return SetBatch<ResourceType>{};
}