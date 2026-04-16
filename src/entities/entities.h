#pragma once

#include <bitset>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "component.hpp"
#include "entity.hpp"



template<typename T_Parent, typename... T_Components>
class System;



/**
 * @class Entities
 * @brief This is used to manage a set of entities and their components.
 */
class Entities
{
public:

	Entities() noexcept = default;
	~Entities() = default;

	Entity make_entity();
	void kill_entity(Entity entity);

	/**
	 * @brief Adds a component of type T to the specified entity.
	 * @tparam T The type of the component to remove.
	 * @param entity The target entity.
	 * @param component The component data to add.
	 */
	template <typename T>
	void add_component(Entity entity, T component) 
	{
		get_component<T>().set(entity, component);
		const uint32_t id = entity.ID();
		m_signatures[id].set(gen_component_id<T>());
	}

	/**
	 * @brief Removes the component of type T from the specified entity.
	 * @tparam T The type of the component to remove.
	 * @param entity The target entity.
	 */
	template <typename T>
	void remove_component(Entity entity) 
	{
		get_component<T>()->remove(entity);
		const uint32_t id = entity.ID();
		if (id < m_signatures.size()) m_signatures[id].reset(gen_component_id<T>());
	}

	/**
	 * @brief Checks if the specified entity possesses a component of type T.
	 * @tparam T The type of the component to check.
	 * @return true if the entity has the component, else false.
	 */
	template <typename T>
	bool has_component(Entity entity) const 
	{
		const uint32_t id = entity.ID();
		return id < m_signatures.size() && m_signatures[id].test(gen_component_id<T>());
	}

	/**
	 * @brief Get the component for type T of a specific entity.
	 * @return A reference to the component data. 
	 */
	template <typename T>
	T &get_component(Entity entity)
	{
		return get_component<T>().get(entity);
	}
	
	bool check(Entity entity) const;

	template<typename T_Parent, typename... T_Components>
	friend class System;

private:

	/**
	 * @brief Get the component for type T.
	 * @tparam T The type of the component.
	 * @return A reference to the internal Component<T> storage.
	 * @note This will create the Component<T> if necessary.
	 */
	template <typename T>
	Component<T> &get_component() 
	{
		uint32_t id = gen_component_id<T>();
		if (id >= m_components.size()) m_components.resize(id+1);
		if (!m_components[id]) m_components[id] = std::make_unique<Component<T>>();
		return *static_cast<Component<T>*>(m_components[id].get());
	}

	/**
	 * @brief Get the component pointer for type T.
	 * @tparam T The type of the component.
	 * @return A pointer to the internal Component<T> storage, or nullptr.
	 */
	template <typename T>
	Component<T> *get_component_ptr() 
	{
		uint32_t id = gen_component_id<T>();
		if (id >= m_components.size() || !m_components[id]) return nullptr;
		return static_cast<Component<T>*>(m_components[id].get());
	}

	/**
	 * @brief Execute a system process method on matching entities.
	 * @tparam Components The list of component types required by the system.
	 * @tparam SystemInstance The calling system class type.
	 * @tparam Args Variadic types for additional arguments.
	 * @param sys The system instance invoking the update.
	 * @param args Arguments to forward to the sys.process() method.
	 */
	template <typename... Components, typename SystemInstance, typename... Args>
	void run_system(SystemInstance &sys, Args &&...args) 
	{
		if (sizeof...(Components) == 0) return;
		std::bitset<64> system_signature;
		(system_signature.set(gen_component_id<Components>()), ...);

		// Get the smaller entity vector.
		const std::vector<Entity>* best_entities_vector = nullptr;
		size_t min_size = std::numeric_limits<size_t>::max();
		auto consider_as_leader = [&](auto* component) {
			size_t size = (component) ? component->size() : 0;
			if (size < min_size) {
				min_size = size;
				best_entities_vector = (component) ? &component->entities() : nullptr;
			}
		};
		(consider_as_leader(get_component_ptr<Components>()), ...);
		if (!best_entities_vector || best_entities_vector->empty()) return;

		// Execute the system process on each entity if they have all the components.
		for (const Entity entity : *best_entities_vector) {
			const uint32_t id = entity.ID();
			if (id >= m_signatures.size() || (m_signatures[id] & system_signature) != system_signature) continue;
			sys.process(entity, get_component<Components>().get(entity)..., std::forward<Args>(args)...);
		}
	}

	std::vector<std::unique_ptr<IComponent>> m_components{}; // List of all existing components.
	std::vector<std::bitset<64>> m_signatures{}; // Masks for components checking.
	std::vector<Entity> m_free_ids{};   // List of all free entity ids.
	std::vector<uint16_t> m_versions{}; // List of all ids versions.
	uint32_t m_entity_counter{0};       // Counter to generate new entity ids.

};
