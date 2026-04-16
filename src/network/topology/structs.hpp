#pragma once

#include "../../entities/entity.hpp"
#include "../../entities/entities.h"
#include "../../utils/octree.hpp"
#include "../../neuron.hpp"



/**
 * @struct Position
 * @brief Contains the position of an entity in space.
 */
struct Position
{
	float x; // x position of the entity in space.
	float y; // y position of the entity in space.
	float z; // z position of the entity in space.
};



/**
 * @enum class OctreeEntityType
 * @brief Enum all possible entity type for octree research.
 */
namespace OctreeEntityType
{
	enum Enum
	{
		Soma,
		Axon,
		Dendrite,
		Synapse
	};
};

/**
 * @struct OctreeEntity
 * @brief Represent one element and its position in a octree.
 */
struct OctreeEntity
{
	float x;       // x position of the element in the tree.
	float y;       // y position of the element in the tree.
	float z;       // z position of the element in the tree.
	Entity entity; // The entity associated with this octree element.
	OctreeEntityType::Enum type; // Type of the entity for research.
};

/**
 * @struct Soma
 * @brief Contain the data of a neuron soma entity.
 */
struct Soma
{
	Entity axon_root;           // Starting node of the neuron axon tree.
	Entity dendrite_root;       // Starting node of the neuron dendrite tree.
	NeuronModelType::Enum type; // Type of the neuron.
};

/**
 * @struct Axon
 * @brief Contain the data of one node of an axon tree.
 */
struct Axon
{
	Entity soma_root;           // Entity of the parent neuron soma.
	Entity parent;              // Parent axon node entity or self if root.
	Entity child;               // Child axon node entity or self if terminal.
	Entity sibiling;            // Next sibiling of this node or self if there is no sibiling.
	float time_from_soma;       // Time from the soma to this node in us.
	NeuronModelType::Enum type; // Type of the parent neuron.
	bool myelinated;            // true if this section of the axon is myelinated.
};

/**
 * @struct Dendrite
 * @brief Contain the data of one dendrite of an axon tree.
 */
struct Dendrite
{
	Entity soma_root;   // Entity of the parent neuron soma. 
	Entity parent;      // Parent dendrite node entity or self if root.
	Entity child;       // Child dendrite node entity or self if terminal.
	Entity sibiling;    // Next sibiling of this node or self if there is no sibiling.
	float time_to_soma; // Time from this node to the soma in us.
	NeuronModelType::Enum type; // Type of the parent neuron.
};

/**
 * @struct Synapse
 * @brief Contain the data of one synapse between two entities.
 */
struct Synapse
{
	Entity parent;       // Node from where the information is received.
	Entity child;        // Node where the information is send.
	float weight;        // Weight of the synapse. This is basically the conductance.
	float persistence;   // Persistance of the synapse. Resistance to pruning.
	bool child_dendrite; // true if the synapse ends on a dendrite.
	bool child_axon;     // true if the synapse ends on an axon.
	bool child_soma;     // true if the synapse ends on a soma.
};



/**
 * @brief Add a soma entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param octree The octree to add the entity to.
 * @param s 
 * @param p The position of the entity.
 */
inline Entity make_soma_entity(Entities &entities, Octree<OctreeEntity> &octree, const Soma &s, const Position &p)
{
	Entity e = entities.make_entity();
	entities.add_component<Soma>(e, s);
	entities.add_component<Position>(e, p);
	octree.insert({p.x, p.y, p.z, e, OctreeEntityType::Soma});
	return e;
}

/**
 * @brief Add an axon entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param octree The octree to add the entity to.
 * @param a 
 * @param p The position of the entity.
 */
inline Entity make_axon_entity(Entities &entities, Octree<OctreeEntity> &octree, const Axon &a, const Position &p)
{
	Entity e = entities.make_entity();
	entities.add_component<Axon>(e, a);
	entities.add_component<Position>(e, p);
	octree.insert({p.x, p.y, p.z, e, OctreeEntityType::Axon});
	return e;
}

/**
 * @brief Add a dendrite entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param octree The octree to add the entity to.
 * @param d 
 * @param p The position of the entity.
 */
inline Entity make_dendrite_entity(Entities &entities, Octree<OctreeEntity> &octree, const Dendrite &d, const Position &p)
{
	Entity e = entities.make_entity();
	entities.add_component<Dendrite>(e, d);
	entities.add_component<Position>(e, p);
	octree.insert({p.x, p.y, p.z, e, OctreeEntityType::Dendrite});
	return e;
}

/**
 * @brief Add a synapse entity in the given Entities.
 * @param entities The Entities where to add the new entity.
 * @param s The synapse definition.
 */
inline Entity make_synapse_entity(Entities &entities, const Synapse &s)
{
	Entity e = entities.make_entity();
	entities.add_component<Synapse>(e, s);
	return e;
}
