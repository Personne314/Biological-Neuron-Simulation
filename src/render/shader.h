#pragma once

#include <GL/glew.h>
#include <string>
#include <stdint.h>
#include <filesystem>
#include <vector>



/**
 * @struct UniformInfo
 * @brief Info of one uniform in the shader.
 */
struct UniformInfo
{
	std::string name;	// Name of the binding.
	GLenum type;		// GL_FLOAT, GL_FLOAT_VEC3, GL_INT, etc.
	int32_t location;	// glGetUniformLocation.
	int32_t size;		// Array size.
};

/**
 * @struct TextureBindingInfo
 * @brief Info of one texture binding in the shader.
 */
struct TextureBindingInfo
{
	std::string name;	// Name of the binding.
	GLenum type;		// GL_SAMPLER_2D, GL_SAMPLER_2D_ARRAY, etc.
	int32_t location;	// glGetUniformLocation.
	int32_t binding;	// Value of the uniform (the texture unit index).
};

/**
 * @struct SSBOBindingInfo
 * @brief Info of one SSBO binding in the shader.
 */
struct SSBOBindingInfo
{
	std::string name;	// Name of the binding.
	GLenum type;		// Usually GL_SHADER_STORAGE_BLOCK.
	uint32_t binding;	// Binding point (layout(binding=X))
	uint32_t data_size;	// Block size in bytes.
};



/**
 * @class Shader
 * @brief Load and compile a shader from the disk. 
 */
class Shader
{
public:

	Shader() noexcept = default;
	Shader(const char *vert, const char *frag);
	Shader(const std::filesystem::path &path_vert, const std::filesystem::path &path_frag);
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other) noexcept :
		m_init(other.m_init),
		m_vertID(other.m_vertID),
		m_fragID(other.m_fragID),
		m_progID(other.m_progID),
		m_uniform_info(std::move(other.m_uniform_info)),
		m_texture_binding_info(std::move(other.m_texture_binding_info)),
		m_ssbo_binding_info(std::move(other.m_ssbo_binding_info))
	{
		other.m_init = false;
		other.m_progID = 0;
		other.m_vertID = 0;
		other.m_fragID = 0;
	}

	Shader& operator=(Shader&& other) noexcept
	{
		if (this != &other) {
			if (m_progID) glDeleteProgram(m_progID);
			if (m_vertID) glDeleteShader(m_vertID);
			if (m_fragID) glDeleteShader(m_fragID);
			m_init = other.m_init;
			m_progID = other.m_progID;
			m_vertID = other.m_vertID;
			m_fragID = other.m_fragID;
			m_uniform_info = std::move(other.m_uniform_info);
			m_texture_binding_info = std::move(other.m_texture_binding_info);
			m_ssbo_binding_info = std::move(other.m_ssbo_binding_info);
			other.m_init = false;
			other.m_progID = 0;
			other.m_vertID = 0;
			other.m_fragID = 0;
		}
		return *this;
	}

	~Shader();

	void use() const { glUseProgram(m_progID); }
	void unuse() const { glUseProgram(0); }

	int32_t uniform(std::string name) const { return glGetUniformLocation(m_progID, name.c_str()); }
	int32_t ssbo(std::string name) const 
	{
		GLuint resource_index = glGetProgramResourceIndex(m_progID, GL_SHADER_STORAGE_BLOCK, name.c_str());
		if (resource_index == GL_INVALID_INDEX) return -1;
		GLenum property = GL_BUFFER_BINDING;
		GLint binding_point = -1;
		glGetProgramResourceiv(m_progID, GL_SHADER_STORAGE_BLOCK, resource_index, 1, &property, 1, nullptr, &binding_point);
		return binding_point;
	}

	const std::vector<UniformInfo> &uniform_info() const { return m_uniform_info; }
	const std::vector<TextureBindingInfo> &texture_info() const { return m_texture_binding_info; }
	const std::vector<SSBOBindingInfo> &ssbo_info() const { return m_ssbo_binding_info; }

	bool isInit() const { return m_init; }

private:

	void compileSources(const char *vert, const char *frag);
	bool compileShader(uint32_t &shader, GLenum type, const char *source);

	bool m_init{false}; // true if the shader is well initialized.

	uint32_t m_vertID{0}; // Vertex shader id.
	uint32_t m_fragID{0}; // Framgent shader id.
	uint32_t m_progID{0}; // Program id.

	std::vector<UniformInfo> m_uniform_info{};                // Uniform informations.
	std::vector<TextureBindingInfo> m_texture_binding_info{}; // Texture bindings informations.
	std::vector<SSBOBindingInfo> m_ssbo_binding_info{};       // SSBO bindings informations.

};
