#include "engine.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>



/**
 * @brief Create a RenderEngine and init the Sphere and Line OpenGL objects.
 * @param sphere_vs Path to the sphere vertex shader source file.
 * @param sphere_fs Path to the sphere fragment shader source file.
 * @param line_vs Path to the line vertex shader source file.
 * @param line_fs Path to the line fragment shader source file.
 */
RenderEngine::RenderEngine(
	const std::filesystem::path &sphere_vs, 
	const std::filesystem::path &sphere_fs,
	const std::filesystem::path &line_vs, 
	const std::filesystem::path &line_fs
) {
	const float vertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f
	};

	// Sphere initialization.
	glGenVertexArrays(1, &m_sphere_vao);
	glGenBuffers(1, &m_sphere_vbo);

	glBindVertexArray(m_sphere_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// Line initialization.
	glGenVertexArrays(1, &m_line_vao);
	glGenBuffers(1, &m_line_vbo);
	glBindVertexArray(m_line_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_line_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(sizeof(glm::vec3)));

	// Load shaders.
	m_sphere_shader = std::make_unique<Shader>(sphere_vs, sphere_fs);
	m_line_shader = std::make_unique<Shader>(line_vs, line_fs);

}

/**
 * @brief Free all OpenGL objects.
 */
RenderEngine::~RenderEngine()
{
	if (m_sphere_vao) glDeleteVertexArrays(1, &m_sphere_vao);
	if (m_sphere_vbo) glDeleteBuffers(1, &m_sphere_vbo);
	if (m_line_vao) glDeleteVertexArrays(1, &m_line_vao);
	if (m_line_vbo) glDeleteBuffers(1, &m_line_vbo);
}



/**
 * @brief Update the stored spheres before rendering.
 * @param spheres List of all spheres to render.
 */
void RenderEngine::update_spheres(const std::vector<Sphere> &spheres)
{
	if (spheres.empty()) return;
	m_sphere_ssbo = std::make_unique<SSBO>(GPUType::GPU_VEC4, spheres.size() * 2, spheres.data());
	m_nb_spheres = spheres.size();
}

/**
 * @brief Update the stored lines before rendering.
 * @param lines List of all pairs of points to render as lines.
 * lines.size() is expected to be even.
 */
void RenderEngine::update_lines(const std::vector<Point> &lines)
{
	if (lines.empty()) return;
	glBindVertexArray(m_line_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_line_vbo);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(Point), lines.data(), GL_DYNAMIC_DRAW);
	m_nb_points = lines.size();
}

/**
 * @brief Render all stored spheres.
 * @param view Camera view matrix.
 * @param proj Screen projection matrix.
 */
void RenderEngine::render_spheres(const glm::mat4 &view, const glm::mat4 &proj)
{
	if (!m_sphere_shader || !m_sphere_ssbo) return;

	m_sphere_shader->use();
	glUniformMatrix4fv(m_sphere_shader->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m_sphere_shader->uniform("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_sphere_ssbo->id());
	
	glBindVertexArray(m_sphere_vao);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, m_nb_spheres);
}

/**
 * @brief Render all stored lines.
 * @param view Camera view matrix.
 * @param proj Screen projection matrix.
 */
void RenderEngine::render_lines(const glm::mat4& view, const glm::mat4& proj)
{
if (!m_line_shader) return;

	m_line_shader->use();
	
	glUniformMatrix4fv(m_line_shader->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m_line_shader->uniform("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	
	glBindVertexArray(m_line_vao);
	glDrawArrays(GL_LINES, 0, m_nb_points);
}
