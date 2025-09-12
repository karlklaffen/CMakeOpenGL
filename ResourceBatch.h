//#pragma once
//
//#include <iostream>
//#include <unordered_map>
//#include <unordered_set>
//#include <string>
//
//#include "Utils.h"
//
//class SetBatch<ResourceType> {
//
//public:
//	SetBatch<ResourceType>();
//	SetBatch<ResourceType>(ResourceType type, const std::unordered_set<std::string>& names);
//
//	bool TryAdd(ResourceType type, std::string_view name);
//	bool TryAddMultiple(ResourceType type, const std::unordered_set<std::string>& names);
//	bool TryRemove(ResourceType type, std::string_view name);
//	bool TryRemoveMultiple(ResourceType type, const std::unordered_set<std::string>& names);
//	bool Merge(const SetBatch<ResourceType>& other);
//
//	const std::unordered_map<ResourceType, std::unordered_set<std::string>>& GetResources() const;
//
//	std::string ToString() const;
//
//	SetBatch<ResourceType> operator+(const SetBatch<ResourceType>& other) const;
//	SetBatch<ResourceType> operator-(const SetBatch<ResourceType>& other) const;
//
//private:
//	std::unordered_map<ResourceType, std::unordered_set<std::string>> Resources;
//};