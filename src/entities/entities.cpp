#include "entities.h"

#include <bitset>
#include <cstdint>
#include <memory>



namespace chroma::ecs
{

/**
 * @brief Make a new entity and return its id.
 * @return The id of the entity.
 */
Entity Entities::make_entity()
{
	if (m_free_ids.size()) {
		Entity id = m_free_ids.back();
		m_free_ids.pop_back();
		return id;
	}
	uint32_t id = m_entity_counter++;
	m_versions.push_back(0);
	m_signatures.push_back(std::bitset<64>());
	return Entity(id, 0);
}

/**
 * @brief Kill an entity and add its id to the free id list.
 * @param entity The id of the entity to kill.
 */
void Entities::kill_entity(Entity entity)
{
	const uint32_t id = entity.ID();
	const uint16_t version = entity.version();
	if (id >= m_entity_counter || m_versions[id] != version) return;
	for (std::unique_ptr<IComponent> &c : m_components) if (c) c->remove(entity);
	const Entity new_entity = Entity(id, version+1);
	m_free_ids.push_back(new_entity);
	m_versions[id] = new_entity.version();
	m_signatures[id].reset();
}

/**
 * @brief Check if the entity is alive.
 * @return true if the entity still exists, false if it was killed.
 */
bool Entities::check(Entity entity) const 
{
	const uint32_t id = entity.ID();
	return (id < m_entity_counter) && (m_versions[id] == entity.version());
}

}
