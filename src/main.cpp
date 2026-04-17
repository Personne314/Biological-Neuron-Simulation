#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "network/topology/column.h"
#include "network/topology/structs.hpp"
#include "utils/logger.hpp"
#include "render/structs.hpp"
#include "render/camera.h"
#include "render/engine.h"
#include "network/topology/render_systems.hpp"
#include "utils/octree.hpp"



/**
 * @brief Simulation entry point.
 * @param argc Unused.
 * @param argv Unused.
 */
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



	// Initialize one cortical column.
	Column column = new_column();
	build_column(entities, octree, column);

	// Get the column render data.
	soma_sys.update(entities, 0.0f);
	axon_sys.update(entities, 0.0f);
	dendrite_sys.update(entities, 0.0f);
	synapse_sys.update(entities, 0.0f);

	// Draw the bound of one microcolumn.
	draw_microcolumn_bounds(column, {0,0}, lines);
	draw_microcolumn_bounds(column, { 1,  0}, lines, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	draw_microcolumn_bounds(column, { 0,  1}, lines, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	draw_microcolumn_bounds(column, {-1,  1}, lines, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	draw_microcolumn_bounds(column, {-1,  0}, lines, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	draw_microcolumn_bounds(column, { 0, -1}, lines, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	draw_microcolumn_bounds(column, { 1, -1}, lines, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	// Send them to the engin for rendering.
	engine.update_spheres(spheres);
	engine.update_lines(lines);




	// Main loop.
	bool run = true;
	bool mouse_pressed = false;
	while (run) {

		// Event processing.
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) run = false;
			if (e.type == SDL_MOUSEWHEEL) camera.zoom(10.0f * e.wheel.y);
			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) mouse_pressed = true;
			if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) mouse_pressed = false;
			if (e.type == SDL_MOUSEMOTION && mouse_pressed) camera.rotate((float)e.motion.xrel, (float)e.motion.yrel);
		}

		// Render.
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
