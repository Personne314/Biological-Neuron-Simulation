#pragma once

#include <vector>
#include <glm/vec4.hpp>

#include "../../render/structs.hpp"
#include "../../entities/system.hpp"
#include "structs.hpp"



class SomaRenderSystem : public System<SomaRenderSystem, Soma, Position>
{
public:
	std::vector<Sphere>& render_soma;

	SomaRenderSystem(std::vector<Sphere>& cells) : render_soma(cells) {}

	void update(Entities& entities, float dt) override
	{
		render_soma.clear();
		System<SomaRenderSystem, Soma, Position>::update(entities, dt);
	}

	void process([[maybe_unused]] Entity entity, Soma& soma, Position& pos, [[maybe_unused]] float dt)
	{
		Sphere sph;
		sph.pos_rad = glm::vec4(pos.x, pos.y, pos.z, SomaDiameters[soma.type]);
		sph.color = NeuronColors[soma.type];
		render_soma.push_back(sph);
	}
};



class AxonRenderSystem : public System<AxonRenderSystem, Axon, Position>
{
public:
	std::vector<Point>& render_lines;
	Entities* entities_ptr;

	AxonRenderSystem(Entities& ents, std::vector<Point>& lines) : render_lines(lines), entities_ptr(&ents) {}

	void update(Entities& entities, float dt) override
	{
		render_lines.clear();
		System<AxonRenderSystem, Axon, Position>::update(entities, dt);
	}

	void process([[maybe_unused]] Entity entity, Axon& axon, Position& pos, [[maybe_unused]] float dt)
	{
		if (!entities_ptr->has_component<Position>(axon.parent)) return;

		auto& parent_pos = entities_ptr->get_component<Position>(axon.parent);
		glm::vec4 color = NeuronColors[axon.type]; // On a le type directement sous la main grace a ta duplication !

		render_lines.push_back({glm::vec3(parent_pos.x, parent_pos.y, parent_pos.z), color});
		render_lines.push_back({glm::vec3(pos.x, pos.y, pos.z), color});
	}
};



class DendriteRenderSystem : public System<DendriteRenderSystem, Dendrite, Position>
{
public:
	std::vector<Point>& render_lines;
	Entities* entities_ptr;

	DendriteRenderSystem(Entities& ents, std::vector<Point>& lines) : render_lines(lines), entities_ptr(&ents) {}

	void update(Entities& entities, float dt) override
	{
		render_lines.clear();
		System<DendriteRenderSystem, Dendrite, Position>::update(entities, dt);
	}

	void process([[maybe_unused]] Entity entity, Dendrite& dendrite, Position& pos, [[maybe_unused]] float dt)
	{
		if (!entities_ptr->has_component<Position>(dendrite.parent)) return;

		auto& parent_pos = entities_ptr->get_component<Position>(dendrite.parent);
		glm::vec4 color = NeuronColors[dendrite.type]; 

		render_lines.push_back({glm::vec3(parent_pos.x, parent_pos.y, parent_pos.z), color});
		render_lines.push_back({glm::vec3(pos.x, pos.y, pos.z), color});
	}
};


class SynapseRenderSystem : public System<SynapseRenderSystem, Synapse>
{
public:
	Entities* entities_ptr;
	std::vector<Point>& render_synapse;
	bool use_gradient; // Paramètre de contrôle du mode d'affichage

	SynapseRenderSystem(Entities& ents, std::vector<Point>& lines, bool gradient = false) : entities_ptr(&ents), render_synapse(lines), use_gradient(gradient) {}

	void update(Entities& entities, float dt) override
	{
		render_synapse.clear();
		System<SynapseRenderSystem, Synapse>::update(entities, dt);
	}

	void process([[maybe_unused]] Entity entity, Synapse& synapse, [[maybe_unused]] float dt)
	{
		if (!entities_ptr->has_component<Position>(synapse.parent) || !entities_ptr->has_component<Position>(synapse.child)) return;

		auto& source_pos = entities_ptr->get_component<Position>(synapse.parent);
		auto& target_pos = entities_ptr->get_component<Position>(synapse.child);
		
		glm::vec4 color_source = glm::vec4(1.0f); 
		glm::vec4 color_target = glm::vec4(1.0f);

		if (use_gradient) {
			// Détermination de la couleur de départ
			if (entities_ptr->has_component<Axon>(synapse.parent)) color_source = NeuronColors[entities_ptr->get_component<Axon>(synapse.parent).type];
			else if (entities_ptr->has_component<Soma>(synapse.parent)) color_source = NeuronColors[entities_ptr->get_component<Soma>(synapse.parent).type];

			// Détermination de la couleur d'arrivée en fonction du type d'enfant
			if (synapse.child_dendrite && entities_ptr->has_component<Dendrite>(synapse.child)) color_target = NeuronColors[entities_ptr->get_component<Dendrite>(synapse.child).type];
			else if (synapse.child_soma && entities_ptr->has_component<Soma>(synapse.child)) color_target = NeuronColors[entities_ptr->get_component<Soma>(synapse.child).type];
			else if (synapse.child_axon && entities_ptr->has_component<Axon>(synapse.child)) color_target = NeuronColors[entities_ptr->get_component<Axon>(synapse.child).type];
		} else {
			// Coloration par poids
			if (synapse.weight < 0.0f) color_source = color_target = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			else if (synapse.weight > 0.0f) color_source = color_target = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		// OpenGL interpolera automatiquement le gradient entre ces deux sommets de la ligne
		render_synapse.push_back({glm::vec3(source_pos.x, source_pos.y, source_pos.z), color_source});
		render_synapse.push_back({glm::vec3(target_pos.x, target_pos.y, target_pos.z), color_target});
	}
};
