#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <numbers>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "entities/neuron_systems.hpp"
#include "neuron.hpp"
#include "utils/logger.hpp"
#include "render/structs.hpp"
#include "render/camera.h"
#include "render/engine.h"
#include "entities/neuron_entities.hpp"
#include "utils/octree.hpp"



int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
	
	SDL_Window* window = SDL_CreateWindow("Chroma BioSim", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		1280, 720, SDL_WINDOW_OPENGL);
		
	if (!window) return -1;

	SDL_GL_CreateContext(window);
	glewInit();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	init_logger();

	RenderEngine engine("cell.vert", "cell.frag", "line.vert", "line.frag");





	// Initialisation ECS et Octree
	chroma::ecs::Entities entities;
	Box world_bounds = {-10000.0f, -10000.0f, -10000.0f, 10000.0f, 10000.0f, 10000.0f};
	Octree<OctreeElement> octree(world_bounds);

	// Remplace ceci par la liste exhaustive de ton enum NeuronModelType
	std::vector<NeuronModelType::Enum> neuron_types = {

		NeuronModelType::CortexPyramidalCell,
		NeuronModelType::CortexSpinyStellateCell,
		NeuronModelType::CortexLayer5PyramidalCell,
		NeuronModelType::CortexChatteringCell,
		NeuronModelType::CortexBasketCell,
		NeuronModelType::CortexNonBasketCell,
		NeuronModelType::CortexMartinottiCell,
		NeuronModelType::CortexLayer1NonBasketCell,

		// Thalamus
		NeuronModelType::ThalamusThalamocorticalRelay,
		NeuronModelType::ThalamusReticularNucleusNeuron,
		NeuronModelType::ThalamusTIn_FS,
		NeuronModelType::ThalamusTIn_LTS,

		// Hippocampus
		NeuronModelType::HippocampusCA3PyramidalCell,
		NeuronModelType::HippocampusCA1PyramidalCell,
		NeuronModelType::HippocampusGranuleCell,
		NeuronModelType::HippocampusBasketCell,
		NeuronModelType::HippocampusOLMCell,

		// Basal Ganglia
		NeuronModelType::BasalGangliaMediumSpinyNeuron,
		NeuronModelType::BasalGangliaStriatalFastSpiking,
		NeuronModelType::BasalGangliaPallidalNeuron,

		// Brainstem
		NeuronModelType::BrainstemDopaminergicNeuron,
		NeuronModelType::BrainstemCholinergicNeuron,
		NeuronModelType::BrainstemMesencephalicNeuron,

		// Cerebellum
		NeuronModelType::CerebellumPurkinjeCell,
		NeuronModelType::CerebellumGranuleCell,
		NeuronModelType::CerebellumGolgiCell,
		NeuronModelType::CerebellumStellateCell,

		// Spinal Cord
		NeuronModelType::SpinalCordAlphaMotorNeuron,
		NeuronModelType::SpinalCordGammaMotorNeuron,
		NeuronModelType::SpinalCordRenshawCell,

		// Amygdala
		NeuronModelType::AmygdalaBasolateralPyramidal,
		NeuronModelType::AmygdalaCentralNucleusNeuron,

		// Olfactory Bulb
		NeuronModelType::OlfactoryBulbMitralCell,
		NeuronModelType::OlfactoryBulbTuftedCell,
		NeuronModelType::OlfactoryBulbPeriglomerularCell
	};
	
	int num_somas = neuron_types.size();
	std::vector<Sphere> cells(num_somas);
	SomaRenderSystem soma_render_system(cells);
	
	float spacing = 75.0f;
	float radius = 0.0f;
	
	if (num_somas > 1)
		radius = spacing / (2.0f * std::sin(std::numbers::pi_v<float> / num_somas));

	// Placement en cercle
	for (int i = 0; i < num_somas; ++i) {
		float angle = i * (2.0f * std::numbers::pi_v<float> / num_somas);
		
		Position pos = {
			radius * std::cos(angle),
			0.0f, // Cercle sur le plan XZ
			radius * std::sin(angle)
		};
		
		Soma soma = {0, neuron_types[i]}; // axon = 0 (entité nulle au départ)
		
		// 1. Enregistrement dans l'ECS et l'Octree
		make_soma_entity(entities, octree, soma, pos);

	}

	Camera camera;
	bool running = true;
	bool mousePressed = false;

	std::vector<Point> lines;

	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) running = false;
			if (e.type == SDL_MOUSEWHEEL) camera.zoom((float)e.wheel.y);
			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) mousePressed = true;
			if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) mousePressed = false;
			if (e.type == SDL_MOUSEMOTION && mousePressed) camera.rotate((float)e.motion.xrel, (float)e.motion.yrel);
		}

		lines.clear();
		for (size_t i = 0; i < cells.size(); ++i) {
			size_t next_i = (i + 1) % cells.size();
			glm::vec4 color = cells[i].color;
			
			// Point de départ (utilise la couleur de la sphère de départ)
			lines.push_back({glm::vec3(cells[i].pos_rad.x, cells[i].pos_rad.y, cells[i].pos_rad.z), color});
			// Point d'arrivée
			lines.push_back({glm::vec3(cells[next_i].pos_rad.x, cells[next_i].pos_rad.y, cells[next_i].pos_rad.z), color});
		}

		soma_render_system.update(entities, 0);
		engine.update_spheres(cells);
		engine.update_lines(lines);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100000.0f);
		engine.render_spheres(camera.view(), proj);
		engine.render_lines(camera.view(), proj);

		SDL_GL_SwapWindow(window);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;

}