#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <filesystem>
#include <glm/glm.hpp>

#include "structs.hpp"
#include "shader.h"
#include "ssbo.h"



/**
 * @class RenderEngine
 * @brief Store and render a set of colored spheres and lines.
 */
class RenderEngine
{
public:

	RenderEngine(
		const std::filesystem::path &sphere_vs, 
		const std::filesystem::path &sphere_fs,
		const std::filesystem::path &line_vs, 
		const std::filesystem::path &line_fs
	);
	~RenderEngine();

	void init(
		const std::filesystem::path &vs, const std::filesystem::path &fs,
		const std::filesystem::path &line_vs, const std::filesystem::path &line_fs
	);

	void update_spheres(const std::vector<Sphere> &cells);
	void update_lines(const std::vector<Point> &lines);

	void render_spheres(const glm::mat4 &view, const glm::mat4 &proj);
	void render_lines(const glm::mat4 &view, const glm::mat4 &proj);

private:

	std::unique_ptr<Shader> m_sphere_shader{}; // Shader for rendering spheres.
	std::unique_ptr<Shader> m_line_shader{};   // Shader for rendering lines. 

	std::unique_ptr<SSBO> m_sphere_ssbo{}; // SSBOs for sphere data.

	uint32_t m_sphere_vao{0}; // VAO id for the spheres.
	uint32_t m_sphere_vbo{0}; // VBO id for the spheres.
	uint32_t m_line_vao{0};   // VAO id for the lines.
	uint32_t m_line_vbo{0};   // VBO id for the lines.

	uint32_t m_nb_spheres{0}; // Number of spheres to render.
	uint32_t m_nb_points{0};  // Number of lines to render.

};
