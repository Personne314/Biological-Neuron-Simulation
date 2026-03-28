#include "shader.h"

#include <cstdint>
#include <fstream>

#include "../utils/logger.hpp"



/**
 * @brief Construct a shader from sources.
 * @param vert The source of the vertex shader.
 * @param frag The source of the fragment shader.
 */
Shader::Shader(const char *vert, const char *frag)
{
	compileSources(vert, frag);
}

/**
 * @brief Construct a shader from source files.
 * @param path_vert The path of the vertex shader.
 * @param path_frag The path of the fragment shader.
 */
Shader::Shader(const std::filesystem::path &path_vert, const std::filesystem::path &path_frag)
{

	#ifndef NDEBUG
	print_debug("Loading shader ('{}', '{}')", path_vert.string(), path_frag.string());
	#endif

	// Strings for the sources.
	std::string vert_source, frag_source, line;

	// Open the shader files.  
	std::ifstream file_vert(path_vert.c_str());
	std::ifstream file_frag(path_frag.c_str());
	if (!file_vert) {
		print_error("Failed to compile '{}' : vertex source file does not exists.", path_vert.string());
		return;
	} 
	if (!file_frag) {
		print_error("Failed to compile '{}' : fragment source file does not exists.", path_frag.string());
		return;
	} 

	// Read the vertex shader source code.
	while (getline(file_vert, line)) vert_source += line + '\n';
	file_vert.close();

	// Read the fragment shader source code.
	while (getline(file_frag, line)) frag_source += line + '\n';
	file_frag.close();

	// Compile the shaders.
	compileSources(vert_source.c_str(), frag_source.c_str());

}

/**
 * @brief Destruct a shader.
 */
Shader::~Shader()
{
	if(glIsShader(m_vertID) == GL_TRUE) glDeleteShader(m_vertID);
	if(glIsShader(m_fragID) == GL_TRUE) glDeleteShader(m_fragID);
	if(glIsProgram(m_progID) == GL_TRUE) glDeleteProgram(m_progID);
}



/**
 * @brief Compile shader sources and complete the shader init.
 * @param vert The vertex shader source.
 * @param frag The fragment shader source.
 */
void Shader::compileSources(const char *vert, const char *frag)
{

	// Compile the sources.
	if (!compileShader(m_vertID, GL_VERTEX_SHADER, vert) ||
		!compileShader(m_fragID, GL_FRAGMENT_SHADER, frag)
	) return;

	// Create the program.
	m_progID = glCreateProgram();
	glAttachShader(m_progID, m_vertID);
	glAttachShader(m_progID, m_fragID);

	// Link the program.
	glLinkProgram(m_progID);
	int32_t state = 0;
	glGetProgramiv(m_progID, GL_LINK_STATUS, &state);
	if(state != GL_TRUE) {

		// Get error message.
		int32_t size = 0;
		glGetProgramiv(m_progID, GL_INFO_LOG_LENGTH, &size);
		char *error = new char[size + 1];
		glGetShaderInfoLog(m_progID, size, &size, error);
		error[size] = '\0';

		// Log the error.
		print_error(
			"Failed to link the shader files '{}' and '{}' : {}", 
			vert, frag, error
		);
		delete[] error;
		return;

	}

	// Parse active uniforms.
	int32_t uniform_count = 0;
	glGetProgramiv(m_progID, GL_ACTIVE_UNIFORMS, &uniform_count);
	for (int32_t i = 0; i < uniform_count; ++i) {

		// Uniform infos.
		char name[256];
		GLsizei length = 0;
		int32_t size = 0;
		GLenum type = 0;
		int32_t texUnit = -1;

		// Get the current uniform infos.
		glGetActiveUniform(m_progID, i, sizeof(name), &length, &size, &type, name);
		int32_t location = glGetUniformLocation(m_progID, name);

		// Add the infos to the right vector.
		switch (type) {
		case GL_SAMPLER_1D: case GL_SAMPLER_2D: case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE: case GL_SAMPLER_2D_ARRAY: case GL_SAMPLER_CUBE_MAP_ARRAY:
			glGetUniformiv(m_progID, location, &texUnit);
			m_texture_binding_info.push_back({name, type, location, texUnit});
			break;
		default:
			m_uniform_info.push_back({name, type, location, size});
		}

	}

	// Parse SSBOs.
	int32_t ssbo_count = 0;
	glGetProgramInterfaceiv(m_progID, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &ssbo_count);
	for (int32_t i = 0; i < ssbo_count; ++i) {
		const GLenum props[] = {
			GL_NUM_ACTIVE_VARIABLES,
			GL_BUFFER_BINDING,
			GL_BUFFER_DATA_SIZE
		};

		// Get SSBOs infos.
		int32_t values[3];
		glGetProgramResourceiv(
			m_progID, GL_SHADER_STORAGE_BLOCK,
			i, 3, props, 3, nullptr, values
		);

		// Get SSBOs name.
		char name[256];
		GLsizei length;
		glGetProgramResourceName(
			m_progID, GL_SHADER_STORAGE_BLOCK,
			i, sizeof(name), &length, name
		);

		// Add SSBOs info to the vector.
		m_ssbo_binding_info.push_back({name, (uint32_t)values[1], GL_SHADER_STORAGE_BLOCK, (uint32_t)values[2]});
	
	}

	// The shader is now usable.
	m_init = true;

}



/**
 * @brief Compile a shader source code.
 * @param shader The variable where to store the shader id.
 * @param type The type of the shader to compile.
 * @param source The source of the shader.
 */
bool Shader::compileShader(uint32_t &shader, GLenum type, const char *source)
{

	// Create a new shader.
	shader = glCreateShader(type);
	if (shader == 0) {
		print_error("Failed to compile '{}' : type {} does not exists.", source, type);
		return false;
	} 

	// Compile the shader.
	glShaderSource(shader, 1, (GLchar**)&source, 0);
	glCompileShader(shader);
	int32_t state = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
	if (state != GL_TRUE) {

		// Get error messages.
		int32_t size = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
		char *error = new char[size + 1];
		glGetShaderInfoLog(shader, size, &size, error);
		error[size] = '\0';

		// Log the error.
		print_error("Failed to compile '{}' : {}", source, error);
		delete [] error;
		return false;

	}
	
	// The shader is now compiled.
	return true;

}
