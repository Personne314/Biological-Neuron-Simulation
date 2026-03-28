#include "ssbo.h"

#include "../utils/logger.hpp"



/**
 * @brief SSBO constructor. Initialize an OpenGL buffer.
 * @param type The type of the stored elements.
 * @param size The number of elements in the buffer.
 * @param usage The general usage of this buffer.
 * @param buffer The initial values to copy. Can be nullptr.
 */
SSBO::SSBO(GPUType type, size_t length, const void *buffer) : 
	m_id(0),
	m_type(type),
	m_length(length),
	m_size(length * GPUTYPE_INFO(type).aligned_size_bytes)
{

	// Create the buffer id.
	glGenBuffers(1, &m_id);
	if (m_id == 0) {
		print_error("glGenBuffers failure for SSBO. Is the OpenGL context valid?");
		return;
	}

	// Allocate the buffer and copy the given data.
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_size, buffer, GL_STATIC_DRAW);
		GLenum err = glGetError();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Error checking.
	if (err == GL_OUT_OF_MEMORY) {
		print_error("glBufferData couldn't allocate the buffer (GL_OUT_OF_MEMORY).");
		glDeleteBuffers(1, &m_id);
		m_id = 0;
	} else if (err != GL_NO_ERROR) {
		print_error("glBufferData failure : {}", err);
		glDeleteBuffers(1, &m_id);
		m_id = 0;
	}

}
