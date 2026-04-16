#pragma once

#include <cstdint>
#include <vector>

#include "entity.hpp"



/**
 * @brief Return a new unique id upon each call.
 * @return The uint32_t id.
 */
inline uint32_t _gen_unique_component_id() 
{
	static uint32_t last_id = 0;
	return ++last_id;
}

/**
 * @brief Template function to generate one unique if for each Component<T>.
 * @tparam T The type of the component.
 * @return The uint32_t id.
 * @note Two identical T will give the same id.
 */
template <typename T>
inline uint32_t gen_component_id()
{
    static uint32_t typeID = _gen_unique_component_id();
    return typeID;
}



/**
 * @interface IComponent
 * @brief This is used for storing Component in a container.
 */
class IComponent 
{
public:
	virtual ~IComponent() = default;
	virtual void remove(Entity entityID) = 0;
	virtual bool check(Entity entityID) const = 0;
};



/**
 * @class Component
 * @brief Sparse vector for one entity component.
 */
template <typename T>
class Component : public IComponent
{
public:

	Component() noexcept = default;
	~Component() = default;

	/**
	 * @brief Set an entity component to a given value.
	 * @param entity The entity to set the component of.
	 * @param value The value of the entity component. 
	 */
	void set(Entity entity, T value) 
	{
		const uint32_t id = entity.ID();
		if (id >= m_sparse.size()) m_sparse.resize(id+1, -1);

		// Set if the entity is present.
		if (m_sparse[id] != -1) {
			m_data[m_sparse[id]] = value;
			return;
		}

		// Insertion if the component doesn't exists for the given entity.
		m_sparse[id] = m_size;
		m_data.push_back(value);
		m_entities.push_back(entity);
		++m_size;
	}

	/**
	 * @brief Remove an entity from this component.
	 * @param entity The entity to remove.
	 */
	void remove(Entity entity) override
	{
		const uint32_t id = entity.ID();
		if (id >= m_sparse.size() || m_sparse[id] == -1) return;
		const int32_t index_deleted = m_sparse[id];
		const int32_t index_last = m_size-1;

		// Swap the deleted entity with the last one.
		if (index_deleted != index_last) {
			const Entity entity_last = m_entities[index_last];
			m_data[index_deleted] = m_data[index_last];
			m_entities[index_deleted] = entity_last;
			m_sparse[entity_last.ID()] = index_deleted;
		}

		// Delete the last element.
		m_size--;
		m_data.pop_back();
		m_entities.pop_back();
		m_sparse[id] = -1;

	}

	/**
	 * @brief Check if an entity have this component.
	 * @param entity The entity to check.
	 * @return true if the entity was found in this component.
	 */
	bool check(Entity entity) const override 
	{
		const uint32_t id = entity.ID();
		return id < m_sparse.size() && m_sparse[id] != -1;
	}

	T &get(Entity entity) { return m_data[m_sparse[entity.ID()]]; }
	std::vector<T> &data() { return m_data; }
	const std::vector<Entity> &entities() const { return m_entities; }
	uint32_t size() const { return m_size; }

private:

	std::vector<int32_t> m_sparse{};	// Element id in m_data for each entity.
	std::vector<Entity> m_entities{};	// Entity id of each m_data element. 
	std::vector<T> m_data{};			// Actual data vector.
	uint32_t m_size{0};					// Number of entities in this component.

};
