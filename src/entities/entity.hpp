#pragma once

#include <cstdint>



/**
 * @struct Entity
 * @brief uint32_t with some methods for entity id bit-packing.
 */
struct Entity 
{
	static constexpr uint32_t ID_MASK = 0xFFFFF;
	static constexpr uint32_t VERSION_MASK = 0xFFF;
	static constexpr uint32_t VERSION_SHIFT = 20;

	uint32_t id{0};	// The id of the entity.

	Entity(uint32_t id) : id(id) {}
	Entity(uint32_t id, uint32_t version) : 
		id((id & ID_MASK) | ((version & VERSION_MASK) << VERSION_SHIFT)) 
	{}

	uint32_t ID() const { return id & ID_MASK; }
	uint32_t version() const { return (id >> VERSION_SHIFT) & VERSION_MASK; }
	
	bool operator==(const Entity& other) const { return id == other.id; }
	bool operator!=(const Entity& other) const { return id != other.id; }
	
};
