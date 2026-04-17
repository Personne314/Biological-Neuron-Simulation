#pragma once

#include <cstddef>
#include <vector>
#include <glm/vec4.hpp>

#include "../../render/structs.hpp"
#include "../../entities/system.hpp"
#include "structs.hpp"



/**
 * @class SomaRenderSystem
 * @brief System used to render the somas.
 */
class SomaRenderSystem : public System<SomaRenderSystem, Soma, Position>
{
public:

	/**
	 * @brief Construct a soma render system linked to a given vector.
	 * @param cells The vector where to store the generated data.
	 */
	SomaRenderSystem(std::vector<Sphere> &cells) : 
		m_soma(cells)
	{}

	/**
	 * @brief Update function to call in order to generate all spheres.
	 * @param entities The entities to render.
	 * @param dt Unused.
	 */
	void update(Entities &entities, float dt = 0) override
	{
		System<SomaRenderSystem, Soma, Position>::update(entities, dt);
	}

	/**
	 * @brief Process one entity to generate its sphere.
	 * @param entity Unused.
	 * @param soma The entity soma.
	 * @param pos The entity position in space.
	 * @param dt Unused.
	 */
	void process([[maybe_unused]] Entity entity, Soma &soma, Position &pos, [[maybe_unused]] float dt)
	{
		Sphere sph;
		sph.pos_rad = glm::vec4(pos.x, pos.y, pos.z, SomaDiameters[soma.type]);
		sph.color = NeuronColors[soma.type];
		m_soma.push_back(sph);
	}

private:

	std::vector<Sphere> &m_soma; // A reference to the vector where to store the soma spheres.

};



/**
 * @class AxonRenderSystem
 * @brief System used to render the axons.
 */
class AxonRenderSystem : public System<AxonRenderSystem, Axon, Position>
{
public:

	/**
	 * @brief Construct an axon render system linked to a given vector.
	 * @param entities The entities to use in process in order to get extra data.
	 * @param lines The vector where to store the generated data.
	 */
	AxonRenderSystem(Entities &entities, std::vector<Point> &lines) : 
		m_lines(lines), m_entities(&entities)
	{}

	/**
	 * @brief Update function to call in order to generate all lines.
	 * @param entities The entities to render.
	 * @param dt Unused.
	 */
	void update(Entities &entities, float dt = 0) override
	{
		System<AxonRenderSystem, Axon, Position>::update(entities, dt);
	}

	/**
	 * @brief Process one entity to generate its line.
	 * @param entity Unused.
	 * @param axon The entity axon.
	 * @param pos The entity position in space.
	 * @param dt Unused.
	 */
	void process([[maybe_unused]] Entity entity, Axon &axon, Position &pos, [[maybe_unused]] float dt)
	{
		if (!m_entities->has_component<Position>(axon.parent)) return;
		auto& parent_pos = m_entities->get_component<Position>(axon.parent);
		glm::vec4 color = NeuronColors[axon.type];
		m_lines.push_back({glm::vec3(parent_pos.x, parent_pos.y, parent_pos.z), color});
		m_lines.push_back({glm::vec3(pos.x, pos.y, pos.z), color});
	}

private:

	std::vector<Point> &m_lines;   // A reference to the vector where to store the axon lines.
	Entities *m_entities{nullptr}; // A pointer to the entities to get extra data during process.

};



/**
 * @class DendriteRenderSystem
 * @brief System used to render the dendrites.
 */
class DendriteRenderSystem : public System<DendriteRenderSystem, Dendrite, Position>
{
public:

	/**
	 * @brief Construct a dendrite render system linked to a given vector.
	 * @param entities The entities to use in process in order to get extra data.
	 * @param lines The vector where to store the generated data.
	 */
	DendriteRenderSystem(Entities &entities, std::vector<Point> &lines) : 
		m_lines(lines), m_entities(&entities) 
	{}

	/**
	 * @brief Update function to call in order to generate all lines.
	 * @param entities The entities to render.
	 * @param dt Unused.
	 */
	void update(Entities &entities, float dt) override
	{
		System<DendriteRenderSystem, Dendrite, Position>::update(entities, dt);
	}

	/**
	 * @brief Process one entity to generate its line.
	 * @param entity Unused.
	 * @param dendrite The entity dendrite.
	 * @param pos The entity position in space.
	 * @param dt Unused.
	 */
	void process([[maybe_unused]] Entity entity, Dendrite &dendrite, Position &pos, [[maybe_unused]] float dt)
	{
		if (!m_entities->has_component<Position>(dendrite.parent)) return;
		auto &parent_pos = m_entities->get_component<Position>(dendrite.parent);
		glm::vec4 color = NeuronColors[dendrite.type]; 
		m_lines.push_back({glm::vec3(parent_pos.x, parent_pos.y, parent_pos.z), color});
		m_lines.push_back({glm::vec3(pos.x, pos.y, pos.z), color});
	}

private:

	std::vector<Point> &m_lines;   // A reference to the vector where to store the dendrite lines.
	Entities* m_entities{nullptr}; // A pointer to the entities to get extra data during process.	

};



/**
 * @class SynapseRenderSystem
 * @brief System used to render the synapses.
 */
class SynapseRenderSystem : public System<SynapseRenderSystem, Synapse>
{
public:

	/**
	 * @brief Construct a synapse render system linked to a given vector.
	 * @param entities The entities to use in process in order to get extra data.
	 * @param lines The vector where to store the generated data.
	 */
	SynapseRenderSystem(Entities &entities, std::vector<Point> &lines, bool gradient = false) : 
		m_lines(lines), m_entities(&entities), use_gradient(gradient) 
	{}

	/**
	 * @brief Update function to call in order to generate all lines.
	 * @param entities The entities to render.
	 * @param dt Unused.
	 */
	void update(Entities &entities, float dt) override
	{
		System<SynapseRenderSystem, Synapse>::update(entities, dt);
	}

	/**
	 * @brief Process one entity to generate its line.
	 * @param entity Unused.
	 * @param synapse The entity synapse.
	 * @param pos The entity position in space.
	 * @param dt Unused.
	 */
	void process([[maybe_unused]] Entity entity, Synapse &synapse, [[maybe_unused]] float dt)
	{
		if (!m_entities->has_component<Position>(synapse.parent) || !m_entities->has_component<Position>(synapse.child)) return;

		// Get the source and target position.
		const Position source_pos = m_entities->get_component<Position>(synapse.parent);
		const Position target_pos = m_entities->get_component<Position>(synapse.child);
		glm::vec4 color_source = glm::vec4(1.0f); 
		glm::vec4 color_target = glm::vec4(1.0f);

		// In gradient mode, get the synapse color depending of the somas colors and the nature of the connection.
		if (use_gradient) {
			if (m_entities->has_component<Axon>(synapse.parent)) color_source = NeuronColors[m_entities->get_component<Axon>(synapse.parent).type];
			else if (m_entities->has_component<Soma>(synapse.parent)) color_source = NeuronColors[m_entities->get_component<Soma>(synapse.parent).type];
			if (synapse.child_dendrite && m_entities->has_component<Dendrite>(synapse.child)) color_target = NeuronColors[m_entities->get_component<Dendrite>(synapse.child).type];
			else if (synapse.child_soma && m_entities->has_component<Soma>(synapse.child)) color_target = NeuronColors[m_entities->get_component<Soma>(synapse.child).type];
			else if (synapse.child_axon && m_entities->has_component<Axon>(synapse.child)) color_target = NeuronColors[m_entities->get_component<Axon>(synapse.child).type];
		
		// Else we do a colorization depending of the weight.
		} else {
			if (synapse.weight < 0.0f) color_source = color_target = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			else if (synapse.weight > 0.0f) color_source = color_target = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		// Add the data.
		m_lines.push_back({glm::vec3(source_pos.x, source_pos.y, source_pos.z), color_source});
		m_lines.push_back({glm::vec3(target_pos.x, target_pos.y, target_pos.z), color_target});

	}

private:

	std::vector<Point> &m_lines;   // A reference to the vector where to store the synapse lines.
	Entities *m_entities{nullptr}; // A pointer to the entities to get extra data during process.
	bool use_gradient{false};      // Control of the rendering mode.

};
