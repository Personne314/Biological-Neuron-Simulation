#pragma once 


#pragma once

#include <vector>
#include <glm/vec4.hpp>

#include "system.hpp"
#include "neuron_entities.hpp"
#include "../render/structs.hpp"



class SomaRenderSystem : public chroma::ecs::System<SomaRenderSystem, Soma, Position>
{
public:

	std::vector<Sphere> &render_soma;

	SomaRenderSystem(std::vector<Sphere> &cells) : render_soma(cells) {}

	void update(chroma::ecs::Entities &entities, float dt) override
	{
		render_soma.clear();
		chroma::ecs::System<SomaRenderSystem, Soma, Position>::update(entities, dt);
	}

	void process(chroma::ecs::Entity entity, Soma &soma, Position &pos, float dt)
	{
		Sphere sph;
		sph.pos_rad = glm::vec4(pos.x, pos.y, pos.z, SomaDiameters[soma.type]);
		sph.color = NeuronColors[soma.type];
		render_soma.push_back(sph);
	}

};



class AxonRenderSystem : public chroma::ecs::System<AxonRenderSystem, Axon>
{
public:

	std::vector<Point>& render_lines;
	chroma::ecs::Entities *render_axon;

	AxonRenderSystem(chroma::ecs::Entities &render_axon, std::vector<Point>& lines) : 
		render_lines(lines), render_axon(&render_axon)
	{}

	void update(chroma::ecs::Entities& entities, float dt) override
	{
		render_lines.clear();
		chroma::ecs::System<AxonRenderSystem, Axon>::update(entities, dt);
	}

	void process(chroma::ecs::Entity entity, Axon& axon, float dt)
	{
		if (!render_axon || !render_axon->has_component<Soma>(axon.soma) || !render_axon->has_component<Position>(axon.soma)) return;

		auto& soma = render_axon->get_component<Soma>(axon.soma);
		auto& soma_pos = render_axon->get_component<Position>(axon.soma);
		glm::vec4 color = NeuronColors[soma.type];

		Position prev_pos = soma_pos;

		for (chroma::ecs::Entity pt_ent : axon.points) {
			if (!render_axon->has_component<Position>(pt_ent)) continue;
			auto& curr_pos = render_axon->get_component<Position>(pt_ent);
			render_lines.push_back({glm::vec3(prev_pos.x, prev_pos.y, prev_pos.z), color});
			render_lines.push_back({glm::vec3(curr_pos.x, curr_pos.y, curr_pos.z), color});
			prev_pos = curr_pos;
		}
	}
};



class SynapseRenderSystem : public chroma::ecs::System<SynapseRenderSystem, Synapse>
{
public:

	chroma::ecs::Entities *entities;
	std::vector<Point> &render_synapse;

	SynapseRenderSystem(chroma::ecs::Entities &entities, std::vector<Point> &render_synapse) : 
		entities(&entities), render_synapse(render_synapse) 
	{}

	void update(chroma::ecs::Entities &entities, float dt) override
	{
		render_synapse.clear();
		chroma::ecs::System<SynapseRenderSystem, Synapse>::update(entities, dt);
	}

	void process(chroma::ecs::Entity entity, Synapse &synapse, float dt)
	{
		auto& soma_pos = entities->get_component<Position>(synapse.soma);
		auto& axon_pos = entities->get_component<Position>(synapse.axon_point);
		glm::vec4 color = glm::vec4(1.0f);
		if (entities->has_component<Soma>(synapse.soma)) {
			auto& soma = entities->get_component<Soma>(synapse.soma);
			color = NeuronColors[soma.type];
		}
		render_synapse.push_back({glm::vec3(axon_pos.x, axon_pos.y, axon_pos.z), color});
		render_synapse.push_back({glm::vec3(soma_pos.x, soma_pos.y, soma_pos.z), color});
	}

};


