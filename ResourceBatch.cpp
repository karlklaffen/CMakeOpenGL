//#include "SetBatch<ResourceType>.h"
//
//SetBatch<ResourceType>::SetBatch<ResourceType>() {
//
//}
//
//SetBatch<ResourceType>::SetBatch<ResourceType>(ResourceType type, const std::unordered_set<std::string>& names) {
//	TryAddMultiple(type, names);
//}
//
//bool SetBatch<ResourceType>::TryAdd(ResourceType type, std::string_view name) {
//	std::unordered_map<ResourceType, std::unordered_set<std::string>>::iterator it = Resources.find(type);
//
//	if (it == Resources.end()) {
//		Resources.try_emplace(type, std::unordered_set<std::string>{ std::string(name) });
//		return false;
//	}
//
//	if (it->second.contains(std::string(name)))
//		return true;
//
//	it->second.insert(std::string(name));
//
//	return false;
//}
//
//bool SetBatch<ResourceType>::TryAddMultiple(ResourceType type, const std::unordered_set<std::string>& names) {
//	std::unordered_map<ResourceType, std::unordered_set<std::string>>::iterator it = Resources.find(type);
//
//	if (it == Resources.end()) {
//		Resources.try_emplace(type, names);
//		return false;
//	}
//
//	size_t numAlreadyPresent = 0;
//
//	for (std::string_view name : names) {
//		if (it->second.contains(std::string(name)))
//			numAlreadyPresent++;
//		else
//			it->second.insert(std::string(name));
//	}
//
//	if (numAlreadyPresent == 0)
//		return false;
//
//	//return numAlreadyPresent == names.size() ? Utils::ErrorReturn::ALL_ALREADY_PRESENT : Utils::ErrorReturn::SOME_ALREADY_PRESENT;
//	return true;
//}
//
//bool SetBatch<ResourceType>::TryRemove(ResourceType type, std::string_view name) {
//	std::unordered_map<ResourceType, std::unordered_set<std::string>>::iterator it = Resources.find(type);
//
//	if (it == Resources.end())
//		return true;
//
//	if (it->second.contains(std::string(name))) {
//		it->second.erase(std::string(name));
//		return false;
//	}
//
//	return true;
//}
//
//bool SetBatch<ResourceType>::TryRemoveMultiple(ResourceType type, const std::unordered_set<std::string>& names) {
//	std::unordered_map<ResourceType, std::unordered_set<std::string>>::iterator it = Resources.find(type);
//
//	if (it == Resources.end())
//		return true;
//
//	size_t numNotPresent = 0;
//
//	for (std::string_view name : names) {
//		if (it->second.contains(std::string(name)))
//			it->second.erase(std::string(name));
//		else
//			numNotPresent++;
//	}
//
//	if (it->second.size() == 0)
//		Resources.erase(type);
//
//	if (numNotPresent == 0)
//		return false;
//
//	return true;
//}
//
//bool SetBatch<ResourceType>::Merge(const SetBatch<ResourceType>& other) {
//
//	// if other list has a type group not present in this list, add that group
//	for (auto& [type, group] : other.Resources) {
//		std::unordered_map<ResourceType, std::unordered_set<std::string>>::iterator it = Resources.find(type);
//
//		if (it == Resources.end())
//			Resources.try_emplace(type, group);
//		else
//			it->second.insert(group.begin(), group.end());
//	}
//
//	return false;
//}
//
//const std::unordered_map<ResourceType, std::unordered_set<std::string>>& SetBatch<ResourceType>::GetResources() const {
//	return Resources;
//}
//
//std::string SetBatch<ResourceType>::ToString() const {
//	unsigned int count = 0;
//
//	for (const auto& [type, group] : Resources) {
//		count += group.size();
//	}
//
//	return std::format("SetBatch<ResourceType>({} elements)", count);
//}
//
//SetBatch<ResourceType> SetBatch<ResourceType>::operator+(const SetBatch<ResourceType>& other) const {
//
//	SetBatch<ResourceType> result = *this;
//	for (const auto& [type, names] : other.GetResources()) {
//		result.TryAddMultiple(type, names);
//	}
//
//	return result;
//}
//
//SetBatch<ResourceType> SetBatch<ResourceType>::operator-(const SetBatch<ResourceType>& other) const {
//	const std::unordered_map<ResourceType, std::unordered_set<std::string>>& secondResources = other.GetResources();
//
//	SetBatch<ResourceType> result;
//
//	for (const auto& [type, group] : Resources) { // loop through our resources
//
//		std::unordered_map<ResourceType, std::unordered_set<std::string>>::const_iterator it = secondResources.find(type);
//
//		if (it == secondResources.end()) // if this resource type group is not in to-be-subtracted resources, add all of our resources of this type
//			result.TryAddMultiple(type, group);
//		else // if this resource type group is in our to-be-subtracted resources, add our resources minus the ones in the other group
//			result.TryAddMultiple(type, Utils::InFirstNotSecond(group, it->second));
//	}
//
//	return result;
//}
//
////std::string SetBatch<ResourceType>::GetResourceTypeName(ResourceType type) {
////	switch (type) {
////	case ResourceType::TEXTURE:
////		return "Texture";
////	case ResourceType::MODEL:
////		return "Model";
////	case ResourceType::CUBEMAP:
////		return "Cubemap";
////	case ResourceType::FONT:
////		return "Font";
////	case ResourceType::SHADER:
////		return "Shader";
////	case ResourceType::INTERFACE:
////		return "Interface";
////	case ResourceType::SCENE:
////		return "Scene";
////	}
////
////	return "Unknown";
////}
