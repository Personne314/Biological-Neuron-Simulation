#pragma once

#include <vector>

#include "entities.h"
#include "entity.hpp"
#include "../utils/octree.hpp"
#include "../neuron.hpp"



/**
 * @struct Position
 * @brief Position of an entity in space.
 */
struct Position
{
	float x;      // x position of the entity in space.
	float y;      // y position of the entity in space.
	float z;      // z position of the entity in space.
};

/**
 * @struct Soma
 * @brief Describe a neuron cell body.
 */
struct Soma
{
    chroma::ecs::Entity axon;   // Entity of the axon.
	NeuronModelType::Enum type; // Axon's neuron type.
};

/**
 * @struct Axon
 * @brief Describe a neuron axon.
 */
struct Axon
{
    std::vector<chroma::ecs::Entity> points; // Points of the axon from closest to the soma.
	chroma::ecs::Entity soma;                // Soma associated with this axon.
};

/**
 * @struct AxonPoint
 * @brief Describe one point of an axon an its connections.
 */
struct AxonPoint
{                       
    std::vector<chroma::ecs::Entity> synapses; // All synapses connected to this axon node.
	chroma::ecs::Entity axon;                  // Parent axon.
    float time_from_soma;                      // Cumulated signal propagation time from the soma.
    bool myelinated; // true if this part of the axon is myelinated. This affect this node connectivity.
    bool end;        // true of the last axon point.
};

/**
 * @struct Synapse
 * @brief Describe one synapse.
 */
struct Synapse
{
    chroma::ecs::Entity soma;       // The soma where this synapse is starting from.
    chroma::ecs::Entity axon_point; // The axon point on which this synapse is connected.
    float weight;      // Weight of the connection. This affect the information transmition
    float persistence; // Persistence of the connection. This affect the synapse resistance to pruning.
};

/**
 * @struct OctreeElement
 * @brief Represent one element and its position in a octree.
 */
struct OctreeElement
{
	float x;      // x position of the element in the tree.
	float y;      // y position of the element in the tree.
	float z;      // z position of the element in the tree.
	bool is_soma; // true if the element is a Soma.
};



/**
 * @brief Add a soma entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param octree The octree where to add the new entity.
 * @param s The soma of the entity.
 * @param p The position of the entity.
 * @return The entity id.
 */
inline chroma::ecs::Entity make_soma_entity(
	chroma::ecs::Entities &entities,
	Octree<OctreeElement> &octree,
	Soma s, 
	Position p
) {
	chroma::ecs::Entity e = entities.make_entity();
	entities.add_component<Soma>(e,s);
	entities.add_component<Position>(e,p);
	octree.insert({p.x,p.y,p.z, true});
	return e;
}

/**
 * @brief Add an axon entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param a The axon of the entity.
 * @return The entity id.
 */
inline chroma::ecs::Entity make_axon_entity(
	chroma::ecs::Entities &entities,
	const Axon &a
) {
	chroma::ecs::Entity e = entities.make_entity();
	entities.add_component<Axon>(e,a);
	return e;
}

/**
 * @brief Add an axon point entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param octree The octree where to add the new entity.
 * @param ap The axon point of the entity.
 * @param p The position of the entity.
 * @return The entity id.
 */
inline chroma::ecs::Entity make_axonpoint_entity(
	chroma::ecs::Entities &entities, 
	Octree<OctreeElement> &octree,
	const AxonPoint &ap, 
	Position p
) {
	chroma::ecs::Entity e = entities.make_entity();
	entities.add_component<AxonPoint>(e,ap);
	entities.add_component<Position>(e,p);
	octree.insert({p.x,p.y,p.z, false});
	return e;
}

/**
 * @brief Add a synapse entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param s The synapse of the entity.
 * @return The entity id.
 */
inline chroma::ecs::Entity make_synapse_entity(
	chroma::ecs::Entities &entities,
	Synapse s
) {
	chroma::ecs::Entity e = entities.make_entity();
	entities.add_component<Synapse>(e,s);
	return e;
}
