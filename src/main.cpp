#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "network/topology/microcolumn.h"
#include "network/topology/structs.hpp"
#include "neuron.hpp"
#include "utils/logger.hpp"
#include "render/structs.hpp"
#include "render/camera.h"
#include "render/engine.h"
#include "network/topology/render_systems.hpp"
#include "utils/octree.hpp"



int main(int argc, char* argv[])
{
	(void)argc; (void)argv;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
	
	// Create a window.
	SDL_Window *window = SDL_CreateWindow("Chroma BioSim", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		1280, 720, SDL_WINDOW_OPENGL);	
	if (!window) return -1;

	// Create the OpenGL context.
	SDL_GL_CreateContext(window);
	glewInit();
	glClearColor(0.75f, 0.7f, 0.7f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init_logger();

	// Initialize the render engine and the camera.
	RenderEngine engine("cell.vert", "cell.frag", "line.vert", "line.frag");
	Camera camera;

	// Initialize the topology ECS.
	Entities entities;
	Box world_bounds = {-10000.0f, -10000.0f, -10000.0f, 10000.0f, 10000.0f, 10000.0f};
	Octree<OctreeEntity> octree(world_bounds);

	// Create render buffers and render systems.
	std::vector<Sphere> spheres;
	std::vector<Point> lines;
	SomaRenderSystem soma_sys(spheres);
	AxonRenderSystem axon_sys(entities, lines);
	DendriteRenderSystem dendrite_sys(entities, lines);
	SynapseRenderSystem synapse_sys(entities, lines, true);







	// Microcolumn creation.
	
	MicroColumnDescriptor desc;
	desc.radius = 30.0f;
	desc.height = 2000.0f;
	desc.position = glm::vec3(0.0f, -1000.0f, 0.0f); // Centré sur l'axe Y
	desc.direction = glm::vec3(0.0f, 1.0f, 0.0f);
	
	float bounds[6] = {0.1f, 0.25f, 0.4f, 0.7f, 0.9f, 1.0f};
	std::copy(std::begin(bounds), std::end(bounds), desc.layer_boundaries);

	std::vector<LayerRecipe> column_recipes[6];
	column_recipes[0].push_back({5, NeuronModelType::CortexLayer1NonBasketCell});
	column_recipes[1].push_back({20, NeuronModelType::CortexPyramidalCell});
	column_recipes[1].push_back({5, NeuronModelType::CortexBasketCell});
	column_recipes[2].push_back({25, NeuronModelType::CortexPyramidalCell});
	column_recipes[2].push_back({5, NeuronModelType::CortexChatteringCell});
	column_recipes[3].push_back({30, NeuronModelType::CortexSpinyStellateCell});
	column_recipes[4].push_back({15, NeuronModelType::CortexLayer5PyramidalCell});
	column_recipes[4].push_back({5, NeuronModelType::CortexMartinottiCell});
	column_recipes[5].push_back({15, NeuronModelType::CortexPyramidalCell});

	build_microcolumn(entities, octree, desc, column_recipes);







	// Main loop.
	bool run = true;
	while (run) {

		// Event processing.
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) run = false;
			if (e.type == SDL_MOUSEWHEEL) camera.zoom(10.0f * e.wheel.y);
		}



		spheres.clear();
		lines.clear();

		soma_sys.update(entities, 0.0f);
		axon_sys.update(entities, 0.0f);
		dendrite_sys.update(entities, 0.0f);
		synapse_sys.update(entities, 0.0f);

		engine.update_spheres(spheres);
		engine.update_lines(lines);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100000.0f);
		engine.render_spheres(camera.view(), proj);
		engine.render_lines(camera.view(), proj);



		// Swap buffers and show the rendering.
		SDL_GL_SwapWindow(window);
	}

	// Done.
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;

}
