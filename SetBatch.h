#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>
#include <format>

#include "Utils.h"

template<typename T>
class SetBatch {

public:
	SetBatch() {

	}

	SetBatch(T type, const std::unordered_set<std::string>& names) {
		TryAddMultiple(type, names);
	}

	bool TryAdd(T type, std::string_view name) {
		auto it = Sets.find(type);

		if (it == Sets.end()) {
			Sets.try_emplace(type, std::unordered_set<std::string>{ std::string(name) });
			return false;
		}

		if (it->second.contains(std::string(name)))
			return true;

		it->second.insert(std::string(name));

		return false;
	}

	bool TryAddMultiple(T type, const std::unordered_set<std::string>& names) {
		auto it = Sets.find(type);

		if (it == Sets.end()) {
			Sets.try_emplace(type, names);
			return false;
		}

		size_t numAlreadyPresent = 0;

		for (std::string_view name : names) {
			if (it->second.contains(std::string(name)))
				numAlreadyPresent++;
			else
				it->second.insert(std::string(name));
		}

		if (numAlreadyPresent == 0)
			return false;

		//return numAlreadyPresent == names.size() ? Utils::ErrorReturn::ALL_ALREADY_PRESENT : Utils::ErrorReturn::SOME_ALREADY_PRESENT;
		return true;
	}

	bool TryRemove(T type, std::string_view name) {
		auto it = Sets.find(type);

		if (it == Sets.end())
			return true;

		if (it->second.contains(std::string(name))) {
			it->second.erase(std::string(name));
			return false;
		}

		return true;
	}

	bool TryRemoveMultiple(T type, const std::unordered_set<std::string>& names) {
		auto it = Sets.find(type);

		if (it == Sets.end())
			return true;

		size_t numNotPresent = 0;

		for (std::string_view name : names) {
			if (it->second.contains(std::string(name)))
				it->second.erase(std::string(name));
			else
				numNotPresent++;
		}

		if (it->second.size() == 0)
			Sets.erase(type);

		if (numNotPresent == 0)
			return false;

		return true;
	}

	bool Merge(const SetBatch<T>& other) {

		// if other list has a type group not present in this list, add that group
		for (auto& [type, group] : other.Sets) {
			auto it = Sets.find(type);

			if (it == Sets.end())
				Sets.try_emplace(type, group);
			else
				it->second.insert(group.begin(), group.end());
		}

		return false;
	}

	const std::unordered_map<T, std::unordered_set<std::string>>& GetSets() const {
		return Sets;
	}

	std::string ToString() const {
		unsigned int count = 0;

		for (const auto& [type, group] : Sets) {
			count += group.size();
		}

		return std::format("SetBatch({} elements)", count);
	}

	SetBatch<T> operator+(const SetBatch<T>& other) const {

		SetBatch<T> result = *this;
		for (const auto& [type, names] : other.GetSets()) {
			result.TryAddMultiple(type, names);
		}

		return result;
	}

	SetBatch<T> operator-(const SetBatch<T>& other) const {
		const auto& secondSets = other.GetSets();

		SetBatch<T> result;

		for (const auto& [type, group] : Sets) { // loop through our resources

			auto it = secondSets.find(type);

			if (it == secondSets.end()) // if this resource type group is not in to-be-subtracted resources, add all of our resources of this type
				result.TryAddMultiple(type, group);
			else // if this resource type group is in our to-be-subtracted resources, add our resources minus the ones in the other group
				result.TryAddMultiple(type, Utils::InFirstNotSecond(group, it->second));
		}

		return result;
	}

private:
	std::unordered_map<T, std::unordered_set<std::string>> Sets;
};

