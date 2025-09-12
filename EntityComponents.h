#pragma once

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>
#include <unordered_set>
#include <set>
#include <queue>
#include <iostream>

class ModelRenderer {
	std::string ModelName;
};

class ComponentGroupBase {

public:
	ComponentGroupBase() {

	}

	virtual void TryRemoveEntity(unsigned int id) = 0; // virtual function call is cheaper than dynamic cast, possible for this function

};

template<typename T>
class ComponentGroup : public ComponentGroupBase {

public:
	ComponentGroup() {

	}

	void AddEntity(unsigned int id, T& component) {
		EntityMapping.try_emplace(id, Components.size());
		Components.push_back(std::move(component));
	}

	void TryRemoveEntity(unsigned int id) override {
		std::unordered_map<unsigned int, unsigned int>::iterator it = EntityMapping.find(id);

		unsigned int index = it->second;

		if (index < Components.size() - 1) {
			unsigned int lastIndex = Components.size() - 1;

			for (auto& [entityId, pos] : EntityMapping) { // change entity that maps to last component to map to new index to account for smart delete
				if (pos == lastIndex) {
					lastIndex = index;
					break;
				}
			}

			std::swap(Components.at(index), Components.at(lastIndex)); // swap component areas
		}

		Components.pop_back(); // erase garbage end value
		EntityMapping.erase(it); // erase entity mapping corresponding to this id
	}

	std::vector<T>& GetComponents() {
		return Components;
	}

	T& GetComponent(unsigned int entityId) {
		return Components.at(EntityMapping.at(entityId));
	}

private:

	std::vector<T> Components;
	std::unordered_map<unsigned int, unsigned int> EntityMapping;

	inline static unsigned int NumComponents = 0;
};

class EntityManager {

	template<typename T>
	ComponentGroup<T>& GetComponentGroup() {
		return *dynamic_cast<ComponentGroup<T>*>(Components.at(GetComponentID<T>()).get());
	}

public:
	EntityManager() : NumComponents(0), NumEntities(0) {

	}

	template<typename T>
	unsigned int GetComponentID() {
		static unsigned int id = NumComponents++;
		return id;
	}

	template<typename T>
	bool HasComponent() {
		unsigned int thisId = GetComponentID<T>();
		for (const auto& [id, group] : Components)
			if (id == thisId)
				return true;

		return false;
	}

	bool EntityExists(std::string_view str) {
		for (const auto& [name, id] : EntityIDs)
			if (name == str)
				return true;
		return false;
	}

	void AddEntity(std::string_view entityName) {

		unsigned int id = 0;

		if (AvailableEntityIDs.empty())
			id = NumEntities++;
		else {
			id = AvailableEntityIDs.front();
			AvailableEntityIDs.pop();
		}

		EntityIDs.try_emplace(std::string(entityName), id);
	}

	void RemoveEntity(std::string_view entityName) {
		const std::string& nameStr = std::string(entityName);
		unsigned int id = EntityIDs.at(nameStr);
		for (auto& [typeID, group] : Components) {
			group->TryRemoveEntity(id);
		}
		EntityIDs.erase(nameStr);
		AvailableEntityIDs.push(id);
	}

	template<typename T>
	void AddComponent(std::string_view entityName, T component) {
		if (!HasComponent<T>())
			Components.try_emplace(GetComponentID<T>(), std::make_unique<ComponentGroup<T>>());

		GetComponentGroup<T>().AddEntity(EntityIDs.at(std::string(entityName)), component);
	}

	template<typename T>
	void RemoveComponent(std::string_view entityName) {
		Components.at(GetComponentID<T>())->TryRemoveEntity(EntityIDs.at(std::string(entityName)));
	}

	template<typename T>
	std::vector<T>& GetComponents() {
		return GetComponentGroup<T>().GetComponents();
	}

	template<typename T>
	T& GetComponent(std::string_view entityName) {
		return GetComponentGroup<T>().GetComponent(EntityIDs.at(std::string(entityName)));
	}

	unsigned int NumComponents;
	unsigned int NumEntities;
	std::unordered_map<unsigned int, std::unique_ptr<ComponentGroupBase>> Components; // component type id to group of that type
	std::unordered_map<std::string, unsigned int> EntityIDs;
	std::queue<unsigned int> AvailableEntityIDs;
};